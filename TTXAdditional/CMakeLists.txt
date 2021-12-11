cmake_minimum_required(VERSION 3.8)

project(TTX VERSION 1.0.6)

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
