cmake_minimum_required(VERSION 2.8.9)
project (test_brain)

include_directories(${Brain_SOURCE_DIR})

add_executable(test_brain main.c)
target_link_libraries (test_brain LINK_PUBLIC Brain)

