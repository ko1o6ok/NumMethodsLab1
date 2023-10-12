cmake_minimum_required(VERSION 3.24)

project(libNM1_lib)

add_library(libNM1_lib SHARED library.cpp library.h)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dll)