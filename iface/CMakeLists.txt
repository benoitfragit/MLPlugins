cmake_minimum_required(VERSION 2.8.9)
project(BrainPluginIface)

add_compile_options(-o2
                    -g3
                    -Wextra
                    -Wshadow)

find_package(GTK3 REQUIRED)

if (LIBBRAIN_DOUBLE_PRECISION)
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION=1)
endif(LIBBRAIN_DOUBLE_PRECISION)

set(BRAINPLUGINIFACE_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTK3_INCLUDE_DIRS})

set(BRAINPLUGINIFACE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${BRAINPLUGINIFACE_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

include_directories(${BRAINPLUGINIFACE_SOURCE_DIR})

add_library(BrainPluginIface SHARED ${SOURCES} ${HEADERS})

target_link_libraries(BrainPluginIface PUBLIC BrainCore)
target_include_directories(BrainPluginIface PUBLIC ${BRAINPLUGINIFACE_INCLUDE_DIR})


install(TARGETS BrainPluginIface
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)
