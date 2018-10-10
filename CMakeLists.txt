# RIAPS Timesync Service
#
# Copyright (C) Vanderbilt University, ISIS 2016-2017
#

cmake_minimum_required (VERSION 3.0.0)
project (riaps-timesync VERSION 1.1.1)


set(arch "amd64" CACHE STRING "amd64/armhf")
set(riaps_prefix "/opt/riaps/" CACHE STRING "the riaps prefix")

#Set the platform
if (${arch} STREQUAL "armhf")
	 set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)
	 set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
	 set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
	 set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
	 set(CMAKE_CXX_FLAGS "-std=c++11")
else()
	 set(CMAKE_C_COMPILER gcc)
	 set(CMAKE_CXX_COMPILER g++)
	 set(CMAKE_CXX_FLAGS "-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0")
endif()

set(CMAKE_INSTALL_PREFIX ${riaps_prefix}/${arch})


set(DEPENDENCIES ${riaps_prefix})
set (LIBALLPATH_INCLUDE ${DEPENDENCIES}/${arch}/include)
set (LIBALLPATH_LIB ${DEPENDENCIES}/${arch}/lib)
include_directories(${LIBALLPATH_INCLUDE})
link_directories(${LIBALLPATH_LIB})

# Debug binaries are to be copied into "./bin" directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_HOME_DIRECTORY}/bin)



add_subdirectory(doc)
#add_subdirectory(python)
include_directories(src)

add_library(riaps_ts SHARED src/riaps_ts.c src/chrony.c)
add_executable(test_timesync src/test_timesync.c)
target_link_libraries(test_timesync riaps_ts m)

install(TARGETS riaps_ts DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/riaps_ts
        FILES_MATCHING PATTERN "*.h")
install(PROGRAMS timesync/timesyncctl DESTINATION bin)
install(PROGRAMS timesync/timesyncd DESTINATION bin)
install(DIRECTORY timesync/config DESTINATION share/timesync)
