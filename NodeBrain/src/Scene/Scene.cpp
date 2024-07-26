#include "NBpch.h"
#include "Scene.h"

#include "Renderer/Mesh.h"

namespace NodeBrain
{
	static std::shared_ptr<Mesh> testMesh;

	Scene::Scene(Renderer& renderer)
		: m_Renderer(renderer)
	{
		// temp
		testMesh = std::make_shared<Mesh>(m_Renderer.GetContext(), "Assets/Models/teapot.obj");
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

		{
			NB_PROFILE_SCOPE("Draw Sprites");
			auto view = m_Registry.view<TransformComponent, MeshComponent, MaterialComponent>();
			Renderer& renderer = m_Renderer;
			entt::registry& reg = m_Registry;

			// TODO: Parallelize. Apple clang does not support std::execution::par so figure something out.
			std::for_each(view.begin(), view.end(), [&view, &renderer, &reg](auto entity)
				{
					const glm::mat4& transform = std::as_const(reg).get<TransformComponent>(entity).GetTransform();
					const MaterialComponent& material = std::as_const(reg).get<MaterialComponent>(entity);

					// temp until mesh class is implemented.
					switch (std::as_const(reg).get<MeshComponent>(entity).Type)
					{
						case MeshType::Plane: renderer.SubmitQuad(transform, material); break;
						case MeshType::Cube: renderer.SubmitCube(transform, material); break;
						case MeshType::None: break;
					}
				});
		}

		{ // test draw mesh
			m_Renderer.DrawMesh(glm::mat4(1.0f), testMesh);
		}

		m_Renderer.EndScene();
	}
}
