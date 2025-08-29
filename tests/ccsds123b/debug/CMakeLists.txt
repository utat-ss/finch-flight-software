cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test-ccsds123b)

file(GLOB TEST_SOURCES src/*.c)
target_sources(app PRIVATE ${TEST_SOURCES})
