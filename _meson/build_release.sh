vcpkg_target_triplet='x64-linux'
vcpkg_host_triplet='x64-linux'

export BOOST_INCLUDEDIR="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/include"
export BOOST_LIBRARYDIR="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/lib"

meson setup tmp/Release . --buildtype=release -Db_ndebug=true -Dpkg_config_path="$VCPKG_ROOT/installed/${vcpkg_target_triplet}/lib/pkgconfig"
# meson compile -C tmp/Release
meson install -C tmp/Release
