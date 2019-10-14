cmake_minimum_required(VERSION 3.15)
project(RegExp)

set(CMAKE_CXX_STANDARD 17)

add_library(RegExp SHARED library.cpp library.h)