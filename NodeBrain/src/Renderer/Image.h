#pragma once

namespace NodeBrain
{
	enum class ImageFormat { None = 0, RGBA8, RGBA16 };

	struct ImageConfiguration
	{
		uint32_t Width, Height;
		ImageFormat ImageFormat;
	};

	class Image
	{
	public:
		virtual ~Image() = default;
		static std::shared_ptr<Image> Create(const ImageConfiguration& configuration); // TODO: implement
	};
}