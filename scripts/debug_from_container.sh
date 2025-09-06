#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

ZEPHYR_IMAGE=/build/zephyr/zephyr.elf
SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

if [ ! -f "$ZEPHYR_IMAGE" ]; then
  echo "Error: couldn't find a zephyr image at $ZEPHYR_IMAGE"
  exit 1
fi

/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-gdb -q "$ZEPHYR_IMAGE" --command "$SCRIPT_DIR"/gdb_commands_debug
