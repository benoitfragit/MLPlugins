cmake_minimum_required(VERSION 2.8.9)
project(BrainView)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

include_directories(${GTK3_INCLUDE_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

set(LIBBRAINCTRL_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${LIBBRAINCTRL_SOURCE_DIR}/*.c)

add_executable(BrainView ${SOURCES})

link_directories(${CMAKE_INSTALL_PREFIX}/lib
                 ${GTK3_LIBRARY_DIRS})
target_link_libraries(BrainView BrainCore BrainCtrl ${GTK3_LIBRARIES})

install(TARGETS BrainView
        RUNTIME DESTINATION bin
        COMPONENT all)
