#include "Utils.h"

#include <fstream>
#include <iostream>

#include "../External/imgui/imgui.h"

namespace Llyn
{
	namespace Utils
	{
		std::string GetFileContents(const char* _filePath)
		{
			std::ifstream in(_filePath, std::ios::binary);
			if (in)
			{
				std::string contents;
				in.seekg(0, std::ios::end);
				contents.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&contents[0], contents.size());
				in.close();
				return(contents);
			}
			throw(errno);
		}

		std::vector<unsigned char> ExtractTexturePart(const unsigned char* _data, int _channels, glm::vec2 _imageSize,
		                                              glm::vec2 _faceSize, glm::vec2 _startPos)
		{
			std::vector<unsigned char> data(_faceSize.x * _faceSize.y * _channels);

			for (int y = 0; y < static_cast<int>(_faceSize.y); ++y)
			{
				for (int x = 0; x < static_cast<int>(_faceSize.x); ++x)
				{
					int pX = x + static_cast<int>(_startPos.x);
					int pY = y + static_cast<int>(_startPos.y);
					for (int c = 0; c < _channels; ++c)
					{
						int idxImage = (pY * static_cast<int>(_imageSize.x) + pX) * _channels + c;
						int idxArray = (y * static_cast<int>(_faceSize.y) + x) * _channels + c;

						data[idxArray] = _data[idxImage];
					}
				}
			}
			return data;
		}

		void DrawVec3(glm::vec3* _vec, bool _angle, const std::string& _id)
		{
			auto drawAxis = [&](float& _value, ImVec4 _color, const char* _label)
				{
					ImGui::PushStyleColor(ImGuiCol_FrameBg, _color);

					ImGui::SetNextItemWidth(60);

					ImGui::DragFloat(_label, &_value, 0.1f, -359, 359);

					ImGui::PopStyleColor();
				};

			drawAxis(_vec->x, ImVec4(1.0f, 0.0f, 0.0f, 0.4f), std::string("##DrawVec3X" + _id).c_str());
			ImGui::SameLine();											   
			drawAxis(_vec->y, ImVec4(0.0f, 1.0f, 0.0f, 0.4f), std::string("##DrawVec3Y" + _id).c_str());
			ImGui::SameLine();											 
			drawAxis(_vec->z, ImVec4(0.0f, 0.0f, 1.0f, 0.4f), std::string("##DrawVec3Z" + _id).c_str());
		}

		bool CheckFileExtension(const char* _file, const char* _extension)
		{
			std::string pathStr = _file;
			size_t dotPos = pathStr.find_last_of('.');

			if (dotPos == std::string::npos || pathStr.substr(dotPos + 1) != _extension)
			{
				return false;
			}

			return true;
		}
	}
}
