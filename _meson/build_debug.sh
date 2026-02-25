vcpkg_target_triplet='x64-linux'
vcpkg_host_triplet='x64-linux'

export BOOST_INCLUDEDIR="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/include"
export BOOST_LIBRARYDIR="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/debug/lib"

meson setup tmp/Debug . --buildtype=debug -Dpkg_config_path="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/debug/lib/pkgconfig"
# meson compile -C tmp/Debug
meson install -C tmp/Debug
