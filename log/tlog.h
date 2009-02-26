#pragma once

#include "..\mtcore\interfaces.h"
#include "..\mtcore\kernel.h"

#include "..\messages\log.h"

#include <fstream>

class TLog : public Kernel::ITask
{
private:
	std::ofstream logFile;

public:
	void Init();
	void Terminate();

	void Handler(const WriteToLog &message);
};