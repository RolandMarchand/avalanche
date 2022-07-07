cmake_minimum_required(VERSION 3.22)

project(Avalanche VERSION 0.1
  DESCRIPTION "A programming language."
  LANGUAGES C)

add_library(vm STATIC
  src/constant.c
  src/lump.c)

target_include_directories(vm PUBLIC ./)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(avalanche src/main.c)
target_link_libraries(avalanche PUBLIC vm)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
