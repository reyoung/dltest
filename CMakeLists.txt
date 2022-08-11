CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(dlopen_test)

add_library(foo SHARED foo.cpp)
add_library(bar SHARED bar.cpp)

add_executable(main main.cpp 3rd/plthook/plthook_elf.c)
target_include_directories(main PUBLIC 3rd/plthook/)

add_dependencies(main foo bar)