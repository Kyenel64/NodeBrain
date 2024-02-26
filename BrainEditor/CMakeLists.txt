project ("BrainEditor")

# Source files
add_executable(${PROJECT_NAME} "src/main.cpp" "src/BrainEditor.h" "src/BrainEditor.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

# Libraries
target_link_libraries(${PROJECT_NAME} NodeBrain)

# Post build commands
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Assets ${PROJECT_BINARY_DIR}/Assets)