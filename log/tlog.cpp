#include "tlog.h"

using namespace std;

void TLog::Init()
{
	logFile.open("log.txt", ios_base::out | ios_base::trunc);
	logFile.close();
}

void TLog::Terminate()
{
    logFile.close();
}

void TLog::Handler(const WriteToLog &message)
{
	logFile.open("log.txt", ios_base::out | ios_base::app);
	logFile << message.Str;
	logFile.close();
}
