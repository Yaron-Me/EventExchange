#!/bin/bash

# Default values
RUN_AFTER_BUILD=false
CLEAN_BUILD=false
RUN_TESTS=false

# Parse command line options
while getopts "rcth" opt; do
    case $opt in
        r)
            RUN_AFTER_BUILD=true
            ;;
        c)
            CLEAN_BUILD=true
            ;;
        t)
            RUN_TESTS=true
            ;;
        h)
            echo "Usage: $0 [-r] [-c] [-t] [-h]"
            echo "  -r  Run the executable after successful build"
            echo "  -c  Clean build (remove build directory first)"
            echo "  -t  Run tests after successful build"
            echo "  -h  Show this help message"
            exit 0
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            echo "Use -h for help"
            exit 1
            ;;
    esac
done

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
if ! [ -d "build" ]; then
    mkdir build
fi

cd build

cmake ..

echo "Building project..."
if make -j$(sysctl -n hw.ncpu); then
    echo "Build successful."
    if [ "$RUN_TESTS" = true ]; then
        echo "Running tests..."
        DB_PATH="testdb.db3" ./tests
    fi
    if [ "$RUN_AFTER_BUILD" = true ]; then
        ./exchange
    fi
else
    echo "Build failed."
    exit 1
fi

