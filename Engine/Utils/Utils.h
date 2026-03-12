#ifndef UTILS__H
#define UTILS__H

#include <string>
#include <vector>


namespace Llyn
{
	namespace Utils
	{
		std::string GetFileContents(const char* _filePath);
		std::vector<unsigned char> ExtractTexturePart(const unsigned char* _data, int channels, int _faceSize, int _startX, int startY);
	}
}

#endif