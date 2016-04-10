cmake_minimum_required(VERSION 2.8.9)
add_subdirectory(core)
add_subdirectory(test)

# add a tar archive
set(DIST_DIR "Brain")
add_custom_target(dist
                  COMMAND rm -rf ${DIST_DIR}
                  COMMAND mkdir  ${DIST_DIR}
                  COMMAND cp -r ${CMAKE_SOURCE_DIR}/* ${DIST_DIR} || true
                  COMMAND rm -rf ${DIST_DIR}/build
                  COMMAND mkdir ${DIST_DIR}/build
                  COMMAND tar --exclude="*~" --exclude="._*" -zcvf ${DIST_DIR}.tar.gz ${DIST_DIR}
                  COMMAND rm -rf  ${DIST_DIR}
                          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
