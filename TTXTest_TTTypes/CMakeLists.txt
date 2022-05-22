cmake_minimum_required(VERSION 3.8)

project(TTXTest_TTTypes)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXTest_TTTypes SHARED
  ttxtest_tttypes.c
 )

set_target_properties(
  TTXTest_TTTypes
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXTest_TTTypes
  #
  gdi32
  TTXCommon
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXTest_TTTypes
  RUNTIME
  DESTINATION .
  )
