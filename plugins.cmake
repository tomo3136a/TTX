project(TTX VERSION 1.1.0)

add_subdirectory(../TTX/common TTXCommon)
set_target_properties(TTXCommon PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXPlugin TTXPlugin)
set_target_properties(TTXPlugin PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXShortcut TTXShortcut)
set_target_properties(TTXShortcut PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXUserKey TTXUserKey)
set_target_properties(TTXUserKey PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXReconnect TTXReconnect)
set_target_properties(TTXReconnect PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXEnv TTXEnv)
set_target_properties(TTXEnv PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXDuration TTXDuration)
set_target_properties(TTXDuration PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXPackage TTXPackage)
set_target_properties(TTXPackage PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXReport TTXReport)
set_target_properties(TTXReport PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/ttxtest ttxtest)
set_target_properties(ttxtest PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXTest_Exports TTXTest_Exports)
set_target_properties(TTXTest_Exports PROPERTIES FOLDER TTX)

add_subdirectory(../TTX/TTXTest_TTTypes TTXTest_TTTypes)
set_target_properties(TTXTest_TTTypes PROPERTIES FOLDER TTX)
