cmake_minimum_required(VERSION 2.8)
project(sdtable)
project(sdtable_test)

add_subdirectory(example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        SDTable.cpp
        SDTable.h
)

add_library(sdtable ${SOURCE_FILES})

add_executable(sdtable_test ${TEST_SOURCE_FILES})
target_link_libraries(sdtable_test sdtable)