cmake_minimum_required(VERSION 2.8.9)
project (test_brain)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LIBXML2)

include_directories(${BrainBuilder_BINARY_DIR}
                    ${BrainBuilder_SOURCE_DIR}
                    ${Brain_BINARY_DIR}
                    ${Brain_SOURCE_DIR}
                    ${LIBXML2_INCLUDE_DIR})

add_executable(test_brain main.c)
target_link_libraries (test_brain LINK_PUBLIC BrainBuilder Brain ${LIBXML2_LIBRARIES})
