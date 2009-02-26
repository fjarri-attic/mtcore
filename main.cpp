#include "mtcore\kernel.h"

#include "messages\log.h"
#include "log\tlog.h"

#include "messages\game.h"
#include "game\tgame.h"

#include "messages\renderer.h"
#include "renderer\trenderer.h"

int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd)
{
	Kernel::Init(1);

	Kernel::SetStartingParameters(hInstance, lpCmdLine, nShowCmd);

	Kernel::AddTask<TLog>(0);
	Kernel::RegisterHandler<TLog, WriteToLog>();

	Kernel::AddTask<TGame>(0);
	Kernel::RegisterHandler<TGame, UserWantsExit>();

	Kernel::AddTask<TRenderer>(0);
	Kernel::RegisterHandler<TRenderer, InitRenderer>();
	Kernel::RegisterHandler<TRenderer, CloseRenderer>();
	Kernel::RegisterHandler<TRenderer, RestartRenderer>();

	Kernel::StartCycle();

	Kernel::Release();

	return 0;
}