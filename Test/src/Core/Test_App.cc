#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(App, PushLayer) 
{
    NodeBrain::App* app = new NodeBrain::App("Test");
    app->PushLayer(nullptr);
    EXPECT_EQ(app->GetLayersSize(), 0);

    delete app;
}