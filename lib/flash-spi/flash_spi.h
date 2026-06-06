#ifndef FLASH_SPI_H
#define FLASH_SPI_H

#include <zephyr/device.h>
#include <stdint.h>
#include <stddef.h>

/* * We added cs_pin as a parameter so this library can control 
 * both flash chip 0 and flash chip 1! 
 */
/* Erases one complete sector (usually 64KB) starting at the given address */
int spi_flash_erase_sector(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr);

/* Writes up to 256 bytes to an ALREADY ERASED memory address */
int spi_flash_write(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr, const uint8_t *data, size_t len);

/* Reads data from the flash chip */
int spi_flash_read(const struct device *spi, const struct device *gpio, uint32_t cs_pin, uint32_t addr, uint8_t *result, size_t len);

#endif /* FLASH_SPI_H */