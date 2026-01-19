#
# Options specific to the Lagom (host) build
#

include(${CMAKE_CURRENT_LIST_DIR}/common_options.cmake NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_LIST_DIR}/lagom_install_options.cmake)

# lto1 uses a crazy amount of RAM in static builds.
# Disable LTO for static gcc builds unless explicitly asked for.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT BUILD_SHARED_LIBS)
    set(RELEASE_LTO_DEFAULT OFF)
else()
    set(RELEASE_LTO_DEFAULT ON)
endif()

cryfox_option(ENABLE_ADDRESS_SANITIZER OFF CACHE BOOL "Enable address sanitizer testing in gcc/clang")
cryfox_option(ENABLE_MEMORY_SANITIZER OFF CACHE BOOL "Enable memory sanitizer testing in gcc/clang")
cryfox_option(ENABLE_FUZZERS OFF CACHE BOOL "Build fuzzing targets")
cryfox_option(ENABLE_FUZZERS_LIBFUZZER OFF CACHE BOOL "Build fuzzers using Clang's libFuzzer")
cryfox_option(ENABLE_FUZZERS_OSSFUZZ OFF CACHE BOOL "Build OSS-Fuzz compatible fuzzers")
cryfox_option(LAGOM_TOOLS_ONLY OFF CACHE BOOL "Don't build libraries, utilities and tests, only host build tools")
cryfox_option(ENABLE_LAGOM_CCACHE ON CACHE BOOL "Enable ccache for Lagom builds")
cryfox_option(LAGOM_USE_LINKER "" CACHE STRING "The linker to use (e.g. lld, mold) instead of the system default")
cryfox_option(LAGOM_LINK_POOL_SIZE "" CACHE STRING "The maximum number of parallel jobs to use for linking")
cryfox_option(ENABLE_LTO_FOR_RELEASE ${RELEASE_LTO_DEFAULT} CACHE BOOL "Enable link-time optimization for release builds")
cryfox_option(ENABLE_LAGOM_COVERAGE_COLLECTION OFF CACHE STRING "Enable code coverage instrumentation for lagom binaries in clang")

if (ANDROID OR APPLE)
    cryfox_option(ENABLE_QT OFF CACHE BOOL "Build cryfox application using Qt GUI")
else()
    cryfox_option(ENABLE_QT ON CACHE BOOL "Build cryfox application using Qt GUI")
endif()
