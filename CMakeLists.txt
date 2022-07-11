cmake_minimum_required(VERSION 3.22)

project(Avalanche VERSION 0.1
  DESCRIPTION "A programming language."
  LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD_REQUIRED NO)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++03 -g -Wno-sign-compare")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(vm STATIC
  src/vm.cpp
  src/lump.cpp
  src/constant.cpp)
target_include_directories(vm PUBLIC ./)

add_library(_debug STATIC
  src/debug/disassembler.cpp)
target_include_directories(_debug PUBLIC ./)

add_executable(avalanche src/main.cpp)
target_link_libraries(avalanche PUBLIC vm _debug)



