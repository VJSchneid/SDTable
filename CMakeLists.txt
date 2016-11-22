cmake_minimum_required(VERSION 3.5)
project(SDTable)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        SDTable.cpp
        SDTable.h
        )

add_executable(sdtable ${SOURCE_FILES})
#add_library(sdtable ${SOURCE_FILES})