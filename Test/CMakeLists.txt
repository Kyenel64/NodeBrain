add_subdirectory("thirdparty/googletest")

project("NBTest")

add_executable(${PROJECT_NAME} 
    "src/Core/Test_Timer.cc"
    "src/Core/Test_Input.cc"
    "src/Core/Test_Event.cc"

    "src/Core/Test_Window.cc"
    "src/Core/Test_App.cc")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

target_link_libraries(${PROJECT_NAME} gtest gtest_main NodeBrain)
add_test( ${PROJECT_NAME} ${PROJECT_NAME} )