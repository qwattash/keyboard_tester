set(CMAKE_TOOLCHAIN_FILE arduino-cmake/cmake/ArduinoToolchain.cmake)
cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(qkboard-prototype C CXX ASM)

# print_board_list()
# print_programmer_list()
#set(ARDUINO_CXX_FLAGS "${ARDUINO_CXX_FLAGS} -std=c++11 -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS "${ARDUINO_CXX_FLAGS} -std=c++14 -fno-threadsafe-statics -O3")

set(ARDUINO_DEFAULT_BOARD qkboard)
set(ARDUINO_DEFAULT_PORT /dev/ttyACM3)

set(sketch_SRCS
  "init.cpp"
  )

generate_avr_firmware(kboard
  BOARD qkboard
  BOARD_CPU atmega32u4
  SRCS ${sketch_SRCS}
  PORT "/dev/ttyACM3"
  SERIAL picocom @SERIAL_PORT@ -b 9600 -l
  )
