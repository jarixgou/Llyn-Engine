#include "Logger.h"

#include <iostream>

Logger& Logger::Get()
{
	static Logger logger;
	return logger;
}

void Logger::Print(const std::string& _message, LoggerLevel _level)
{
	if (m_level == _level)
	{
		return;
	}

	// Get the current time
	std::time_t time = std::time(nullptr);

	LoggerInfo newInfo;
	newInfo.level = _level;
	localtime_s(&newInfo.time, &time);
	newInfo.message = _message;

	std::cout << "[" + std::to_string(newInfo.time.tm_hour) + ":" + std::to_string(newInfo.time.tm_min) + ":" +
		std::to_string(newInfo.time.tm_sec) + "]" + " [" + GetLevelInString(newInfo.level) + "]" + " " + _message << "\n";
}

std::string Logger::GetLevelInString(LoggerLevel _level)
{
	std::string level;

	if (_level == LoggerLevel::Info)
	{
		level = "Info";
	}
	else if (_level == LoggerLevel::Warning)
	{
		level = "Warning";
	}
	else if (_level == LoggerLevel::Error)
	{
		level = "Error";
	}
	else if (_level == LoggerLevel::Critical)
	{
		level = "Critical";
	}
	else
	{
		level = "UNKNOW";
	}

	return level;
}
