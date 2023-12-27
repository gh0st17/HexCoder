cmake_minimum_required(VERSION 3.27)

project(hexcoder)
set(SOURCE_LIB hexcoder.cpp instructions.cpp)
add_library(hexcoder STATIC ${SOURCE_LIB})