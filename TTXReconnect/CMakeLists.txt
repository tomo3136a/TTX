cmake_minimum_required(VERSION 3.8)

project(TTXReconnect)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(USE_UNICODE_API)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  ../common/ttxcommon.h
  ../common/ttxcommon.c
  ../common/ttxversion.h
  ../common/ttxversion.c
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
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )
