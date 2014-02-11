#include "Cleaner.h"

Cleaner::Cleaner()
{
}

Cleaner::Cleaner(char* path){
	_logger = Logger();
	_path = path;
}

Cleaner::~Cleaner()
{
}

HANDLE Cleaner::FindFile(WIN32_FIND_DATA* findFileData, HANDLE hFind){
	if (hFind == NULL){
		char newPath[1000];
		size_t size = strlen(_path);
		if (_path[size - 1] == '\\'){
			strcpy(newPath, _path);
			newPath[strlen(_path)] = '\0';
			strcat_s(newPath, "*.tmp*");
		}
		else{
			strcpy(newPath, _path);
			newPath[strlen(_path)] = '\0';
			strcat_s(newPath, "\\*.tmp*");
		}
		_logger.Log("Seaching... ");
		_logger.Log(newPath);

		hFind = FindFirstFile(newPath, findFileData);
		return hFind;
	}
	else{
		int result = FindNextFile(hFind, findFileData);
		if (result == 0){
			return INVALID_HANDLE_VALUE;
		}
		return hFind;
	}
}

// Reference: http://msdn.microsoft.com/en-us/library/aa364418%28VS.85%29.aspx
void Cleaner::Clean(){
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = NULL;
	while (true){
		hFind = FindFile(&findFileData, hFind);
		if (hFind == NULL || hFind == INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_NOT_FOUND){
			_logger.Log("No file can be found");
			break;
		}

		char newPath[1000];
		size_t size = strlen(_path);
		strcpy(newPath, _path);
		newPath[strlen(_path)] = '\0';
		if (_path[size - 1] != '\\'){
			strcat_s(newPath, "\\");
		}

		strcat_s(newPath, findFileData.cFileName);

		DeleteFile(newPath);
		memcpy(newPath + strlen(newPath), "\0\0", 2);
		SHFILEOPSTRUCT fileOp = {
			NULL,
			FO_DELETE,
			newPath,
			NULL,
			FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NOCONFIRMATION,
			FALSE,
			NULL,
			NULL
		};

		int result = SHFileOperation(&fileOp);

		if (result != 0){
			_logger.Log("The file or directory can't be removed");
			_logger.Log(newPath);
		}
		else{
			_logger.Log("Deleted File: ");
			_logger.Log(newPath);
		}
	}

	FindClose(hFind);
}