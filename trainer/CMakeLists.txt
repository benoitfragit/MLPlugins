cmake_minimum_required(VERSION 2.8.9)
project(BrainTrainer)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow")

find_package(GTK3 REQUIRED)

configure_file ("${PROJECT_SOURCE_DIR}/include/trainer_config.h.in"
                "${PROJECT_BINARY_DIR}/include/trainer_config.h")

if (LIBBRAIN_DOUBLE_PRECISION)
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION=1)
endif(LIBBRAIN_DOUBLE_PRECISION)

set(BRAINTRAINER_INCLUDE_DIR
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK3_INCLUDE_DIRS})

set(BRAINTRAINER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${BRAINTRAINER_SOURCE_DIR}/*.c)

add_executable(BrainTrainer ${SOURCES})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainTrainer BrainCore ${CMAKE_DL_LIBS} ${GTK3_LIBRARIES})
target_include_directories(BrainTrainer PUBLIC ${BRAINTRAINER_INCLUDE_DIR})

install(TARGETS BrainTrainer
        RUNTIME DESTINATION bin
        COMPONENT all)

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/resources/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/resources
        FILES_MATCHING PATTERN "*.glade"  PATTERN "*.glade")
