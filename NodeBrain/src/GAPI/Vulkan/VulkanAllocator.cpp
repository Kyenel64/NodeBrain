#include "NBpch.h"
#include "VulkanAllocator.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanAllocator::VulkanAllocator()
	{
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.physicalDevice = VulkanRenderContext::Get()->GetPhysicalDevice()->GetVkPhysicalDevice();
		allocatorCreateInfo.device = VulkanRenderContext::Get()->GetDevice()->GetVkDevice();
		allocatorCreateInfo.instance = VulkanRenderContext::Get()->GetVkInstance();
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

		VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, &m_VMAAllocator));
	}

	VulkanAllocator::~VulkanAllocator()
	{
		vmaDestroyAllocator(m_VMAAllocator);
		m_VMAAllocator = VK_NULL_HANDLE;
	}

}