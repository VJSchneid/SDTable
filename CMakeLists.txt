cmake_minimum_required(VERSION 3.6)
project(SDTable)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        SDTable.cpp
        SDTable.h
        )

add_library(SDTable ${SOURCE_FILES})