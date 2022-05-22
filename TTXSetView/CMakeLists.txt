cmake_minimum_required(VERSION 3.8)

project(TTXSetView)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  resource.h
  verinfo.h
  ttxsetview.c
  ttxsetview.rc
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXSetView SHARED
  ${SRC}
  )

set_target_properties(
  TTXSetView
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXSetView
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
  TARGETS TTXSetView
  RUNTIME
  DESTINATION .
  )
