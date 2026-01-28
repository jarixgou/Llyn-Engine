#ifndef UTILS__H
#define UTILS__H

#include <string>

#include "../Macro.h"

namespace ENGINE_NAME
{
	namespace Utils
	{
		std::string GetFileContents(const char* _filePath);
	}
}

#endif