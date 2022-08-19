#include "scene.h"

using namespace ftxui;

Scene::Scene()
{}

Component Scene::Render()
{
	Component components = this->InitComponents();
	return Renderer(components, [&]{
		return this->RenderComponents();
	});

	// // add menu
	// int bottom_size = 1;
	// auto bottom = Renderer([] { return text("bottom") | center; });
	// container = ResizableSplitBottom(bottom, container, &bottom_size);

	// auto renderer = Renderer(container, [&] {
	// 	return container->Render() | border;
	// });

	// screen_->Loop(renderer);
}
