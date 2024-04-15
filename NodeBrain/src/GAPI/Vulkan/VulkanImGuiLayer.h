#pragma once

#include "Core/ImGuiLayer.h"
#include "Core/Layer.h"

namespace NodeBrain
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer() = default;
		~VulkanImGuiLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
	private:
	};
}