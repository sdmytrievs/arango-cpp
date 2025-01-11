# The path where cmake config files are installed
set(JSONARANGO_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/arango-cpp)

install(EXPORT arango-cppTargets
    FILE arango-cppTargets.cmake
    NAMESPACE arango-cpp::
    DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR}
    COMPONENT cmake)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/arango-cppConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/modules/arango-cppConfig.cmake.in
    ${CMAKE_BINARY_DIR}/arango-cppConfig.cmake
    INSTALL_DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR}
    PATH_VARS JSONARANGO_INSTALL_CONFIGDIR)

install(FILES
    ${CMAKE_BINARY_DIR}/arango-cppConfig.cmake
    ${CMAKE_BINARY_DIR}/arango-cppConfigVersion.cmake
    DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR})
