cmake_minimum_required(VERSION 3.8)

project(TTXDuration)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  verinfo.h
  ttxduration.c
  ttxduration.rc
  readme.md
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXDuration SHARED
  ${SRC}
  )

set_target_properties(
  TTXDuration
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXDuration
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
  TARGETS TTXDuration
  RUNTIME
  DESTINATION .
  )
