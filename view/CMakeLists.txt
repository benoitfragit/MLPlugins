cmake_minimum_required(VERSION 2.8.9)
project(BrainView)

set(CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

set(LIBBRAINCTRL_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${LIBBRAINCTRL_SOURCE_DIR}/*.c)

add_executable(BrainView ${SOURCES})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainView BrainCore BrainCtrl)

install(TARGETS BrainView
        RUNTIME DESTINATION bin
        COMPONENT all)
