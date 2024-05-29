#pragma once

#include "Core/KeyCode.h"

namespace NodeBrain
{
	enum class EventType
	{ 
		None = 0, 
		WindowClosedEvent, WindowResizedEvent, WindowMinimizedEvent,
		KeyPressedEvent, KeyReleasedEvent,
		MousePressedEvent, MouseReleasedEvent, MouseMovedEvent, MouseScrolledEvent
	};

	class Event
	{
	public:
		Event() = default;
		explicit Event(EventType type) : m_Type(type) {}

		EventType GetType() const { return m_Type; }

		template<typename T>
		bool AttachEventFunction(std::function<void(T&)> func)
		{
			if (func && m_Type == T::GetStaticType())
			{
				func(*(T*)this);
				return true;
			}
			
			return false;
		}

	private:
		EventType m_Type = EventType::None;
	};



	// --- Window Events ---
	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent() : Event(EventType::WindowClosedEvent) {}

		static EventType GetStaticType() { return EventType::WindowClosedEvent; }
	};

	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(uint32_t width, uint32_t height) : Event(EventType::WindowResizedEvent), m_Width(width), m_Height(height) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		static EventType GetStaticType() { return EventType::WindowResizedEvent; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class WindowMinimizedEvent : public Event
	{
	public:
		explicit WindowMinimizedEvent(bool minimized) : Event(EventType::WindowMinimizedEvent), m_Minimized(minimized) {}

		bool IsMinimized() const { return m_Minimized; }
		static EventType GetStaticType() { return EventType::WindowMinimizedEvent; }

	private:
		bool m_Minimized;
	};



	// --- Key Events ---
	class KeyPressedEvent : public Event
	{
	public:
		explicit KeyPressedEvent(Key key) : Event(EventType::KeyPressedEvent), m_Key(key) {}

		Key GetKey() const { return m_Key; }

		static EventType GetStaticType() { return EventType::KeyPressedEvent; }
	private:
		Key m_Key;
	};



	class KeyReleasedEvent : public Event
	{
	public:
		explicit KeyReleasedEvent(Key key) : Event(EventType::KeyReleasedEvent), m_Key(key) {}

		Key GetKey() const { return m_Key; }

		static EventType GetStaticType() { return EventType::KeyReleasedEvent; }
	private:
		Key m_Key;
	};



	// --- Mouse Events ---
	class MousePressedEvent : public Event
	{
	public:
		explicit MousePressedEvent(MouseButton mouseButton) : Event(EventType::MousePressedEvent), m_MouseButton(mouseButton) {}

		MouseButton GetMouseButton() const { return m_MouseButton; }

		static EventType GetStaticType() { return EventType::MousePressedEvent; }
	private:
		MouseButton m_MouseButton;
	};

	class MouseReleasedEvent : public Event
	{
	public:
		explicit MouseReleasedEvent(MouseButton mouseButton) : Event(EventType::MouseReleasedEvent), m_MouseButton(mouseButton) {}

		MouseButton GetMouseButton() const { return m_MouseButton; }

		static EventType GetStaticType() { return EventType::MouseReleasedEvent; }
	private:
		MouseButton m_MouseButton;
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float xpos, float ypos) : Event(EventType::MouseMovedEvent), m_XPos(xpos), m_YPos(ypos) {}

		float GetXPos() const { return m_XPos; }
		float GetYPos() const { return m_YPos; }

		static EventType GetStaticType() { return EventType::MouseMovedEvent; }
	private:
		float m_XPos;
		float m_YPos;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : Event(EventType::MouseScrolledEvent), m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		static EventType GetStaticType() { return EventType::MouseScrolledEvent; }
	private:
		float m_XOffset;
		float m_YOffset;
	};
}