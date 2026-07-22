cmake_minimum_required(VERSION 3.16)
project(qianxin_cpp_ep6_examples CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(01_range_basics 01_range_basics.cpp)
add_executable(02_range_adaptors 02_range_adaptors.cpp)
add_executable(03_view_laziness 03_view_laziness.cpp)
add_executable(04_range_factories 04_range_factories.cpp)
add_executable(05_projections 05_projections.cpp)
add_executable(06_comprehensive 06_comprehensive.cpp)
add_executable(07_performance 07_performance.cpp)

add_custom_target(run_all
    COMMAND 01_range_basics
    COMMAND 02_range_adaptors
    COMMAND 03_view_laziness
    COMMAND 04_range_factories
    COMMAND 05_projections
    COMMAND 06_comprehensive
    COMMAND 07_performance
    COMMENT "Running all examples..."
)
