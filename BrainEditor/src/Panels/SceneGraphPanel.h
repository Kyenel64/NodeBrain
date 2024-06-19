#pragma once

#include <ImGui/imgui.h>

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	class SceneGraphPanel
	{
	public:
		SceneGraphPanel() = default;
		~SceneGraphPanel() = default;

		void Draw(const std::shared_ptr<Scene>& scene);

		[[nodiscard]] Entity GetSelectedEntity() const { return m_SelectedEntity; }
		
	private:
		Entity m_SelectedEntity;
		std::shared_ptr<Scene> m_ActiveScene;
	};
}