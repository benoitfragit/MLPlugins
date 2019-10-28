cmake_minimum_required(VERSION 2.8.9)
project(MLPPlugin)

file(GLOB_RECURSE   EXCHANGE  ${CMAKE_CURRENT_SOURCE_DIR}/exchange/*.py)
file(GLOB_RECURSE   UI        ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.py)
file(GLOB           COMMON    ${CMAKE_CURRENT_SOURCE_DIR}/*.py)

install(FILES ${EXCHANGE}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/MLP/exchange
        COMPONENT mlp_exchange)

install(FILES ${UI}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/MLP/ui
        COMPONENT mlp_ui)

install(FILES ${COMMON}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/MLP
        COMPONENT mlp_common)
