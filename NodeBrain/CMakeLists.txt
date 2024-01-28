# NodeBrain Exe
add_executable (NodeBrain "src/NodeBrain.cpp" "src/NodeBrain.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NodeBrain PROPERTY CXX_STANDARD 20)
endif()
find_package(Vulkan REQUIRED)
# Library
target_link_libraries(${PROJECT_NAME} glm glfw spdlog ImGui Vulkan::Vulkan)