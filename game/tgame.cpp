#include "tgame.h"


void TGame::Init()
{
	state = 0;

	Kernel::SendMessage(WriteToLog("Starting game\n"));


	Kernel::SendMessage(InitRenderer());
}

void TGame::Handler(const UserWantsExit &message)
{
	state++;

	if(state == 1)
	{
		Kernel::SendMessage(WriteToLog("User wants exit - deny!\n"));
		Kernel::SendMessage(RestartRenderer());
		return;
	}

	if(state == 2)
	{
		Kernel::SendMessage(WriteToLog("User wants exit - allow!\n"));
		Kernel::SendMessage(CloseRenderer());
	}
}

