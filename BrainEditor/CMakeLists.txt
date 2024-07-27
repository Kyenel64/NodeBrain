project ("BrainEditor")

# Source files
add_executable(${PROJECT_NAME}
"src/main.cpp"
"src/BrainEditor.h" "src/BrainEditor.cpp"
"src/Panels/EntityGraphPanel.h" "src/Panels/EntityGraphPanel.cpp"
"src/Panels/SceneGraphPanel.h" "src/Panels/SceneGraphPanel.cpp"
"src/TestEditor.h" "src/TestEditor.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE ${NODEBRAIN_PATH}/include)

# Libraries
target_link_libraries(${PROJECT_NAME} NodeBrain)

# Post build commands
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Assets ${PROJECT_BINARY_DIR}/Assets)


# --- Compile Shaders ---------------------------------------------------------
# Taken from Sean at https://stackoverflow.com/questions/60202782/what-is-the-function-of-add-custom-target-in-cmake
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Assets/Shaders/Compiled)

file(GLOB SHADERS
        ${SHADER_SOURCE_DIR}/*.vert
        ${SHADER_SOURCE_DIR}/*.frag
        ${SHADER_SOURCE_DIR}/*.comp
        ${SHADER_SOURCE_DIR}/*.geom)

add_custom_command(
        COMMAND
        ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        OUTPUT ${SHADER_BINARY_DIR}
        COMMENT "Creating ${SHADER_BINARY_DIR}"
)

foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME)
    add_custom_command(
            COMMAND
            ${glslc_executable}
            -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
            ${source}
            OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
            DEPENDS ${source} ${SHADER_BINARY_DIR}
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})
