cmake_minimum_required(VERSION 2.8.9)
project(MLPExample)
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/examples_config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/src/examples_config.h")

file(GLOB EXAMPLE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

if (LIBBRAIN_DOUBLE_PRECISION)
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION=1)
endif(LIBBRAIN_DOUBLE_PRECISION)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR}/include)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
add_executable(MLPExample ${EXAMPLE_SRCS})
target_link_libraries(MLPExample BrainMLP BrainCore -lm)
target_include_directories(MLPExample PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)

install(TARGETS MLPExample
        RUNTIME DESTINATION bin
        COMPONENT all)
