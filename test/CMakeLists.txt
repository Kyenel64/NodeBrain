project("NBTest")

add_subdirectory("thirdparty/googletest")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(${PROJECT_NAME} Core/Test_Timer.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

target_link_libraries(${PROJECT_NAME} gtest gtest_main NodeBrain)
add_test( ${PROJECT_NAME} ${PROJECT_NAME} )