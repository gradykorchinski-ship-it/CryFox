#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

CRYFOX_SOURCE_DIR="$(realpath "${DIR}"/../..)"

# shellcheck source=/dev/null
. "${CRYFOX_SOURCE_DIR}/Meta/shell_include.sh"

# shellcheck source=/dev/null
. "${CRYFOX_SOURCE_DIR}/Meta/find_compiler.sh"

pick_host_compiler

BUILD_DIR=${BUILD_DIR:-"${CRYFOX_SOURCE_DIR}/Build"}
CACHE_DIR=${CACHE_DIR:-"${BUILD_DIR}/caches"}

# HACK: This export of XDG_CACHE_HOME is required to make vcpkg happy.
# This is because vcpkg tries to find a cache directory by:
#   1) checking $XDG_CACHE_HOME
#   2) appending "/.cache" to $HOME
# The problem is, in the Android build environment, neither of those environment variables are set.
# This causes vcpkg to fail; so, we set a dummy $XDG_CACHE_HOME, ensuring that vcpkg is happy.
# (Note that vcpkg appends "/vcpkg" to the cache directory we give it.)
# (And this also works on macOS, despite the fact that $XDG_CACHE_HOME is a Linux-ism.)
export XDG_CACHE_HOME="$CACHE_DIR"

"$CRYFOX_SOURCE_DIR"/Meta/cryfox.py vcpkg

cmake -S "${CRYFOX_SOURCE_DIR}/Meta/Lagom" -B "$BUILD_DIR/lagom-tools" \
    -GNinja -Dpackage=LagomTools \
    -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/lagom-tools-install"  \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCRYFOX_CACHE_DIR="$CACHE_DIR" \
    -DBUILD_SHARED_LIBS=OFF \
    -DLAGOM_TOOLS_ONLY=ON \
    -DINSTALL_LAGOM_TOOLS=ON \
    -DCMAKE_TOOLCHAIN_FILE="$CRYFOX_SOURCE_DIR/Build/vcpkg/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_INSTALL_OPTIONS="--no-print-usage" \
    -DVCPKG_OVERLAY_TRIPLETS="$CRYFOX_SOURCE_DIR/Meta/CMake/vcpkg/release-triplets" \
    -DVCPKG_ROOT="$CRYFOX_SOURCE_DIR/Build/vcpkg" \
    -DVCPKG_MANIFEST_DIR="$CRYFOX_SOURCE_DIR"

ninja -C "$BUILD_DIR/lagom-tools" install
