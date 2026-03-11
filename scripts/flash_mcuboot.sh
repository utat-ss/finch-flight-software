#!/bin/bash

set -e

echo "=== Flashing MCUboot Setup ==="

echo "1. Flashing MCUboot bootloader..."
pyocd flash -t stm32g431rbtx build_mcuboot/zephyr/zephyr.hex

echo "2. Flashing fast_blinky to slot 0 (0x08008800)..."
pyocd flash -t stm32g431rbtx --base-address 0x08008800 build_fast/zephyr/zephyr.signed.bin

echo "3. Flashing slow_blinky to slot 1 (0x08014800)..."
pyocd flash -t stm32g431rbtx --base-address 0x08014800 build_slow/zephyr/zephyr.signed.bin

echo "=== Done! Reset board to boot ==="
