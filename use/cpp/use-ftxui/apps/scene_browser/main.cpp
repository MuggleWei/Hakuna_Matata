#include "muggle/c/muggle_c.h"
#include "scene_manager.h"
#include "scene/login_scene.h"
#include "scene/posts_scene.h"

#define REGISTER_SCENE(id, cl) \
	SceneManager::Instance()->RegisterScene(id, []() -> Scene* { return new cl(); })

int main()
{
	// init log
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, "log/tbbs_client.log");

	// register scene
	REGISTER_SCENE(SCENE_ID_LOGIN, LoginScene);
	REGISTER_SCENE(SCENE_ID_POSTS, PostsScene);

	// run
	SceneManager::Instance()->Run(SCENE_ID_LOGIN);

	return 0;
}
