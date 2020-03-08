message(STATUS "Building test suite: ${BuildTests}")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pass" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

include(CTest)
enable_testing()
find_package(Threads)
add_subdirectory(googletest REQUIRED)

# -------------------------------------
# Build test program
# -------------------------------------

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

add_executable(test_main  main.cpp )
target_link_libraries(test_main gtest jsonarango velocypack curl ${CMAKE_THREAD_LIBS_INIT} )

target_include_directories(test_main PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../src
)

add_test(NAME test_main COMMAND test_main)
