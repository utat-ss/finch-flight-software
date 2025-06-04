/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/devicetree.h>

#define FLASH_NODE DT_NODELABEL(w25q64)
const struct device *flash_dev = DEVICE_DT_GET(FLASH_NODE); // get pointer to flash device from dts

LOG_MODULE_REGISTER(eps);


// TODO init flashDev based on a .overlay or board folder (somewhere)
// no label in stm32f303 --> cant find 302 

void read_id(const struct spi_dt_spec *spi_dev)
{
	uint8_t transfer_buf[] = { 0x9F };  // command byte to request JEDEC id
	uint8_t receive_buf[3] = { 0 };     // 3 bytes received : manufacturer id, memory type, capacity

	const struct spi_buf transfer_bufs_wrapped[] = {
		{
			.buf = transfer_buf,
			.len = sizeof(transfer_buf)
		}
	};

	const struct spi_buf receive_bufs_wrapped[] = {
		{
			.buf = receive_buf,
			.len = sizeof(receive_buf)
		}
	};

	const struct spi_buf_set transfer_bufs_wrapped_multiple = {
		.buffers = transfer_bufs_wrapped,
		.count = ARRAY_SIZE(transfer_bufs_wrapped)
	};

	const struct spi_buf_set receive_bufs_wrapped_multiple = {
		.buffers = receive_bufs_wrapped,
		.count = ARRAY_SIZE(receive_bufs_wrapped)
	};

	int returnCode = spi_transceive_dt(spi_dev, &transfer_bufs_wrapped_multiple, &receive_bufs_wrapped_multiple);
	if (returnCode < 0) {
		printk("SPI transceive failed, code: %d\n", returnCode);
		return;
	}

	printk("Flash ID (manufac id, mem type, capacity): %02x %02x %02x\n", receive_buf[0], receive_buf[1], receive_buf[2]);
}

void test_spi_write(const struct spi_dt_spec *spi_dev)
{
	const char transfer_string[] = "Hello SPI";

	const struct spi_buf transfer_buf = {
		.buf = (void *)transfer_string,  // pointer to string, void pointer instead of char as req. by .buf
		.len = sizeof(transfer_string) - 1  // size of string data, not including \0
	};

	const struct spi_buf_set transfer_buf_multiple = {
		.buffers = &transfer_buf,
		.count = 1
	};

	int returningCode = spi_write(spi_dev->bus, &spi_dev->config, &transfer_buf_multiple);
	if (returningCode < 0) {
		printk("SPI write failed: %d\n", returningCode);
	} else {
		printk("Sent string over SPI: %s\n", transfer_string);
	}
}

void test_spi_write_dt(const struct spi_dt_spec *spi_dev)
{
	const char transfer_string[] = "Hello SPI";

	const struct spi_buf transfer_buf = {
		.buf = (void *)transfer_string,  // pointer to string, void pointer instead of char as req. by .buf
		.len = sizeof(transfer_string) - 1  // size of string data, not including \0
	};

	const struct spi_buf_set transfer_buf_multiple = {
		.buffers = &transfer_buf,
		.count = 1
	};

	int returningCode = spi_write_dt(spi_dev, &transfer_buf_multiple);
	if (returningCode < 0) {
		printk("SPI write failed: %d\n", returningCode);
	} else {
		printk("Sent string over SPI: %s\n", transfer_string);
	}
}

void test_spi_read_func(const struct spi_dt_spec *spi_dev)
{
	char receive_string[10] = {0};  // initialize with zeros

	const struct spi_buf receive_buf = {
		.buf = receive_string,
		.len = sizeof(receive_string)  // number of bytes to read
	};

	const struct spi_buf_set receive_buf_multiple = {
		.buffers = &receive_buf,
		.count = 1
	};

	int returningCode = spi_read(spi_dev->bus, &spi_dev->config, &receive_buf_multiple);
	if (returningCode < 0) {
		printk("SPI read failed: %d\n", returningCode);
	} else {
		printk("Received string: %.*s\n", (int)sizeof(receive_string), receive_string);
	}
}

void test_spi_read_transceive(const struct spi_dt_spec *spi_dev)
{
	uint8_t command_buf[] = { 0x03 };
	uint8_t receive_buf[10] = {0}; // buf to store response (string)
	
	const struct spi_buf receive_bufs_wrapped[] = {
		{
			.buf = NULL,						// when transmitting command, receive "dummy" bytes
			.len = sizeof(command_buf)
		},
		{
			.buf = (void *)receive_buf,				// when reading, receive resopnse (string)
			.len = sizeof(receive_buf)
		}
	};

	uint8_t dummy_transfer[10] = {0};  // "dummy" transmit to keep spi clock running for data coming in
	const struct spi_buf transfer_bufs_wrapped[] = {
		{
			.buf = (void *)command_buf,
			.len = sizeof(command_buf)
		},
		{
			.buf = (void *)dummy_transfer,
			.len = sizeof(dummy_transfer)
		}
	};
	
	const struct spi_buf_set transfer_bufs_multiple = {  
		.buffers = transfer_bufs_wrapped,
		.count = 2
	};
	const struct spi_buf_set receive_bufs_multiple = {
		.buffers = receive_bufs_wrapped,
		.count = 2
	};
	
	int returningCode = spi_transceive(spi_dev->bus, &spi_dev->config, &transfer_bufs_multiple, &receive_bufs_multiple);
	if (returningCode < 0) {
		printk("SPI transceive failed: %d\n", returningCode);
		return;
	}
	
	printk("Received SPI string: %.*s\n", (int)sizeof(receive_buf), receive_buf);
}

int main(void)
{	
	static const struct spi_dt_spec spi_info = SPI_DT_SPEC_GET(FLASH_NODE, SPI_WORD_SET(8), 0); // get how to speak to flash device

	if (!device_is_ready(spi_info.bus)) {
		printk("SPI device not ready.\n");
		return 0;
	}

	read_id(&spi_info);

	printk("waiting");
	k_msleep(2000);
	printk("done waiting");

	test_spi_write(&spi_info);

	printk("waiting");
	k_msleep(2000);
	printk("done waiting");

	test_spi_read_func(&spi_info);

	printk("waiting");
	k_msleep(2000);
	printk("done waiting");

	test_spi_read_transceive(&spi_info);
}
