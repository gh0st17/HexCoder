cmake_minimum_required(VERSION 3.27)

project(hash)
set(SOURCE_LIB md5.cpp SHA256.cpp sha512.cpp)
add_library(hash STATIC ${SOURCE_LIB})