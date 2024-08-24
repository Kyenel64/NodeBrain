#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

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

	struct MaterialComponent
	{
		std::shared_ptr<Material> Material = nullptr;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	enum class MeshType { None = 0, Quad = 1, Cube = 2, Custom = 3 }; // TODO: temp until mesh class is implemented.

	struct MeshComponent
	{
		MeshType Type = MeshType::None;
		std::shared_ptr<Mesh> Mesh = nullptr;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
	};
}
