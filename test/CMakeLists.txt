add_subdirectory("thirdparty/googletest")

project("NBTest")

add_executable(${PROJECT_NAME} Core/Test_Timer.cc Core/Test_App.cc Core/Test_Window.cc Core/Test_Event.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

target_link_libraries(${PROJECT_NAME} gtest gtest_main NodeBrain)
add_test( ${PROJECT_NAME} ${PROJECT_NAME} )