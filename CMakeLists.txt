cmake_minimum_required(VERSION 2.8)

project(furl-project)

include(CMakeOptions.txt)

if (DEBUG_MODE)
        if(NOT WIN32)
                add_definitions(-ggdb3)
        endif()
endif()

include(CTest)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)

include(FindPkgConfig)

set(FURL_INCLUDE_DIRS "${furl-project_SOURCE_DIR}/src/lib/include/")
set(FURL_LIBRARY "${furl-project_SOURCE_DIR}/src/lib/libfurll.a")

add_subdirectory(src)
