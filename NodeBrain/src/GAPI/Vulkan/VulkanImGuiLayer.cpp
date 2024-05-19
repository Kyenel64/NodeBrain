#include "NBpch.h"
#include "VulkanImGuiLayer.h"

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "Core/App.h"
#include "Renderer/Renderer.h"

class GLFWwindow;

namespace NodeBrain
{
	VulkanImGuiLayer::VulkanImGuiLayer()
	{
		NB_PROFILE_FN();
	}

	VulkanImGuiLayer::~VulkanImGuiLayer()
	{
		NB_PROFILE_FN();

		ImGui_ImplVulkan_Shutdown();
		
		vkDestroyDescriptorPool(VulkanRenderContext::Get()->GetVkDevice(), m_ImGuiDescriptorPool, nullptr);
		m_ImGuiDescriptorPool = VK_NULL_HANDLE;
	}

	void VulkanImGuiLayer::OnAttach()
	{
		NB_PROFILE_FN();

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
		initInfo.ColorAttachmentFormat = VK_FORMAT_B8G8R8A8_SRGB; // temp
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&initInfo, VK_NULL_HANDLE);

		VkInstance vkInstance = VulkanRenderContext::Get()->GetVkInstance();
		m_vkCmdBeginRenderingKHR = (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(vkInstance, "vkCmdBeginRenderingKHR");
		m_vkCmdEndRenderingKHR = (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(vkInstance, "vkCmdEndRenderingKHR");
	}

	void VulkanImGuiLayer::OnDetach()
	{
		NB_PROFILE_FN();
	}

	void VulkanImGuiLayer::OnEvent(Event& e)
	{
		NB_PROFILE_FN();
	}

	void VulkanImGuiLayer::BeginFrame()
	{
		NB_PROFILE_FN();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::EndFrame()
	{
		NB_PROFILE_FN();
		
		ImGui::Render();

		VulkanSwapchain& swapchain = VulkanRenderContext::Get()->GetSwapchain();
		VkCommandBuffer cmdBuffer = swapchain.GetCurrentFrameData().CommandBuffer;
		VkImage image = swapchain.GetCurrentImageData().Image;
		VkImageView imageView = swapchain.GetCurrentImageData().ImageView;

		Utils::TransitionImage(cmdBuffer, image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkRenderingAttachmentInfo colorAttachmentInfo = {};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.imageView = imageView; // Target image
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo = {};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.renderArea.extent = swapchain.GetVkExtent();
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.viewMask = 0;
		renderingInfo.layerCount = 1;

		m_vkCmdBeginRenderingKHR(cmdBuffer, &renderingInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
		m_vkCmdEndRenderingKHR(cmdBuffer);

		Utils::TransitionImage(cmdBuffer, image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
	}
}