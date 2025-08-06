#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
pyocd gdbserver --allow-remote --target stm32h743zitx --pack "$SCRIPT_DIR"/../packs/h7.pack
