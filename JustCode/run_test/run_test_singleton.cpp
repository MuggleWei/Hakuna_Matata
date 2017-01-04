#include <iostream>
#include "singleton.h"

class GameManager : public Singleton<GameManager>
{
public:
	void Init()
	{
		std::cout << "Game manager initialize" << std::endl;
	}
};

int main(int argc, char *argv[])
{
	GameManager::Instance().Init();

	return 0;
}