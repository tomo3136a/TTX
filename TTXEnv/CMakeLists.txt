cmake_minimum_required(VERSION 3.8)

project(TTXEnv)

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
  ttxenv.c
  ttxenv.rc
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXEnv SHARED
  ${SRC}
  )

set_target_properties(
  TTXEnv
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXEnv
  #
  gdi32
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXEnv
  RUNTIME
  DESTINATION .
  )
