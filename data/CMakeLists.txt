cmake_minimum_required(VERSION 2.8.9)

# install all xsd files
install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/schemas/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/schemas
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xsd")
