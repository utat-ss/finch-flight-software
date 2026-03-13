/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#include <string.h>

LOG_MODULE_REGISTER(finch_adcs);

static const struct device *const finch_adcs_usart = DEVICE_DT_GET(DT_NODELABEL(usart2));

static const uint8_t FINCH_ADCS_HEADER_LEN = 4;
static const uint8_t FINCH_ADCS_HEADER_READ = 0xC9;

#define FINCH_ADCS_RX_LEN 64

static const int FINCH_ADCS_FIRST_BYTE_TIMEOUT_MS = 1000;
static const int FINCH_ADCS_INNER_BYTE_TIMEOUT_MS = 500;

static uint8_t rx_buf[FINCH_ADCS_RX_LEN];
static volatile size_t rx_len;
static K_SEM_DEFINE(rx_sem, 0, 1);

static const uint8_t finch_adcs_register_map[] = {
	[FINCH_ADCS_REGISTER_SNID] = 0x00
};

static const uint8_t finch_adcs_register_index[] = {
	[FINCH_ADCS_REGISTER_SNID] = 0x01
};

// static uint8_t finch_adcs_generate_checksum(const uint8_t *buf, size_t len)
// {
// 	uint8_t sum = 0;

// 	while (--len) {
// 		sum += buf[len];
// 	}

// 	return 0xFF - sum + 1;
// }

static void finch_adcs_uart_isr(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	while (uart_irq_rx_ready(dev)) {
		uint8_t byte;
		uart_fifo_read(dev, &byte, 1);

		if (rx_len < FINCH_ADCS_RX_LEN) {
			rx_buf[rx_len++] = byte;
		}
		k_sem_give(&rx_sem);
	}
}

static finch_adcs_ret finch_adcs_transact(const uint8_t *cmd, size_t cmd_len,
					uint8_t *resp, size_t resp_size)
{
	rx_len = 0;
	k_sem_reset(&rx_sem);

	uart_irq_rx_enable(finch_adcs_usart);

	for (size_t i = 0; i < cmd_len; i++) {
		uart_poll_out(finch_adcs_usart, cmd[i]);
	}

	k_timeout_t timeout = K_MSEC(FINCH_ADCS_FIRST_BYTE_TIMEOUT_MS);

	while (k_sem_take(&rx_sem, timeout) == 0) {
		timeout = K_MSEC(FINCH_ADCS_INNER_BYTE_TIMEOUT_MS);
	}

	uart_irq_rx_disable(finch_adcs_usart);

	if (rx_len == 0) {
		return FINCH_ADCS_RET_NONE_READ;
	}

	size_t copy_len = (rx_len < resp_size) ? rx_len : resp_size;

	memcpy(resp, rx_buf, copy_len);

	return FINCH_ADCS_RET_OK;
}

finch_adcs_ret finch_adcs_init(void)
{
	if (!device_is_ready(finch_adcs_usart)) {
		return -1;
	}

	if (uart_irq_callback_set(finch_adcs_usart, finch_adcs_uart_isr)) {
		return -1;
	}

	return FINCH_ADCS_RET_OK;
}

finch_adcs_ret finch_adcs_register_read(
	finch_adcs_register reg,
	uint8_t *rx,
	uint8_t rx_l)
{
	/* Command is made up of header bytes + checksum byte. */
	uint8_t cmd[FINCH_ADCS_HEADER_LEN + 1];

	/* ADCS header bytes. */
	cmd[0] = FINCH_ADCS_HEADER_READ;
	cmd[1] = finch_adcs_register_index[reg];
	cmd[2] = 0x03;
	cmd[3] = finch_adcs_register_map[reg];

	/* ADCS checksum byte. */
	cmd[FINCH_ADCS_HEADER_LEN] = 0x33;//finch_adcs_generate_checksum(cmd, FINCH_ADCS_HEADER_LEN);

	return finch_adcs_transact(cmd, sizeof(cmd),
								rx, rx_l);
}
