#include "SceneGraphPanel.h"

namespace NodeBrain
{
	void SceneGraphPanel::Draw(const std::shared_ptr<Scene>& scene)
	{
		m_ActiveScene = scene;

		ImGui::Begin("Scene Graph");

		// --- Add Entity Button ---
		if (ImGui::Button("+"))
		{
			m_ActiveScene->CreateEntity();
		}


		// --- Entity Tree ---
		auto view = m_ActiveScene->View<TagComponent>();
		for (auto e : view)
		{
			auto entity = Entity(e);
			ImGui::PushID((int)(uint32_t)entity);

			ImGuiTreeNodeFlags flags = m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", m_ActiveScene->GetComponent<TagComponent>(entity).Tag.c_str());

			if (ImGui::IsItemClicked())
				m_SelectedEntity = entity;

			if (opened)
			{
				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		ImGui::End();
	}
}