add_library(cryfox SHARED
    src/main/cpp/CryFoxActivity.cpp
    src/main/cpp/WebViewImplementationNative.cpp
    src/main/cpp/WebViewImplementationNativeJNI.cpp
    src/main/cpp/ALooperEventLoopImplementation.cpp
    src/main/cpp/TimerExecutorService.cpp
    src/main/cpp/JNIHelpers.cpp
)
target_link_libraries(cryfox PRIVATE jnigraphics android)

include(../cmake/AndroidExtras.cmake)
create_cryfox_bundle(cryfox)
