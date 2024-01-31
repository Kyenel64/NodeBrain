#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

static void testCallback(NodeBrain::MouseMovedEvent& e)
{

}

TEST(Event, AttachEventFunction)
{
    NodeBrain::MouseMovedEvent mouseMovedEvent(100, 200);
    std::function<void(NodeBrain::MouseMovedEvent&)> func = testCallback;
    bool called = mouseMovedEvent.AttachEventFunction<NodeBrain::MouseMovedEvent>(func);
    EXPECT_TRUE(called);

    NodeBrain::WindowClosedEvent windowClosedEvent;
    called = windowClosedEvent.AttachEventFunction<NodeBrain::MouseMovedEvent>(func);
    EXPECT_FALSE(called);
}