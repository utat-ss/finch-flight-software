/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OBC_FLASH_H_UTIL
#define OBC_FLASH_H_UTIL

#include <zephyr/device.h>
#include <stdint.h>
#include <stddef.h>

/* Flash constants that apps may need */
#define FLASH_TEST_ADDR 0x00200000U

/**
 * Print the JEDEC ID of the flash chip
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @return 0 on success, negative error code on failure
 */
int obc_print_id(const struct device *spi, const struct device *gpio, int cs_pin);

/**
 * Unlock all block protections on the flash chip
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @return 0 on success, negative error code on failure
 */
int obc_flash_global_unlock(const struct device *spi, const struct device *gpio, int cs_pin);

/**
 * This erases the target sector, writes the data, and waits for completion.
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @param addr Address to write to 
 * @param data Pointer to 4 bytes of data to write
 * @return 0 on success, negative error code on failure
 */
int obc_prog(const struct device *spi, const struct device *gpio, int cs_pin, uint32_t addr, const uint8_t *data);

/**
 * Erase a sector of the flash memory at the specified address.
 * 
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @param addr Address of the sector to erase
 * @return 0 on success, negative error code on failure
 */
int obc_erase_sector(const struct device *spi, const struct device *gpio, int cs_pin, uint32_t addr);

/**
 * Read 4 bytes of data from the flash memory at the test address and store it in the provided
 * buffer.
 *
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @param addr Address to read from
 * @param result Pointer to buffer where read data will be stored (4 bytes)
 * @return 0 on success, negative error code on failure
 */
int obc_read(const struct device *spi, const struct device *gpio, int cs_pin, uint32_t addr, uint8_t *result);

/**
 * Reset the flash memory state
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @return 0 on success, negative error code on failure
 */
int obc_reset_state(const struct device *spi, const struct device *gpio, int cs_pin);

/**
 * Erase the entire flash chip
 *
 * @param spi Pointer to SPI device
 * @param gpio Pointer to GPIO device
 * @param cs_pin GPIO pin number used for manual CS control
 * @return 0 on success, negative error code on failure
 */
int obc_erase_all_mem(const struct device *spi, const struct device *gpio, int cs_pin);

#endif
