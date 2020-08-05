cmake_minimum_required(VERSION 3.15)

project(CCSDS123ISSUE C)
include(cmake/PreventInSourceBuilds.cmake)

# enable cache system
include(cmake/Cache.cmake)
include(cmake/CompilerWarnings.cmake)

include_directories(src)
add_subdirectory(src binaries)
set_project_warnings(ccsds)
