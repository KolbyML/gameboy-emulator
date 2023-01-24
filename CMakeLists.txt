cmake_minimum_required(VERSION 3.16)
project(gameboy-emulator)

set(CMAKE_CXX_STANDARD 20)


if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs")
else()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs/cmake-modules")
endif ()
find_package(SDL2 REQUIRED)

set(SOURCES
        src/main.cpp
        src/utils.cpp
        src/cpu.cpp
        src/memory_bus.cpp
        src/ppu.cpp)

add_executable(gameboy-emulator ${SOURCES})
target_link_libraries(gameboy-emulator PRIVATE "${SDL2_LIBRARY}")
target_include_directories(gameboy-emulator PRIVATE "${SDL2_INCLUDE_DIR}")