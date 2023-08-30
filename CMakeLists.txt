cmake_minimum_required(VERSION 3.0)
project(Joystick VERSION 1.0)

find_package(Threads REQUIRED)
find_package(serial REQUIRED)

set(offboard_SOURCES
  src/Joystick.cc)

add_definitions(-DUSE_LOGITECH)
add_executable(fc_offboard framework/offboard_framework.cc ${offboard_SOURCES})

target_include_directories(fc_offboard PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(fc_offboard serial::serial Threads::Threads)

install(TARGETS serial
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
