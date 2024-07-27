#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"
#include "TestEditor.h"

#define NB_TEST_EDITOR

int main()
{
	NB_PROFILE_BEGIN("Profile", "Profile.log");

	auto* window = new NodeBrain::Window("NodeBrain", 1280, 720);
	auto* renderContext = new NodeBrain::VulkanRenderContext(*window);
	auto* rendererAPI = new NodeBrain::VulkanRendererAPI(*renderContext);
	auto* gui = new NodeBrain::VulkanImGuiLayer(*renderContext);
	auto* renderer = new NodeBrain::Renderer(*rendererAPI);
#ifdef NB_TEST_EDITOR
	NodeBrain::Layer* testEditor = new NodeBrain::TestEditor(*renderer);
	auto* app = new NodeBrain::App("NodeBrain", *window, *renderer, gui);
	app->PushLayer(*testEditor);
#else
	NodeBrain::Layer* brainEditor = new NodeBrain::BrainEditor(*renderer);
	auto* app = new NodeBrain::App("NodeBrain", *window, *renderer, gui);
	app->PushLayer(*brainEditor);
#endif


	app->Run();


	delete app;
#ifdef NB_TEST_EDITOR
	delete testEditor;
#else
	delete brainEditor;
#endif
	delete renderer;
	delete gui;
	delete rendererAPI;
	delete renderContext;
	delete window;

	NB_PROFILE_END();


	return 0;
}
