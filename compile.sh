#!/bin/bash

# check if boost directory exists
if ! [ -d "boost_1_88_0" ]; then
    rm -f boost_1_88_0.tar.gz  # Remove any existing corrupted file
    wget https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.gz
    tar -xzf boost_1_88_0.tar.gz
    rm boost_1_88_0.tar.gz
else
    echo "Boost directory already exists."
fi

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