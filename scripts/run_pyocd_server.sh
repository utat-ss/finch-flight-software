#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

trap "echo 'Exiting...'; exit 0" SIGINT

if [ $# -ne 0 ] && [ $# -ne 1 ]; then
    echo "Error: must pass 0 or 1 arguments"
    echo "Usage: $0 [TARGET]"
    exit 1
fi

while true; do
    if [ $# -eq 0 ]; then
        pyocd gdbserver --allow-remote --pack "$SCRIPT_DIR"/../packs/stm32g4.pack --pack "$SCRIPT_DIR"/../packs/stm32h7.pack
    elif [ $# -eq 1 ]; then
        pyocd gdbserver --allow-remote --pack "$SCRIPT_DIR"/../packs/stm32g4.pack --pack "$SCRIPT_DIR"/../packs/stm32h7.pack --target $1
    fi

    if [ $? -ne 0 ]; then
        exit 1
    fi

    echo "pyOCD exited, restarting in 2s... (Ctrl+C to stop)"
    sleep 2
done
