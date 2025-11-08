# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(
  CONFIG_FINCH_CSPUTILS
  src/main.c
)

# --- Always include all libcsp sources ---
file(GLOB_RECURSE CSPUTILS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/modules/libcsp/src/*.c
)

zephyr_library_sources(${CSPUTILS_SOURCES})

# Include libcsp headers so compiler can find them
zephyr_include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/modules/libcsp/include
)