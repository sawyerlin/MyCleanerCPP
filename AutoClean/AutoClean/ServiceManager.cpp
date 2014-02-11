#include "ServiceManager.h"

char* _servicePath;
char* _serviceName;
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE statusHandle;
HANDLE stopEventHandle = NULL;
Logger _logger;
RegisterManager _rManager;

ServiceManager::ServiceManager()
{
	_logger = Logger();
}

ServiceManager::ServiceManager(char* serviceName, char* servicePath, RegisterManager rManager){
	_serviceName = serviceName;
	_servicePath = servicePath;
	_logger = Logger();
	_rManager = rManager;
}

ServiceManager::~ServiceManager()
{
}

BOOL ServiceManager::InstallService(){

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == 0){
		_logger.Log("Install - Can't find SCManager");
		return false;
	}

	SC_HANDLE schService = CreateService(
		schSCManager,
		_serviceName,
		_serviceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_DEMAND_START, // Le nettoyage du service doit s’effectuer en manuel (paramètre « dwStartType » de CreateService)
		SERVICE_ERROR_NORMAL,
		_servicePath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
		);

	if (schService == 0){
		_logger.Log("Install - Can't install service");
		return false;
	}

	_logger.Log("Install - Service is installed successfully");
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

BOOL ServiceManager::UnistallService(){
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == 0){
		_logger.Log("Unistall - Can't find SCManager");
		return false;
	}

	SC_HANDLE schService = OpenService(schSCManager, _serviceName, SERVICE_ALL_ACCESS);
	if (schService == 0){
		_logger.Log("Unistall - Can't find the Service");
		return false;
	}

	if (!DeleteService(schService)){
		_logger.Log("Unistall - Can't delete the Service");
		return false;
	}

	_logger.Log("Unistall - Service is deleted successfully");
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID ReportSvcStatus(
	DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	serviceStatus.dwCurrentState = dwCurrentState;
	serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
	serviceStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		serviceStatus.dwControlsAccepted = 0;
	else serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		serviceStatus.dwCheckPoint = 0;
	else serviceStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(statusHandle, &serviceStatus);
}

VOID Clean(){
	_rManager.OpenRegKey();
	std::string str = _rManager.GetRegKeyValue("path");
	char path[1000];
	int length = str.length();
	for (int i = 0; i < length; i++){
		path[i] = str[i];
	}
	path[length] = '\0';
	Cleaner cleaner = Cleaner(path);

	while (1){
		std::this_thread::sleep_for(std::chrono::seconds(10));
		cleaner.Clean();
	}
}

VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with 
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.

	stopEventHandle = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (stopEventHandle == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.
	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	_logger.Log("ServiceMain - Service is started");

	std::thread cleanThread(Clean);

	while (1)
	{

		// Check whether to stop the service.

		WaitForSingleObject(stopEventHandle, INFINITE);

		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);

		std::terminate();

		return;
	}
}

void WINAPI ServiceControlHandler(DWORD Opcode){
	switch (Opcode){
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		SetEvent(stopEventHandle);
		ReportSvcStatus(serviceStatus.dwCurrentState, NO_ERROR, 0);
		_logger.Log("Service Controller - The service is stopped");
		break;
	case SERVICE_CONTROL_PAUSE:
		_logger.Log("Service Controller - The service is paused");
		break;
	case SERVICE_CONTROL_CONTINUE:
		_logger.Log("Service Controller - The service is continued");
		break;
	case SERVICE_CONTROL_INTERROGATE:
		_logger.Log("Service Controller - The service is interrogated");
		break;
	default:
		_logger.Log("Service Controller - The service is started");
		break;
	}
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv){
	DWORD status = E_FAIL;
	statusHandle = RegisterServiceCtrlHandler(_serviceName, ServiceControlHandler);
	if (statusHandle == NULL){
		_logger.Log("ServiceMain - Status Handle is not registered successfully");
		return;
	}

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwServiceSpecificExitCode = 0;

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
	_logger.Log("ServiceMain - Service is starting ...");

	SvcInit(argc, argv);
}

void ServiceManager::Dispatch(){
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{ _serviceName, ServiceMain },
		{ NULL, NULL }
	};
	StartServiceCtrlDispatcher(DispatchTable);
}