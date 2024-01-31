// Class interface for user-written code.
// Use: Create a class derived from Layer -> Create NodeBrain app ->
//	Push layer to app -> call app->Run() -> delete app
#pragma once

#include "Core/Event.h"

namespace NodeBrain
{
	class Layer
	{
	public:
		Layer() = default;
		~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnEvent(Event& e) {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnUpdateGUI() {}
	};
}