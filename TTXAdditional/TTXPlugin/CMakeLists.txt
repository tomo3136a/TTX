cmake_minimum_required(VERSION 3.8)

project(TTXPlugin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  resource.h
  ../common/ttxcommon.h
  ../common/ttxcommon.c
  verinfo.h
  ttxplugin.c
  ttxplugin.rc
  readme.md
  )

include_directories(
  ../../../teraterm/teraterm
  ../../../teraterm/common
  ../common
  )

add_library(
  TTXPlugin SHARED
  ${SRC}
  )

set_target_properties(
  TTXPlugin
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXPlugin
  #
  version
  gdi32
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXPlugin
  RUNTIME
  DESTINATION .
  )
