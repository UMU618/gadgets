cmake_minimum_required (VERSION 3.19)
project (gadgets)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

add_subdirectory(count_lines)
add_subdirectory(process_dump)
