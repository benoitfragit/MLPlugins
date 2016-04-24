cmake_minimum_required(VERSION 2.8.9)
project(Brain)

option(ENABLE_GLIB_EXTRA "Enable logging" ON)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall")

find_package(LIBXML2)

set(EXTRA_LIBS "")
if (ENABLE_GLIB_EXTRA)
    find_package(glib-2.0)
    include_directories(${GLIB_INCLUDE_DIR})
    include_directories(${GLIB_CONFIG_INCLUDE_DIR})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${GLIB_LIBRARY})
endif(ENABLE_GLIB_EXTRA)

configure_file (
    "${PROJECT_SOURCE_DIR}/brain_config.h.in"
    "${PROJECT_BINARY_DIR}/brain_config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

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
            brain_data_reader.c
            brain_data_reader.h
            brain_xml_utils.c
            brain_xml_utils.h
            brain_logging_utils.c
            brain_logging_utils.h)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES})

include_directories(${LIBXML2_INCLUDE_DIR})

target_link_libraries(Brain ${LIBXML2_LIBRARIES} ${EXTRA_LIBS} "-lm")

# add a .pc file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brain.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/brain.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brain.pc
        DESTINATION lib/pkgconfig/)
