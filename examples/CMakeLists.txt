# options
message(STATUS "Building jsonArango examples: ${BUILD_EXAMPLES}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/examples-cfg.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

if(BUILD_EXAMPLES)
    set(Examples
        exampleCRUD
        exampleCollections
        exampleQuery
        exampleSelection
        exampleGraphCRUD
#        exampleOptions
        time_test
    )

    foreach (exampleName ${Examples})
        add_executable(${exampleName} ${exampleName}.cpp)
        if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
          target_link_libraries( ${exampleName} jsonarango-static velocypack libcurl )
        else()
          target_link_libraries(${exampleName} jsonarango velocypack curl pthread )
        endif()
    endforeach()
endif()
