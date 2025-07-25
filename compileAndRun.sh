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
    cd build
    cmake ..
else
    echo "Build directory already exists."
    cd build
fi

# compile the project
if make; then
    echo "Build successful."
else
    echo "Build failed."
fi

# run the compiled program
if [ -f "exchange" ]; then
    echo "Running the program..."
    ./exchange
else
    echo "Compiled program not found. Please check the build process."
fi
