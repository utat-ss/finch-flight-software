#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
pyocd gdbserver --allow-remote --target stm32g431rbtx --pack "$SCRIPT_DIR"/../packs/g4.pack
