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
}
