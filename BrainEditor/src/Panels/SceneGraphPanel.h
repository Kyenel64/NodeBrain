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

		void Draw(std::shared_ptr<Scene> scene);

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		
	private:
		Entity m_SelectedEntity;
	};
}