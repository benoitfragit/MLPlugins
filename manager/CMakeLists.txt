cmake_minimum_required(VERSION 2.8.9)
project(BrainManager)

find_package(GTK3 REQUIRED)
find_package(GModule REQUIRED)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

configure_file ("${PROJECT_SOURCE_DIR}/include/brain_manager_config.h.in"
                "${PROJECT_BINARY_DIR}/include/brain_manager_config.h")

set(LIBBRAINMANAGER_INCLUDE_DIR
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK3_INCLUDE_DIRS}
    ${GModule_INCLUDE_DIRS})

set(LIBBRAINMANAGER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${LIBBRAINMANAGER_SOURCE_DIR}/*.c)

add_executable(BrainManager ${SOURCES})

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainManager BrainCore ${GTK3_LIBRARIES} ${GModule_LIBRARIES})
target_include_directories(BrainManager PUBLIC ${LIBBRAINMANAGER_INCLUDE_DIR})

install(TARGETS BrainManager
        RUNTIME DESTINATION bin
        COMPONENT all)

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/icons/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/icons
        FILES_MATCHING PATTERN "*.png"  PATTERN "*.png")

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/schemas/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/schemas
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xsd")
