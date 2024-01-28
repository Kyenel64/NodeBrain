project ("BrainEditor")

# Source files
add_executable(${PROJECT_NAME} "src/main.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

# Libraries
target_link_libraries(${PROJECT_NAME} NodeBrain)