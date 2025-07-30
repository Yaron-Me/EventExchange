cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Exchange VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source files
set(SOURCES
    main.cpp
    utility/UUID.cpp
    exchange/Order.cpp
    exchange/OrderQueue.cpp
    exchange/User.cpp
    exchange/Share.cpp
    exchange/Stock.cpp
    exchange/Exchange.cpp
)

# Define header files (optional but good practice)
set(HEADERS
    utility/UUID.hpp
    exchange/Order.hpp
    exchange/OrderQueue.hpp
    exchange/User.hpp
    exchange/Share.hpp
    exchange/Stock.hpp
    exchange/Exchange.hpp
)

# Add the executable with all source files
add_executable(exchange ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(exchange SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/boost_1_88_0    # For Boost headers
)

target_include_directories(exchange PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}     # For local headers
)

# Enhanced compiler flags for strict compilation
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(exchange PRIVATE
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
        -O1
    )
endif()