# options
message(STATUS "Building jsonArango examples: ${BUILD_EXAMPLES}")
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

if(BUILD_EXAMPLES)
    set(Examples
        exampleCRUD
        exampleQuery
        time_test
    )

    foreach (exampleName ${Examples})
        add_executable(${exampleName} ${exampleName}.cpp)
        target_link_libraries(${exampleName} jsonarango velocypack curl )
    endforeach()
endif()
