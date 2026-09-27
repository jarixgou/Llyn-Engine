#ifndef UTILS__H
#define UTILS__H

#include <string>
#include <vector>

#include "../Vector/FwdVec3.h"

namespace Utils
{
	std::vector<char> ReadFile(const std::string& _filename);
	bool CheckFileExtension(const std::string& _file, const std::string& _ext);

	bool DrawVec3(Vec3f& _v, std::string _id);
}

#endif