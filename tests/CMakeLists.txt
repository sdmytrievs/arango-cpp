message(STATUS "Build test suite for local ArangoDB: ${BULID_LOCAL_TESTS}")
message(STATUS "Build test suite for remote ArangoDB: ${BULID_REMOTE_TESTS}")

#https://stackoverflow.com/questions/12540970/how-to-make-gtest-build-mdd-instead-of-mtd-by-default-using-cmake
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
   set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

include(CTest)
enable_testing()
find_package(Threads)
add_subdirectory(googletest REQUIRED)

# -------------------------------------
# Build test program
# -------------------------------------

if(BULID_LOCAL_TESTS MATCHES ON)
  add_definitions( -DTestLocalServer )
endif()

if(BULID_REMOTE_TESTS MATCHES ON)
  add_definitions( -DTestRemoteServer )
endif()


LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

add_executable(test_main  main.cpp )
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries(test_main gtest_main arango-cpp::arango-cpp-static Threads::Threads)
else()
  target_link_libraries(test_main gtest arango-cpp::arango-cpp Threads::Threads)
endif()

target_include_directories(test_main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src )

add_test(NAME test_main COMMAND test_main)
