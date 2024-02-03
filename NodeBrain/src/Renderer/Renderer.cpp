#include "NBpch.h"
#include "Renderer.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> s_RendererAPI = nullptr;
	GAPI s_GAPI = GAPI::Vulkan; // TEMP

	void Renderer::Init()
	{
		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");
	}

	void Renderer::Shutdown()
	{
		s_RendererAPI.reset();
		NB_INFO("Shutdown renderer");
	}

	GAPI Renderer::GetGAPI() { return s_GAPI; }
}