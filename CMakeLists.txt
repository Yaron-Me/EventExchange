cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Exchange VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Automatically find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Remove main.cpp from library sources (since we'll have separate executables)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Find system packages  
find_package(Boost 1.89 REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(Catch2 3 REQUIRED)

# Create a library with all the core functionality
add_library(exchange_lib ${SOURCES})
target_include_directories(exchange_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(exchange_lib PUBLIC
    CROW_USE_BOOST=1
    CROW_STATIC_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/src/static"
)
target_link_libraries(exchange_lib PUBLIC
    Boost::boost
    SQLiteCpp
    SQLite::SQLite3
)

# Tests executable
add_executable(tests tests/test.cpp)
target_link_libraries(tests PRIVATE 
    exchange_lib 
    Catch2::Catch2WithMain
)

# Main executable
add_executable(exchange src/main.cpp)
target_link_libraries(exchange PRIVATE exchange_lib)

# Enhanced compiler flags for strict compilation
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(exchange_lib PRIVATE
        -Wall                      # Enable all warnings
        -Wextra                    # Enable extra warnings
        -pedantic                  # Ensure strict standard compliance
        -Werror                    # Treat warnings as errors
        -Wconversion               # Warn about implicit conversions
        # -Wsign-conversion          # Warn about sign conversions
        -Wold-style-cast           # Warn about C-style casts
        -Wshadow                   # Warn about shadowed variables
        -Wnon-virtual-dtor         # Warn about classes with virtual functions but no virtual destructor
        -Wcast-align               # Warn about pointer casts that increase alignment requirements
        -Wunused                   # Warn about unused variables
        -Woverloaded-virtual       # Warn about methods that override virtual methods but aren't marked virtual
        -Wpedantic                 # Reject non-standard C++
        -Wdouble-promotion         # Warn about float being implicitly promoted to double
        # -O1
    )
endif()