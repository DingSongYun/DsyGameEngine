cmake_minimum_required(VERSION 3.1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project(DsyGameEngine)
include_directories("${PROJECT_SOURCE_DIR}/")
include_directories("${PROJECT_SOURCE_DIR}/Framework")
add_subdirectory(Framework)
add_subdirectory(Empty)