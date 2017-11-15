cmake_minimum_required(VERSION 3.7)
project(boost.python_demo)
SET( CMAKE_VERBOSE_MAKEFILE on )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

add_compile_options(-std=c++11) 
add_executable(snowflake snowflake.cpp)

