#include "RegisterManager.h"

RegisterManager::RegisterManager()
{
}

RegisterManager::RegisterManager(HKEY rootKey, char* subKey, DWORD accessRight){
	_rootKey = rootKey;
	_subKey = subKey;
	_accessRight = accessRight;
	_currentKey = NULL;
	_logger = Logger();
}

RegisterManager::~RegisterManager()
{
	if (_currentKey != NULL){
		RegCloseKey(_currentKey);
	}
}

VOID RegisterManager::CreateRegKey(){
	if (_currentKey != NULL){
		_logger.Log("The key is already created\n");
		return;
	}

	LONG result = RegCreateKeyEx(
		_rootKey,
		_subKey,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		_accessRight,
		NULL,
		&_currentKey,
		NULL
		);

	if (result != ERROR_SUCCESS){
		_logger.Log("The creation of key is failed\n");
	}
}

VOID RegisterManager::OpenRegKey(){
	if (_currentKey != NULL){
		_logger.Log("The key is opened\n");
		return;
	}

	HKEY newRegKey = NULL;
	LONG result = RegOpenKeyEx(
		_rootKey,
		_subKey,
		0,
		_accessRight,
		&_currentKey
		);

	if (result != ERROR_SUCCESS){
		_logger.Log("The opening of key is failed\n");
	}
}

VOID RegisterManager::SetRegKeyValue(char* valueName, char* value){
	if (_currentKey == NULL){
		_logger.Log("NO key can be Found\n");
		return;
	}

	DWORD size = lstrlen(value);
	LONG result = RegSetValueEx(_currentKey, valueName, 0, REG_SZ, (BYTE*)value, size);

	if (result != ERROR_SUCCESS){
		_logger.Log("Can't set key value\n");
	}
}

std::string RegisterManager::GetRegKeyValue(char* key){
	if (_currentKey == NULL){
		_logger.Log("No key can be Found\n");
		return NULL;
	}

	char returnValue[512];
	DWORD dwBufferSize = sizeof(returnValue);
	DWORD type = REG_SZ;
	LONG result = RegQueryValueEx(_currentKey, key, NULL, &type, (BYTE*)returnValue, &dwBufferSize);

	if (result != ERROR_SUCCESS){
		_logger.Log("The finding of key is failed\n");
		return NULL;
	}
	
	return std::string(returnValue);
}

VOID RegisterManager::DeleteRegKey(){
	if (_currentKey == NULL){
		_logger.Log("No key can be deleted\n");
		return;
	}

	LONG result = RegDeleteKeyEx(_rootKey, _subKey, _accessRight, 0);

	if (result != ERROR_SUCCESS){
		_logger.Log("The deletion of key is failed\n");
	}
}
