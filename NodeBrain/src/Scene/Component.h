#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NodeBrain
{
	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		glm::mat4 GetTransform()
		{
			NB_PROFILE_FN();

			return glm::translate(glm::mat4(1.0f), Position);
		}
	};
}