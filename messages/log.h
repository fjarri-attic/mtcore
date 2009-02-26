#pragma once

#include <string>

class WriteToLog
{
public:
	std::string Str;

	WriteToLog(std::string &str) : Str(str) {}
	WriteToLog(const char *str) : Str(str) {}
};