# Specify the minimum version of CMake required to build this project.
cmake_minimum_required(VERSION 3.16)

# Set the project name and language.
project(aetherchess_engine CXX)

# Set the C++ standard to C++20, as specified in the build guide.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the current source directory to the include path.
# This allows us to use #include "core/types.h" instead of #include "aetherchess/engine/core/types.h".
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define the source files for the engine executable.
set(ENGINE_SOURCES
    main.cpp
    zobrist/zobrist.cpp
    movegen/attacks.cpp
    movegen/movegen.cpp
)

# Create the engine executable from the source files.
add_executable(aetherchess ${ENGINE_SOURCES})

# Optional: Add common compiler flags for release builds.
# These flags enable optimizations and warnings.
if(CMAKE_COMPILER_IS_GNU_CXX OR CMAKE_COMPILER_IS_CLANG_CXX)
    target_compile_options(aetherchess PRIVATE -Wall -Wextra -O3 -DNDEBUG)
endif()

# Print a status message to the user.
message(STATUS "AetherChess engine configured. To build, run: cmake --build .")
