#include "posts_scene.h"

#include "muggle/c/muggle_c.h"

using namespace ftxui;

PostsScene::PostsScene()
	: leftSize_(20)
	, postContent_(nullptr)
{
	static std::string emptyStr = "";
	postContent_ = &emptyStr;

	titles_.push_back("C");
	contents_.push_back(
		"C is a general-purpose computer programming language. "
		"It was created in the 1970s by Dennis Ritchie, and remains "
		"very widely used and influential.");

	titles_.push_back("Python");
	contents_.push_back(
		"Python is a high-level, interpreted, general-purpose programming "
		"language. Its design philosophy emphasizes code readability with "
		"the use of significant indentation.");

	titles_.push_back("C++");
	contents_.push_back(
		"C++ is a general-purpose programming language created by Danish "
		"computer scientist Bjarne Stroustrup as an extension of the C "
		"programming language, or \"C with Classes\".");

	titles_.push_back("Java");
	contents_.push_back(
		"Java is a high-level, class-based, object-oriented programming "
		"language that is designed to have as few implementation dependencies "
		"as possible.");

	titles_.push_back("Go");
	contents_.push_back(
		"Go is a statically typed, compiled programming language designed "
		"at Google by Robert Griesemer, Rob Pike, and Ken Thompson.");
}

ftxui::Component PostsScene::InitComponents()
{
	for (int i = 0; i < (int)titles_.size(); i++)
	{
		titleBtns_.push_back(Button(&titles_[i], [&, i]{
			this->onClickTitle(i);
		}));
	}

	left_ = Container::Vertical(titleBtns_);
	// main_ = Renderer([] { return text("Main") | center; });
	main_ = Renderer([&] { return  hflow(paragraph(*postContent_)); });

	container_ = ResizableSplitLeft(left_, main_, &leftSize_);

	return container_;
}

ftxui::Element PostsScene::RenderComponents()
{
	Element element = container_->Render() | border;
	return element;
}

void PostsScene::onClickTitle(int idx)
{
	LOG_INFO("click title: %s", titles_[idx].c_str());
	postContent_ = &contents_[idx];
}
