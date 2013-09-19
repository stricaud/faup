cmake_minimum_required(VERSION 2.8)

project(faup-project)

message("CMake system name: ${CMAKE_SYSTEM_NAME}")

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
message(FATAL_ERROR "CMake generation is not allowed within the source directory!
Remove the CMakeCache.txt file and try again from another folder, e.g.:

   rm CMakeCache.txt
   mkdir build
   cd build
   cmake ..
")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${faup-project_SOURCE_DIR}/cmake)

include(CMakeOptions.txt)

set(CMAKE_BUILD_TYPE "Release")

if (DEBUG_MODE)
add_definitions(-DFAUP_DEBUG=1)
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

# Find LUA
# If we don't have lua, then we have no modules, it still works, but is less cool :)
find_package(Lua51)
if(${LUA51_FOUND})
  add_definitions(-DFAUP_LUA_MODULES=1)
  message("Lua 5.1 found. Faup will build with modules.")
endif()

# Manage the version
file(STRINGS VERSION VERSION)
set(FAUP_VERSION ${VERSION})
configure_file("src/lib/include/faup/version.h.cmake" "src/lib/include/faup/version.h")

# Mac OS Specifics
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOS=1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "WIN32")
  add_definitions(-DWIN32=1)
endif()

set(FAUP_INCLUDE_DIRS "${faup-project_SOURCE_DIR}/src/lib/include/" "${faup-project_BINARY_DIR}/src/lib/include")

set(FAUP_LIBRARY "${faup-project_BINARY_DIR}/src/lib/libfaupl.so")
if(WIN32)
	set(FAUP_LIBRARY "${faup-project_BINARY_DIR}/src/lib/${CMAKE_BUILD_TYPE}/faup_static.lib")
endif(WIN32)
if(APPLE)
	set(FAUP_LIBRARY "${faup-project_BINARY_DIR}/src/lib/libfaupl.dylib")
endif(APPLE)

# Set our env variables
set(FAUP_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/faup/")
add_definitions(-DFAUP_DATA_DIR="${FAUP_DATA_DIR}")

add_subdirectory(src)

include(CMakePackage.txt)
