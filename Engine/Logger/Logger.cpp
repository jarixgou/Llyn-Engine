#include "Logger.h"

#include <ctime>
#include <iostream>
#include <string>

namespace Llyn
{
	std::string GetColorLog(LoggerType _type);
	std::string GetLogName(LoggerType _type);

	void Logger(LoggerType _type, const char* _content, const char* _file, int _line)
	{
		std::string color = GetColorLog(_type);
		std::string name = GetLogName(_type);

		std::string file = std::string(_file).substr(std::string(_file).find_last_of("/\\") + 1);

		std::time_t time = std::time(0);
		std::tm tm;
		localtime_s(&tm, &time);
		std::cout << "[" << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << "]" << " " << "[" << file << ":" << _line << "]"
		<< " " << color << "[" << name << "]" <<
			"\033[0m" << " " << _content << std::endl;
	}

	std::string GetColorLog(LoggerType _type)
	{
		std::string color;

		if (_type == LoggerType::MEMORY)
		{
			color = "\033[35m"; // Magenta
		}
		else if (_type == LoggerType::DEBUG)
		{
			color = "\033[34m";	// BLUE
		}
		else if (_type == LoggerType::INFO)
		{
			color = "\033[32m"; // GREEN 
		}
		else if (_type == LoggerType::WARNING)
		{
			color = "\033[33m"; // Yellow
		}
		else if (_type == LoggerType::CRITICAL)
		{
			color = "\033[31m"; // Red
		}

		return color;
	}

	std::string GetLogName(LoggerType _type)
	{
		std::string name;

		if (_type == LoggerType::MEMORY)
		{
			name = "MEMORY";
		}
		else if (_type == LoggerType::DEBUG)
		{
			name = "DEBUG";
		}
		else if (_type == LoggerType::INFO)
		{
			name = "INFO";
		}
		else if (_type == LoggerType::WARNING)
		{
			name = "WARNING";
		}
		else if (_type == LoggerType::CRITICAL)
		{
			name = "CRITICAL";
		}

		return name;
	}
}
