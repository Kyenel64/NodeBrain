#pragma once

#include <entt/entt.hpp>

#include "Renderer/Renderer.h"
#include "Renderer/EditorCamera.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

namespace NodeBrain
{
	class Scene
	{
	public:
		explicit Scene(Renderer* renderer);
		~Scene() = default;

		[[nodiscard]] Entity CreateEntity();

		template<typename T, typename... Args>
		T& AddComponent(Entity entity, Args&&... args)
		{
			//NB_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Registry.emplace<T>((entt::entity)entity, std::forward<Args>(args)...);
			return component;
		}

		void OnUpdate(const std::shared_ptr<EditorCamera>& editorCamera);

	private:
		Renderer* m_Renderer;

		entt::registry m_Registry;
	};
}
