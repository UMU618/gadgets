cmake_minimum_required (VERSION 3.19)

add_executable(sln2slnx ../../src/umutech/sln2slnx/sln2slnx.cpp)
set_property(TARGET sln2slnx PROPERTY CXX_STANDARD 20)

find_package(Boost 1.88.0 REQUIRED COMPONENTS algorithm filesystem nowide)

target_link_libraries(sln2slnx PRIVATE Boost::filesystem Boost::nowide)
