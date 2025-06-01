#include "Game.h"
#include <iostream>

int main()
{
	auto game = CreateGame();
	auto actor = Niflect::MakeShared<CActor>();
	game->AddActor(actor);

	game->Start();
	while (true)
	{
		auto key = std::cin.get();
		if (key == 'q')
			break;
	}
	game->Stop();
	return 0;
}