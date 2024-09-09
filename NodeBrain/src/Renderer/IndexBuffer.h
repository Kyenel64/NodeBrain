#pragma once

#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(const uint32_t* data, uint32_t indexCount) = 0;

		// Total number of indices (Not in bytes).
		[[nodiscard]] virtual uint32_t GetSize() const = 0;
		[[nodiscard]] virtual uint32_t GetIndexCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(RenderContext& renderContext, const uint32_t* data, uint32_t indexCount);
	};
}
