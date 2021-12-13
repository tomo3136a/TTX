cmake_minimum_required(VERSION 3.8)

project(TTXReport)

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
  ../common/stringlist.h
  ../common/stringlist.c
  verinfo.h
  ttxreport.c
  ttxreport.rc
  )

include_directories(
  ../../../teraterm/teraterm
  ../../../teraterm/common
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXReport SHARED
  ${SRC}
  )

set_target_properties(
  TTXReport
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXReport
  #
  gdi32
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXReport
  RUNTIME
  DESTINATION .
  )
