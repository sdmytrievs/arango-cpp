# Require a certain version of cmake
cmake_minimum_required(VERSION 2.8)

# Set the name of the project
project(jsonarango)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH ON)

# Ensure proper configuration if in a conda environment
include(CondaAware)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
find_library(CURL_LIB libcurl)
else()
find_library(CURL_LIB curl)
endif()
if(NOT CURL_LIB)
  message(FATAL_ERROR "curl library not found")
endif()

find_library(VELOCYPACK_LIB velocypack)
if(NOT VELOCYPACK_LIB)
  message(FATAL_ERROR "velocypack library not found")
endif()

# Define variables with the GNU standard installation directories
include(GNUInstallDirs)


# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
option(BUILD_STATIC_LIBS "Build static libraries." ON)

option(BUILD_EXAMPLES "Build the python wrappers and python package thermofun." ON)
option(BuildLocalDBTests "Build test suite" ON)
option(BuildRemoteDBTests "Build test suite" ON)

# Currently is not setup to produce a dynamic library using MSVC, only static
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-std=c++11)

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options(-std=c++11 -Wall -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-std=c++11 -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-std=c++11 -Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /EHsc
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()


# Set the jsonarango source directory path
set(JSONARANGO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(JSONARANGO_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)


# Set the include directories
include_directories(${JSONARANGO_SOURCE_DIR})
include_directories(${JSONARANGO_HEADER_DIR})
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif()

# Build jsonio library
add_subdirectory(src)

if(BUILD_EXAMPLES)
# Build the examples
    add_subdirectory(examples)
endif()

# Run the tests
if(BuildRemoteDBTests OR BuildLocalDBTests )
    add_subdirectory(tests)
endif()

include(CPack) #should be last command
