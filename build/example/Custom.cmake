# This file is included in the root CMakeLists.txt

set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE "Debug")

# Definitions
add_definitions(-DJNIREF=1 -DBOOST_SIGNALS=1)

# CMakeLists in tree are setting SRCS
add_subdirectory("src")
add_subdirectory("example")

set(DIRS "src")
