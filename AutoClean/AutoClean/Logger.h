#pragma once
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

class Logger
{
public:
	Logger();
	~Logger();
	VOID Log(std::string);
private:
	FILE* _file;
	char* _path;
	std::string Format(std::string);
	std::string CurrentDateTime();
};

