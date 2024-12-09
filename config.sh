#! /bin/bash

BUILD_TYPE=Debug
if [ $# -gt 0 ]; 
then
    BUILD_TYPE=$1
fi

PLATFORM_TYPE="Unix"
if [ "$(uname)" != "Linux" ]; 
then
    PLATFORM_TYPE="MinGW"
fi

cmake -S . -B build -G "$PLATFORM_TYPE Makefiles" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=install
