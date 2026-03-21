#!/bin/bash
# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

flash_elf() {
    local elf=$1
    if [ ! -f "$elf" ]; then
        echo "Error: couldn't find image at $elf"
        exit 1
    fi
    echo "Flashing $elf..."
    sed "s|ELF_PATH|$elf|g" "$SCRIPT_DIR"/gdb_commands_flash_elf > /tmp/gdb_flash_tmp
    /zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb -q "$elf" --command /tmp/gdb_flash_tmp
    echo "Waiting for pyOCD to recover..."
    sleep 3
}

flash_bin() {
    local elf=$1
    local bin=$2
    if [ ! -f "$bin" ]; then
        echo "Error: couldn't find image at $bin"
        exit 1
    fi
    echo "Flashing $bin..."
    sed "s|BIN_PATH|$bin|g" "$SCRIPT_DIR"/gdb_commands_flash_bin > /tmp/gdb_flash_tmp
    /zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb -q "$elf" --command /tmp/gdb_flash_tmp
    echo "Waiting for pyOCD to recover..."
    sleep 3
}

flash_elf /build/mcuboot/zephyr/zephyr.elf
flash_bin /build/blinky1/zephyr/zephyr.elf /build/blinky1/zephyr/zephyr.signed.hex
flash_bin /build/blinky2/zephyr/zephyr.elf /build/blinky2/zephyr/zephyr.signed.slot1.hex