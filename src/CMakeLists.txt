
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/arango-cpp/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

link_directories(${CMAKE_BINARY_DIR}/velocypack)

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    add_library(arango-cpp SHARED ${HEADER_FILES_TOP} ${HEADER_FILES} ${SOURCE_FILES})
    add_library(arango-cpp::arango-cpp ALIAS arango-cpp)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      target_link_libraries( arango-cpp PRIVATE velocypack PRIVATE libcurl)
    else()
      target_link_libraries( arango-cpp PRIVATE velocypack PRIVATE curl)
    endif()

    if(USE_SPDLOG_PRECOMPILED)
       target_compile_definitions(arango-cpp PUBLIC USE_SPDLOG_PRECOMPILED)
       #target_link_libraries(arango-cpp PUBLIC spdlog::spdlog )
       target_link_libraries(arango-cpp PUBLIC fmt::fmt )
    endif()

    install( TARGETS arango-cpp
         EXPORT arango-cppTargets DESTINATION "lib" COMPONENT libraries)

    # Install debug symbols
    if(MSVC)
       install(
             FILES $<TARGET_PDB_FILE:arango-cpp>
             DESTINATION ${CMAKE_INSTALL_BINDIR}
             COMPONENT libraries
             OPTIONAL)
    endif()
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    add_library(arango-cpp-static STATIC ${HEADER_FILES_TOP} ${HEADER_FILES} ${SOURCE_FILES})
    add_library(arango-cpp::arango-cpp-static ALIAS arango-cpp-static)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
         target_link_libraries( arango-cpp-static PRIVATE velocypack PRIVATE libcurl)
    else()
         target_link_libraries( arango-cpp-static PRIVATE velocypack PRIVATE curl)
    endif()
    if(USE_SPDLOG_PRECOMPILED)
        target_compile_definitions(arango-cpp-static PUBLIC USE_SPDLOG_PRECOMPILED)
        #target_link_libraries(arango-cpp-static PUBLIC spdlog::spdlog )
        target_link_libraries(arango-cpp-static PUBLIC fmt::fmt )
    endif()

   install( TARGETS arango-cpp-static
            EXPORT arango-cppTargets DESTINATION "lib" COMPONENT libraries)
endif()

install(
    DIRECTORY   "${ARANGO_HEADER_DIR}/arango-cpp"
    DESTINATION include
)

# Install the cmake config files that permit users to use find_package(arango-cpp)
#include(arango-cppInstallCMakeConfigFiles)
