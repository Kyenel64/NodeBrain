project ("NodeBrain")

set (ImGuiSrcFiles 
"thirdparty/ImGui/imconfig.h" "thirdparty/ImGui/imgui.h" "thirdparty/ImGui/imgui.cpp" "thirdparty/ImGui/imgui_demo.cpp" 
"thirdparty/ImGui/imgui_draw.cpp" "thirdparty/ImGui/imgui_internal.h" "thirdparty/ImGui/imgui_tables.cpp" 
"thirdparty/ImGui/imgui_widgets.cpp" "thirdparty/ImGui/imstb_rectpack.h" "thirdparty/ImGui/imstb_textedit.h" "thirdparty/ImGui/imstb_truetype.h"
"thirdparty/ImGui/backends/imgui_impl_vulkan.h" "thirdparty/ImGui/backends/imgui_impl_vulkan.cpp"
"thirdparty/ImGui/backends/imgui_impl_glfw.h" "thirdparty/ImGui/backends/imgui_impl_glfw.cpp")

set (CoreFiles 
"src/Core/App.h" "src/Core/App.cpp"
"src/Core/Layer.h"
"src/Core/Timer.h"
"src/Core/Window.h" "src/Core/Window.cpp"
"src/Core/Log.h" "src/Core/Log.cpp"
"src/Core/Event.h"
"src/Core/KeyCode.h"
"src/Core/Input.h" "src/Core/Input.cpp"
"src/Core/Instrumentor.h" "src/Core/Instrumentor.cpp"
"src/Core/ImGuiLayer.h")

set (VulkanFiles 
"src/GAPI/Vulkan/VulkanRendererAPI.h" "src/GAPI/Vulkan/VulkanRendererAPI.cpp"
"src/GAPI/Vulkan/VulkanPhysicalDevice.h" "src/GAPI/Vulkan/VulkanPhysicalDevice.cpp"
"src/GAPI/Vulkan/VulkanDevice.h" "src/GAPI/Vulkan/VulkanDevice.cpp"
"src/GAPI/Vulkan/VulkanSwapchain.h" "src/GAPI/Vulkan/VulkanSwapchain.cpp" 
"src/GAPI/Vulkan/VulkanRenderContext.h" "src/GAPI/Vulkan/VulkanRenderContext.cpp"
"src/GAPI/Vulkan/VulkanImage.h" "src/GAPI/Vulkan/VulkanImage.cpp"
"src/GAPI/Vulkan/VulkanShader.h" "src/GAPI/Vulkan/VulkanShader.cpp"
"src/GAPI/Vulkan/VulkanGraphicsPipeline.h" "src/GAPI/Vulkan/VulkanGraphicsPipeline.cpp"
"src/GAPI/Vulkan/VulkanUtils.h" "src/GAPI/Vulkan/VulkanUtils.cpp"
"src/GAPI/Vulkan/VulkanComputePipeline.h" "src/GAPI/Vulkan/VulkanComputePipeline.cpp"
"src/GAPI/Vulkan/VulkanImGuiLayer.h" "src/GAPI/Vulkan/VulkanImGuiLayer.cpp"
"src/GAPI/Vulkan/VulkanVertexBuffer.h" "src/GAPI/Vulkan/VulkanVertexBuffer.cpp"
"src/GAPI/Vulkan/VulkanIndexBuffer.h" "src/GAPI/Vulkan/VulkanIndexBuffer.cpp"
"src/GAPI/Vulkan/VulkanUniformBuffer.h" "src/GAPI/Vulkan/VulkanUniformBuffer.cpp"
"src/GAPI/Vulkan/VulkanDescriptorSet.h" "src/GAPI/Vulkan/VulkanDescriptorSet.cpp")

set (RendererFiles 
"src/Renderer/Renderer.h" "src/Renderer/Renderer.cpp"
"src/Renderer/RendererAPI.h"
"src/Renderer/RenderContext.h"
"src/Renderer/Shader.h" "src/Renderer/Shader.cpp"
"src/Renderer/GraphicsPipeline.h" "src/Renderer/GraphicsPipeline.cpp"
"src/Renderer/Image.h" "src/Renderer/Image.cpp"
"src/Renderer/ComputePipeline.h" "src/Renderer/ComputePipeline.cpp"
"src/Renderer/VertexBuffer.h" "src/Renderer/VertexBuffer.cpp"
"src/Renderer/IndexBuffer.h" "src/Renderer/IndexBuffer.cpp"
"src/Renderer/UniformBuffer.h" "src/Renderer/UniformBuffer.cpp"
"src/Renderer/DescriptorSet.h" "src/Renderer/DescriptorSet.cpp"
"src/Renderer/EditorCamera.h" "src/Renderer/EditorCamera.cpp")

set (SceneFiles
"src/Scene/Scene.h" "src/Scene/Scene.cpp"
"src/Scene/Entity.h"
"src/Scene/Component.h"
"src/Scene/EntityGraph.h" "src/Scene/EntityGraph.cpp"
"src/Scene/Node.h")

# Source files
add_library ( ${PROJECT_NAME} 
    ${CoreFiles}
    ${RendererFiles}
    ${VulkanFiles}
    ${ImGuiSrcFiles}
    ${SceneFiles}
    "thirdparty/VMA/vk_mem_alloc.h" "thirdparty/VMA/vk_mem_alloc.cpp"
    "src/Utils/FileUtils.h" "src/Utils/FileUtils.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/NBpch.h")

# Dependencies
find_package(Vulkan REQUIRED)
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")
add_subdirectory("thirdparty/EnTT")

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGui/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/EnTT/single_include)

# Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw Vulkan::Vulkan EnTT spdlog glm)

# Defines
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:NB_DEBUG>)
target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:NB_RELEASE>)
IF (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NB_WINDOWS)
ELSEIF (APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NB_APPLE)
ELSEIF (LINUX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NB_LINUX)
ENDIF()

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_DEFINE_MATH_OPERATORS)