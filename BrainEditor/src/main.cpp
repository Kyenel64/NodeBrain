#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"

int main()
{
	NodeBrain::App* app = new NodeBrain::App();
	app->PushLayer(new NodeBrain::BrainEditor());

	NB_PROFILE_BEGIN("Runtime", "Runtime.log");
	app->Run();
	NB_PROFILE_END();

	delete app;

	return 0;
}
