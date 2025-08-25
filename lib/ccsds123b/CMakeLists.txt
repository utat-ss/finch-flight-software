# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(
  CONFIG_FINCH_CCSDS123B
  src/arrays.c
  src/constants.c
  src/encoder.c
  src/image.c
  src/predictor.c
  src/util.c
)
