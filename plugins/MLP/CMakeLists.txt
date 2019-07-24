cmake_minimum_required(VERSION 2.8.9)
project(BrainPluginMLP)

find_package(GTK3 REQUIRED)

set(BRAINPLUGINMLP_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK3_INCLUDE_DIRS})

set(BRAINPLUGINMLP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${BRAINPLUGINIFACE_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

include_directories(${BRAINPLUGINMLP_SOURCE_DIR})

add_library(BrainPluginMLP SHARED ${SOURCES} ${HEADERS})

target_link_libraries(BrainPluginMLP PUBLIC BrainCore)
target_include_directories(BrainPluginMLP PUBLIC ${BRAINPLUGINIFACE_INCLUDE_DIR})


install(TARGETS BrainPluginMLP
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)
