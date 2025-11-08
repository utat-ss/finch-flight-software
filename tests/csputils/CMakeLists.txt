cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test-csputils)

# Test source files
file(GLOB SOURCES src/*.c)

# Include util.c from the library
set(CSPUTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/csputils/src/util.c
)

# Include all libcsp source files
file(GLOB_RECURSE LIBCSP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/csputils/modules/libcsp/src/*.c
)

# Add all sources to the app
target_sources(app PRIVATE ${SOURCES} ${CSPUTILS_SOURCES} ${LIBCSP_SOURCES})

# Include paths so compiler can find headers
zephyr_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/csputils/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/csputils/modules/libcsp/include
)