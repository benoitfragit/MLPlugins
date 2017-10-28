cmake_minimum_required(VERSION 2.8.9)

enable_testing()

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/config_all.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/src/config_all.h")

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

foreach(testSrc ${TEST_SRCS})
        get_filename_component(testName ${testSrc} NAME_WE)

        include_directories(${CMAKE_CURRENT_BINARY_DIR}/src
                            ${CMAKE_CURRENT_BINARY_DIR}/include
                            ${LIBBRAINMLP_INCLUDE_DIRS})

        link_directories(${CMAKE_INSTALL_PREFIX}/lib)
        add_executable(${testName} ${testSrc})
        target_link_libraries(${testName} BrainCore BrainMPL)
        target_include_directories(${testName} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)

        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testName} )

        add_custom_command(TARGET ${testName}
                           POST_BUILD
                           COMMAND ${testName}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                           COMMENT "Running ${testName}"
                           VERBATIM)
endforeach(testSrc)
