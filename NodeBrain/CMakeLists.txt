project ("NodeBrain")

set (ImGuiSrcFiles "thirdparty/ImGui/imconfig.h" "thirdparty/ImGui/imgui.h" "thirdparty/ImGui/imgui.cpp" "thirdparty/ImGui/imgui_demo.cpp" 
"thirdparty/ImGui/imgui_draw.cpp" "thirdparty/ImGui/imgui_internal.h" "thirdparty/ImGui/imgui_tables.cpp" 
"thirdparty/ImGui/imgui_widgets.cpp" "thirdparty/ImGui/imstb_rectpack.h" "thirdparty/ImGui/imstb_textedit.h" "thirdparty/ImGui/imstb_truetype.h"
"thirdparty/ImGui/backends/imgui_impl_vulkan.h" "thirdparty/ImGui/backends/imgui_impl_vulkan.cpp"
"thirdparty/ImGui/backends/imgui_impl_glfw.h" "thirdparty/ImGui/backends/imgui_impl_glfw.cpp")

set (CoreFiles "src/Core/App.h" "src/Core/App.cpp" "src/Core/Layer.h" "src/Core/Timer.h" "src/Core/Window.h" 
"src/Core/Window.cpp"  "src/Core/Log.h" "src/Core/Log.cpp" "src/Core/Event.h" "src/Core/KeyCode.h" "src/Core/Input.h" "src/Core/Input.cpp" 
"src/Core/Instrumentor.h" "src/Core/Instrumentor.cpp" "src/Core/ImGuiLayer.h" "src/Core/ImGuiLayer.cpp" "src/Core/ImGuiBackends.cpp")

set (VulkanFiles "src/GAPI/Vulkan/VulkanRendererAPI.h" "src/GAPI/Vulkan/VulkanRendererAPI.cpp" 
"src/GAPI/Vulkan/VulkanPhysicalDevice.h" "src/GAPI/Vulkan/VulkanPhysicalDevice.cpp"  "src/GAPI/Vulkan/VulkanDevice.h" 
"src/GAPI/Vulkan/VulkanDevice.cpp" "src/GAPI/Vulkan/VulkanSwapchain.h" "src/GAPI/Vulkan/VulkanSwapchain.cpp" 
"src/GAPI/Vulkan/VulkanRenderContext.h" "src/GAPI/Vulkan/VulkanRenderContext.cpp" "src/GAPI/Vulkan/VulkanImage.h" 
"src/GAPI/Vulkan/VulkanImage.cpp" "src/GAPI/Vulkan/VulkanShader.h" "src/GAPI/Vulkan/VulkanShader.cpp" 
"src/GAPI/Vulkan/VulkanRenderPass.h" "src/GAPI/Vulkan/VulkanRenderPass.cpp" "src/GAPI/Vulkan/VulkanGraphicsPipeline.h" 
"src/GAPI/Vulkan/VulkanGraphicsPipeline.cpp" "src/GAPI/Vulkan/VulkanFramebuffer.h" "src/GAPI/Vulkan/VulkanFramebuffer.cpp"
"src/GAPI/Vulkan/VulkanUtils.h" "src/GAPI/Vulkan/VulkanAllocator.h" "src/GAPI/Vulkan/VulkanAllocator.cpp"
"src/GAPI/Vulkan/VulkanComputePipeline.h" "src/GAPI/Vulkan/VulkanComputePipeline.cpp"
"src/GAPI/Vulkan/VulkanImGuiLayer.h" "src/GAPI/Vulkan/VulkanImGuiLayer.cpp")

set (RendererFiles "src/Renderer/Renderer.h" "src/Renderer/Renderer.cpp" "src/Renderer/RendererAPI.h" "src/Renderer/RendererAPI.cpp" 
"src/Renderer/RenderContext.h" "src/Renderer/RenderContext.cpp" "src/Renderer/Shader.h" "src/Renderer/Shader.cpp" 
"src/Renderer/RenderPass.h" "src/Renderer/RenderPass.cpp" "src/Renderer/GraphicsPipeline.h" "src/Renderer/GraphicsPipeline.cpp" 
"src/Renderer/Framebuffer.h" "src/Renderer/Framebuffer.cpp" "src/Renderer/Image.h" "src/Renderer/Image.cpp"
"src/Renderer/ComputePipeline.h" "src/Renderer/ComputePipeline.cpp")

# Source files
add_library ( ${PROJECT_NAME} 
    ${CoreFiles}
    ${RendererFiles}
    ${VulkanFiles}
    ${ImGuiFiles} 
    "thirdparty/VMA/vk_mem_alloc.h" "thirdparty/VMA/vk_mem_alloc.cpp" 
    "src/Utils/FileUtils.h" "src/Utils/FileUtils.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/NBpch.h")

# Dependencies
find_package(Vulkan REQUIRED)
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGui/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ImGui/backends)

# Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC glfw Vulkan::Vulkan spdlog glm)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:NB_DEBUG>)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:NB_RELEASE>)
IF (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NB_WINDOWS)
ELSEIF (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NB_APPLE)
ELSEIF (LINUX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NB_LINUX)
ENDIF()