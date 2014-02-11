#pragma once
#include <Windows.h>
#include <Winreg.h>
#include <tchar.h>
#include <iostream>
#include "Logger.h"

class RegisterManager
{
public:
	RegisterManager();
	RegisterManager(HKEY, char*, DWORD);
	~RegisterManager();
	VOID CreateRegKey();
	VOID OpenRegKey();
	VOID SetRegKeyValue(char*, char*);
	std::string GetRegKeyValue(char*);
	VOID DeleteRegKey();
private:
	HKEY _rootKey;
	char* _subKey;
	DWORD _accessRight;
	HKEY _currentKey;
	Logger _logger;
};

