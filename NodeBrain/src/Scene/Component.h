#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Renderer/Texture2D.h"

namespace NodeBrain
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		explicit TagComponent(std::string tag) : Tag(std::move(tag)) {}
		TagComponent(const TagComponent&) = default;
	};

	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

	private:
		// Rotation in degrees
		glm::vec3 EulerRotation = glm::vec3(0.0f);
		glm::quat QuatRotation = glm::quat();

	public:

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		[[nodiscard]] glm::mat4 GetTransform() const
		{
			NB_PROFILE_FN();

			return glm::translate(glm::mat4(1.0f), Position) *
					glm::mat4_cast(QuatRotation) *
					glm::scale(glm::mat4(1.0f), Scale);
		}

		// Rotation in degrees
		void SetEulerRotation(const glm::vec3& rotation)
		{
			EulerRotation = rotation;
			QuatRotation = glm::quat(glm::radians(rotation));
		}

		void SetQuatRotation(const glm::quat& quat)
		{
			QuatRotation = quat;
			EulerRotation = glm::degrees(glm::eulerAngles(quat));
		}

		[[nodiscard]] const glm::vec3& GetEulerRotation() const { return EulerRotation; }
		[[nodiscard]] const glm::quat& GetQuatRotation() const { return QuatRotation; }
	};

	struct SpriteComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<Texture2D> Texture = nullptr;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
	};
}