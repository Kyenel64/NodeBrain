#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Window, Init) 
{
    NodeBrain::Window window("Test", 1280, 720);
    EXPECT_EQ(window.GetWidth(), 1280);
    EXPECT_EQ(window.GetHeight(), 720);
    EXPECT_TRUE(window.GetGLFWWindow() != nullptr);
}