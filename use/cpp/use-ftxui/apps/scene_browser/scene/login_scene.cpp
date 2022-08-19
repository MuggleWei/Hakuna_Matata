#include "login_scene.h"

#include "muggle/c/muggle_c.h"
#include "scene_manager.h"

using namespace ftxui;

LoginScene::LoginScene()
	: btnLabel_("login")
{}

ftxui::Component LoginScene::InitComponents()
{
	// user
	inputUser_ = Input(&user_, "user");

	// passwd
	passwdOption_.password = true;
	inputPasswd_ = Input(&passwd_, "passwd", passwdOption_);

	// login button
	loginBtn_ = Button(&btnLabel_, std::bind(&LoginScene::onLoginBtnClick, this));

	return Container::Vertical({
		inputUser_,
		inputPasswd_,
		loginBtn_,
	});
}
ftxui::Element LoginScene::RenderComponents()
{
	return vbox({
		hbox(text("  user: "), inputUser_->Render()),
		hbox(text("passwd: "), inputPasswd_->Render()),
		separator(),
		hbox(loginBtn_->Render()) | center,
	}) | center;
}

void LoginScene::onLoginBtnClick()
{
	LOG_INFO("login btn click: user=%s", user_.c_str());
	SceneManager::Instance()->SwitchScene(SCENE_ID_POSTS);
}
