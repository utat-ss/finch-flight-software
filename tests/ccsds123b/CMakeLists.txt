cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test-ccsds123b)

include_directories(../../include)

file(GLOB TEST_SOURCES src/*.c)
file(GLOB CCSDS123B_SOURCES ../../lib/ccsds123b/src/*.c)
target_sources(app PRIVATE ${TEST_SOURCES} ${CCSDS123B_SOURCES})
