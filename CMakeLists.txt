cmake_minimum_required (VERSION 3.28)

project ("NodeBrain")

# Variables
set(NODEBRAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/NodeBrain)
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/NodeBrain/thirdparty/ImGui)

# Include sub-projects.
add_subdirectory("NodeBrain")
add_subdirectory("BrainEditor")