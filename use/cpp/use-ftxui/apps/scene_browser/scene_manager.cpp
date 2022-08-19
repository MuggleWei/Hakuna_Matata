#include "scene_manager.h"
#include "muggle/c/muggle_c.h"

using namespace ftxui;

SceneManager* SceneManager::Instance()
{
	static SceneManager mgr;
	return &mgr;
}

SceneManager::SceneManager()
	: screen_(ftxui::ScreenInteractive::Fullscreen())
	, cur_scene_id_(-1)
{}
SceneManager::~SceneManager()
{}

void SceneManager::RegisterScene(int id, fnGenScene fn)
{
	sceneGenerator_[id] = fn;
}

void SceneManager::Run(int id)
{
	cur_scene_id_.store(id);

	do {
		auto it = sceneGenerator_.find(cur_scene_id_.load());
		if (it == sceneGenerator_.end())
		{
			LOG_ERROR("try to run scene that not exists: %d", id);
			exit(EXIT_FAILURE);
		}

		// for ctrl-C exit
		cur_scene_id_.store(-1);

		// generate scene
		auto fn = it->second;
		Scene *scene = fn();

		// render
		this->Render(scene);

		// release scene
		delete scene;

		// for ctrl-C exit
		if (cur_scene_id_.load() == -1)
		{
			break;
		}
	} while(true);
}

void SceneManager::SwitchScene(int id)
{
	auto it = sceneGenerator_.find(id);
	if (it == sceneGenerator_.end())
	{
		LOG_ERROR("try to switch scene that not exists: %d", id);
		return;
	}

	cur_scene_id_.store(id);

	// force scene exit
	auto fnExit = screen_.ExitLoopClosure();
	fnExit();
	// screen_.PostEvent(ftxui::Event::Custom);
}

void SceneManager::Render(Scene *scene)
{
	// render scene
	auto container = scene->Render();

	// add menu
	int menu_size = 1;
	auto menu = Renderer([] { return text("Hello World") | center; });
	container = ResizableSplitTop(menu, container, &menu_size);

	auto renderer = Renderer(container, [&] {
		return container->Render() | border;
	});

	// render loop
	screen_.Loop(renderer);
}
