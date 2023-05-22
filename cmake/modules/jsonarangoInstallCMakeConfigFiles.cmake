# The path where cmake config files are installed
set(JSONARANGO_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/jsonarango)

install(EXPORT jsonarangoTargets
    FILE jsonarangoTargets.cmake
    NAMESPACE jsonarango::
    DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR}
    COMPONENT cmake)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/jsonarangoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/jsonarangoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/jsonarangoConfig.cmake
    INSTALL_DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR}
    PATH_VARS JSONARANGO_INSTALL_CONFIGDIR)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/jsonarangoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/jsonarangoConfigVersion.cmake
    DESTINATION ${JSONARANGO_INSTALL_CONFIGDIR})
