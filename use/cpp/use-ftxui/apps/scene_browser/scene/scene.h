#ifndef SCENE_BROWSER_SCENE_H_
#define SCENE_BROWSER_SCENE_H_

#include <memory>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

class Scene
{
public:
	Scene();
	virtual ~Scene() = default;

	virtual ftxui::Component Render();

	virtual ftxui::Component InitComponents() = 0;
	virtual ftxui::Element RenderComponents() = 0;

protected:
	ftxui::ScreenInteractive *screen_;
};

#endif // !SCENE_BROWSER_SCENE_H_
