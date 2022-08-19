#ifndef SCENE_BROWSER_SCENE_MANAGER_H_
#define SCENE_BROWSER_SCENE_MANAGER_H_

#include "scene/scene.h"
#include <atomic>
#include <map>

enum
{
	SCENE_ID_NULL = 0,
	SCENE_ID_LOGIN, //!< login scene
	SCENE_ID_POSTS, //!< posts scene
};

typedef std::function<Scene*()> fnGenScene;

class SceneManager
{
public:
	static SceneManager* Instance();

public:
	SceneManager();
	virtual ~SceneManager();

	void RegisterScene(int id, fnGenScene fn);

	void Run(int id);

	void SwitchScene(int id);

protected:
	void Render(Scene *scene);

private:
	ftxui::ScreenInteractive screen_;
	std::map<int, fnGenScene> sceneGenerator_;

	std::atomic<int> cur_scene_id_;
};

#endif // !SCENE_BROWSER_SCENE_MANAGER_H_
