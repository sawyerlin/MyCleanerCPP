#include "ServiceManager.h"
#include "RegisterManager.h"
#include "Cleaner.h"
#include "Logger.h"

#define SERVICE_NAME "AutoCleaner"
#define REG_KEY_SUB_PATH "SOFTWARE\\ETNA\\DiskCleaner"
#define ATTRIBUTE "path"

HKEY key = HKEY_LOCAL_MACHINE;

int _tmain(int argc, char* argv[]){

	char servicePath[500];
	GetModuleFileName(NULL, servicePath, 500);
	RegisterManager rManager = RegisterManager(HKEY_LOCAL_MACHINE, REG_KEY_SUB_PATH, KEY_ALL_ACCESS);
	ServiceManager sManager = ServiceManager(SERVICE_NAME, servicePath, rManager);

	if (argc > 1){
		if (!strcmp(argv[1], "-i"))
			sManager.InstallService();
		else if (!strcmp(argv[1], "-d"))
			sManager.UnistallService();

		if (argc > 2){
			if (!strcmp(argv[1], "-p")){
				rManager.CreateRegKey();
				rManager.OpenRegKey();
				rManager.SetRegKeyValue(ATTRIBUTE, argv[2]);
			}
		}
	}
	else{
		sManager.Dispatch();
	}

	return (EXIT_SUCCESS);
}