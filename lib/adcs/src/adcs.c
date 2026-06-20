/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <finch/adcs/adcs.h>

#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

uint8_t const ADCS_ID_SIZE = 12;
static uint8_t const adcs_id_expected[] = {0x54, 0x41, 0x44, 0x31, 0x30, 0x32,
					   0x30, 0x35, 0x33, 0x44, 0x00, 0x00};

/* ADCS is connected via USART2. */
static const struct device *const adcs_uart = DEVICE_DT_GET(DT_ALIAS(adcs_uart));

/* Global buffer to store ADCS RX bytes. */
#define ADCS_RX_BUF_SIZE 32
static uint8_t adcs_rx_buf[ADCS_RX_BUF_SIZE];
static uint8_t adcs_rx_len;
static uint8_t adcs_rx_expected;
static K_SEM_DEFINE(adcs_rx_sem, 0, 1);

LOG_MODULE_REGISTER(adcs);

static void adcs_uart_isr(const struct device *dev, void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	while (uart_irq_rx_ready(dev)) {
		uint8_t received_byte;
		uart_fifo_read(dev, &received_byte, 1);

		if (adcs_rx_len < ADCS_RX_BUF_SIZE) {
			adcs_rx_buf[adcs_rx_len++] = received_byte;
		}

		if (adcs_rx_len == adcs_rx_expected) {
			k_sem_give(&adcs_rx_sem);
		}
	}
}

adcs_rc_t adcs_init(void)
{
	if (!device_is_ready(adcs_uart)) {
		return ADCS_RC_ERR;
	}

	uart_irq_callback_set(adcs_uart, adcs_uart_isr);
	uart_irq_rx_enable(adcs_uart);

	return ADCS_RC_OK;
}

static bool adcs_verify_checksum(const uint8_t *buf, uint8_t len)
{
	uint8_t sum = 0;
	for (uint8_t i = 0; i < len; ++i) {
		sum += buf[i];
	}

	return sum == 0;
}

adcs_rc_t adcs_get_id(uint8_t *id, uint8_t id_size)
{
	__ASSERT(id, "ID array cannot be NULL.");
	__ASSERT(id_size == ADCS_ID_SIZE, "ID array must be exactly %u bytes.", ADCS_ID_SIZE);

	/* Reset RX semaphore and count state */
	k_sem_reset(&adcs_rx_sem);
	adcs_rx_len = 0U;
	adcs_rx_expected = 4U + id_size + 1U;

	/* 5 byte command to read the ID. */
	uint8_t adcs_id_cmd[] = {0xc9, 0x01, 0x03, 0x00, 0x33};

	/* Send the command to read the ID. */
	for (size_t i = 0; i < sizeof(adcs_id_cmd); ++i) {
		uart_poll_out(adcs_uart, adcs_id_cmd[i]);
	}

	/* Wait for the ADCS to finish replying. */
	if (k_sem_take(&adcs_rx_sem, K_MSEC(1000)) != 0) {
		LOG_ERR("ADCS ID response timeout");
		return ADCS_RC_ERR;
	}

	if (adcs_rx_len < adcs_rx_expected) {
		LOG_ERR("ADCS ID response too short (%u bytes)", adcs_rx_len);
		return ADCS_RC_ERR;
	}

	uint8_t adcs_rx_i = 0;

	/* Verify the 4 byte RX header matches 4 byte command header. */
	for (; adcs_rx_i < 4; ++adcs_rx_i) {
		if (adcs_rx_buf[adcs_rx_i] != adcs_id_cmd[adcs_rx_i]) {
			return ADCS_RC_ERR;
		}
	}

	/* Output ID */
	for (uint8_t i = 0; i < id_size; ++i, ++adcs_rx_i) {
		if (adcs_rx_buf[adcs_rx_i] != adcs_id_expected[i]) {
			/* TODO (?): return bitmap of where the ID failed to match with expected */
			LOG_ERR("ADCS ID has unexpected value at position %u, 0x%02x", i,
				adcs_rx_buf[adcs_rx_i]);
			return ADCS_RC_ERR;
		}

		id[i] = adcs_rx_buf[adcs_rx_i];
	}

	/* Verify checksum. */
	if (!adcs_verify_checksum(adcs_rx_buf, adcs_rx_len)) {
		LOG_ERR("ADCS Checksum error");
		return ADCS_RC_ERR;
	}

	return ADCS_RC_OK;
}


/* 
 * Generic read - builds and sends a read request, waits for response 
 * into adcs_rx_buf. Caller is responsible for parsing adcs_rx_buf.
 * expected_data_bytes should be data_count * 4.
 */
static adcs_rc_t adcs_read(const uint8_t *cmd, uint8_t cmd_len, uint8_t expected_data_bytes)
{
    k_sem_reset(&adcs_rx_sem);
    adcs_rx_len = 0U;
    adcs_rx_expected = 4U + expected_data_bytes + 1U; /* header + data + checksum */

    for (size_t i = 0; i < cmd_len; ++i) {
        uart_poll_out(adcs_uart, cmd[i]);
    }

    if (k_sem_take(&adcs_rx_sem, K_MSEC(1000)) != 0) {
        LOG_ERR("ADCS response timeout");
        return ADCS_RC_ERR;
    }

    if (adcs_rx_len < adcs_rx_expected) {
        LOG_ERR("ADCS response too short (%u bytes)", adcs_rx_len);
        return ADCS_RC_ERR;
    }

    if (!adcs_verify_checksum(adcs_rx_buf, adcs_rx_len)) {
        LOG_ERR("ADCS checksum error");
        return ADCS_RC_ERR;
    }

    return ADCS_RC_OK;
}

adcs_rc_t adcs_get_status(uint32_t *stat)
{
    __ASSERT(stat, "Status pointer cannot be NULL.");

    /* Read STAT register (0x80), 1 data count (4 bytes), user map (0x00) */
    uint8_t cmd[] = {0xC9, 0x80, 0x01, 0x00, 0x56};

    adcs_rc_t rc = adcs_read(cmd, sizeof(cmd), 4U);
    if (rc != ADCS_RC_OK) {
        return rc;
    }

    /* Verify header echo matches command header */
    for (uint8_t i = 0; i < 4; ++i) {
        if (adcs_rx_buf[i] != cmd[i]) {
            LOG_ERR("ADCS status response header mismatch at byte %u", i);
            return ADCS_RC_ERR;
        }
    }

    /* Parse 4 byte STAT value (little-endian) starting at byte 4 */
    *stat = (uint32_t)adcs_rx_buf[4]
          | (uint32_t)adcs_rx_buf[5] << 8
          | (uint32_t)adcs_rx_buf[6] << 16
          | (uint32_t)adcs_rx_buf[7] << 24;

    return ADCS_RC_OK;
}
