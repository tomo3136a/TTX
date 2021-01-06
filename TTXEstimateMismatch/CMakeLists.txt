cmake_minimum_required(VERSION 3.8)

project(TTXEstimateMismatch)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(USE_UNICODE_API)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  ../common/ttxcommon.h
  ../common/ttxcommon.c
  ttxestimatemismatch.c
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
