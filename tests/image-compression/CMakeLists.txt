cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test-image-compression)

include_directories(../../libs/image-compression/include)

file(GLOB SOURCES src/*.c)
file(GLOB LIB ../../libs/image-compression/src/*.c)
target_sources(app PRIVATE ${SOURCES} ${LIB})
