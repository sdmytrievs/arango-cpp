
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonarango/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
add_library(JSONARANGO_OBJECTS OBJECT ${HEADER_FILES_TOP} ${HEADER_FILES} ${SOURCE_FILES} )

if(USE_SPDLOG_PRECOMPILED)
   if(NOT TARGET spdlog::spdlog)
       find_package(spdlog CONFIG REQUIRED)
   endif()
endif()

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    add_library(jsonarango SHARED $<TARGET_OBJECTS:JSONARANGO_OBJECTS>)
    add_library(jsonarango::jsonarango ALIAS jsonarango)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
      target_link_libraries( jsonarango PRIVATE velocypack libcurl)
    else()
      target_link_libraries( jsonarango PRIVATE velocypack curl)
    endif()

    if(USE_SPDLOG_PRECOMPILED)
       target_compile_definitions(jsonarango PUBLIC USE_SPDLOG_PRECOMPILED)
       target_link_libraries(jsonarango PUBLIC spdlog::spdlog )
    endif()

    install( TARGETS jsonarango
         EXPORT jsonarangoTargets DESTINATION "lib" COMPONENT libraries)

    # Install debug symbols
    if(MSVC)
       install(
             FILES $<TARGET_PDB_FILE:jsonarango>
             DESTINATION ${CMAKE_INSTALL_BINDIR}
             COMPONENT libraries
             OPTIONAL)
    endif()
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    add_library(jsonarango-static STATIC $<TARGET_OBJECTS:JSONARANGO_OBJECTS>)
    add_library(jsonarango::jsonarango-static ALIAS jsonarango-static)

    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
         target_link_libraries( jsonarango-static PRIVATE velocypack libcurl)
    else()
         target_link_libraries( jsonarango-static PRIVATE velocypack curl)
    endif()
    if(USE_SPDLOG_PRECOMPILED)
        target_compile_definitions(jsonarango-static PUBLIC USE_SPDLOG_PRECOMPILED)
        target_link_libraries(jsonarango-static PUBLIC spdlog::spdlog )
    endif()

   install( TARGETS jsonarango-static
            EXPORT jsonarangoTargets DESTINATION "lib" COMPONENT libraries)
endif()

install(
    DIRECTORY   "${JSONARANGO_HEADER_DIR}/jsonarango"
    DESTINATION include
)
                  
