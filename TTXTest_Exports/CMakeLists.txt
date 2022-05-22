cmake_minimum_required(VERSION 3.8)

project(TTXTest_Exports)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  verinfo.h
  ttxtest_exports.c
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXTest_Exports SHARED
  ${SRC}
  )

set_target_properties(
  TTXTest_Exports
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXTest_Exports
  #
  gdi32
  ttpcmn
  TTXCommon
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXTest_Exports
  RUNTIME
  DESTINATION .
  )
