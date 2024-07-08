#pragma once

namespace NodeBrain
{
	enum class ImageFormat { None = 0, RGBA8, RGBA16 };

	struct Texture2DConfiguration
	{
		uint32_t Width, Height;
		ImageFormat Format;
	};

	class Texture2D
	{
	public:
		virtual ~Texture2D() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;

		// Pointer address is equal to VkDescriptorSet. Can only be accessed by ImGui.
		virtual uint64_t GetAddress() = 0;
		[[nodiscard]] virtual const Texture2DConfiguration& GetConfiguration() const = 0;

		static std::shared_ptr<Texture2D> Create(RenderContext& context, const Texture2DConfiguration& configuration);
		static std::shared_ptr<Texture2D> Create(RenderContext& context, std::filesystem::path path);
	};
}
