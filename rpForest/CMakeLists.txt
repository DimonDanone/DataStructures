cmake_minimum_required(VERSION 3.15)
project(rpForest)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(rpForestlib)
add_subdirectory(rpForestlib)

add_executable(rpForestTest main.cpp log_duration.h)
target_link_libraries(rpForestTest rpForest)