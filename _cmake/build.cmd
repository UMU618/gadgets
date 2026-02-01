if "%VCPKG_ROOT%"=="" (
    echo VCPKG_ROOT is not set!
    exit /b 1
)

if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo vcpkg.exe is not found!
    exit /b 2
)

pushd "%~dp0"
cmake -B tmp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -G "NMake Makefiles JOM"
if %ERRORLEVEL% neq 0 goto skip
cmake --build tmp
:skip
popd
