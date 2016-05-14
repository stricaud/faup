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
include(CMakeGetDistribution.txt)
include(GNUInstallDirs)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()


#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

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

include(GNUInstallDirs)

include(FindPkgConfig)

include(TestBigEndian)
TEST_BIG_ENDIAN(BIGENDIAN)
if (${BIGENDIAN})
  add_definitions(-DHAVE_LITTLE_ENDIAN=0)
  add_definitions(-DHAVE_BIG_ENDIAN=1)
else()
  add_definitions(-DHAVE_LITTLE_ENDIAN=1)
  add_definitions(-DHAVE_BIG_ENDIAN=0)
endif()

#
# Check if functions exists
#
include(CheckFunctionExists)
check_function_exists(vsnprintf HAVE_VSNPRINTF)
if (${HAVE_VSNPRINTF})
  add_definitions(-DHAVE_VSNPRINTF=1)
endif()
check_function_exists(snprintf HAVE_SNPRINTF)
if (${HAVE_SNPRINTF})
  add_definitions(-DHAVE_SNPRINTF=1)
endif()


# Find LUA
# If we don't have lua, then we have no modules, it still works, but is less cool :)
if (NOT FAUP_NOLUA)
  pkg_check_modules(LUA lua>=5.1)
  if (NOT "${LUA_FOUND}")
    pkg_check_modules(LUA lua5.3>=5.1)
  endif()
  if (NOT "${LUA_FOUND}")
    pkg_check_modules(LUA lua5.2>=5.1)
  endif()
  if (NOT "${LUA_FOUND}")
    pkg_check_modules(LUA lua5.1>=5.1)
  endif()  
endif()

if(${LUA_FOUND})
  add_definitions(-DFAUP_LUA_MODULES=1)
  message("Lua found. Faup will build with modules.")
  message("LUA version: ${LUA_VERSION}")
  message("LUA libraries: ${LUA_LIBRARIES}")
  message("LUA include dirs: ${LUA_INCLUDE_DIR}")
endif()

pkg_check_modules(LIBCACA caca)
if (${LIBCACA_FOUND})
  add_definitions(-DFAUP_CACA=1)
  message("Libcaca found, faup will enable snapshot browsing")
else()
  message("Libcaca was not found, faup won't offer snapshot browsing")
endif()

# Manage the version
file(STRINGS VERSION VERSION)
set(FAUP_VERSION ${VERSION})
configure_file("src/lib/include/faup/version.h.cmake" "src/lib/include/faup/version.h")
install(FILES "${faup-project_BINARY_DIR}/src/lib/include/faup/version.h" DESTINATION include/faup COMPONENT Headers)

message("CMake system: ${CMAKE_SYSTEM_NAME}")
# Mac OS Specifics
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOS=1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "WIN32")
  add_definitions(-DWIN32=1)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX=1)
  set(FAUP_LINK_LIBRARIES pthread)
  message("Discovered distribution type: ${LINUX_DIST}")
endif()

set(FAUP_INCLUDE_DIRS "${faup-project_SOURCE_DIR}/src/lib/include/" "${faup-project_BINARY_DIR}/src/lib/include")

set(FAUP_LIBRARY faupl)
if(WIN32)
	set(FAUP_LIBRARY "${faup-project_BINARY_DIR}/src/lib/${CMAKE_BUILD_TYPE}/faup_static.lib")
endif(WIN32)
if(APPLE)
	set(FAUP_LIBRARY "${faup-project_BINARY_DIR}/src/lib/libfaupl.dylib")
endif(APPLE)

# Set our env variables
set(FAUP_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/faup/")
add_definitions(-DFAUP_DATA_DIR="${FAUP_DATA_DIR}")

#
# Make version variables from the VERSION file
#
file(STRINGS "VERSION" FAUP_VERSION)
string(REGEX MATCH "^[0-9]+" FAUP_VERSION_MAJOR ${FAUP_VERSION})
string(REGEX MATCH "[0-9]+$" FAUP_VERSION_MINOR ${FAUP_VERSION})
message("Version: ${FAUP_VERSION}")
message("Version Major: ${FAUP_VERSION_MAJOR}")
message("Version Minor: ${FAUP_VERSION_MINOR}")

add_subdirectory(doc)
add_subdirectory(src)

include(CMakePackage.txt)
