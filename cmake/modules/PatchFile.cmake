# use GNU Patch from any platform

if(WIN32)
  # prioritize Git Patch on Windows as other Patches may be very old and incompatible.
  find_package(Git)
  if(Git_FOUND)
    get_filename_component(GIT_DIR ${GIT_EXECUTABLE} DIRECTORY)
    get_filename_component(GIT_DIR ${GIT_DIR} DIRECTORY)
  endif()
endif()

find_program(PATCH
NAMES patch
HINTS ${GIT_DIR}
PATH_SUFFIXES usr/bin
)

if(NOT PATCH)
  message(FATAL_ERROR "Did not find GNU Patch")
endif()

execute_process(COMMAND ${PATCH} --input=${CMAKE_SOURCE_DIR}/velocypack.patch --ignore-whitespace --forward
TIMEOUT 15
COMMAND_ECHO STDOUT
#RESULT_VARIABLE ret
WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/velocypack"
)


#if(NOT ret EQUAL 0)
#  message(FATAL_ERROR "Failed to apply patch ")
#endif()
