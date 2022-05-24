cmake_minimum_required(VERSION 3.8)

set(PACKAGE_NAME "TTXEstimateMismatch")

project(${PACKAGE_NAME})

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
  ttxestimatemismatch.c
  test_v${TT_VERSION}.c
  )

add_library(
  ${PACKAGE_NAME} SHARED
  ${SRC}
  )

set_target_properties(
  ${PACKAGE_NAME}
  PROPERTIES
  PREFIX ""
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"
  )

target_include_directories(
  ${PACKAGE_NAME}
  PRIVATE
  ../../teraterm/teraterm
  ../../teraterm/common
  ../common
  )

target_link_libraries(
  ${PACKAGE_NAME}
  PRIVATE
  TTXCommon
  )

configure_file(
  ../common/verinfo.h.in
  verinfo.h
  @ONLY
  )

install(
  TARGETS ${PACKAGE_NAME}
  RUNTIME
  DESTINATION .
  )
