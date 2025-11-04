/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

static const struct device *const usart1 = DEVICE_DT_GET(DT_NODELABEL(usart1));

#define RX_BUF_SIZE 64

static uint8_t rx_buf_uart[RX_BUF_SIZE];
static volatile size_t rx_len_uart;

static K_SEM_DEFINE(rx_sem_uart, 0, 1);

static void adcs_uart_isr(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	while (uart_irq_rx_ready(dev)) {
		uint8_t received_byte;
		uart_fifo_read(dev, &received_byte,1);
	
		rx_buf_uart[rx_len_uart++] = received_byte;
	
		k_sem_give(&rx_sem_uart);
	}
}

adcs_ret adcs_init()
{
	uart_irq_callback_set(usart1, adcs_uart_isr);
	// uart_irq_rx_enable(usart1);

	return ADCS_RET_OK;
}

adcs_ret adcs_get_id(uint8_t *id)
{
	for (int i = 0; i < ADCS_ID_SIZE; ++i) {
		id[i] = i;
	}

	return ADCS_RET_OK;
}
