#pragma once

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	enum class GAPI { None = 0, Vulkan };
	class Renderer
	{
	public:
		static void Init();

		static GAPI GetGAPI();

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;
		static GAPI s_GAPI;
	};
}