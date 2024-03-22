#pragma once

namespace NodeBrain
{
	struct ImageConfiguration
	{
		uint32_t Width, Height;
	};

	class Image
	{
	public:
		virtual ~Image() = default;
		static std::shared_ptr<Image> Create(const ImageConfiguration& configuration); // TODO: implement
	};
}