#include "Utils.h"

#include <fstream>
#include <ios>
#include <iostream>

#include "../Logger.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../Vector/Vec3.h"

#include "../Render/Format.h"

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

	bool DrawVec3(Vec3f& _v, std::string _id)
	{
		bool touched = false;

		ImGui::PushID(_id.c_str());

		// X component
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

		ImGui::Button("##Bx");

		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##Fx", &_v.x, 1, -FLT_MAX, FLT_MAX, "%.3f"))
		{
			touched = true;
		}

		// Y component
		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

		ImGui::Button("##By");

		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##Fy", &_v.y, 1, -FLT_MAX, FLT_MAX, "%.3f"))
		{
			touched = true;
		}

		// Z component
		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));

		ImGui::Button("##Bz");

		ImGui::PopItemFlag();
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##Fz", &_v.z, 1, -FLT_MAX, FLT_MAX, "%.3f"))
		{
			touched = true;
		}

		ImGui::PopID();

		return touched;
	}

	VkFormat GetVkFormat(const Format& _format)
	{
		switch (_format)
		{
		case FORMAT_R8_SINT:				return VK_FORMAT_R8_SINT;
		case FORMAT_R8G8_SINT:				return VK_FORMAT_R8G8_SINT;
		case FORMAT_R8G8B8_SINT:			return VK_FORMAT_R8G8B8_SINT;
		case FORMAT_R8G8B8A8_SINT:			return VK_FORMAT_R8G8B8A8_SINT;
		case FORMAT_R8_UINT:				return VK_FORMAT_R8_UINT;
		case FORMAT_R8G8_UINT:				return VK_FORMAT_R8G8_UINT;
		case FORMAT_R8G8B8_UINT:			return VK_FORMAT_R8G8B8_UINT;
		case FORMAT_R8G8B8A8_UINT:			return VK_FORMAT_R8G8B8A8_UINT;
		case FORMAT_R16_SINT:				return VK_FORMAT_R16_SINT;
		case FORMAT_R16G16_SINT:			return VK_FORMAT_R16G16_SINT;
		case FORMAT_R16G16B16_SINT:			return VK_FORMAT_R16G16B16_SINT;
		case FORMAT_R16G16B16A16_SINT:		return VK_FORMAT_R16G16B16A16_SINT;
		case FORMAT_R16_UINT:				return VK_FORMAT_R16_UINT;
		case FORMAT_R16G16_UINT:			return VK_FORMAT_R16G16_UINT;
		case FORMAT_R16G16B16_UINT:			return VK_FORMAT_R16G16B16_UINT;
		case FORMAT_R16G16B16A16_UINT:		return VK_FORMAT_R16G16B16A16_UINT;
		case FORMAT_R16_SFLOAT:				return VK_FORMAT_R16_SFLOAT;
		case FORMAT_R16G16_SFLOAT:			return VK_FORMAT_R16G16_SFLOAT;
		case FORMAT_R16G16B16_SFLOAT:		return VK_FORMAT_R16G16B16_SFLOAT;
		case FORMAT_R16G16B16A16_SFLOAT:	return VK_FORMAT_R16G16B16A16_SFLOAT;
		case FORMAT_R32_SINT:				return VK_FORMAT_R32_SINT;
		case FORMAT_R32G32_SINT:			return VK_FORMAT_R32G32_SINT;
		case FORMAT_R32G32B32_SINT:			return VK_FORMAT_R32G32B32_SINT;
		case FORMAT_R32G32B32A32_SINT:		return VK_FORMAT_R32G32B32A32_SINT;
		case FORMAT_R32_UINT:				return VK_FORMAT_R32_UINT;
		case FORMAT_R32G32_UINT:			return VK_FORMAT_R32G32_UINT;
		case FORMAT_R32G32B32_UINT:			return VK_FORMAT_R32G32B32_UINT;
		case FORMAT_R32G32B32A32_UINT:		return VK_FORMAT_R32G32B32A32_UINT;
		case FORMAT_R32_SFLOAT:				return VK_FORMAT_R32_SFLOAT;
		case FORMAT_R32G32_SFLOAT:			return VK_FORMAT_R32G32_SFLOAT;
		case FORMAT_R32G32B32_SFLOAT:		return VK_FORMAT_R32G32B32_SFLOAT;
		case FORMAT_R32G32B32A32_SFLOAT:	return VK_FORMAT_R32G32B32A32_SFLOAT;
		case FORMAT_MAX:
			LOGGER_ERROR("Invalid format ! ");
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		LOGGER_ERROR("Unsupported format ! ");
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	}
}
