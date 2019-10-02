# options
message(STATUS "Building jsonio examples: ${BuildJsonioExamples}")
if(BuildJsonioExamples)
    set(Examples
        exampleCRUD
        exampleQuery
        exampleSelection
        exampleLoadValues
        exampleComplexKey
    )

    foreach (exampleName ${Examples})
        add_executable(${exampleName} ${exampleName}.cpp)
        target_link_libraries(${exampleName} jsonarango )
    endforeach()
endif()
