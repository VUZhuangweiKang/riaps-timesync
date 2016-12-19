# RIAPS Timesync Service
#
# Copyright (C) Vanderbilt University, ISIS 2016
#

cmake_minimum_required (VERSION 2.8)
project (riaps-timesync)

# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

add_subdirectory(doc)
include_directories(src)

add_library(riaps_ts SHARED src/riaps_ts.c src/chrony.c)
add_executable(test_timesync src/test_timesync.c)
target_link_libraries(test_timesync riaps_ts m)

install(TARGETS riaps_ts DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/riaps_ts
        FILES_MATCHING PATTERN "*.h")
