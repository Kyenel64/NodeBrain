#pragma once

namespace NodeBrain
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual uint64_t GetAddress() const = 0;

		static std::shared_ptr<UniformBuffer> Create(const void* data, uint32_t size);
	};
}