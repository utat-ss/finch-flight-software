# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
pyocd gdbserver --allow-remote --target stm32h743zitx --pack "$ScriptDir\..\packs\h7.pack"
