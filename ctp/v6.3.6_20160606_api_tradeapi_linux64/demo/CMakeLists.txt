cmake_minimum_required(VERSION 3.7)
project(ctp_demo)
SET( CMAKE_VERBOSE_MAKEFILE on )
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")
include_directories(../)
include_directories(/usr/local/taf/cpp/include)
link_directories(../)
link_directories(/usr/local/taf/cpp/lib)
set(CTP_LIBS thostmduserapi thosttraderapi)
add_compile_options(-std=c++11) 
add_executable(ctp_demo demo.cpp)
target_link_libraries(ctp_demo ${CTP_LIBS} parse tafutil tafservant pthread dl)

add_executable(ctp_query_demo ctp_query_demo.cpp)
target_link_libraries(ctp_query_demo ${CTP_LIBS} parse tafutil tafservant pthread dl)

add_executable(submarketdata_demo submarketdata_demo.cpp)
target_link_libraries(submarketdata_demo ${CTP_LIBS} parse tafutil tafservant pthread dl)

add_executable(trader_demo testTraderApi.cpp TraderSpi.cpp)
target_link_libraries(trader_demo ${CTP_LIBS} parse tafutil tafservant pthread dl)

