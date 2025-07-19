cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ccsds123b)

include_directories(../../include)

file(GLOB CCSDS123B_SOURCES src/*.c)
target_sources(app PRIVATE ${CCSDS123B_SOURCES})
