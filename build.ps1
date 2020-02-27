
set CMAKE_NB_CORES=2
set CMAKE_ENABLE_INSTALL=1

set LIBBRAIN_ROOT_DIR=$(pwd)
set LIBBRAIN_SOURCE_DIR=$LIBBRAIN_ROOT_DIR
set LIBBRAIN_BUILD_TYPE=debug
set LIBBRAIN_VERSION_MAJOR=1
set LIBBRAIN_VERSION_MINOR=0
set LIBBRAIN_VERSION_PATCH=0
set LIBBRAIN_AUTHOR=benoitfragit
set LIBBRAIN_NAME=MLP
set LIBBRAIN_DESCRIPTION="MLPlugins developed by Benoitfragit"
set LIBBRAIN_VERSION=$LIBBRAIN_VERSION_MAJOR.$LIBBRAIN_VERSION_MINOR.$LIBBRAIN_VERSION_PATCH
set LIBBRAIN_BUILD_DIR=$LIBBRAIN_ROOT_DIR/build/$LIBBRAIN_BUILD_TYPE/mlplugins
set LIBBRAIN_INSTALL_DIR=$LIBBRAIN_ROOT_DIR/install/$LIBBRAIN_BUILD_TYPE/mlplugins-$LIBBRAIN_VERSION
set LIBBRAIN_MODULE_PATH=$LIBBRAIN_ROOT_DIR/scripts/cmake/Modules
set LIBBRAIN_ARCHIVE_TYPE=DEB
BRAIN_ENABLE_DOUBLE_PRECISION=1
BRAIN_ENABLE_LOGGING=1
BRAIN_ENABLE_TESTING=0
BRAIN_ENABLE_DOC=0


if ( -not (Test-Path $LIBBRAIN_BUILD_DIR))
{
    mkdir -p $LIBBRAIN_BUILD_DIR
}

if ( -not (Test-Path $LIBBRAIN_INSTALL_DIR))
{
    mkdir -p $LIBBRAIN_INSTALL_DIR
}

cd $LIBBRAIN_BUILD_DIR

clear;
clear;

cmake -DCMAKE_BUILD_TYPE=$LIBBRAIN_BUILD_TYPE           \
      -DCMAKE_INSTALL_PREFIX:PATH=$LIBBRAIN_INSTALL_DIR \
      -DCMAKE_MODULE_PATH=$LIBBRAIN_MODULE_PATH         \
      -DBRAIN_ENABLE_TESTING=$BRAIN_ENABLE_TESTING      \
      -DBRAIN_ENABLE_DOC=$BRAIN_ENABLE_DOC              \
      -DBRAIN_ENABLE_LOGGING=$BRAIN_ENABLE_LOGGING      \
      -DBRAIN_ENABLE_DOUBLE_PRECISION=$BRAIN_ENABLE_DOUBLE_PRECISION \
      -DBRAIN_VERSION=$LIBBRAIN_VERSION \
      -DBRAIN_AUTHOR=$LIBBRAIN_AUTHOR \
      -DBRAIN_DESCRIPTION=$LIBBRAIN_DESCRIPTION \
      -DBRAIN_NAME=$LIBBRAIN_NAME \
      $LIBBRAIN_SOURCE_DIR

make clean
make -j$CMAKE_NB_CORES

#if [ $BRAIN_ENABLE_DOC -eq 1 ]; then
#    make doc
#fi

#if [ $CMAKE_ENABLE_INSTALL -eq 1 ]; then
    make install
#fi

#if [ $BRAIN_ENABLE_TESTING -eq 1 ]; then
#    make test
#fi

exit 0