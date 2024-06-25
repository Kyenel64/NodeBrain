#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"

int main()
{
	NB_PROFILE_BEGIN("Profile", "Profile.log");

	auto* window = new NodeBrain::Window("NodeBrain", 1280, 720);
	auto* renderContext = new NodeBrain::VulkanRenderContext(*window);
	auto* rendererAPI = new NodeBrain::VulkanRendererAPI(*renderContext);
	auto* gui = new NodeBrain::VulkanImGuiLayer(*renderContext);
	auto* renderer = new NodeBrain::Renderer(*rendererAPI);

	NodeBrain::Layer* brainEditor = new NodeBrain::BrainEditor(*renderer);
	auto* app = new NodeBrain::App("NodeBrain", *window, *renderer, gui);
	app->PushLayer(*brainEditor);


	app->Run();



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
