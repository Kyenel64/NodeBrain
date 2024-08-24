#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"
#include "TestEditor.h"

#define NB_TEST_EDITOR

int main()
{
	NB_PROFILE_BEGIN("Profile", "Profile.log");

	NodeBrain::Window window("NodeBrain", 1280, 720);
	NodeBrain::VulkanRenderContext renderContext(window);
	NodeBrain::VulkanRendererAPI rendererAPI(renderContext);
	NodeBrain::VulkanImGuiLayer gui(renderContext);
	NodeBrain::Renderer renderer(rendererAPI);

	NodeBrain::App app("NodeBrain", window, renderer);
	NodeBrain::TestEditor testEditor(renderer);
	app.PushLayer(testEditor);

	app.Run();

	NB_PROFILE_END();

	return 0;
}
