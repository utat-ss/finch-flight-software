# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

board_runner_args(pyocd "--target=stm32h743zitx")
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
