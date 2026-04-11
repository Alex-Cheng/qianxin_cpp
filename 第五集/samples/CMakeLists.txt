cmake_minimum_required(VERSION 3.16)
project(qianxin_cpp_ep5_examples CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 01 - 类型萃取基础
add_executable(01_type_traits_basics 01_type_traits_basics.cpp)
target_compile_features(01_type_traits_basics PRIVATE cxx_std_20)

# 02 - 类型变换
add_executable(02_type_transformations 02_type_transformations.cpp)
target_compile_features(02_type_transformations PRIVATE cxx_std_20)

# 03 - 手写类型萃取
add_executable(03_custom_type_trait 03_custom_type_trait.cpp)
target_include_directories(03_custom_type_trait PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_features(03_custom_type_trait PRIVATE cxx_std_20)

# 04 - requires 表达式
add_executable(04_requires_expression 04_requires_expression.cpp)
target_compile_features(04_requires_expression PRIVATE cxx_std_20)

# 05 - 标准库 Concept
add_executable(05_standard_concepts 05_standard_concepts.cpp)
target_compile_features(05_standard_concepts PRIVATE cxx_std_20)

# 06 - 自定义 Concept
add_executable(06_custom_concept 06_custom_concept.cpp)
target_compile_features(06_custom_concept PRIVATE cxx_std_20)

# 07 - 类型萃取 + Concept 组合拳
find_package(TBB QUIET)
add_executable(07_combination 07_combination.cpp)
target_compile_features(07_combination PRIVATE cxx_std_20)
if(TBB_FOUND)
    target_link_libraries(07_combination PRIVATE TBB::tbb)
endif()

# 运行所有示例的便捷 target
add_custom_target(run_all
    COMMAND 01_type_traits_basics
    COMMAND 02_type_transformations
    COMMAND 03_custom_type_trait
    COMMAND 04_requires_expression
    COMMAND 05_standard_concepts
    COMMAND 06_custom_concept
    COMMAND 07_combination
    COMMENT "Running all examples..."
)
