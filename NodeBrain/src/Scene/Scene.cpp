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

		Entity entity = Entity(m_Registry.create());
		AddComponent<TransformComponent>(entity);

		m_EntityGraphs[entity] = EntityGraph();

		return entity;
	}

	void Scene::OnUpdate(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Image>& targetImage)
	{
		NB_PROFILE_FN();

		// --- Entity Graph ---
		auto v = m_Registry.view<TransformComponent>();
		for (auto entity : v)
			m_EntityGraphs[(Entity)entity].Evaluate();


		// --- Rendering ---
		m_Renderer->BeginScene(editorCamera, targetImage);

		// Quads
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
			m_Renderer->SubmitQuad(m_Registry.get<TransformComponent>(entity).GetTransform(), { 1.0f, 0.0f, 1.0f, 1.0f });

		m_Renderer->EndScene();
	}
}