cmake_minimum_required(VERSION 3.8)

project(TTXCommon)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  ttxcommon.h
  ttxcommon.c
  ttxversion.c
  stringlist.c
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  )

add_library(
  TTXCommon STATIC
  ${SRC}
  )

set_target_properties(
  TTXCommon
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXCommon
  #
  gdi32
  )
