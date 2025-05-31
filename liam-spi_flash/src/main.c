/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/spi.h>

LOG_MODULE_REGISTER(eps);

// TODO init flashDev based on a .overlay or board folder (somewhere)
// no label in stm32f303 --> cant find 302 
const struct device *const flashDev = DEVICE_DT_GET();

void test_writing_word(const struct device *flashDev)
{
	// reference: spi to bitbang device
	// make sure flash mem is clear
	int returningCode;
	size_t erase_block_size = flash_get_erase_block_size(flashDev);
	returningCode = flash_erase(flashDev, 0, erase_block_size);
	if (returningCode < 0) {
		printk("Failed to erase, code: %d\n", returningCode);
		return;
	}

	// writing
	uint8_t buf[16] = "Employment"; // 16 byte buffer to write with
	returningCode = flash_write(flashDev, 0, buf, sizeof(buf));
	if (returningCode < 0) {
		printk("Failed to write, code: %d\n", returningCode);
		return;
	}

	printk("Write success!\n");
}

void test_reading_word(const struct device *flashDev)
{
	int returningCode;
	uint8_t bufForRead[16] = {0}; // will unload into bufForRead
	size_t erase_block_size = flash_get_erase_block_size(flash_dev);
	returningCode = flash_read(flashDev, 0, bufForRead, sizeof(bufForRead));
	if (returningCode < 0) {
		printk("Failed to read, code: %d\n", returningCode);
		return;
	}
	printk("Flash read data: ");
   for (int i = 0; i < sizeof(bufForRead); i++) {
		printk("%02x ", bufForRead[i]);
	}
	printk("\n");
}

// void test_erase(const struct device *flashDev)
// {

// }

int main(void)
{	

	if (!device_is_ready(flashDev)) {
		printk("Flash device not ready.\n");
		return 0;
	}

	test_writing_word(flashDev);

	printk("waiting");
	k_msleep(2000);
	printk("done waiting");

	test_reading_word(flashDev);

	test_writing_word(flashDev);

	printk("waiting");
	k_msleep(2000);
	printk("done waiting");

	test_reading_word(flashDev);
	

	
}
