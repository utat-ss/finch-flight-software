#!/bin/bash
# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

# flashing an ELF directly
flash_elf() {
    local elf=$1
    if [ ! -f "$elf" ]; then
        echo "Error: couldn't find image at $elf"
        exit 1
    fi
    echo "Flashing $elf..."

    # Fill the ELF path into the GDB command template and write to a temp file. The template contains a placeholder string ELF_PATH which sed replaces with the actual path before GDB runs it
    sed "s|ELF_PATH|$elf|g" "$SCRIPT_DIR"/gdb_commands_flash_elf > /tmp/gdb_flash_tmp
    /zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb -q "$elf" --command /tmp/gdb_flash_tmp
    
    # waiting 3 seconds because i found that for some reason trying to flash another image directly after the first one doesn't work
    echo "Waiting for pyOCD to recover..."
    sleep 3
}

# flashing a signed hex file (mcu boot needs the images it deals with to be signed)
flash_hex() {
    local elf=$1
    local bin=$2
    if [ ! -f "$bin" ]; then
        echo "Error: couldn't find image at $bin"
        exit 1
    fi
    echo "Flashing $bin..."

    # Fill the hex path into the GDB command template and write to a temp file.
    # The template contains a placeholder string BIN_PATH which sed replaces
    # with the actual path before GDB runs it.
    # i got this code from claude and it passed the ELF into gdb, but i don't think that's actually necessary (could be wrong tho)
    sed "s|BIN_PATH|$bin|g" "$SCRIPT_DIR"/gdb_commands_flash_bin > /tmp/gdb_flash_tmp
    /zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb -q "$elf" --command /tmp/gdb_flash_tmp
    sleep 3
}

flash_elf /build/mcuboot/zephyr/zephyr.elf
flash_hex /build/blinky1/zephyr/zephyr.elf /build/blinky1/zephyr/zephyr.signed.hex
flash_hex /build/blinky2/zephyr/zephyr.elf /build/blinky2/zephyr/zephyr.signed.slot1.hex