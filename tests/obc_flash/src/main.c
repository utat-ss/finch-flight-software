/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <finch/obc_flash/util.h>
#include <zephyr/ztest.h>

/* SPI controller */
#define SPI_NODE DT_NODELABEL(spi1)
/* Manual CS GPIO. Controlling CS pin manually. */
#define CS_GPIO_NODE DT_NODELABEL(gpioa)
#define CS_PIN       4

static const struct device *spi;
static const struct device *gpio;

static void *obc_flash_setup(void)
{
	spi = DEVICE_DT_GET(SPI_NODE);
	gpio = DEVICE_DT_GET(CS_GPIO_NODE);

	zassert_true(device_is_ready(spi), "SPI device is not ready");
	zassert_true(device_is_ready(gpio), "GPIO device is not ready");

	int ret = gpio_pin_configure(gpio, CS_PIN, GPIO_OUTPUT_HIGH);
	zassert_equal(ret, 0, "Failed to configure CS pin");

    ret = obc_flash_global_unlock(spi, gpio, CS_PIN);
    zassert_equal(ret, 0, "Failed to unlock flash");

	return NULL;
}

ZTEST_SUITE(obc_flash, NULL, obc_flash_setup, NULL, NULL, NULL);

ZTEST(obc_flash, test_print_jedec_id)
{
    int ret = obc_print_id(spi, gpio, CS_PIN);
    zassert_equal(ret, 0, "SPI JEDEC test failed");
}

ZTEST(obc_flash, test_write_and_read)
{
	const uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};

	/* Erase the sector before programming it. */
	int ret = obc_erase_sector(spi, gpio, CS_PIN, FLASH_TEST_ADDR);
	zassert_ok(ret, "Sector erase failed (ret=%d)", ret);

	/* Program four bytes without erasing the sector again. */
	ret = obc_prog(spi, gpio, CS_PIN, FLASH_TEST_ADDR, data);
	zassert_ok(ret, "Program failed (ret=%d)", ret);

	/* Read the programmed bytes back from the same address. */
	uint8_t read_data[4] = {0};
	ret = obc_read(spi, gpio, CS_PIN, FLASH_TEST_ADDR, read_data);
	zassert_ok(ret, "Read failed (ret=%d)", ret);

	zassert_mem_equal(read_data, data, sizeof(data), "Read data does not match written data");
}
