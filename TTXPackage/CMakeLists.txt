cmake_minimum_required(VERSION 3.8)

project(TTXPackage)

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
  ttxpackage.c
  ttxpackage.rc
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXPackage SHARED
  ${SRC}
  )

set_target_properties(
  TTXPackage
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXPackage
  #
  gdi32
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS TTXPackage
  RUNTIME
  DESTINATION .
  )
