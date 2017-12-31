cmake_minimum_required(VERSION 2.8.9)
project(BrainTrainer)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

set(BRAINTRAINER_INCLUDE_DIR
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(BRAINTRAINER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${BRAINTRAINER_SOURCE_DIR}/*.c)

add_executable(BrainTrainer ${SOURCES})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainTrainer BrainCore ${CMAKE_DL_LIBS})
target_include_directories(BrainTrainer PUBLIC ${BRAINTRAINER_INCLUDE_DIR})

install(TARGETS BrainTrainer
        RUNTIME DESTINATION bin
        COMPONENT all)
