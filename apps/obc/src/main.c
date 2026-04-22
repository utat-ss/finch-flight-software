/*
 * Copyright (c) 2026 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <finch/csp/csp.h>

#include <csp/csp.h>
#include <csp/csp_buffer.h>
#include <csp/csp_rtable.h>
#include <csp/csp_types.h>
#include <csp/drivers/can_zephyr.h>
#include <csp/interfaces/csp_if_can.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <errno.h>

LOG_MODULE_REGISTER(obc);

/* CSP port bound to the JEDEC service. */
#define JEDEC_CSP_PORT   10

/* CAN bitrate - must match boards/obc/obc.dts (fdcan1 bitrate). */
#define CAN_BITRATE      100000

/* Flash CS line on GPIOA pin 4 (SPI1 NSS). */
#define FLASH_CS_PIN     4

#define JEDEC_ID_LEN     6

static const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
static const struct device *const spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1));
static const struct device *const gpioa   = DEVICE_DT_GET(DT_NODELABEL(gpioa));

static const struct spi_config spi_cfg = {
	.frequency = 1875000,
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB,
	.slave = 0,
};

/* ------------------------------------------------------------------------
 * JEDEC / SPI read
 * ------------------------------------------------------------------------ */

static int read_jedec_id(uint8_t *out, size_t out_len)
{
	uint8_t cmd = 0x9F;

	struct spi_buf tx_buf = { .buf = &cmd, .len = sizeof(cmd) };
	struct spi_buf rx_buf = { .buf = out,  .len = out_len };
	struct spi_buf_set tx = { .buffers = &tx_buf, .count = 1 };
	struct spi_buf_set rx = { .buffers = &rx_buf, .count = 1 };

	gpio_pin_set(gpioa, FLASH_CS_PIN, 0);

	int ret = spi_write(spi_dev, &spi_cfg, &tx);

	if (ret == 0) {
		ret = spi_read(spi_dev, &spi_cfg, &rx);
	}

	gpio_pin_set(gpioa, FLASH_CS_PIN, 1);

	return ret;
}

/* ------------------------------------------------------------------------
 * CSP service: JEDEC ID read
 *
 * Triggered by any CSP packet arriving on port JEDEC_CSP_PORT whose
 * payload starts with 0x0000. Replies with the 6-byte JEDEC ID over
 * the same CSP connection, which routes back out on CAN.
 * ------------------------------------------------------------------------ */

static int jedec_service_cb(void *user_data, csp_packet_t *packet)
{
	ARG_UNUSED(user_data);

	LOG_INF("CSP req: src=%u sport=%u dport=%u len=%u",
		packet->id.src, packet->id.sport, packet->id.dport,
		(unsigned int)packet->length);

	if (packet->length < 2 ||
	    packet->data[0] != 0x00 || packet->data[1] != 0x00) {
		LOG_WRN("Ignoring CSP request: payload is not 0x0000");
		csp_buffer_free(packet);
		return CSP_ERR_INVAL;
	}

	uint8_t jedec[JEDEC_ID_LEN] = { 0 };
	int ret = read_jedec_id(jedec, sizeof(jedec));

	if (ret < 0) {
		LOG_ERR("JEDEC read failed (%d)", ret);
		csp_buffer_free(packet);
		return CSP_ERR_DRIVER;
	}

	LOG_INF("JEDEC ID: %02X %02X %02X %02X %02X %02X",
		jedec[0], jedec[1], jedec[2], jedec[3], jedec[4], jedec[5]);

	csp_packet_t *reply = csp_buffer_get(sizeof(jedec));

	if (reply == NULL) {
		LOG_ERR("csp_buffer_get failed");
		csp_buffer_free(packet);
		return CSP_ERR_NOMEM;
	}

	memcpy(reply->data, jedec, sizeof(jedec));
	reply->length = sizeof(jedec);

	const uint16_t reply_dst  = packet->id.src;
	const uint8_t  reply_port = packet->id.sport;

	csp_sendto_reply(packet, reply, 0);

	csp_buffer_free(packet);

	LOG_INF("CSP reply sent to addr=%u sport=%u", reply_dst, reply_port);

	return CSP_ERR_NONE;
}

/* ------------------------------------------------------------------------
 * Entry point
 * ------------------------------------------------------------------------ */

int main(void)
{
	int ret;

	ret = finch_csp_init();
	if (ret < 0) {
		LOG_ERR("finch_csp_init failed (%d)", ret);
		return ret;
	}

	if (!device_is_ready(can_dev)) {
		LOG_ERR("CAN device not ready");
		return -ENODEV;
	}
	if (!device_is_ready(spi_dev)) {
		LOG_ERR("SPI device not ready");
		return -ENODEV;
	}
	if (!device_is_ready(gpioa)) {
		LOG_ERR("GPIOA device not ready");
		return -ENODEV;
	}

	/* CS idles high; pulsed low inside read_jedec_id(). */
	ret = gpio_pin_configure(gpioa, FLASH_CS_PIN, GPIO_OUTPUT_HIGH);
	if (ret < 0) {
		LOG_ERR("gpio_pin_configure CS failed (%d)", ret);
		return ret;
	}

	/* Attach libcsp's stock Zephyr CAN driver.  It installs an RX filter
	 * matching our CSP address, spins up an RX thread that feeds frames
	 * into csp_can_rx(), registers the interface, sets the bitrate, and
	 * calls can_start() internally.
	 */
	csp_iface_t *can_iface = NULL;

	ret = csp_can_open_and_add_interface(
		can_dev,
		"CAN",
		CONFIG_FINCH_CSP_NODE_ADDRESS,
		CAN_BITRATE,
		CONFIG_FINCH_CSP_NODE_ADDRESS,
		CFP2_DST_MASK,
		&can_iface);
	if (ret != CSP_ERR_NONE) {
		LOG_ERR("csp_can_open_and_add_interface failed (%d)", ret);
		return -EIO;
	}

	/* Default route: anything not destined for us goes out on CAN. */
	ret = csp_rtable_set(0, 0, can_iface, CSP_NO_VIA_ADDRESS);
	if (ret != CSP_ERR_NONE) {
		LOG_ERR("csp_rtable_set failed (%d)", ret);
		return -EIO;
	}

	ret = csp_bind_callback(jedec_service_cb, JEDEC_CSP_PORT);
	if (ret != CSP_ERR_NONE) {
		LOG_ERR("csp_bind_callback failed (%d)", ret);
		return -EIO;
	}

	LOG_INF("CSP-over-CAN ready (addr=%u, JEDEC port=%u)",
		CONFIG_FINCH_CSP_NODE_ADDRESS, JEDEC_CSP_PORT);

	/* Main thread is idle; the CSP router thread and the CAN RX thread
	 * owned by the libcsp CAN driver do all the work.
	 */
	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
