cmake_minimum_required(VERSION 2.8.9)
project(Py)

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/mlp.py.in"
                "${CMAKE_INSTALL_PREFIX}/wrapper/py/mlp.py")

file(GLOB_RECURSE PYFILES ${CMAKE_CURRENT_SOURCE_DIR}/*.py)


install(FILES ${PYFILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/wrapper/py/
        COMPONENT pyfiles)
