cmake_minimum_required(VERSION 2.8)

project(furl-project)

include(CTest)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)

include(FindPkgConfig)

set(FURL_INCLUDE_DIRS "${furl_SOURCE_DIR}/src/lib/include/")
set(FURL_LIBRARY "${furl_SOURCE_DIR}/src/lib/libfurll.a")

add_subdirectory(src)
