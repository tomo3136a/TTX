cmake_minimum_required(VERSION 3.8)

set(PACKAGE_NAME "TTXPlugin")

project(${PACKAGE_NAME})

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  pluginlistdlg.c
  ttxplugin.c
  ttxplugin.rc
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
  version
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
