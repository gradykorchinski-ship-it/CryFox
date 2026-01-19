include(cmake/ResourceFiles.cmake)

function(create_cryfox_bundle target_name)
    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME "CryFox"
        MACOSX_BUNDLE_GUI_IDENTIFIER org.cryfox.CryFox
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_INFO_PLIST "${CRYFOX_SOURCE_DIR}/UI/Info.plist"
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER org.cryfox.CryFox
    )

    if (APPLE)
        set(bundle_dir "$<TARGET_BUNDLE_DIR:${target_name}>")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${bundle_dir}/Contents/Resources"
            COMMAND "iconutil" --convert icns "${CRYFOX_SOURCE_DIR}/UI/Icons/macos/app_icon.iconset" --output "${bundle_dir}/Contents/Resources/app_icon.icns"
        )

        # Note: This symlink is removed in the install commands
        # This makes the bundle in the build directory *NOT* relocatable
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E create_symlink "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${bundle_dir}/Contents/lib"
        )

        if (NOT CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo" AND "arm64" IN_LIST CMAKE_OSX_ARCHITECTURES)
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND codesign -s - -v -f --entitlements "${CRYFOX_SOURCE_DIR}/Meta/debug.plist" "${bundle_dir}"
            )
        else()
            add_custom_target(apply-debug-entitlements
                COMMAND codesign -s - -v -f --entitlements "${CRYFOX_SOURCE_DIR}/Meta/debug.plist" "${bundle_dir}"
                USES_TERMINAL
            )
        endif()
    endif()

    if (APPLE)
        set(resource_base_dir "$<TARGET_BUNDLE_DIR:${target_name}>/Contents/Resources")
    else()
        set(resource_base_dir "${CMAKE_BINARY_DIR}/${IN_BUILD_PREFIX}${CMAKE_INSTALL_DATADIR}/Lagom")
    endif()

    copy_resources_to_build(${resource_base_dir} ${target_name})
endfunction()

# Select UI Framework
if (ENABLE_QT)
   add_subdirectory(Qt)
elseif (APPLE)
   add_subdirectory(AppKit)
elseif(ANDROID)
   add_subdirectory(Android)
else()
    # TODO: Check for other GUI frameworks here when we move them in-tree
    return()
endif()

if (NOT TARGET cryfox)
    message(FATAL_ERROR "UI Framework selection must declare a cryfox target")
endif()

if (TARGET cryfox_impl)
    set(CRYFOX_TARGET cryfox_impl PUBLIC)
else()
    set(CRYFOX_TARGET cryfox PRIVATE)
endif()

set(CRYFOX_LIBS AK LibAuth LibBrowser LibCore LibFileSystem LibGfx LibImageDecoderClient LibIPC LibJS LibMain LibWeb LibWebView LibRequests LibURL)
target_link_libraries(${CRYFOX_TARGET} PRIVATE ${CRYFOX_LIBS})
target_link_libraries(${CRYFOX_TARGET} PRIVATE OpenSSL::Crypto OpenSSL::SSL)

target_include_directories(${CRYFOX_TARGET} ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${CRYFOX_TARGET} ${CRYFOX_SOURCE_DIR})
target_include_directories(${CRYFOX_TARGET} ${CRYFOX_SOURCE_DIR}/Services/)

if(WIN32)
    target_link_libraries(${CRYFOX_TARGET} LibDevTools)
endif()

function(set_helper_process_properties)
    set(targets ${ARGV})
    if (APPLE OR WIN32)
        # Store helper processes in the same bundle directory as the main application
        set_target_properties(${targets} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:cryfox>")
    else()
        set_target_properties(${targets} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${IN_BUILD_PREFIX}${CMAKE_INSTALL_LIBEXECDIR}")
    endif()
endfunction()

add_custom_target(run
    COMMAND "${CMAKE_COMMAND}" -E env "CRYFOX_SOURCE_DIR=${CRYFOX_SOURCE_DIR}" "$<TARGET_FILE:cryfox>" $ENV{LAGOM_ARGS}
    USES_TERMINAL
    VERBATIM
)

if (APPLE)
    add_custom_target(debug-cryfox
        COMMAND "${CMAKE_COMMAND}" -E env "CRYFOX_SOURCE_DIR=${CRYFOX_SOURCE_DIR}" lldb "$<TARGET_BUNDLE_DIR:cryfox>"
        USES_TERMINAL
    )
else()
    add_custom_target(debug-cryfox
        COMMAND "${CMAKE_COMMAND}" -E env "CRYFOX_SOURCE_DIR=${CRYFOX_SOURCE_DIR}" gdb "$<TARGET_FILE:cryfox>"
        USES_TERMINAL
    )
endif()

set(cryfox_helper_processes ImageDecoder RequestServer WebContent WebWorker)

add_dependencies(cryfox ${cryfox_helper_processes})
# FIXME: Increase support for building targets on Windows
if (NOT WIN32)
    add_dependencies(WebDriver cryfox)
endif()

set_helper_process_properties(${cryfox_helper_processes})
if (APPLE)
    set_helper_process_properties(WebDriver)
endif()

if(NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/InstallRules.cmake)
endif()
