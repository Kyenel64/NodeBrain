project ("NodeBrain")

# Source files
add_library (${PROJECT_NAME}    "src/Core/App.h" "src/Core/App.cpp" "src/nbpch.h" "src/nbpch.cpp" "src/Core/Layer.h")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_precompile_headers(${PROJECT_NAME} PUBLIC "src/nbpch.h")

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