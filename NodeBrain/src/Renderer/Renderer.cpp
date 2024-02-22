#include "NBpch.h"
#include "Renderer.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> s_RendererAPI = nullptr;
	GAPI s_GAPI = GAPI::Vulkan; // TEMP

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FN();

		s_RendererAPI.reset();
		NB_INFO("Shutdown renderer");
	}

	GAPI Renderer::GetGAPI() { return s_GAPI; }
}