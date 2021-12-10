cmake_minimum_required(VERSION 3.8)

project(TTXEstimateMismatch)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

if(TT4)
set(TT_VERSION "4")
else()
set(TT_VERSION "5")
endif()

set(SRC
  ../common/ttxcommon.h
  ../common/ttxcommon.c
  ttxestimatemismatch.c
  test_v${TT_VERSION}.c
  readme.md
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXEstimateMismatch SHARED
  ${SRC}
  )

set_target_properties(
  TTXEstimateMismatch
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXEstimateMismatch
  #
  version
  gdi32
  )

install(
  TARGETS TTXEstimateMismatch
  RUNTIME
  DESTINATION .
  )
  