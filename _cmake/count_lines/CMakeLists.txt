cmake_minimum_required (VERSION 3.19)

add_executable(count_lines ../../src/count_lines/count_lines.cpp)
set_property(TARGET count_lines PROPERTY CXX_STANDARD 20)

add_definitions(-DUSE_FMTLIB)

find_package(Boost 1.88.0 REQUIRED COMPONENTS algorithm filesystem nowide program_options)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(count_lines PRIVATE Boost::filesystem Boost::nowide Boost::program_options)
#target_link_libraries(count_lines PRIVATE fmt::core fmt::format)
target_link_libraries(count_lines PRIVATE fmt::fmt-header-only)
