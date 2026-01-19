qt_add_executable(cryfox main.cpp)
target_sources(cryfox PRIVATE
    Application.cpp
    Autocomplete.cpp
    BrowserWindow.cpp
    EventLoopImplementationQt.cpp
    EventLoopImplementationQtEventTarget.cpp
    FindInPageWidget.cpp
    Icon.cpp
    LocalAuthDialog.cpp
    LocationEdit.cpp
    Menu.cpp
    ProfileButton.cpp
    ProfilePanel.cpp
    Settings.cpp
    StringUtils.cpp
    Tab.cpp
    TabBar.cpp
    TVGIconEngine.cpp
    WebContentView.cpp
    cryfox.qrc
)
target_link_libraries(cryfox PRIVATE Qt::Core Qt::Gui Qt::Widgets)
create_cryfox_bundle(cryfox)

if (WIN32)
    lagom_windows_bin(cryfox CONSOLE)
    qt_generate_deploy_script(
        TARGET cryfox
        OUTPUT_SCRIPT cryfox_deploy_script
        CONTENT "
qt_deploy_runtime_dependencies(
    EXECUTABLE $<TARGET_FILE:cryfox>
    PLUGINS_DIR $<TARGET_FILE_DIR:cryfox>
    NO_TRANSLATIONS
)
")

    add_custom_command(TARGET cryfox POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -P ${cryfox_deploy_script}
        COMMAND_EXPAND_LISTS
    )
endif()
