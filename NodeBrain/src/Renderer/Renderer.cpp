#include "NBpch.h"
#include "Renderer.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/CommandBuffer.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> s_RendererAPI = nullptr;
	GAPI s_GAPI = GAPI::Vulkan; // TEMP

	struct RendererData
	{
		std::shared_ptr<CommandPool> CommandPool;
		std::shared_ptr<CommandBuffer> CommandBuffer;
		std::shared_ptr<Shader> TestVertexShader;
		std::shared_ptr<Shader> TestFragmentShader;
		std::shared_ptr<GraphicsPipeline> TestPipeline;
	};

	static RendererData* s_Data;

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_Data = new RendererData();

		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");

		s_Data->CommandPool = CommandPool::Create();
		s_Data->CommandBuffer = CommandBuffer::Create(s_Data->CommandPool);

		s_Data->TestVertexShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv");
		s_Data->TestFragmentShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv");
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = s_Data->TestVertexShader;
		pipelineConfig.FragmentShader = s_Data->TestFragmentShader;
		s_Data->TestPipeline = GraphicsPipeline::Create(pipelineConfig);
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FN();

		WaitForGPU();

		delete s_Data;

		s_RendererAPI.reset();
		NB_INFO("Shutdown renderer");
	}

	void Renderer::WaitForGPU()
	{
		s_RendererAPI->WaitForGPU();
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame(s_Data->CommandBuffer);
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame(s_Data->CommandBuffer);
	}

	void Renderer::Begin(std::shared_ptr<Framebuffer> framebuffer)
	{
		if (framebuffer)
			s_RendererAPI->BeginRenderPass(s_Data->CommandBuffer, framebuffer->GetConfiguration().RenderPass);
		else
			s_RendererAPI->BeginRenderPass(s_Data->CommandBuffer);
	}

	void Renderer::End()
	{
		s_RendererAPI->EndRenderPass(s_Data->CommandBuffer);
	}

	void Renderer::DrawTestTriangle()
	{
		s_RendererAPI->DrawTestTriangle(s_Data->CommandBuffer, s_Data->TestPipeline);
	}

	GAPI Renderer::GetGAPI() { return s_GAPI; }
}