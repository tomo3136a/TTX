cmake_minimum_required(VERSION 3.8)

set(PACKAGE_NAME "TTXCommon")

project(${PACKAGE_NAME})

if(TT4)
add_definitions(-DTT4)
else()
add_definitions(-DUNICODE -D_UNICODE)
endif()

set(SRC
  ttxcommon.c
  ttxcmn_ui.c
  ttxcmn_string.c
  ttxversion.c
  stringlist.c
  infoview.c
  )

add_library(
  ${PACKAGE_NAME} STATIC
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
  )

target_link_libraries(
  ${PACKAGE_NAME}
  PRIVATE
  #
  gdi32
  )
