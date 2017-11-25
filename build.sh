#!/bin/bash

CMAKE_NB_CORES=2
CMAKE_ENABLE_DOC=0
CMAKE_ENABLE_INSTALL=1
CMAKE_ENABLE_TESTING=1
CMAKE_ENABLE_LOGGING=1

LIBBRAIN_ROOT_DIR=$(pwd)
LIBBRAIN_SOURCE_DIR=$LIBBRAIN_ROOT_DIR
LIBBRAIN_BUILD_TYPE=debug
LIBBRAIN_VERSION_MAJOR=1
LIBBRAIN_VERSION_MINOR=0
LIBBRAIN_VERSION_PATCH=0
LIBBRAIN_BUILD_DIR=$LIBBRAIN_ROOT_DIR/build/$LIBBRAIN_BUILD_TYPE/libbrain
LIBBRAIN_INSTALL_DIR=$LIBBRAIN_ROOT_DIR/install/$LIBBRAIN_BUILD_TYPE/libbrain-$LIBBRAIN_VERSION_MAJOR.$LIBBRAIN_VERSION_MINOR.$LIBBRAIN_VERSION_PATCH
LIBBRAIN_MODULE_PATH=$LIBBRAIN_ROOT_DIR/scripts/cmake/Modules
LIBBRAIN_ARCHIVE_TYPE=DEB

if [ ! -d "$LIBBRAIN_BUILD_DIR" ]; then
    mkdir -p $LIBBRAIN_BUILD_DIR
fi

if [ ! -d "$LIBBRAIN_INSTALL_DIR" ]; then
    mkdir -p $LIBBRAIN_INSTALL_DIR
fi

cd $LIBBRAIN_BUILD_DIR

clear;
clear;

cmake -DCMAKE_BUILD_TYPE=$LIBBRAIN_BUILD_TYPE           \
      -DCMAKE_INSTALL_PREFIX:PATH=$LIBBRAIN_INSTALL_DIR \
      -DCMAKE_MODULE_PATH=$LIBBRAIN_MODULE_PATH         \
      -DCMAKE_ENABLE_TESTING=$CMAKE_ENABLE_TESTING      \
      -DCMAKE_ENABLE_DOC=$CMAKE_ENABLE_DOC              \
      -DCMAKE_ENABLE_LOGGING=$CMAKE_ENABLE_LOGGING      \
      $LIBBRAIN_SOURCE_DIR

make clean
make -j$CMAKE_NB_CORES

if [ $CMAKE_ENABLE_DOC -eq 1 ]; then
    make doc
fi

if [ $CMAKE_ENABLE_INSTALL -eq 1 ]; then
    make install
fi

if [ $CMAKE_ENABLE_TESTING -eq 1 ]; then
    make test
fi

exit 0
