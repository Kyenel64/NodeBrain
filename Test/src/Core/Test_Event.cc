#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Event, Window)
{
    NodeBrain::WindowClosedEvent windowClosedEvent;
    EXPECT_EQ(windowClosedEvent.GetType(), NodeBrain::WindowClosedEvent::GetStaticType());

    NodeBrain::WindowResizedEvent windowResizedEvent(100, 200);
    EXPECT_EQ(windowResizedEvent.GetType(), NodeBrain::WindowResizedEvent::GetStaticType());
    EXPECT_EQ(windowResizedEvent.GetWidth(), 100);
    EXPECT_EQ(windowResizedEvent.GetHeight(), 200);

    NodeBrain::WindowMinimizedEvent windowMinimizedEvent(true);
    EXPECT_EQ(windowMinimizedEvent.GetType(), NodeBrain::WindowMinimizedEvent::GetStaticType());
    EXPECT_TRUE(windowMinimizedEvent.IsMinimized());
}

TEST(Event, Key)
{
    NodeBrain::KeyPressedEvent keyPressedEvent(NodeBrain::Key::A);
    EXPECT_EQ(keyPressedEvent.GetType(), NodeBrain::KeyPressedEvent::GetStaticType());
    EXPECT_EQ(keyPressedEvent.GetKey(), NodeBrain::Key::A);

    NodeBrain::KeyReleasedEvent keyReleasedEvent(NodeBrain::Key::A);
    EXPECT_EQ(keyReleasedEvent.GetType(), NodeBrain::KeyReleasedEvent::GetStaticType());
    EXPECT_EQ(keyReleasedEvent.GetKey(), NodeBrain::Key::A);
}

TEST(Event, Mouse)
{
    NodeBrain::MousePressedEvent mousePressedEvent(NodeBrain::MouseButton::Left);
    EXPECT_EQ(mousePressedEvent.GetType(), NodeBrain::MousePressedEvent::GetStaticType());
    EXPECT_EQ(mousePressedEvent.GetMouseButton(), NodeBrain::MouseButton::Left);

    NodeBrain::MouseReleasedEvent mouseReleasedEvent(NodeBrain::MouseButton::Left);
    EXPECT_EQ(mouseReleasedEvent.GetType(), NodeBrain::MouseReleasedEvent::GetStaticType());
    EXPECT_EQ(mouseReleasedEvent.GetMouseButton(), NodeBrain::MouseButton::Left);

    NodeBrain::MouseMovedEvent mouseMovedEvent(200, 300);
    EXPECT_EQ(mouseMovedEvent.GetType(), NodeBrain::MouseMovedEvent::GetStaticType());
    EXPECT_EQ(mouseMovedEvent.GetXPos(), 200);
    EXPECT_EQ(mouseMovedEvent.GetYPos(), 300);

    NodeBrain::MouseScrolledEvent mouseScrolledEvent(200, 100);
    EXPECT_EQ(mouseScrolledEvent.GetType(), NodeBrain::MouseScrolledEvent::GetStaticType());
    EXPECT_EQ(mouseScrolledEvent.GetXOffset(), 200);
     EXPECT_EQ(mouseScrolledEvent.GetYOffset(), 100);
}