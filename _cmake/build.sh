#!/usr/bin/env bash

if [ -z "$VCPKG_ROOT" ]; then
    echo "VCPKG_ROOT is not set!"
    exit 1
fi

if [ ! -f "${VCPKG_ROOT}/vcpkg" ]; then
    echo "vcpkg is not found!"
    exit 2
fi

pushd "$(dirname "$0")"
cmake -B tmp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -G Ninja
if [ $? -eq 0 ]; then
    cmake --build tmp
fi
popd
