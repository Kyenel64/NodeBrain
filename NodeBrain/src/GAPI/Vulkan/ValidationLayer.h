#pragma once

#include <vulkan/vulkan.h>

namespace NodeBrain
{
	class ValidationLayer
	{
	public:
		ValidationLayer();
		~ValidationLayer();

		void Setup(VkInstance instance);

		uint32_t GetLayerCount() const { return m_ValidationLayers.empty() ? 0 : m_ValidationLayers.size(); }
		const std::vector<const char*>& GetLayers() const { return m_ValidationLayers; }

		void PopulateCreateInfo(VkInstanceCreateInfo& createInfo, std::vector<const char*>& extensions, VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);

	private:
		bool CheckValidationLayerSupport();

	private:
		VkInstance m_VkInstance = nullptr;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		std::vector<const char*> m_ValidationLayers;
	};
}