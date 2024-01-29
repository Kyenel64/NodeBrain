#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Window, Init) 
{
    NodeBrain::Window window;
    bool initSuccess = window.Init();
    EXPECT_EQ(initSuccess, true);
}