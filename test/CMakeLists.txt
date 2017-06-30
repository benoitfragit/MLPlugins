cmake_minimum_required(VERSION 2.8.9)

#Setup CMake to run tests
enable_testing()

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/config_all.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/src/config_all.h")

#I like to keep test files in a separate source directory called test
file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c)

#Run through each source
foreach(testSrc ${TEST_SRCS})
        #Extract the filename without an extension (NAME_WE)
        get_filename_component(testName ${testSrc} NAME_WE)


        include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

        #Add compile target
        add_executable(${testName} ${testSrc})

        #link targets and dependencies
        link_directories(${CMAKE_INSTALL_PREFIX}/lib)
        target_link_libraries(${testName} Brain)
        target_include_directories(${testName} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)

        #I like to move testing binaries into a testBin directory
#        set_target_properties(${testName} PROPERTIES
#            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/testBin)

        #Finally add it to test execution -
        #Notice the WORKING_DIRECTORY and COMMAND
        add_test(NAME ${testName}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testName} )
endforeach(testSrc)

# install all xml files
install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/src/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xml")
