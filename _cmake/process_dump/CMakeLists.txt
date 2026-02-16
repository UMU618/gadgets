cmake_minimum_required (VERSION 3.19)

add_executable(process_dump ../../src/process_dump/process_dump.cpp)
set_property(TARGET process_dump PROPERTY CXX_STANDARD 20)

add_definitions(-DUSE_FMTLIB)

find_package(Boost 1.88.0 REQUIRED COMPONENTS algorithm filesystem nowide)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(process_dump PRIVATE Boost::filesystem Boost::nowide)
