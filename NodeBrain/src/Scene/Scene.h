#pragma once

#include <entt/entt.hpp>

#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Scene/EntityGraph.h"

namespace NodeBrain
{
	class Scene
	{
	public:
		explicit Scene(Renderer& renderer);
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "Empty Entity");

		template<typename T, typename... Args>
		T& AddComponent(Entity entity, Args&&... args)
		{
			NB_PROFILE_FN();

			NB_ASSERT(!HasComponent<T>(entity), "Entity already has component.");
			T& component = m_Registry.emplace<T>((entt::entity)entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			NB_PROFILE_FN();

			NB_ASSERT(HasComponent<T>(entity), "Entity does not have component.");
			return m_Registry.get<T>((entt::entity)entity);
		}

		template<typename T>
		bool HasComponent(Entity entity)
		{
			NB_PROFILE_FN();

			return m_Registry.any_of<T>((entt::entity)entity);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			NB_PROFILE_FN();

			NB_ASSERT(HasComponent<T>(entity), "Entity does not have component.");
			m_Registry.remove<T>((entt::entity)entity);
		}

		template<typename... T>
		auto View()
		{
			NB_PROFILE_FN();

			return m_Registry.view<T...>();
		}

		void OnEditorUpdate(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Framebuffer>& targetFramebuffer = nullptr);
		//void OnRuntimeUpdate(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Image>& targetImage = nullptr);

		EntityGraph& GetEntityGraph(Entity entity)
		{
			NB_PROFILE_FN();

			NB_ASSERT(entity, "Entity null.");
			NB_ASSERT(m_EntityGraphs.find(entity) != m_EntityGraphs.end(), "Could not find entity graph for given entity.");
			return m_EntityGraphs.at(entity);
		}



	private:
		Renderer& m_Renderer;

		entt::registry m_Registry;
		std::unordered_map<Entity, EntityGraph> m_EntityGraphs;
	};
}
