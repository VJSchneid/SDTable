cmake_minimum_required(VERSION 2.8)
project(sdtable)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        SDTable.cpp
        SDTable.h
)

add_library(sdtable ${SOURCE_FILES})
