project ("NodeBrain")

# Source files
add_library (${PROJECT_NAME} "src/TestClass.h" "src/TestClass.cpp" )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Dependencies
find_package(Vulkan REQUIRED)
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")
add_subdirectory("thirdparty/ImGui")

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw spdlog Vulkan::Vulkan PUBLIC glm ImGui)