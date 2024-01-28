# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NodeBrain")

# Include sub-projects.
add_subdirectory("NodeBrain")
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")

# ImGui TODO: put in ImGui CMakeLists.txt
set(IMGUI "thirdparty/ImGui")
add_library (ImGui STATIC "${IMGUI}/imconfig.h" "${IMGUI}/imgui.h" "${IMGUI}/imgui.cpp" "${IMGUI}/imgui_demo.cpp" "${IMGUI}/imgui_draw.cpp"
    "${IMGUI}/imgui_internal.h" "${IMGUI}/imgui_tables.cpp" "${IMGUI}/imgui_widgets.cpp" "${IMGUI}/imstb_rectpack.h" "${IMGUI}/imstb_textedit.h"
    "${IMGUI}/imstb_truetype.h")
target_include_directories(ImGui PUBLIC ${IMGUI})