cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Exchange VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source files
set(SOURCES
    src/database/exchange.cpp
    src/database/users.cpp
    src/database/utility.cpp

    src/utility/uuid.cpp

    src/exchange/Order.cpp
    src/exchange/OrderQueue.cpp
    src/exchange/User.cpp
    src/exchange/Share.cpp
    src/exchange/Stock.cpp
    src/exchange/Exchange.cpp

    src/api/user.cpp
    src/api/stock.cpp

    src/routes/home.cpp

    src/main.cpp
)

# Define header files (optional but good practice)
set(HEADERS
    src/database/exchange.hpp
    src/database/users.hpp
    src/database/utility.hpp

    src/utility/uuid.hpp

    src/exchange/Order.hpp
    src/exchange/OrderQueue.hpp
    src/exchange/User.hpp
    src/exchange/Share.hpp
    src/exchange/Stock.hpp
    src/exchange/Exchange.hpp

    src/api/api.hpp
    src/api/user.hpp
    src/api/stock.hpp

    src/routes/routes.hpp
    src/routes/home.hpp
)

# Add the executable with all source files
add_executable(exchange ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(exchange SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/boost_1_88_0    # For Boost headers
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/Crow-1.2.1-Linux/include  # For Crow headers
)

target_include_directories(exchange PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}     # For local headers
)

# Define preprocessor macros to use Boost with Crow
target_compile_definitions(exchange PRIVATE
    CROW_USE_BOOST=1
    CROW_STATIC_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/src/static"
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/SQLiteCpp-3.3.3)


target_link_libraries(exchange
    SQLiteCpp
    sqlite3
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