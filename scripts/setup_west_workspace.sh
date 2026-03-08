#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

set -e

source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../finch-firmware-env.sh"
source "$(dirname "${FINCH_FIRMWARE_ROOT}")/.venv/bin/activate"
cd "${FINCH_FIRMWARE_ROOT}"
west init --local --mf west.yml && west update
west zephyr-export
west packages pip --install
west sdk install --install-dir $(dirname "${FINCH_FIRMWARE_ROOT}")/zephyr-sdk --toolchains arm-zephyr-eabi
