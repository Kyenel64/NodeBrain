#pragma once

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	enum class GAPI { None = 0, Vulkan };
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static GAPI GetGAPI();
	};
}