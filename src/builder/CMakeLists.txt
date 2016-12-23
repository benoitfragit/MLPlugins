cmake_minimum_required(VERSION 2.8.9)
project(BrainBuilder)
set(BrainBuilder_VERSION_MAJOR 1)
set(BrainBuilder_VERSION_MINOR 0)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra")

find_package(LIBXML2)

include_directories(${Brain_BINARY_DIR}
                    ${Brain_SOURCE_DIR}
                    ${LIBXML2_INCLUDE_DIR}
                    ${PROJECT_BINARY_DIR})

configure_file (${PROJECT_SOURCE_DIR}/brain_utils_config.h.in
                ${PROJECT_BINARY_DIR}/brain_utils_config.h)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/brainbuilder.pc.in
                ${CMAKE_CURRENT_BINARY_DIR}/brainbuilder.pc @ONLY)

set(SOURCES brain_xml_utils.c
            brain_logging_utils.c
            brain_builder.c)

add_library(BrainBuilder SHARED ${SOURCES})

target_link_libraries(BrainBuilder LINK_PUBLIC Brain ${LIBXML2_LIBRARIES} "-lm")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brainbuilder.pc
        DESTINATION lib/pkgconfig/)