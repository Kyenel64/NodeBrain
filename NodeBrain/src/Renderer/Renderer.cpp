#include "NBpch.h"
#include "Renderer.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
	GAPI Renderer::s_GAPI = GAPI::Vulkan; // TEMP

	void Renderer::Init()
	{
		s_RendererAPI->Init();
	}

	GAPI Renderer::GetGAPI() { return s_GAPI; }
}