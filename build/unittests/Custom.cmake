# This file is included in the root CMakeLists.txt

set(VERSION_MAJOR 2)
set(VERSION_MINOR 1)

set(CMAKE_BUILD_TYPE "Debug")

# BOOST_UNIT_TEST_FRAMEWORK
find_package(Boost 1.54 REQUIRED COMPONENTS system thread unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${Boost_LIBRARIES})
set(LIBS ${LIBS} ${Boost_THREAD_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_SYSTEM_LIBRARY})
add_definitions(-DBOOST_TEST_DYN_LINK)

# Definitions
add_definitions(-DJNIREF=1 -DBOOST_SIGNALS=1)

# CMakeLists in tree are setting SRCS
add_subdirectory("src")

set(DIRS "src")
set(SRCS ${SRCS} ${TEST_SRCS})
