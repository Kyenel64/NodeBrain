#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

struct GLFWwindow;

TEST(Window, Init) 
{
    NodeBrain::Window window;
    EXPECT_FALSE(window.GetGLFWWindow() == nullptr);
}