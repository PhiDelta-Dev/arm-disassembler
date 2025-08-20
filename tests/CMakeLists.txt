find_package(Catch2)

add_executable(tests)


set(CMAKE_CXX_MODULE_STD 1)
target_compile_features(arm_disassembler PRIVATE cxx_std_23)
target_compile_features(tests PRIVATE cxx_std_23)

if (WIN32)
file(COPY
    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/modules/std.ixx"
    DESTINATION
    ${PROJECT_BINARY_DIR}/std_module/
)

target_sources(tests
    PRIVATE
    FILE_SET CXX_MODULES 
    BASE_DIRS ${PROJECT_BINARY_DIR}/std_module
    FILES
    ${PROJECT_BINARY_DIR}/std_module/std.ixx
)
endif()

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TESTS_DIR "${CMAKE_SOURCE_DIR}/tests")
target_include_directories(tests PRIVATE ${SRC_DIR} ${TESTS_DIR})

target_sources(tests
    PRIVATE
    FILE_SET CXX_MODULES    
    BASE_DIRS ${SRC_DIR}
    FILES
    ${SRC_DIR}/utility/strong_type.cpp
    ${SRC_DIR}/utility/unsigned_integer.cpp
    ${SRC_DIR}/utility/bit_manipulation.cpp
    ${SRC_DIR}/utility/packed_struct.cpp
)
target_sources(tests 
    PRIVATE 
    utility/strong_type.cpp
    utility/unsigned_integer.cpp
    utility/bit_manipulation.cpp
    utility/packed_struct.cpp
)

target_compile_options(tests PRIVATE 
    -Wall 
    -Wextra 
    -Wshadow 
    -Wnon-virtual-dtor 
    -pedantic
    -O3
    -std=c++2c
)


add_test(NAME tests COMMAND tests)