cmake_minimum_required(VERSION 3.5)
project(boost.python_demo)
IF (APPLE)
    SET(CMAKE_CXX_COMPILER "/usr/local/bin/g++")
ENDIF()

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

add_library(vector_indexing_suite SHARED vector_indexing_suite.cpp)
target_link_libraries(vector_indexing_suite boost_python pthread dl)
set_target_properties(vector_indexing_suite PROPERTIES PREFIX "")
set_target_properties(vector_indexing_suite PROPERTIES OUTPUT_NAME "vector_indexing_suite_ext")

add_library(pytype_function SHARED pytype_function.cpp)
target_link_libraries(pytype_function boost_python pthread dl)
set_target_properties(pytype_function PROPERTIES PREFIX "")
set_target_properties(pytype_function PROPERTIES OUTPUT_NAME "pytype_function_ext")

add_library(datetime_convert SHARED datetime_convert.cpp)
target_link_libraries(datetime_convert boost_python pthread dl)
set_target_properties(datetime_convert PROPERTIES PREFIX "")
set_target_properties(datetime_convert PROPERTIES OUTPUT_NAME "datetime_convert")
