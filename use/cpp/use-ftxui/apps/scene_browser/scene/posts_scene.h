#ifndef SCENE_BROWSER_POSTS_SCENE_H_
#define SCENE_BROWSER_POSTS_SCENE_H_

#include "scene/scene.h"

class PostsScene : public Scene
{
public:
	PostsScene();
	virtual ~PostsScene() = default;

	virtual ftxui::Component InitComponents() override;
	virtual ftxui::Element RenderComponents() override;

	void onClickTitle(int idx);

protected:
	int leftSize_;
	std::vector<std::string> titles_;
	std::vector<std::string> contents_;

	std::string *postContent_;

	std::vector<ftxui::Component> titleBtns_;

	ftxui::Component left_;
	ftxui::Component main_;
	ftxui::Component container_;
};

#endif // !SCENE_BROWSER_POSTS_SCENE_H_
