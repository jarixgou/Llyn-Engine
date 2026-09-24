#ifndef UTILS__H
#define UTILS__H

#include <string>
#include <vector>

namespace Utils
{
	std::vector<char> ReadFile(const std::string& _filename);
	bool CheckFileExtension(const std::string& _file, const std::string& _ext);
}

#endif