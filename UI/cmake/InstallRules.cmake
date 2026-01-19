
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(package CryFox)

set(cryfox_applications cryfox ${cryfox_helper_processes})

set(app_install_targets ${cryfox_applications})

install(TARGETS cryfox
  EXPORT cryfoxTargets
  RUNTIME
    COMPONENT cryfox_Runtime
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  BUNDLE
    COMPONENT cryfox_Runtime
    DESTINATION bundle
  LIBRARY
    COMPONENT cryfox_Runtime
    NAMELINK_COMPONENT cryfox_Development
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET browser
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILE_SET cryfox
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(TARGETS ${cryfox_helper_processes}
  EXPORT cryfoxTargets
  RUNTIME
    COMPONENT cryfox_Runtime
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}
)

include("${CRYFOX_SOURCE_DIR}/Meta/Lagom/get_linked_lagom_libraries.cmake")
foreach (application IN LISTS cryfox_applications)
  get_linked_lagom_libraries("${application}" "${application}_lagom_libraries")
  list(APPEND all_required_lagom_libraries "${${application}_lagom_libraries}")
endforeach()
list(REMOVE_DUPLICATES all_required_lagom_libraries)

# Remove cryfox shlib if it exists
list(REMOVE_ITEM all_required_lagom_libraries cryfox)

if (APPLE)
    # Fixup the app bundle and copy:
    #   - Libraries from lib/ to CryFox.app/Contents/lib
    # Remove the symlink we created at build time for the lib directory first
    install(CODE "
    file(REMOVE \${CMAKE_INSTALL_PREFIX}/bundle/CryFox.app/Contents/lib)
    set(lib_dir \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    if (IS_ABSOLUTE ${CMAKE_INSTALL_LIBDIR})
      set(lib_dir ${CMAKE_INSTALL_LIBDIR})
    endif()

    set(contents_dir \${CMAKE_INSTALL_PREFIX}/bundle/CryFox.app/Contents)
    file(COPY \${lib_dir} DESTINATION \${contents_dir})
  "
            COMPONENT cryfox_Runtime)
endif()

install(TARGETS ${all_required_lagom_libraries}
  EXPORT cryfoxTargets
  COMPONENT cryfox_Runtime
  LIBRARY
    COMPONENT cryfox_Runtime
    NAMELINK_COMPONENT cryfox_Development
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET server
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILE_SET cryfox
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    cryfox_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(cryfox_INSTALL_CMAKEDIR)

install(
    FILES cmake/CryFoxInstallConfig.cmake
    DESTINATION "${cryfox_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT cryfox_Development
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${cryfox_INSTALL_CMAKEDIR}"
    COMPONENT cryfox_Development
)

install(
    EXPORT cryfoxTargets
    NAMESPACE cryfox::
    DESTINATION "${cryfox_INSTALL_CMAKEDIR}"
    COMPONENT cryfox_Development
)

if (NOT APPLE)
    # On macOS the resources are handled via the MACOSX_PACKAGE_LOCATION property on each resource file
    install_cryfox_resources("${CMAKE_INSTALL_DATADIR}/Lagom" cryfox_Runtime)
endif()

if (ENABLE_INSTALL_FREEDESKTOP_FILES)
    set(FREEDESKTOP_RESOURCE_DIR "${CRYFOX_SOURCE_DIR}/Meta/CMake/freedesktop")
    string(TIMESTAMP DATE "%Y-%m-%d" UTC)
    execute_process(
        COMMAND git rev-parse --short=10 HEAD
        WORKING_DIRECTORY ${CRYFOX_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    configure_file("${FREEDESKTOP_RESOURCE_DIR}/org.cryfox.CryFox.metainfo.xml.in" "${CMAKE_CURRENT_BINARY_DIR}/org.cryfox.CryFox.metainfo.xml" @ONLY)
    install(FILES
        "${FREEDESKTOP_RESOURCE_DIR}/org.cryfox.CryFox.svg"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
        COMPONENT cryfox_Runtime
    )
    install(FILES
        "${FREEDESKTOP_RESOURCE_DIR}/org.cryfox.CryFox.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
        COMPONENT cryfox_Runtime
    )
    install(FILES
        "${FREEDESKTOP_RESOURCE_DIR}/org.cryfox.CryFox.service"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/dbus-1/services"
        COMPONENT cryfox_Runtime
    )
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/org.cryfox.CryFox.metainfo.xml"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo"
        COMPONENT cryfox_Runtime
    )
endif()
