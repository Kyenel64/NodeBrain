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
		explicit Scene(Renderer* renderer);
		~Scene() = default;

		Entity CreateEntity(const std::string& name = "Empty Entity");

		template<typename T, typename... Args>
		T& AddComponent(Entity entity, Args&&... args)
		{
			NB_ASSERT(!HasComponent<T>(entity), "Entity already has component!");
			T& component = m_Registry.emplace<T>((entt::entity)entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent(Entity entity) { return m_Registry.get<T>((entt::entity)entity); }

		template<typename T>
		bool HasComponent(Entity entity) { return m_Registry.any_of<T>(entity); }

		template<typename T>
		void RemoveComponent(Entity entity) { m_Registry.remove<T>(entity); }

		void OnEditorUpdate(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Image>& targetImage = nullptr);
		//void OnRuntimeUpdate(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Image>& targetImage = nullptr);

		EntityGraph& GetEntityGraph(Entity entity) { return m_EntityGraphs[entity]; }

		template<typename... T>
		auto View()
		{
			return m_Registry.view<T...>();
		}

	private:
		Renderer* m_Renderer;

		entt::registry m_Registry;
		std::unordered_map<Entity, EntityGraph> m_EntityGraphs;
	};
}
