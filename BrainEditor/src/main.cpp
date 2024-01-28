#include <NodeBrain/NodeBrain.h>

#include "BrainEditor.h"

int main()
{
	NodeBrain::App* app = new NodeBrain::App();
	app->PushLayer(new NodeBrain::BrainEditor());
	app->Run();
	delete app;

	return 0;
}
