#include "NBpch.h"
#include "Input.h"

namespace NodeBrain
{
	static std::unordered_map<Key, InputState> s_KeyState;
	static std::queue<Key> s_UnhandledKeys;

	static std::unordered_map<MouseButton, InputState> s_MouseButtonState;
	static std::queue<MouseButton> s_UnhandledMouseButtons;

	static glm::vec2 s_MousePosition;

	bool Input::IsKeyPressed(Key key)
	{
		NB_PROFILE_FN();
		
		return s_KeyState.find(key) != s_KeyState.end() && s_KeyState[key] == InputState::Pressed;
	}

	bool Input::IsKeyHeld(Key key)
	{
		NB_PROFILE_FN();

		return s_KeyState.find(key) != s_KeyState.end() && s_KeyState[key] == InputState::Held;
	}

	bool Input::IsKeyReleased(Key key)
	{
		NB_PROFILE_FN();

		return s_KeyState.find(key) != s_KeyState.end() && s_KeyState[key] == InputState::Released;
	}

	void Input::SetKeyState(Key key, InputState keyState)
	{
		NB_PROFILE_FN();

		s_KeyState[key] = keyState;
		if (keyState != InputState::None)
			s_UnhandledKeys.push(key);
	}

	InputState Input::GetKeyState(Key key)
	{
		NB_PROFILE_FN();

		return s_KeyState.find(key) != s_KeyState.end() ? s_KeyState[key] : InputState::None;
	}

	void Input::SetMouseButtonState(MouseButton mouseButton, InputState mouseState)
	{
		NB_PROFILE_FN();

		s_MouseButtonState[mouseButton] = mouseState;
		if (mouseState != InputState::None)
			s_UnhandledMouseButtons.push(mouseButton);
	}

	InputState Input::GetMouseButtonState(MouseButton mouseButton)
	{
		NB_PROFILE_FN();

		return s_MouseButtonState.find(mouseButton) != s_MouseButtonState.end() ? s_MouseButtonState[mouseButton] : InputState::None;
	}

	void Input::ProcessPollStates()
	{
		NB_PROFILE_FN();

		while (!s_UnhandledKeys.empty())
		{
			Key key = s_UnhandledKeys.front();
			// Set keystate to held right after first pressed.
			if (Input::GetKeyState(key) == InputState::Pressed)
				Input::SetKeyState(key, InputState::Held);
			else if (Input::GetKeyState(key) == InputState::Released)
				Input::SetKeyState(key, InputState::None);
			s_UnhandledKeys.pop();
		}

		while (!s_UnhandledMouseButtons.empty())
		{
			MouseButton mouseButton = s_UnhandledMouseButtons.front();
			// Set mouse button state to held right after first pressed.
			if (Input::GetMouseButtonState(mouseButton) == InputState::Pressed)
				Input::SetMouseButtonState(mouseButton, InputState::Held);
			else if (Input::GetMouseButtonState(mouseButton) == InputState::Released)
				Input::SetMouseButtonState(mouseButton, InputState::None);
			s_UnhandledMouseButtons.pop();
		}
	}

	bool Input::IsMouseButtonPressed(MouseButton mouseButton)
	{
		NB_PROFILE_FN();

		return s_MouseButtonState.find(mouseButton) != s_MouseButtonState.end() && s_MouseButtonState[mouseButton] == InputState::Pressed;
	}

	bool Input::IsMouseButtonHeld(MouseButton mouseButton)
	{
		NB_PROFILE_FN();

		return s_MouseButtonState.find(mouseButton) != s_MouseButtonState.end() && s_MouseButtonState[mouseButton] == InputState::Held;
	}

	bool Input::IsMouseButtonReleased(MouseButton mouseButton)
	{
		NB_PROFILE_FN();

		return s_MouseButtonState.find(mouseButton) != s_MouseButtonState.end() && s_MouseButtonState[mouseButton] == InputState::Released;
	}

	glm::vec2 Input::GetMousePosition()
	{
		NB_PROFILE_FN();

		return s_MousePosition;
	}

	void Input::SetMousePosition(const glm::vec2& pos)
	{
		NB_PROFILE_FN();

		s_MousePosition = pos;
	}
}