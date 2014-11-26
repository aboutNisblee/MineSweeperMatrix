########################################################
# Modular CMake build script                           #
#
# Author: Moritz Nisblé <moritz.nisble@gmx.de>
#
# To build a platform specific build system with CMake
# you need to call CMake from the build directory and
# pass the path to the root CMakeLists.txt (this build
# script) as last argument.
# To pass additional arguments into this script you may
# use the -DVARIABLE= syntax.
# To make this build script as much modular as possible
# you can pass a custom file that is included into this
# file. This custom file should be specified with the
# INCLUDE_CMAKE variable.
# (i.e. -DINCLUDE_CMAKE="Custom.txt")
# Note that the commands and paths in that custom file
# are interpreted in the scope of this root script!
#
# The following variables should be specified when
# building the build system:
# PROJECT       - Specify project name
# INCLUDE_CMAKE - Custom CMake rules
#
# The following variables should be specified by the
# custom CMake file:
# BUILD   - "static" or "shared" for library building
#           else an executable will be built
# DIRS    - The directories that should be included
# SRCS    - All sources
# LIBS    - Custom libs
#

set(BUILD)
set(DIRS)
set(SRCS)
set(LIBS)

########################################################
# Helpers

macro(add_sources SOURCE_VAR)
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

    foreach(_src ${ARGN})
        get_filename_component(_src ${_src} NAME)

        if(_relPath)
            list(APPEND ${SOURCE_VAR} "${_relPath}/${_src}")
        else()
            list(APPEND ${SOURCE_VAR} "${_src}")
        endif()
    endforeach()

    if(_relPath)
        set(${SOURCE_VAR} ${${SOURCE_VAR}} PARENT_SCOPE)
    endif()
endmacro()

function(print_list text list)
    message(STATUS "********************************************************************************")
    message(STATUS "${text}")
    foreach(item ${${list}})
        message(STATUS "    ${item}")
    endforeach()
    message(STATUS "********************************************************************************")
endfunction()

########################################################
# BEFORE INCLUDE
# This settings are valid for the included custom file

cmake_minimum_required(VERSION 2.8.9)

# Set project name
if("${PROJECT}" STREQUAL "")
	message(FATAL_ERROR "PROJECT variable not specified!")
endif()
project(${PROJECT})

# Set a PROJECT_DIR variable to find the dir above us
get_filename_component(PROJECT_DIR "${CMAKE_SOURCE_DIR}" REALPATH)
message(STATUS "Project directory is: ${PROJECT_DIR}")

# Set output directory
message(STATUS "Setting runtime output dir to: ${PROJECT_BINARY_DIR}")
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add module path
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_DIR}/cmake/Modules/")
#message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

#Be verbose to enable eclipse to parse the includes
set(CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "**Initial variable status**
    CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}
    PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}
    PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}")

# Set system symbol
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DWINDOWS)
endif()

# Compiler settings
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fmessage-length=0")
endif()
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fmessage-length=0")
endif()

########################################################
# INCLUDE CUSTOM FILE

if(NOT "${INCLUDE_CMAKE}" STREQUAL "")
	find_file(_include_cmake "${INCLUDE_CMAKE}" PATHS "${PROJECT_BINARY_DIR}" NO_DEFAULT_PATH)
	if(NOT EXISTS "${_include_cmake}")
		message(FATAL_ERROR "Cannot find specified custom CMake file: \"${INCLUDE_CMAKE}\"")
	endif()
	message(STATUS "Custom CMake file: ${_include_cmake}")
	include("${_include_cmake}")
endif()

########################################################
# ANALYSE SETTINGS FROM CUSTOM FILE

# Include directories set in DIRS variable
include_directories(${DIRS})

########################################################
# PRINT DEBUG INFO

if(CMAKE_BUILD_TYPE MATCHES Debug)
	print_list("Sources:" SRCS)
	print_list("Includes:" DIRS)
endif()

########################################################
# BUILD

# Choose build type as specified by BUILD
if(BUILD MATCHES "static")
	message(STATUS "Building static library")
	add_library(${PROJECT_NAME} STATIC ${SRCS})
elseif(BUILD MATCHES "shared")
	message(STATUS "Building shared library")
	add_library(${PROJECT_NAME} SHARED ${SRCS})
else()
	#Build executable
	message(STATUS "Building executable")
	add_executable(${PROJECT_NAME} ${SRCS})
endif()

########################################################
# LINK

# Link against...
target_link_libraries(${PROJECT_NAME} ${LIBS})
