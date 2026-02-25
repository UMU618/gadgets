if ($env:VCPKG_ROOT -eq "") {
    echo VCPKG_ROOT is not set!
    Exit-PSHostProcess
}

if (-not (Test-Path (Join-Path $env:VCPKG_ROOT "vcpkg.exe"))) {
    echo vcpkg.exe is not found!
    Exit-PSHostProcess
}

cmake -S . -B tmp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
if (-not $?) {
    exit $LASTEXITCODE
}
cmake --build tmp
