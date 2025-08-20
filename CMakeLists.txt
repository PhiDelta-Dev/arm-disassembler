cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD d0edc3af-4c50-42ea-a356-e2862fe7a444)

set(CXX_STANDARD 26)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(arm_disassembler LANGUAGES CXX)

# Main project
add_subdirectory(src)

# Tests
project(tests LANGUAGES CXX)

include(CTest)
enable_testing()

add_subdirectory(tests)