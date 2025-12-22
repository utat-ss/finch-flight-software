cmake_minimum_required(VERSION 3.20.0)

set(MCUBOOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../bootloader/mcuboot/boot/zephyr)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mcuboot_bootloader)

add_subdirectory(${MCUBOOT_DIR} mcuboot)
