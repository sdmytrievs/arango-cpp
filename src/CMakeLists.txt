
# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES_TOP  ${CMAKE_SOURCE_DIR}/include/jsonarango/*.h )

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES  *.h )

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES  *.cpp )

# Create the shared library using the collected source files
add_library(JSONARANGO_OBJECTS OBJECT ${HEADER_FILES} ${SOURCE_FILES} )

if(USE_SPDLOG_PRECOMPILED)
   if(NOT TARGET spdlog::spdlog)
       find_package(spdlog CONFIG REQUIRED)
   endif()
endif()

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    add_library(JSONARANGO_SHARED SHARED $<TARGET_OBJECTS:JSONARANGO_OBJECTS>)
    if(USE_SPDLOG_PRECOMPILED)
       target_link_libraries(JSONARANGO_SHARED curl velocypack PUBLIC spdlog::spdlog )
    else()
       target_link_libraries(JSONARANGO_SHARED curl velocypack )
    endif()
    set_target_properties(JSONARANGO_SHARED PROPERTIES OUTPUT_NAME jsonarango)
    install(TARGETS JSONARANGO_SHARED DESTINATION lib)
endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    add_library(JSONARANGO_STATIC STATIC $<TARGET_OBJECTS:JSONARANGO_OBJECTS>)
    set_target_properties(JSONARANGO_STATIC PROPERTIES OUTPUT_NAME jsonarango-static)
    install(TARGETS JSONARANGO_STATIC DESTINATION lib)
endif()

install(
    DIRECTORY   "${JSONARANGO_HEADER_DIR}/jsonarango"
    DESTINATION include
)
                  
