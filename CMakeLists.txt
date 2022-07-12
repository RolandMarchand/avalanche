cmake_minimum_required(VERSION 3.22)

project(Avalanche VERSION 0.1
  DESCRIPTION "A programming language."
  LANGUAGES C)

add_library(scanner STATIC
  src/scanner/scanner.c
  src/scanner/source.c
  src/scanner/substring.c
  src/scanner/token_vector.c)
target_include_directories(scanner PUBLIC ./)

add_library(vm STATIC
  src/vm.c
  src/lump.c
  src/constant.c)
target_include_directories(vm PUBLIC ./)

add_library(_debug STATIC
  src/debug/disassembler.c)
target_include_directories(_debug PUBLIC ./)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
add_executable(avalanche src/main.c)
target_link_libraries(avalanche PUBLIC scanner vm _debug)

