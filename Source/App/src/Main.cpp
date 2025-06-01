#include "Game.h"
#include <iostream>

#define EXIT_KEY 'q'

int main()
{
	auto game = CreateGame();
	auto actor = Niflect::MakeShared<CActor>();
	game->AddActor(actor);

	printf(
R"(--------------------------------------
1. Hot-Swap:	Press [Enter]
2. Exit:	Press [%c] then [Enter]
--------------------------------------
)", EXIT_KEY);

	game->Start();
	printf("Started ...\n");

	while (true)
	{
		auto key = std::cin.get();
		if (key == EXIT_KEY)
			break;
	}
	game->Stop();
	return 0;
}