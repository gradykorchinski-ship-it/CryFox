#!/bin/sh

set -e

if [ -z "$CRYFOX_SOURCE_DIR" ]
then
    CRYFOX_SOURCE_DIR="$(git rev-parse --show-toplevel)"
    echo "CryFox root not set. This is fine! Other scripts may require you to set the environment variable first, e.g.:"
    echo "    export CRYFOX_SOURCE_DIR=${CRYFOX_SOURCE_DIR}"
fi

cd "${CRYFOX_SOURCE_DIR}"

find . \( -name Base -o -name Patches -o -name Ports -o -name Root -o -name Toolchain -o -name Build \) -prune -o \( -name '*.ipc' -or -name '*.cpp' -or -name '*.idl' -or -name '*.h' -or -name '*.in' -or -name '*.css' -or -name '*.cmake' -or -name '*.json' -or -name 'CMakeLists.txt' \) -print > cryfox.files
