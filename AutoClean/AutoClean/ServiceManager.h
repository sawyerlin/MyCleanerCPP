// Reference http://msdn.microsoft.com/en-us/library/windows/desktop/ms687413(v=vs.85).aspx

#include <Windows.h>
#include <Winreg.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "Logger.h"
#include "Cleaner.h"
#include "RegisterManager.h"

class ServiceManager
{
public:
	ServiceManager();
	ServiceManager(char*, char*, RegisterManager);
	~ServiceManager();
	BOOL InstallService();
	BOOL UnistallService();
	void Dispatch();
};

