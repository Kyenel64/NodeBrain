#include "SceneGraphPanel.h"

namespace NodeBrain
{
	void SceneGraphPanel::Draw(std::shared_ptr<Scene> scene)
	{
		ImGui::Begin("Scene Graph");

		// --- Add Entity Button ---
		if (ImGui::Button("+"))
		{
			scene->CreateEntity();
		}


		// --- Entity Tree ---
		auto view = scene->View<TagComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e);
			ImGui::PushID(entity);

			ImGuiTreeNodeFlags flags = m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", scene->GetComponent<TagComponent>(entity).Tag.c_str());

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