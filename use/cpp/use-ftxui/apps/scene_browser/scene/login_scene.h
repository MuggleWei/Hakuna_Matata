#ifndef SCENE_BROWSER_LOGIN_SCENE_H_
#define SCENE_BROWSER_LOGIN_SCENE_H_

#include "scene/scene.h"

class LoginScene : public Scene
{
public:
	LoginScene();
	virtual ~LoginScene() = default;

	virtual ftxui::Component InitComponents() override;
	virtual ftxui::Element RenderComponents() override;

	void onLoginBtnClick();

protected:
	std::string user_;
	std::string passwd_;
	std::string btnLabel_;

	ftxui::Component   inputUser_;
	ftxui::InputOption passwdOption_;
	ftxui::Component   inputPasswd_;
	ftxui::Component   loginBtn_;
};

#endif // !SCENE_BROWSER_LOGIN_SCENE_H_
