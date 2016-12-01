cmake_minimum_required(VERSION 3.0)
project(NetTempReader)

#   ____
#  / ___|___  _ __ ___
# | |   / _ \| '__/ _ \
# | |__| (_) | | |  __/
#  \____\___/|_|  \___|
#

set(tempreader_inc_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(tempreader_HEADERS
  ${tempreader_inc_ROOT}/AirSensor.pb.h
  ${tempreader_inc_ROOT}/Defines.h
  ${tempreader_inc_ROOT}/socket.hpp)
  
set(tempreader_HEADERS
  ${tempreader_HEADERS}
  PARENT_SCOPE)
