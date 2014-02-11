#include "Logger.h"

Logger::Logger()
{
	_path = "C:\\Users\\Sawyer\\Desktop\\test\\Log.txt";
}

Logger::~Logger()
{
}

VOID Logger::Log(std::string entry){
	errno_t err = fopen_s(&_file, _path, "a");
	fprintf(_file, "%s", Format(entry).c_str());
	fclose(_file);
}

std::string Logger::Format(std::string entry){
	std::string formattedEntry = "[" + CurrentDateTime() + "] " + entry + "\n";
	return formattedEntry;
}

// Reference: http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
std::string Logger::CurrentDateTime(){
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}