/*
 * Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <lfs.h>
#include <lfs_util.h>

struct lfs_flash_context {
    const struct device *spi;
    const struct device *gpio;
    int cs_pin;
};

/**
 * Callback function for reading data from the flash memory.
 * 
 * @param c Pointer to the lfs_config structure containing the flash context.
 * @param block The block number to read from.
 * @param off The offset within the block to start reading from.
 * @param buffer Pointer to the buffer where the read data will be stored.
 * @param size The number of bytes to read.
 * @return LFS_ERR_OK on success, or a negative error code on failure.
 */
int lfs_cb_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);

/**
 * Callback function for programming (writing) data to the flash memory.
 *  
 * @param c Pointer to the lfs_config structure containing the flash context.
 * @param block The block number to write to.
 * @param off The offset within the block to start writing to.
 * @param buffer Pointer to the buffer containing the data to be written.
 * @param size The number of bytes to write.
 * @return LFS_ERR_OK on success, or a negative error code on failure.
 */
int lfs_cb_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);

/**
 * Callback function for erasing a block in the flash memory.
 * 
 * @param c Pointer to the lfs_config structure containing the flash context.
 * @param block The block number to erase.
 * @return LFS_ERR_OK on success, or a negative error code on failure.
 */
int lfs_cb_erase(const struct lfs_config *c, lfs_block_t block);

/**
 * Callback function for synchronizing the flash memory.
 * 
 * @param c Pointer to the lfs_config structure containing the flash context.
 * @return LFS_ERR_OK on success, or a negative error code on failure.
 */
int lfs_cb_sync(const struct lfs_config *c);
