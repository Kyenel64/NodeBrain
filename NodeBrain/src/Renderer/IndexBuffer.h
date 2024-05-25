#pragma once

#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static std::shared_ptr<IndexBuffer> Create(RenderContext* renderContext, uint32_t* data, uint32_t size);
	};
}