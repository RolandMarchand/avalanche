cmake_minimum_required(VERSION 3.22)

project(Avalanche VERSION 0.1
  DESCRIPTION "A programming language."
  LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(avalanche src/main.c)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
