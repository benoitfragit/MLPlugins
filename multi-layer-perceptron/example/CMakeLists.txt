cmake_minimum_required(VERSION 2.8.9)
project(Example)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/test_train_data.xml.in"
                "${CMAKE_INSTALL_PREFIX}/example/test_train_data.xml")

file(GLOB_RECURSE XML ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)
file(GLOB_RECURSE CSV ${CMAKE_CURRENT_SOURCE_DIR}/*.csv)

install(FILES ${XML}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/example
        COMPONENT example_xml)

install(FILES ${CSV}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/example
        COMPONENT example_csv)
