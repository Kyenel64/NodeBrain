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
	NodeBrain::App app("NodeBrain", window, renderer, &gui);
	NodeBrain::TestEditor editor(renderer);

	app.AddEventFunction(NB_BIND_EVENT_FN(&NodeBrain::TestEditor::OnEvent, &editor));

	while (app.IsRunning())
	{
		app.BeginFrame();

		editor.OnUpdate(app.GetDeltaTime());
		editor.OnUpdateGUI();

		app.EndFrame();
	}

	NB_PROFILE_END();


	return 0;
}
