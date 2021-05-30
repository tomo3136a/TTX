cmake_minimum_required(VERSION 3.8)

project(TTX VERSION 1.0.5)

add_subdirectory(TTXPlugin)
set_target_properties(TTXPlugin PROPERTIES FOLDER TTX)

add_subdirectory(TTXEstimateMismatch)
set_target_properties(TTXEstimateMismatch PROPERTIES FOLDER TTX)

add_subdirectory(TTXShortcut)
set_target_properties(TTXShortcut PROPERTIES FOLDER TTX)

add_subdirectory(TTXUserKey)
set_target_properties(TTXUserKey PROPERTIES FOLDER TTX)

add_subdirectory(TTXReconnect)
set_target_properties(TTXReconnect PROPERTIES FOLDER TTX)

add_subdirectory(TTXEnv)
set_target_properties(TTXEnv PROPERTIES FOLDER TTX)

add_subdirectory(TTXDuration)
set_target_properties(TTXDuration PROPERTIES FOLDER TTX)

add_subdirectory(TTXPackage)
set_target_properties(TTXPackage PROPERTIES FOLDER TTX)

add_subdirectory(TTXReport)
set_target_properties(TTXReport PROPERTIES FOLDER TTX)

configure_file(
  LICENSE
  data/license/${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}/LICENSE
  @ONLY
  )
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Setup/Setup.vdproj.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Setup/Setup.vdproj
  @ONLY
  )

install(
  DIRECTORY
  "${CMAKE_CURRENT_BINARY_DIR}/Release/"
  "${CMAKE_CURRENT_BINARY_DIR}/data/"
  DESTINATION .
)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
