cmake_minimum_required(VERSION 3.0)
project(Joystick VERSION 1.0)

set(offboard_SOURCES
  src/Joystick.cc
  src/Serial.cc)

add_definitions(-DUSE_LOGITECH)
add_executable(fc_offboard framework/offboard_framework.cc ${offboard_SOURCES})

target_include_directories(fc_offboard PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
