﻿cmake_minimum_required(VERSION 3.8)

project(TTXUserKey)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(USE_UNICODE_API)
  add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  resource.h
  ../common/ttxcommon.h
  ../common/ttxcommon.c
  verinfo.h
  ttxuserkey.c
  ttxuserkey.rc
  )

include_directories(
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

add_library(
  TTXUserKey SHARED
  ${SRC}
  )

set_target_properties(
  TTXUserKey
  PROPERTIES
  PREFIX ""
  )

target_link_libraries(
  TTXUserKey
  #
  gdi32
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

configure_file(
  userkey.txt
  ${CMAKE_BINARY_DIR}/data/userkey.txt
  )
