#ifndef LOGGER__H
#define LOGGER__H

#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

enum class LoggerLevel : uint8_t
{
	None,
	Info,
	Warning,
	Error,
	Critical
};

struct LoggerInfo
{
	LoggerLevel level = LoggerLevel::None;
	tm time;
	std::string message;
};

class Logger
{
private:
	LoggerLevel m_level = LoggerLevel::None;
	std::vector<LoggerInfo> m_loggerInfo;
public:
	static Logger& Get();

	void Print(const std::string& _message, LoggerLevel _level);
private:
	static std::string GetLevelInString(LoggerLevel _level);
};

#define LOGGER_INFO(x) Logger::Get().Print(x, LoggerLevel::Info)
#define LOGGER_WARNING(x) Logger::Get().Print(x, LoggerLevel::Warning)
#define LOGGER_ERROR(x) Logger::Get().Print(x, LoggerLevel::Error)
#define LOGGER_CRTICAL(x) Logger::Get().Print(x, LoggerLevel::Critical)

#endif