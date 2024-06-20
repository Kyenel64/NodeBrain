#include "NBpch.h"
#include "EditorCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Input.h"

namespace NodeBrain
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		NB_ASSERT(fov > 0, "Invalid FOV. FOV must not be less than 0.");
		NB_ASSERT(aspectRatio > 0, "Invalid aspect ratio. Aspect ratio must not be less than 0.");

		CalculateViewMatrix();
		CalculateProjectionMatrix();
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		NB_PROFILE_FN();

		glm::vec2 curMousePos = Input::GetMousePosition();
		glm::vec2 delta = curMousePos - m_MousePosition;
		m_MousePosition = curMousePos;


		float speed = 10.0f;
		if (Input::IsKeyHeld(Key::W))
			m_Position += m_Front * speed * deltaTime;
		if (Input::IsKeyHeld(Key::S))
			m_Position -= m_Front * speed * deltaTime;
		if (Input::IsKeyHeld(Key::A))
			m_Position -= m_Right * speed * deltaTime;
		if (Input::IsKeyHeld(Key::D))
			m_Position += m_Right * speed * deltaTime;
		if (Input::IsKeyHeld(Key::Q))
			m_Position -= m_Up * speed * deltaTime;
		if (Input::IsKeyHeld(Key::E))
			m_Position += m_Up * speed * deltaTime;

		if (Input::IsMouseButtonHeld(MouseButton::Right))
		{
			m_Yaw += delta.x * speed * deltaTime;
			m_Pitch += delta.y * speed * deltaTime;
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
		}
		
		CalculateViewMatrix();
	}

	void EditorCamera::Resize(uint32_t width, uint32_t height)
	{
		NB_ASSERT(width, "Invalid width. Width must be a non-zero value.");
		NB_ASSERT(height, "Invalid height. Height must be a non-zero value.");

		m_AspectRatio = (float)width / (float)height;

		CalculateProjectionMatrix();
	}

	void EditorCamera::CalculateViewMatrix()
	{
		NB_PROFILE_FN();

		m_Direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Direction.y = sin(glm::radians(m_Pitch));
		m_Direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(m_Direction);
		m_Right = glm::normalize(glm::cross(m_Front, m_Up));

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void EditorCamera::CalculateProjectionMatrix()
	{
		NB_PROFILE_FN();

		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}
}