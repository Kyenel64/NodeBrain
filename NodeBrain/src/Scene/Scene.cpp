#include "NBpch.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>

namespace NodeBrain
{
	Scene::Scene(Renderer* renderer)
		: m_Renderer(renderer)
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		NB_PROFILE_FN();

		Entity entity = Entity(m_Registry.create());

		AddComponent<TagComponent>(entity, name);
		AddComponent<TransformComponent>(entity);

		m_EntityGraphs[entity] = EntityGraph();

		return entity;
	}

	void Scene::OnEditorUpdate(const std::shared_ptr<EditorCamera> &editorCamera, const std::shared_ptr<Image> &targetImage)
	{
		NB_PROFILE_FN();

		// --- Entity Graph ---
		auto v = m_Registry.view<TransformComponent>();
		for (auto entity : v)
			m_EntityGraphs[(Entity)entity].Evaluate();


		// --- Rendering ---
		m_Renderer->BeginScene(editorCamera, targetImage);

		// Draw Sprites
		auto view = m_Registry.view<TransformComponent, SpriteComponent>();
		for (auto entity : view)
			m_Renderer->SubmitQuad(m_Registry.get<TransformComponent>(entity).GetTransform(), m_Registry.get<SpriteComponent>(entity).Color);

		m_Renderer->EndScene();
	}
}