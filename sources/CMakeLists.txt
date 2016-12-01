cmake_minimum_required(VERSION 3.0)
project(NetTempReader)

#   ____
#  / ___|___  _ __ ___
# | |   / _ \| '__/ _ \
# | |__| (_) | | |  __/
#  \____\___/|_|  \___|
#

set(tempreader_src_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(tempreader_SOURCES
  ${tempreader_src_ROOT}/AirSensor.pb.c
  ${tempreader_src_ROOT}/main.cpp)

set(tempreader_SOURCES
  ${tempreader_SOURCES}
  PARENT_SCOPE)
