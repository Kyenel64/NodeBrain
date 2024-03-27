#pragma once

#include <VMA/vk_mem_alloc.h>

namespace NodeBrain
{
    class VulkanAllocator
    {
    public:
		VulkanAllocator();
		~VulkanAllocator();

		VmaAllocator GetVMAAllocator() const { return m_VMAAllocator; }

	private:
		void Init();
    private:
		VmaAllocator m_VMAAllocator;
    };
}