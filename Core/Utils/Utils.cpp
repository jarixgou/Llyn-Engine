#include "Utils.h"

#include <fstream>
#include <ios>
#include <iostream>

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../Vector/Vec3.h"

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
}
