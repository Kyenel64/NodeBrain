# CMakeList.txt : CMake project for NodeBrain, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (NodeBrain "src/NodeBrain.cpp" "src/NodeBrain.h" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET NodeBrain PROPERTY CXX_STANDARD 20)
endif()

# Include paths
target_include_directories(${PROJECT_NAME} 
    PUBLIC thirdparty/glm
    PUBLIC thirdparty/glfw/include
    PUBLIC thirdparty/spdlog/include
)

# Library paths
target_link_directories(${PROJECT_NAME}
    PRIVATE thirdparty/glm/glm 
    PRIVATE thrdparty/glfw/src
    PRIVATE thirdparty/spdlog/src
)

# Library
target_link_libraries(${PROJECT_NAME} glm glfw spdlog)