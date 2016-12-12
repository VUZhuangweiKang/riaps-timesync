
cmake_minimum_required (VERSION 2.8)
project (riaps-timesync)

add_library (riaps_ts SHARED riaps_ts.c)
add_executable(test_ts test.c)
target_link_libraries (test_ts riaps_ts)