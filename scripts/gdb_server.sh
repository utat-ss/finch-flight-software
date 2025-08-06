#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
G4Pack="$SCRIPT_DIR"/../blobs/Keil.STM32G4xx_DFP.2.1.0.pack
H7Pack="$SCRIPT_DIR"/../blobs/Keil.STM32H7xx_DFP.4.1.2.pack

trap "echo 'Exiting...'; exit 0" SIGINT

if [ $# -ne 0 ] && [ $# -ne 1 ]; then
    echo "Error: must pass 0 or 1 arguments"
    echo "Usage: $0 [TARGET]"
    exit 1
fi

if [ $# -eq 0 ]; then
    echo -e "\033[1;33m  WARNING: running the gdb server without the target argument. \033[0m"
    echo -e "\033[1;33m  This will only work on the Nucleo dev boards. \033[0m"
    echo -e "\033[1;33m  To add the target, run $0 <TARGET> \033[0m"
    echo "Starting in 3s.."
    sleep 3
fi

while true; do
    if [ $# -eq 0 ]; then
        pyocd gdbserver --connect attach --allow-remote --pack ${G4Pack} --pack ${H7Pack}
    elif [ $# -eq 1 ]; then
        pyocd gdbserver --target $1 --connect attach --allow-remote --pack ${G4Pack} --pack ${H7Pack}
    fi

    if [ $? -ne 0 ]; then
        exit 1
    fi

    echo "pyOCD exited, restarting in 2s... (Ctrl+C to stop)"
    sleep 2
done
