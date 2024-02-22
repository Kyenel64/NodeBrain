#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"

int main()
{
	NB_PROFILE_BEGIN("Startup", "Startup.log");
	NodeBrain::App* app = new NodeBrain::App();
	NB_PROFILE_END();

	app->PushLayer(new NodeBrain::BrainEditor());

	NB_PROFILE_BEGIN("Runtime", "Runtime.log");
	app->Run();
	NB_PROFILE_END();

	delete app;

	return 0;
}
