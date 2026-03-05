#ifndef LOGGER__H
#define LOGGER__H

#include <cstdint>

namespace Llyn
{
	enum class LoggerType : uint8_t
	{
		MEMORY,
		DEBUG,
		INFO,
		WARNING,
		CRITICAL
	};

	void Logger(LoggerType _type, const char* _content, const char* _file, int _line);

#define	LOG_MEMORY(x, y, z) \
	Logger(LoggerType::MEMORY, x, y, z)

#define LOG_DEBUG(x, y, z) \
	Logger(LoggerType::DEBUG, x, y, z)

#define LOG_INFO(x, y, z) \
	Logger(LoggerType::INFO, x, y, z)

#define LOG_WARNING(x, y, z) \
	Logger(LoggerType::WARNING, x, y, z)

#define LOG_CRITICAL(x, y, z) \
	Logger(LoggerType::CRITICAL, x, y, z)
}

#endif