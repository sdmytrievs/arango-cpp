# Require a certain version of cmake
cmake_minimum_required(VERSION 3.16)

# Set the name of the project
project(arango-cpp VERSION 1.1.0 LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
message(STATUS "Require C++${CMAKE_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MACOSX_RPATH ON)


# Define variables with the GNU standard installation directories
include(GNUInstallDirs)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
include(CCache)

# Ensure proper configuration if in a conda environment
include(CondaAware)

# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)

option(BUILD_EXAMPLES "Build the examples" OFF)
option(BULID_LOCAL_TESTS "Build test suite for local ArangoDB" OFF)
option(BULID_REMOTE_TESTS "Build test suite for remote ArangoDB" ON)

# Used into conda only
if(DEFINED ENV{CONDA_PREFIX})
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" ON)
else()
   option(USE_SPDLOG_PRECOMPILED "Use spdlog in compiled version" OFF)
endif()

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Currently is not setup to produce a dynamic library using MSVC, only static
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find all arango-cpp dependencies
include(arango-cppFindDeps)

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options(-Wall -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /utf-8
        /EHsc
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()

# test thread safe
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=thread")

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
   include_directories("/usr/local/include")
   link_directories("/usr/local/lib")
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    include_directories("c:/usr/local/include")
    link_directories("c:/usr/local/lib")
endif()

#
#Build velocypack
include(PatchFile)
set(VELOCYPACK_HEADER_DIR ${CMAKE_SOURCE_DIR}/velocypack/include)
include_directories( ${VELOCYPACK_HEADER_DIR})
option(BuildVelocyPackExamples "Build velocypack examples" OFF)
add_subdirectory(velocypack)
install( TARGETS velocypack EXPORT arango-cppTargets
         PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )


# Set the arango-cpp source directory path
set(ARANGO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(ARANGO_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
# Set the include directories
include_directories(${ARANGO_SOURCE_DIR})
include_directories(${ARANGO_HEADER_DIR})

# Build arango-cpp library
add_subdirectory(src)

# Build the examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Run the tests
if(BULID_LOCAL_TESTS OR BULID_REMOTE_TESTS )
    add_subdirectory(tests)
endif()

# Install the cmake config files that permit users to use find_package(arango-cpp)
include(arango-cppInstallCMakeConfigFiles)
