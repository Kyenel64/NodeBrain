#include "NBpch.h"
#include "Scene.h"

namespace NodeBrain
{
	Scene::Scene(Renderer& renderer)
		: m_Renderer(renderer)
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		NB_PROFILE_FN();

		Entity entity(m_Registry.create());

		AddComponent<TagComponent>(entity, name);
		AddComponent<TransformComponent>(entity);

		m_EntityGraphs.emplace(entity, EntityGraph());

		return entity;
	}

	void Scene::OnEditorUpdate(const std::shared_ptr<EditorCamera> &editorCamera, const std::shared_ptr<Image> &targetImage)
	{
		NB_PROFILE_FN();

		// --- Entity Graph ---
		{
			NB_PROFILE_SCOPE("Entity Graph");
			auto view = m_Registry.view<TransformComponent>();
			for (const auto& entity : view)
				m_EntityGraphs[(Entity)entity].Evaluate();
		}


		// --- Rendering ---
		m_Renderer.BeginScene(editorCamera, targetImage);

		// Draw Sprites
		{
			NB_PROFILE_SCOPE("Draw Sprites");
			auto view = m_Registry.view<TransformComponent, SpriteComponent>();
			for (const auto& entity : view)
				m_Renderer.SubmitQuad(m_Registry.get<TransformComponent>(entity).GetTransform(), m_Registry.get<SpriteComponent>(entity).Color);
		}

		m_Renderer.EndScene();
	}
}