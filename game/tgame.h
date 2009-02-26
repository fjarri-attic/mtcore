#pragma once

#include "..\mtcore\interfaces.h"
#include "..\mtcore\kernel.h"

#include "..\messages\log.h"
#include "..\messages\renderer.h"
#include "..\messages\game.h"

class TGame : public Kernel::ITask
{
private:
	int state;

public:
	void Init();

	void Handler(const UserWantsExit &message);
};