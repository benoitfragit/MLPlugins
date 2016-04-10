cmake_minimum_required(VERSION 2.8.9)
project(Brain)
set(CMAKE_BUILD_TYPE debug)

set(SOURCES brain.c
            brain.h
            brain_type.h)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES})

target_link_libraries(Brain "-lm")

# add a .pc file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brain.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/brain.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brain.pc
        DESTINATION lib/pkgconfig/)


