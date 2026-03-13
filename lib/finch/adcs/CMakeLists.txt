# SPDX-License-Identifier: Apache-2.0

zephyr_library_named(adcs)

zephyr_library_include_directories_ifdef(CONFIG_FINCH_ADCS ${ZEPHYR_CURRENT_MODULE_DIR}/include)

zephyr_library_sources_ifdef(
  CONFIG_FINCH_ADCS
  src/adcs.c
)
