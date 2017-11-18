cmake_minimum_required(VERSION 2.8.9)
project(BrainCtrl)

set(CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

configure_file ("${PROJECT_SOURCE_DIR}/include/brain_plugin_config.h.in"
                "${PROJECT_BINARY_DIR}/include/brain_plugin_config.h")

set(LIBBRAINCTRL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIBBRAINCTRL_INCLUDE_DIR
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES ${LIBBRAINCTRL_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

#add_executable(BrainCtrl ${SOURCES} ${HEADERS})
add_library(BrainCtrl SHARED ${SOURCES} ${HEADERS})

#link_directories(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(BrainCtrl BrainCore ${CMAKE_DL_LIBS})
target_include_directories(BrainCtrl PUBLIC ${LIBBRAINCTRL_INCLUDE_DIR})

install(TARGETS BrainCtrl
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)

#install(TARGETS BrainCtrl
#        RUNTIME DESTINATION bin
#        COMPONENT all)

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/schemas/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/schemas
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xsd")
