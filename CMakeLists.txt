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
include_directories(nanopb)
add_subdirectory(includes)
add_subdirectory(sources)

set(nanopb_dir
  ${CMAKE_CURRENT_SOURCE_DIR}/nanopb
  )

set(PROTO_SRCS
  ${nanopb_dir}/pb_common.c
  ${nanopb_dir}/pb_encode.c
  ${nanopb_dir}/pb_decode.c
  )

set(PROTO_HDRS
  ${nanopb_dir}/pb_common.h
  ${nanopb_dir}/pb_encode.h
  ${nanopb_dir}/pb_decode.h
  )

add_executable(NetTempReader ${tempreader_HEADERS} ${tempreader_SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
