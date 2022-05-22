cmake_minimum_required(VERSION 3.8)

project(TTXReconnect)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  verinfo.h
  ttxreconnect.c
  ttxreconnect.rc
  readme.md
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXReconnect SHARED
  ${SRC}
  )

set_target_properties(
  TTXReconnect
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXReconnect
  #
  version
  gdi32
  TTXCommon
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXReconnect
  RUNTIME
  DESTINATION .
  )
