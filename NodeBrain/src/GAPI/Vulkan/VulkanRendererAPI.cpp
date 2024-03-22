#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanCommandBuffer.h"
#include "GAPI/Vulkan/VulkanCommandPool.h"

#define NB_LIST_AVAILABLE_VK_EXTENSTIONS

namespace NodeBrain
{
	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();
	}

	void VulkanRendererAPI::Init()
	{
		NB_PROFILE_FN();
	}

	void VulkanRendererAPI::BeginFrame(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		std::shared_ptr<VulkanCommandBuffer> cmdBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		vkResetCommandBuffer(cmdBuffer->GetVkCommandBuffer(), 0);
		cmdBuffer->Begin();
	}

	void VulkanRendererAPI::EndFrame(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		std::shared_ptr<VulkanCommandBuffer> cmdBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		cmdBuffer->End();
		cmdBuffer->Submit();
	}

	void VulkanRendererAPI::BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<RenderPass> renderPass)
	{
		std::shared_ptr<VulkanCommandBuffer> cmdBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		std::shared_ptr<VulkanRenderPass> rp;
		if (!renderPass)
			rp = VulkanRenderContext::GetInstance()->GetSwapchain().GetRenderPass();
		else
			NB_ASSERT(false, "Not yet implemented");
			
		cmdBuffer->StartRenderPass(rp);
	}

	void VulkanRendererAPI::EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		std::shared_ptr<VulkanCommandBuffer> cmdBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		cmdBuffer->EndRenderPass();
	}

	void VulkanRendererAPI::DrawTestTriangle(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
	{
		std::shared_ptr<VulkanCommandBuffer> cmdBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		cmdBuffer->BindGraphicsPipeline(std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline));
		cmdBuffer->Draw();
	}

	void VulkanRendererAPI::WaitForGPU()
	{
		VkFence fence = VulkanRenderContext::GetInstance()->GetSwapchain().GetInFlightFence();
		vkWaitForFences(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
	}
}