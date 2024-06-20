#pragma once

#include <glm/glm.hpp>

namespace NodeBrain
{
	class EditorCamera
	{
	public:
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
		~EditorCamera() = default;

		void OnUpdate(float deltaTime);

		void Resize(uint32_t width, uint32_t height);

		[[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		[[nodiscard]] float GetAspectRatio() const { return m_AspectRatio; }

	private:
		void CalculateViewMatrix();
		void CalculateProjectionMatrix();

	private:
		glm::mat4 m_ViewMatrix{};
		glm::mat4 m_ProjectionMatrix{};

		glm::vec3 m_Position = { 0.0f, 0.0f, 5.0f };
		glm::vec3 m_Direction = { 0.0f, 0.0f, -1.0f };

		glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Right = { -1.0f, 0.0f, 0.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };

		glm::vec2 m_MousePosition = { 0.0f, 0.0f };

		float m_Pitch = 0.0f, m_Yaw = -90.0f;

		float m_FOV;
		float m_AspectRatio;
		float m_NearClip, m_FarClip;
	};
}