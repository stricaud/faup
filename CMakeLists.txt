cmake_minimum_required(VERSION 2.8)

project(furl-project)

include(CMakeOptions.txt)

set(CMAKE_BUILD_TYPE "Release")

if (DEBUG_MODE)
set(CMAKE_BUILD_TYPE "Debug")
        if(NOT WIN32)
                add_definitions(-g)
        endif()
endif()
add_definitions(-std=c99)

include(CTest)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)

include(FindPkgConfig)

set(FURL_INCLUDE_DIRS "${furl-project_SOURCE_DIR}/src/lib/include/")
if(WIN32)
	set(FURL_LIBRARY "${furl-project_SOURCE_DIR}/src/lib/${CMAKE_BUILD_TYPE}/furll.lib")
else(WIN32)
	set(FURL_LIBRARY "${furl-project_SOURCE_DIR}/src/lib/libfurll.a")
endif(WIN32)
add_subdirectory(src)
