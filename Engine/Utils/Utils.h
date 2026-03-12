#ifndef UTILS__H
#define UTILS__H

#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace Llyn
{
	namespace Utils
	{
		std::string GetFileContents(const char* _filePath);
		std::vector<unsigned char> ExtractTexturePart(const unsigned char* _data, int _channels, glm::vec2 _imageSize, glm::vec2 _faceSize, glm::vec2 _startPos);
	}
}

#endif