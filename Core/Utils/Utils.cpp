#include "Utils.h"

#include <fstream>
#include <ios>
#include <iostream>

namespace Utils
{
	std::vector<char> ReadFile(const std::string& _filename)
	{
		std::ifstream file(_filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			std::cerr << "Failed to open file : " + _filename << std::endl;
		}

		std::vector<char> buffer(file.tellg());

		file.seekg(0, std::ios::beg);
		file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

		file.close();

		return buffer;
	}

	bool CheckFileExtension(const std::string& _file, const std::string& _ext)
	{
		const size_t dotPos = _file.find_last_of('.');

		if (dotPos == std::string::npos || _file.substr(dotPos + 1) != _ext)
		{
			return false;
		}

		return true;
	}
}
