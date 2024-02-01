#include <gtest/gtest.h>
#include <NodeBrain/NodeBrain.h>

TEST(Input, KeyState) 
{
    NodeBrain::Input::SetKeyState(NodeBrain::Key::Enter, NodeBrain::InputState::Pressed);
    EXPECT_TRUE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Enter) == NodeBrain::InputState::Pressed);
    EXPECT_FALSE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Enter) == NodeBrain::InputState::Released);
    EXPECT_FALSE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Enter) == NodeBrain::InputState::Held);

    NodeBrain::Input::SetKeyState(NodeBrain::Key::Space, NodeBrain::InputState::None);
    EXPECT_TRUE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Space) == NodeBrain::InputState::None);
    EXPECT_FALSE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Space) == NodeBrain::InputState::Pressed);
    EXPECT_FALSE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Space) == NodeBrain::InputState::Released);
    EXPECT_FALSE(NodeBrain::Input::GetKeyState(NodeBrain::Key::Space) == NodeBrain::InputState::Held);

    EXPECT_TRUE(NodeBrain::Input::GetKeyState(NodeBrain::Key::G) == NodeBrain::InputState::None);
}

TEST(Input, MouseButtonState)
{
    NodeBrain::Input::SetMouseButtonState(NodeBrain::MouseButton::Left, NodeBrain::InputState::Pressed);
    EXPECT_TRUE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Left) == NodeBrain::InputState::Pressed);
    EXPECT_FALSE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Left) == NodeBrain::InputState::Released);
    EXPECT_FALSE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Left) == NodeBrain::InputState::Held);

    NodeBrain::Input::SetMouseButtonState(NodeBrain::MouseButton::Middle, NodeBrain::InputState::None);
    EXPECT_TRUE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Middle) == NodeBrain::InputState::None);
    EXPECT_FALSE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Middle) == NodeBrain::InputState::Pressed);
    EXPECT_FALSE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Middle) == NodeBrain::InputState::Released);
    EXPECT_FALSE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Middle) == NodeBrain::InputState::Held);

    EXPECT_TRUE(NodeBrain::Input::GetMouseButtonState(NodeBrain::MouseButton::Right) == NodeBrain::InputState::None);
}