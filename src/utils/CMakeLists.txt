cmake_minimum_required(VERSION 2.8.9)
project(BrainUtils)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra")

find_package(LIBXML2)

include_directories("${PROJECT_BINARY_DIR}")

# set the configure file
configure_file (
    ${PROJECT_SOURCE_DIR}/brain_utils_config.h.in
    ${PROJECT_BINARY_DIR}/brain_utils_config.h
)

set(SOURCES brain_data_reader.c
            brain_xml_utils.c
            brain_logging_utils.c)

#Generate the shared library from the sources
add_library(BrainUtils SHARED ${SOURCES})

include_directories(${LIBXML2_INCLUDE_DIR})

target_link_libraries(BrainUtils ${LIBXML2_LIBRARIES} "-lm")
