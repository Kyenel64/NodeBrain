project ("NodeBrain")

# Source files
add_library (${PROJECT_NAME} "src/NodeBrain.h" "src/NodeBrain.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Dependencies
find_package(Vulkan REQUIRED)
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")
# ImGui TODO: put in ImGui CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)
add_library (ImGui "${IMGUI_PATH}/imconfig.h" "${IMGUI_PATH}/imgui.h" "${IMGUI_PATH}/imgui.cpp" "${IMGUI_PATH}/imgui_demo.cpp" "${IMGUI_PATH}/imgui_draw.cpp"
    "${IMGUI_PATH}/imgui_internal.h" "${IMGUI_PATH}/imgui_tables.cpp" "${IMGUI_PATH}/imgui_widgets.cpp" "${IMGUI_PATH}/imstb_rectpack.h" "${IMGUI_PATH}/imstb_textedit.h"
    "${IMGUI_PATH}/imstb_truetype.h")
target_include_directories(ImGui PUBLIC ${IMGUI_PATH})

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw spdlog Vulkan::Vulkan PUBLIC glm ImGui)