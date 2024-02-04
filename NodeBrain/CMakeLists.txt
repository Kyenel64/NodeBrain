project ("NodeBrain")

# Source files
add_library (${PROJECT_NAME} "src/Core/App.h" "src/Core/App.cpp" "src/Core/Layer.h" "src/Core/Timer.h" "src/Core/Window.h" 
    "src/Core/Window.cpp"  "src/Core/Log.h" "src/Core/Log.cpp" "src/Core/Event.h" "src/Core/KeyCode.h" "src/Core/Input.h" 
    "src/Core/Input.cpp"  "src/Renderer/Renderer.h" "src/Renderer/Renderer.cpp" "src/Renderer/RendererAPI.h" 
    "src/Renderer/RendererAPI.cpp" "src/GAPI/Vulkan/VulkanRendererAPI.h" "src/GAPI/Vulkan/VulkanRendererAPI.cpp" 
    "src/GAPI/Vulkan/VulkanValidationLayer.h" "src/GAPI/Vulkan/VulkanValidationLayer.cpp" "src/GAPI/Vulkan/VulkanPhysicalDevice.h"
    "src/GAPI/Vulkan/VulkanPhysicalDevice.cpp"  "src/GAPI/Vulkan/VulkanDevice.h" "src/GAPI/Vulkan/VulkanDevice.cpp"  
    "src/GAPI/Vulkan/VulkanSurface.cpp" "src/GAPI/Vulkan/VulkanSurface.h" "src/GAPI/Vulkan/VulkanSwapChain.h" "src/GAPI/Vulkan/VulkanSwapChain.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/NBpch.h")

# Dependencies
find_package(Vulkan REQUIRED)
add_subdirectory("thirdparty/glm")
add_subdirectory("thirdparty/glfw")
add_subdirectory("thirdparty/spdlog")
add_subdirectory("thirdparty/ImGui")

# Include Paths
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan PUBLIC spdlog glm ImGui)

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