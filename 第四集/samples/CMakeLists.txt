cmake_minimum_required(VERSION 3.14)
project(Episode4_CompileTimeComputation)

# 设置输出目录到 build 文件夹
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 为支持 C++20 特性的示例单独设置
set(CMAKE_CXX_STANDARD_20 20)

# 示例 1：模板元编程 vs constexpr
add_executable(01_template_vs_constexpr 
    01_template_vs_constexpr.cpp)

# 示例 2：if constexpr（需要 C++17）
add_executable(02_if_constexpr 
    02_if_constexpr.cpp)

# 示例 3：编译期查找表
add_executable(03_compile_time_lookup_table 
    03_compile_time_lookup_table.cpp)

# 示例 4：SFINAE 示例
add_executable(04_sfinae_example 
    04_sfinae_example.cpp)

# 示例 5：constexpr vs consteval
add_executable(05_constexpr_vs_consteval 
    05_constexpr_vs_consteval.cpp)

# 示例 6：编译期字符串格式化（C++23）
add_executable(06_compile_time_string_format
    06_compile_time_string_format.cpp)

# 如果编译器支持 C++20，为示例 5 使用 C++20
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "10.0")
        set_target_properties(05_constexpr_vs_consteval PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON)
    endif()
endif()

# 启用优化以查看编译期计算的效果
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O2)
    endif()
endif()

# 打印信息
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
