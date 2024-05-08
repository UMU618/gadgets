pushd "%~dp0"
cmake -B tmp -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -G "NMake Makefiles"
if %ERRORLEVEL% neq 0 goto skip
cmake --build tmp
:skip
popd
