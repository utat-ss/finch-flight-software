/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <finch/obc_littlefs/lfs_callbacks.h>
#include <finch/obc_flash/util.h>
#include <zephyr/device.h>

#include <stdint.h>
#include <string.h>

#define SPI_NODE     DT_NODELABEL(spi1)
#define CS_GPIO_NODE DT_NODELABEL(gpioa)
#define CS_PIN       4

#define FLASH_READ_SIZE 4U

int lfs_cb_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    const struct lfs_flash_context *ctx = c->context;
    const struct device *spi = ctx->spi;
    const struct device *gpio = ctx->gpio;
    const int cs_pin = ctx->cs_pin;

    // destination buffer pointer for reading data into
    uint8_t *dest_buf_ptr = (uint8_t *)buffer;

    // using 64 bit to avoid overflow when calculating the address
    uint64_t address = (uint64_t)block * c->block_size + off;

    // repeatedly reading chunks of FLASH_READ_SIZE bytes until the requested size is read
    while (size > 0U) {
        // temporary buffer for this chunk
        uint8_t read_buffer[FLASH_READ_SIZE];
        // determining the size of next chunk to read
        lfs_size_t chunk_size = size < FLASH_READ_SIZE ? size : FLASH_READ_SIZE;

        // ensuring the address does not exceed the maximum value of a 32-bit unsigned integer
        if (address > UINT32_MAX || obc_read(spi, gpio, cs_pin, (uint32_t)address, read_buffer) != 0) {
            return LFS_ERR_IO;
        }

        memcpy(dest_buf_ptr, read_buffer, chunk_size);
        // advancing buffer ptr to position for next chunk
        dest_buf_ptr += chunk_size;
        // target address in memory to read from
        address += chunk_size;
        // remaining bytes to read
        size -= chunk_size;
    }

    return LFS_ERR_OK;
}

int lfs_cb_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    
    const struct lfs_flash_context *ctx = c->context;
    const struct device *spi = ctx->spi;
    const struct device *gpio = ctx->gpio;
    const int cs_pin = ctx->cs_pin;


    const uint8_t *src_buf_ptr = (const uint8_t *)buffer;

    // using 64 bit to avoid overflow when calculating the address
    uint64_t address = (uint64_t)block * c->block_size + off;

    // repeatedly programming chunks of FLASH_READ_SIZE bytes until the requested size is written
    while (size > 0U) {
        // determining the size of next chunk to write
        lfs_size_t chunk_size = size < FLASH_READ_SIZE ? size : FLASH_READ_SIZE;

        // ensuring the address does not exceed the maximum value of a 32-bit unsigned integer
        if (address > UINT32_MAX || obc_prog(spi, gpio, cs_pin, (uint32_t)address, src_buf_ptr) != 0) {
            return LFS_ERR_IO;
        }

        // advancing src buffer ptr to next chunk to write
        src_buf_ptr += chunk_size;
        // target address to write to in memory
        address += chunk_size;
        // remaining bytes to write
        size -= chunk_size;
    }

    return LFS_ERR_OK;
}

int lfs_cb_erase(const struct lfs_config *c, lfs_block_t block) {
    const struct lfs_flash_context *ctx = c->context;
    const struct device *spi = ctx->spi;
    const struct device *gpio = ctx->gpio;
    const int cs_pin = ctx->cs_pin;

    // using 64 bit to avoid overflow when calculating the address
    uint64_t address = (uint64_t)block * c->block_size;

    // ensuring the address does not exceed the maximum value of a 32-bit unsigned integer
    if (address > UINT32_MAX || obc_erase_sector(spi, gpio, cs_pin, (uint32_t)address) != 0) {
        return LFS_ERR_IO;
    } 

    return LFS_ERR_OK;
}

// always returns success because flash driver handles sync internally
int lfs_cb_sync(const struct lfs_config *c) {
    ARG_UNUSED(c);
    return LFS_ERR_OK;
}
