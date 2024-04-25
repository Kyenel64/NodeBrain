#include "NBpch.h"
#include "VulkanImGuiLayer.h"

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "Core/App.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	VulkanImGuiLayer::~VulkanImGuiLayer()
	{
		ImGui_ImplVulkan_Shutdown();
		
		vkDestroyDescriptorPool(VulkanRenderContext::Get()->GetVkDevice(), m_ImGuiDescriptorPool, nullptr);
		m_ImGuiDescriptorPool = VK_NULL_HANDLE;
	}

	void VulkanImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		GLFWwindow* glfwWindow = App::Get()->GetWindow().GetGLFWWindow();
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);


		// Create descriptor pool
		VkDescriptorPoolSize poolSizes[] = 
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = 1000;
		descriptorPoolCreateInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
		descriptorPoolCreateInfo.pPoolSizes = poolSizes;

		VK_CHECK(vkCreateDescriptorPool(VulkanRenderContext::Get()->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &m_ImGuiDescriptorPool));


		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = VulkanRenderContext::Get()->GetVkInstance();
		initInfo.PhysicalDevice = VulkanRenderContext::Get()->GetPhysicalDevice().GetVkPhysicalDevice();
		initInfo.Device = VulkanRenderContext::Get()->GetVkDevice();
		initInfo.QueueFamily = VulkanRenderContext::Get()->GetPhysicalDevice().FindQueueFamilies().Graphics.value();
		initInfo.Queue = VulkanRenderContext::Get()->GetDevice().GetGraphicsQueue();
		initInfo.DescriptorPool = m_ImGuiDescriptorPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = VK_TRUE;
		initInfo.ColorAttachmentFormat = VulkanRenderContext::Get()->GetSwapchain().GetVkFormat();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&initInfo, VK_NULL_HANDLE);

		// Upload imgui font textures to GPU
		// VulkanRendererAPI::ImmediateSubmit([&](VkCommandBuffer cmd){ ImGui_ImplVulkan_CreateFontsTexture(cmd); });
	}

	void VulkanImGuiLayer::OnDetach()
	{
		
	}

	void VulkanImGuiLayer::OnEvent(Event& e)
	{

	}

	void VulkanImGuiLayer::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::EndFrame()
	{
		ImGui::Render();

		VkCommandBuffer cmdBuffer = VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer;
		Renderer::BeginRenderPass();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
		Renderer::EndRenderPass();
	}
}