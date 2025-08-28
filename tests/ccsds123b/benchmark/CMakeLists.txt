cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(benchmark-ccsds123b)

include_directories(../../../include)

file(GLOB SOURCES src/*.c)

set(CCSDS123B_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/ccsds123b/src/constants.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/ccsds123b/src/encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/ccsds123b/src/predictor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/ccsds123b/src/util.c
)

target_sources(app PRIVATE ${SOURCES} ${CCSDS123B_SOURCES})
