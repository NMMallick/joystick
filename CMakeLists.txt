cmake_minimum_required(VERSION 3.0)
project(Joystick VERSION 1.0)

if(USE_LOGITECH)
  add_compile_definitions(-DUSE_LOGITECH)
else()
  add_compile_definitions(-DUSE_XBOX)
endif()

add_executable(joystick src/Joystick.cc)
add_executable(serial src/Serial.cc)

