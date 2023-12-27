cmake_minimum_required(VERSION 3.27)
project(hex_coder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fdiagnostics-color=always")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fsanitize=address")
  set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Ofast -flto=thin -march=native -DNDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /std:c++20")
endif()

include_directories(include)
set(SOURCE_EXE src/main.cpp src/manager.cpp src/params.cpp)
add_executable(hex_coder ${SOURCE_EXE})

add_subdirectory(libs/hexcoder)
add_subdirectory(libs/hash)

target_link_libraries(hex_coder hexcoder hash)
