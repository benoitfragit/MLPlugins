cmake_minimum_required(VERSION 2.8.9)
project(Brain)
set(Brain_VERSION_MAJOR 0)
set(Brain_VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow -Wunused")

include_directories(${PROJECT_BINARY_DIR})

set(SOURCES brain_settings.c
            brain_network_builder.c
            brain_layer_builder.c
            brain_neuron_builder.c
            brain_activation.c
            brain_costfunction.c
            brain_learning.c
            brain_data_reader.c)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES})

target_link_libraries(Brain "-lm")

# add a .pc file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brain.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/brain.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brain.pc
        DESTINATION lib/pkgconfig/)
