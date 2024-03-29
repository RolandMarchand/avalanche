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
  src/vm/vm.c
  src/vm/value.c
  src/vm/lump.c
  src/vm/constant_vector.c
  src/vm/debug/disassembler.c)
target_include_directories(vm PUBLIC ./)

add_library(compiler STATIC
  src/compiler/compiler.c
  src/compiler/parser.c
  src/compiler/error.c)
target_include_directories(compiler PUBLIC ./)
target_link_libraries(compiler PRIVATE scanner vm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -DDEBUG_TRACE_EXECUTION")

add_executable(avalanche src/main.c)
target_link_libraries(avalanche PUBLIC scanner vm compiler)

