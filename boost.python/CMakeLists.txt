cmake_minimum_required(VERSION 3.7)
project(boost.python_demo)
SET( CMAKE_VERBOSE_MAKEFILE on )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
include_directories(/data/billowqiu/3rdlib/include/)
include_directories(/usr/include/python2.7/)
link_directories(/data/billowqiu/3rdlib/lib/)

add_compile_options(-std=c++11) 
set(HELLO_SO hello_ext)
add_library(${HELLO_SO} SHARED hello.cpp)
target_link_libraries(${HELLO_SO} boost_python pthread dl)
set_target_properties(${HELLO_SO} PROPERTIES PREFIX "")
set_target_properties(${HELLO_SO} PROPERTIES OUTPUT_NAME "hello_ext")

set(extending_so extending)
add_library(${extending_so} SHARED extending.cpp)
target_link_libraries(${extending_so} boost_python pthread dl)
set_target_properties(${extending_so} PROPERTIES PREFIX "")
set_target_properties(${extending_so} PROPERTIES OUTPUT_NAME "extending")


