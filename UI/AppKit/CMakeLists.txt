add_library(cryfox_impl STATIC
    Application/Application.mm
    Application/ApplicationDelegate.mm
    Application/EventLoopImplementationMacOS.mm
    Interface/Autocomplete.mm
    Interface/Event.mm
    Interface/InfoBar.mm
    Interface/CryFoxWebView.mm
    Interface/CryFoxWebViewBridge.cpp
    Interface/CryFoxWebViewWindow.mm
    Interface/Menu.mm
    Interface/Palette.mm
    Interface/SearchPanel.mm
    Interface/Tab.mm
    Interface/TabController.mm
    Utilities/Conversions.mm
)
target_include_directories(cryfox_impl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_options(cryfox_impl PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-fobjc-arc>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-anon-enum-enum-conversion> # Required for CGImageCreate
)
target_compile_features(cryfox_impl PUBLIC cxx_std_23)

add_executable(cryfox MACOSX_BUNDLE
   main.mm
)
target_link_libraries(cryfox_impl PUBLIC "-framework Cocoa -framework UniformTypeIdentifiers" LibUnicode)
target_link_libraries(cryfox PRIVATE cryfox_impl)

create_cryfox_bundle(cryfox)
