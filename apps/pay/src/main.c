/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPI flash diagnostic: tries multiple commands and SPI modes to
 * identify why the flash reads all zeros.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pay);

#define FLASH0_NODE DT_NODELABEL(ext_flash0)
#define FLASH1_NODE DT_NODELABEL(ext_flash1)

#define CMD_RELEASE_POWER_DOWN 0xAB
#define CMD_READ_JEDEC_ID      0x9F
#define CMD_READ_STATUS_REG    0x05

static const struct spi_dt_spec flash_spi[] = {
	SPI_DT_SPEC_GET(FLASH0_NODE, SPI_WORD_SET(8) | SPI_TRANSFER_MSB),
	SPI_DT_SPEC_GET(FLASH1_NODE, SPI_WORD_SET(8) | SPI_TRANSFER_MSB),
};

static void spi_diag(const struct spi_dt_spec *spec, unsigned int idx)
{
	uint8_t tx[4];
	uint8_t rx[4];
	struct spi_buf tx_buf = { .buf = tx, .len = sizeof(tx) };
	struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
	struct spi_buf rx_buf = { .buf = rx, .len = sizeof(rx) };
	struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };
	int ret;

	LOG_INF("=== Flash %u diagnostics ===", idx);

	/* ---- Release from Deep Power-Down (0xAB) ---- */
	tx[0] = CMD_RELEASE_POWER_DOWN;
	tx[1] = 0; tx[2] = 0; tx[3] = 0;
	ret = spi_write_dt(spec, &(struct spi_buf_set){
		.buffers = &(struct spi_buf){ .buf = tx, .len = 1 },
		.count = 1,
	});
	LOG_INF("  Wake (0xAB): ret=%d", ret);
	k_msleep(1);

	/* ---- Read JEDEC ID (0x9F) — Mode 0 ---- */
	memset(rx, 0xAA, sizeof(rx));
	tx[0] = CMD_READ_JEDEC_ID;
	tx[1] = 0; tx[2] = 0; tx[3] = 0;
	ret = spi_transceive_dt(spec, &tx_set, &rx_set);
	LOG_INF("  JEDEC ID  mode0 ret=%d : %02x %02x %02x %02x",
		ret, rx[0], rx[1], rx[2], rx[3]);

	/* ---- Read Status Register (0x05) — Mode 0 ---- */
	memset(rx, 0xAA, sizeof(rx));
	tx[0] = CMD_READ_STATUS_REG;
	tx[1] = 0;
	tx_buf.len = 2;
	rx_buf.len = 2;
	ret = spi_transceive_dt(spec, &tx_set, &rx_set);
	LOG_INF("  Status Reg mode0 ret=%d : %02x %02x",
		ret, rx[0], rx[1]);
	tx_buf.len = 4;
	rx_buf.len = 4;

	/* ---- Read JEDEC ID — Mode 3 (CPOL=1, CPHA=1) ---- */
	{
		struct spi_config cfg3 = spec->config;

		cfg3.operation |= SPI_MODE_CPOL | SPI_MODE_CPHA;

		memset(rx, 0xAA, sizeof(rx));
		tx[0] = CMD_READ_JEDEC_ID;
		tx[1] = 0; tx[2] = 0; tx[3] = 0;
		ret = spi_transceive(spec->bus, &cfg3, &tx_set, &rx_set);
		LOG_INF("  JEDEC ID  mode3 ret=%d : %02x %02x %02x %02x",
			ret, rx[0], rx[1], rx[2], rx[3]);
	}
}

int main(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(flash_spi); i++) {
		if (!spi_is_ready_dt(&flash_spi[i])) {
			LOG_ERR("Flash %u: SPI bus not ready", (unsigned)i);
			continue;
		}
		spi_diag(&flash_spi[i], (unsigned)i);
	}

	LOG_INF("--- done ---");
	LOG_INF("If every rx byte is 0xAA the SPI driver never wrote to the");
	LOG_INF("buffer (peripheral clock or DMA issue).");
	LOG_INF("If every rx byte is 0x00 the bus is clocking but MISO is");
	LOG_INF("stuck low -- check HOLD pin (tie to VCC) and wiring.");
	LOG_INF("If every rx byte is 0xFF MISO is floating -- flash may not");
	LOG_INF("be powered or CS is not reaching the chip.");

	while (1) {
		k_msleep(1000);
	}

	return 0;
}
