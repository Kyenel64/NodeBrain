#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/PipelineLayout.h"
#include "Renderer/PipelineConfiguration.h"
#include "Renderer/GraphicsPipeline.h"

// temp
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanCommandBuffer.h"
#include "GAPI/Vulkan/VulkanCommandPool.h"

#define NB_LIST_AVAILABLE_VK_EXTENSTIONS

namespace NodeBrain
{
	std::shared_ptr<Shader> vertShader;
	std::shared_ptr<Shader> fragShader;
	std::shared_ptr<RenderPass> renderPass;
	PipelineConfiguration pipelineConfig;
	std::shared_ptr<PipelineLayout> pipelineLayout;
	std::shared_ptr<GraphicsPipeline> pipeline;
	std::vector<std::shared_ptr<VulkanFramebuffer>> framebuffers;


	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;
	std::shared_ptr<VulkanCommandBuffer> commandBuffer;
	std::shared_ptr<VulkanCommandPool> commandPool;

	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		Init();
	}

	void VulkanRendererAPI::Init()
	{
		NB_PROFILE_FN();

		vertShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv");
		fragShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv");
		renderPass = RenderPass::Create();
		pipelineConfig = {};
		pipelineConfig.VertexShader = vertShader;
		pipelineConfig.FragmentShader = fragShader;
		pipelineLayout = PipelineLayout::Create(pipelineConfig);
		pipeline = GraphicsPipeline::Create(pipelineLayout, renderPass);
		
		// temp
		std::shared_ptr<VulkanSwapChain> swapchain = VulkanRenderContext::GetInstance()->GetSwapchain();
		for (int i = 0; i < swapchain->GetSwapchainImages().size(); i++)
			framebuffers.push_back(std::make_shared<VulkanFramebuffer>(std::dynamic_pointer_cast<VulkanRenderPass>(renderPass), swapchain->GetSwapchainImages()[i]));

		commandPool = std::make_shared<VulkanCommandPool>();
		commandBuffer = std::make_shared<VulkanCommandBuffer>(commandPool);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateSemaphore(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore);
		vkCreateSemaphore(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore);
		vkCreateFence(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &fenceCreateInfo, nullptr, &inFlightFence);
	}

	void VulkanRendererAPI::DrawTestTriangle()
	{
		vkWaitForFences(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), 1, &inFlightFence);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), VulkanRenderContext::GetInstance()->GetSwapchain()->GetVkSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(commandBuffer->GetVkCommandBuffer(), 0);
		commandBuffer->Begin();
		commandBuffer->StartRenderPass(std::dynamic_pointer_cast<VulkanRenderPass>(renderPass), framebuffers[0]);
		commandBuffer->BindGraphicsPipeline(std::dynamic_pointer_cast<VulkanGraphicsPipeline>(pipeline));
		commandBuffer->Draw();
		commandBuffer->EndRenderPass();

		commandBuffer->End();

		VkCommandBuffer buffer = commandBuffer->GetVkCommandBuffer();
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkResult result = vkQueueSubmit(VulkanRenderContext::GetInstance()->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFence);
		NB_ASSERT(result == VK_SUCCESS, result);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { VulkanRenderContext::GetInstance()->GetSwapchain()->GetVkSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(VulkanRenderContext::GetInstance()->GetDevice()->GetPresentationQueue(), &presentInfo);
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();
	}

	
}