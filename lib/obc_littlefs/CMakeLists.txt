# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

if(CONFIG_FINCH_OBC_LITTLEFS)
    set(LITTLEFS_DIR "${ZEPHYR_BASE}/../modules/lib/littlefs")

    zephyr_include_directories(
        ${LITTLEFS_DIR}
    )

    zephyr_library_sources(
        ${LITTLEFS_DIR}/lfs.c
        ${LITTLEFS_DIR}/lfs_util.c
        src/lfs_callbacks.c
    )
endif()
