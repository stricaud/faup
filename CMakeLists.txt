cmake_minimum_required(VERSION 2.8)

project(furl-project)

include(CMakeOptions.txt)

set(CMAKE_BUILD_TYPE "Release")

if (DEBUG_MODE)
set(CMAKE_BUILD_TYPE "Debug")
        if(NOT WIN32)
                add_definitions(-g)
        endif()
else()
        if(NOT WIN32)
                add_definitions(-O3)
        endif()
endif()
if (NOT WIN32)
	add_definitions(-std=c99 -fPIC)
endif()

include(CTest)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)

include(FindPkgConfig)

# Manage the version
file(STRINGS VERSION VERSION)
set(FURL_VERSION ${VERSION})
configure_file("src/lib/include/furl/version.h.cmake" "src/lib/include/furl/version.h")

# Mac OS Specifics
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOS=1)
endif()

set(FURL_INCLUDE_DIRS "${furl-project_SOURCE_DIR}/src/lib/include/")
if(WIN32)
	set(FURL_LIBRARY "${furl-project_SOURCE_DIR}/src/lib/${CMAKE_BUILD_TYPE}/furll.lib")
else(WIN32)
	set(FURL_LIBRARY "${furl-project_SOURCE_DIR}/src/lib/libfurll.so")
endif(WIN32)
add_subdirectory(src)
