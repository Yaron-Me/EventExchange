#!/bin/bash

if ! [ -d "libs" ]; then
    mkdir libs
fi

cd libs

# check if boost directory exists
if ! [ -d "boost_1_88_0" ]; then
    rm -f boost_1_88_0.tar.gz  # Remove any existing corrupted file
    wget https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz
    tar -xzf boost_1_88_0.tar.gz
    rm boost_1_88_0.tar.gz
else
    echo "Boost directory already exists."
fi

if ! [ -d "Crow-1.2.1-Linux" ]; then
    rm -f Crow-1.2.1-Linux.tar.gz  # Remove any existing corrupted file
    wget https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.tar.gz
    tar -xzf Crow-1.2.1-Linux.tar.gz
    rm Crow-1.2.1-Linux.tar.gz
else
    echo "Crow directory already exists."
fi

if ! [ -d "sqlite-autoconf-3500300" ]; then
    rm -f sqlite-autoconf-3500300.tar.gz  # Remove any existing corrupted file
    wget https://www.sqlite.org/2025/sqlite-autoconf-3500300.tar.gz
    tar -xzf sqlite-autoconf-3500300.tar.gz
    rm sqlite-autoconf-3500300.tar.gz
else
    echo "SQLite directory already exists."
fi

if ! [ -d "SQLiteCpp-3.3.3" ]; then
    rm -f 3.3.3.tar.gz  # Remove any existing corrupted file
    wget https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.3.3.tar.gz
    tar -xzf 3.3.3.tar.gz
    rm 3.3.3.tar.gz
else
    echo "SQLiteCpp directory already exists."
fi

cd ..

# create build directory if it doesn't exist
if ! [ -d "build" ]; then
    mkdir build
fi

cd build
cmake ..

# compile the project
if make; then
    echo "Build successful."
else
    echo "Build failed."
fi

# check if there is an -r flag
if [[ "$1" == "-r" ]]; then
    ./exchange
fi