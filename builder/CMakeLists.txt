cmake_minimum_required(VERSION 2.8.9)
project(BrainBuilder)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

set(LIBBRAINBUILDER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBBRAINBUILDER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${LIBBRAINBUILDER_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${LIBBRAINBUILDER_INCLUDE_DIR}/*.h)

add_executable(BrainBuilder ${SOURCES} ${HEADERS})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainBuilder BrainCore ${CMAKE_DL_LIBS})
target_include_directories(BrainBuilder PUBLIC ${LIBBRAINBUILDER_INCLUDE_DIR})

install(TARGETS BrainBuilder
        RUNTIME DESTINATION bin
        COMPONENT all)
