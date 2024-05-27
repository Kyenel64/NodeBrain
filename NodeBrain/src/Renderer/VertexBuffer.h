#pragma once

#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;
		
		virtual uint64_t GetAddress() const = 0;

		static std::shared_ptr<VertexBuffer> Create(RenderContext* context, const void* data, uint32_t size);
	};
}