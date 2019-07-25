cmake_minimum_required(VERSION 2.8.9)

option(BRAIN_ENABLE_DOUBLE_PRECISION "Enable double precision" OFF)
option(BRAIN_ENABLE_LOGGING          "Enable logging"          OFF)
option(BRAIN_ENABLE_TESTING          "Enable testing"          OFF)
option(BRAIN_ENABLE_DOC              "Enable documentation"    OFF)

if (BRAIN_ENABLE_DOUBLE_PRECISION)
    message(STATUS "Enable DOUBLE precision")
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION)
endif(BRAIN_ENABLE_DOUBLE_PRECISION)

if (BRAIN_ENABLE_LOGGING)
    message(STATUS "Enable logging")
    add_definitions(-DBRAIN_ENABLE_LOGGING)
endif(BRAIN_ENABLE_LOGGING)

add_compile_options(-O2
                    -g3
                    -Wall
                    -Wextra
                    -Wshadow
                    -Wunused
                    -lm)

add_subdirectory(core)
add_subdirectory(MLP)
