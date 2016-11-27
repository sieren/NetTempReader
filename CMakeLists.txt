cmake_minimum_required(VERSION 3.2)

project (NetTempReader)



# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


#  ____       _   _
# |  _ \ __ _| |_| |__  ___
# | |_) / _` | __| '_ \/ __|
# |  __/ (_| | |_| | | \__ \
# |_|   \__,_|\__|_| |_|___/
#

include_directories(includes)
add_subdirectory(includes)
add_subdirectory(sources)

add_executable(NetTempReader ${tempreader_HEADERS} ${tempreader_SOURCES})
  