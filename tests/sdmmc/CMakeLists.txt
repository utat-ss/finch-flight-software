cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test-sdmmc)

# include_directories(../../include)

file(GLOB SOURCES src/*.c)
target_sources(app PRIVATE ${app_sources})
# file(GLOB SDMMC_SOURCES ../../lib/sdmmc/src/*.c)
# target_sources(app PRIVATE ${SOURCES} ${SDMMC_SOURCES})
