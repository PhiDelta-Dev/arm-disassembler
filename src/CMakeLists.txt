add_executable(arm_disassembler)

set(CMAKE_CXX_MODULE_STD 1)
target_compile_features(arm_disassembler PRIVATE cxx_std_23)

if (WIN32)
file(COPY
    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/modules/std.ixx"
    DESTINATION
    ${PROJECT_BINARY_DIR}/std_module/
)

target_sources(arm_disassembler
    PRIVATE
    FILE_SET CXX_MODULES 
    BASE_DIRS ${PROJECT_BINARY_DIR}/std_module
    FILES
    ${PROJECT_BINARY_DIR}/std_module/std.ixx
)
endif()

target_sources(arm_disassembler
    PRIVATE
    FILE_SET CXX_MODULES
    BASE_DIRS .
    FILES

    utility/strong_type.cpp
    utility/unsigned_integer.cpp
    utility/bit_manipulation.cpp
    utility/packed_struct.cpp

    types.cpp
    shift_operand.cpp
    arm_instruction.cpp
    instruction.cpp
    instruction_formatting.cpp
    
    PRIVATE
    main.cpp
)

target_compile_options(arm_disassembler PRIVATE 
    -Wall 
    -Wextra 
    -Wshadow 
    -Wnon-virtual-dtor 
    -pedantic
    -O3
    -std=c++2c
)
