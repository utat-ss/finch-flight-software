# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
pyocd gdbserver --allow-remote --target stm32g431rbtx --pack "$ScriptDir\..\packs\g4.pack"
