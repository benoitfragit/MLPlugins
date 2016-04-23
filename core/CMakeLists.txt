cmake_minimum_required(VERSION 2.8.9)
project(Brain)
set(CMAKE_BUILD_TYPE debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall")

find_package(LIBXML2)

set(SOURCES brain.c
            brain.h
            brain_network_builder.c
            brain_network_builder.h
            brain_layer_builder.c
            brain_layer_builder.h
            brain_neuron_builder.c
            brain_neuron_builder.h
            brain_synapse_builder.c
            brain_synapse_builder.h
            brain_xml_utils.c
            brain_xml_utils.h)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES})

include_directories(${LIBXML2_INCLUDE_DIR})

target_link_libraries(Brain ${LIBXML2_LIBRARIES} "-lm")




# add a .pc file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brain.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/brain.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brain.pc
        DESTINATION lib/pkgconfig/)
