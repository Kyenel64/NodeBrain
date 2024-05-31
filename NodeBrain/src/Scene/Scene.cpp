#include "NBpch.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

namespace NodeBrain
{
	Scene::Scene(Renderer* renderer)
		: m_Renderer(renderer)
	{

	}

	Entity Scene::CreateEntity()
	{
		NB_PROFILE_FN();

		return Entity((uint32_t)m_Registry.create());
	}

	void Scene::OnUpdate(const std::shared_ptr<EditorCamera>& editorCamera)
	{
		NB_PROFILE_FN();

		m_Renderer->BeginScene(editorCamera);

		// Render Quads
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
			m_Renderer->SubmitQuad(m_Registry.get<TransformComponent>(entity).GetTransform(), { 1.0f, 0.0f, 1.0f, 1.0f });

		m_Renderer->EndScene();
	}
}