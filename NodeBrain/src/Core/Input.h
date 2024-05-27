// Poll-based input. See Core/Event.h for event-based input.
#pragma once

#include <glm/glm.hpp>

#include "Core/KeyCode.h"

namespace NodeBrain
{
	class Input
	{
	public:
		// Poll-based. Checks first key press.
		static bool IsKeyPressed(Key key);
		// Poll-based. 
		static bool IsKeyHeld(Key key);
		// Poll-based. 
		static bool IsKeyReleased(Key key);

		static void SetKeyState(Key key, InputState keyState);
		static InputState GetKeyState(Key key);

		// Called OnUpdate() to process keystates
		static void ProcessPollStates();

		// Poll-based. Checks first mouse button press.
		static bool IsMouseButtonPressed(MouseButton mouseButton);
		// Poll-based. 
		static bool IsMouseButtonHeld(MouseButton mouseButton);
		// Poll-based. 
		static bool IsMouseButtonReleased(MouseButton mouseButton);

		static void SetMouseButtonState(MouseButton mouseButton, InputState mouseState);
		static InputState GetMouseButtonState(MouseButton mouseButton);

		// Poll-based. 
		// Returns screen-space coordinates of the cursor relative to the top left corner of the application window.
		static glm::vec2 GetMousePosition();
	};
}