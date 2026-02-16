$vcpkg_target_triplet = 'x64-windows'
$vcpkg_host_triplet = 'x64-windows'

$env:BOOST_INCLUDEDIR = "$env:VCPKG_ROOT\installed\${vcpkg_target_triplet}\include"
$env:BOOST_LIBRARYDIR = "$env:VCPKG_ROOT\installed\${vcpkg_target_triplet}\debug\lib"
$env:PKG_CONFIG = "$env:VCPKG_ROOT\installed\${vcpkg_host_triplet}\tools\pkgconf\pkgconf.exe"

meson setup tmp\Debug . --buildtype=debug -Dpkg_config_path="$env:VCPKG_ROOT\installed\${vcpkg_target_triplet}\debug\lib\pkgconfig"
# meson compile -C tmp\Debug
meson install -C tmp\Debug
