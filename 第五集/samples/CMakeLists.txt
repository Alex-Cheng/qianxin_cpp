cmake_minimum_required(VERSION 3.16)
project(qianxin_cpp_ep5_examples CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(01_type_traits_basics 01_type_traits_basics.cpp)
add_executable(02_type_transformations 02_type_transformations.cpp)
add_executable(03_custom_type_trait 03_custom_type_trait.cpp)
add_executable(04_requires_expression 04_requires_expression.cpp)
add_executable(05_standard_concepts 05_standard_concepts.cpp)
add_executable(06_custom_concept 06_custom_concept.cpp)
add_executable(07_combination 07_combination.cpp)

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
