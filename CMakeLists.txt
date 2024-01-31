cmake_minimum_required (VERSION 3.27)

project ("NodeBrain")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BrainEditor)
enable_testing()

# Disable warnings
if (MSVC)
    add_definitions("/wd4996")
ENDIF()

# Variables
set(NODEBRAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/NodeBrain)
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/NodeBrain/thirdparty/ImGui)

# Include sub-projects.
add_subdirectory("NodeBrain")
add_subdirectory("BrainEditor")
add_subdirectory("test")