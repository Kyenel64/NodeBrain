#pragma once

namespace NodeBrain
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;
		
		virtual uint64_t GetAddress() const = 0;

		static std::shared_ptr<VertexBuffer> Create(uint32_t size);
	};
}