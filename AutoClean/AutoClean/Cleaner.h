#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "Logger.h"
#include <wchar.h>	
#include <Windows.h>


class Cleaner
{
public:
	Cleaner();
	Cleaner(char* path);
	~Cleaner();
	void Clean();
private:
	char* _path;
	Logger _logger;
	HANDLE	FindFile(WIN32_FIND_DATA*, HANDLE);
};

