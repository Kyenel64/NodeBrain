#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"

int main()
{
	NB_PROFILE_BEGIN("Startup", "Startup.log");

	NodeBrain::Window* window = new NodeBrain::Window("NodeBrain", 1280, 720);
	NodeBrain::VulkanRenderContext* renderContext = new NodeBrain::VulkanRenderContext(window);
	NodeBrain::VulkanRendererAPI* rendererAPI = new NodeBrain::VulkanRendererAPI(renderContext);
	NodeBrain::VulkanImGuiLayer* gui = new NodeBrain::VulkanImGuiLayer(renderContext);
	NodeBrain::Renderer* renderer = new NodeBrain::Renderer(rendererAPI);

	NodeBrain::Layer* brainEditor = new NodeBrain::BrainEditor(renderer);
	NodeBrain::App* app = new NodeBrain::App("NodeBrain", window, renderer, gui);
	app->PushLayer(brainEditor);

	NB_PROFILE_END();


	NB_PROFILE_BEGIN("Runtime", "Runtime.log");

	app->Run();

	NB_PROFILE_END();


	NB_PROFILE_BEGIN("Shutdown", "Shutdown.log");

	delete app;
	delete brainEditor;
	delete renderer;
	delete gui;
	delete rendererAPI;
	delete renderContext;
	delete window;

	NB_PROFILE_END();

	return 0;
}
