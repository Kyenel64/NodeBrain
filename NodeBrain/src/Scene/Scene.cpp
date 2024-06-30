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

	void Scene::OnEditorUpdate(const std::shared_ptr<EditorCamera> &editorCamera, const std::shared_ptr<Framebuffer> &targetFramebuffer)
	{
		NB_PROFILE_FN();

		// --- Entity Graph ---
		{
			NB_PROFILE_SCOPE("Entity Graph");
			auto view = m_Registry.view<TransformComponent>();

			std::unordered_map<Entity, EntityGraph>& entityGraphs = m_EntityGraphs;

			// TODO: Parallelize. Apple clang does not support std::execution::par so figure something out.
			std::for_each(view.begin(), view.end(), [&view, &entityGraphs](auto entity)
				{
					entityGraphs[(Entity)entity].Evaluate();
				});
		}


		// --- Rendering ---
		m_Renderer.BeginScene(editorCamera, targetFramebuffer);

		// Draw Sprites
		{
			NB_PROFILE_SCOPE("Draw Sprites");
			auto view = m_Registry.view<TransformComponent, SpriteComponent>();
			Renderer& renderer = m_Renderer;
			entt::registry& reg = m_Registry;

			// TODO: Parallelize. Apple clang does not support std::execution::par so figure something out.
			std::for_each(view.begin(), view.end(), [&view, &renderer, &reg](auto entity)
				{
					renderer.SubmitQuad(std::as_const(reg).get<TransformComponent>(entity).GetTransform(), std::as_const(reg).get<SpriteComponent>(entity).Color);
				});
		}

		m_Renderer.EndScene();
	}
}