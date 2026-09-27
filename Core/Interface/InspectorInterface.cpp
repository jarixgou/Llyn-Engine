#include "InspectorInterface.h"

#include "../Asset/RessourceManager.h"
#include "../Component/Transform.h"
#include "../Component/MeshFilter.h"
#include "../Component/MeshFilterPooler.h"
#include "../Component/MeshRender.h"
#include "../Component/Light/DirLight.h"
#include "../Component/Light/PointLight.h"
#include "../Component/Light/SpotLight.h"
#include "../Entities/ECS.h"
#include "../Entities/Entity.h"
#include "../ImGui/imgui.h"
#include "../Utils/Utils.h"

#include "../Math/Math.h"

void InspectorInterface::Update(Entity* _entity, ECS* _ecs)
{
	ImGui::Begin("Inspector");

	if (_entity == nullptr)
	{
		ImGui::Text("Please select a game object");
		ImGui::End();
		return;
	}

	char nameBuff[FILENAME_MAX];
	std::snprintf(nameBuff, sizeof(nameBuff), "%s", _entity->name.c_str());
	if (ImGui::InputText("##name", nameBuff, sizeof(nameBuff), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		_entity->name = nameBuff;
	}

	ImGui::Separator();

	DrawTransform(_entity->id, _ecs->GetComponent<Transform>(_entity->id));

	if (_ecs->HasComponent<MeshRender>(_entity->id))
	{
		DrawMeshRender(_entity->id, _ecs->GetComponent<MeshRender>(_entity->id));
	}

	if (_ecs->HasComponent<MeshFilter>(_entity->id))
	{
		DrawMeshFilter(_entity->id, _ecs->GetComponent<MeshFilter>(_entity->id));
	}

	if (_ecs->HasComponent<DirLight>(_entity->id))
	{
		if (DrawDirLight(_ecs->GetComponent<DirLight>(_entity->id)))
		{
			_ecs->SetComponentIsDirty<DirLight>(true);
		}
	}

	if (_ecs->HasComponent<PointLight>(_entity->id))
	{
		if (DrawPointLight(_ecs->GetComponent<PointLight>(_entity->id)))
		{
			_ecs->SetComponentIsDirty<PointLight>(true);
		}
	}

	if (_ecs->HasComponent<SpotLight>(_entity->id))
	{
		if (DrawSpotLight(_ecs->GetComponent<SpotLight>(_entity->id)))
		{
			_ecs->SetComponentIsDirty<SpotLight>(true);
		}
	}

	ImGui::Separator();

	ImGui::SetCursorPosX(ImGui::GetItemRectSize().x * 0.5f - 50.0f);
	if (ImGui::Button("Add Component"))
	{
		ImGui::OpenPopup("AddCompoentPopup");
	}

	if (ImGui::BeginPopup("AddCompoentPopup"))
	{
		if (ImGui::MenuItem("Mesh Filter"))
		{

		}


		ImGui::EndPopup();
	}

	ImGui::End();
}

void InspectorInterface::DrawTransform(EntityID _id, Transform* _transform)
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("Position : ");
		ImGui::SameLine();
		Utils::DrawVec3(_transform->pos, "P" + std::to_string(_id));

		ImGui::Text("Rotation : ");
		ImGui::SameLine();
		Vec3f rot = RadToDeg(_transform->rot.GetEuler());
		Utils::DrawVec3(rot, "R" + std::to_string(_id));
		_transform->rot.SetEuler(DegToRad(rot));

		ImGui::Text("Scale    : ");
		ImGui::SameLine();
		Utils::DrawVec3(_transform->scale, "S" + std::to_string(_id));
	}
}

void InspectorInterface::DrawMeshRender(EntityID _id, MeshRender* _meshRender)
{
	if (ImGui::CollapsingHeader("Mesh Render"))
	{
		std::vector<std::string> names = RessourceManager::Get().GetRessourcesName<Material>();
		std::string currentName = RessourceManager::Get().GetRessourceName<Material>(_meshRender->GetMaterialID());

		ImGui::Text("Material : ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##MR", currentName.c_str()))
		{
			static ImGuiTextFilter filter;
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}

			ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
			filter.Draw("##Filter", -FLT_MIN);

			for (size_t i = 0; i < names.size(); ++i)
			{
				const bool isSelected = names[i] == currentName;
				if (filter.PassFilter(names[i].c_str()))
				{
					if (ImGui::Selectable(names[i].c_str(), isSelected))
					{
						_meshRender->SetMaterial(names[i]);
					}
				}
			}

			ImGui::EndCombo();
		}
	}
}

void InspectorInterface::DrawMeshFilter(EntityID _id, MeshFilter* _meshFilter)
{
	if (ImGui::CollapsingHeader("Mesh Filter"))
	{
		std::vector<std::string> names = MeshFilterPooler::Get().GetMeshFiltersName();

		ImGui::Text("Mesh Filter : ");
		ImGui::SameLine();
		if (ImGui::BeginCombo("##MF", _meshFilter->name.c_str()))
		{
			static ImGuiTextFilter filter;
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}

			ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
			filter.Draw("##Filter", -FLT_MIN);

			for (size_t i = 0; i < names.size(); ++i)
			{
				const bool isSelected = names[i] == _meshFilter->name;
				if (filter.PassFilter(names[i].c_str()))
				{
					if (ImGui::Selectable(names[i].c_str(), isSelected))
					{
						*_meshFilter = MeshFilterPooler::Get().GetMesh(names[i]);
					}
				}
			}

			ImGui::EndCombo();
		}
	}
}

bool InspectorInterface::DrawDirLight(DirLight* _dirLight)
{
	bool changed = false;

	if (ImGui::CollapsingHeader("Directional Light"))
	{
		ImGui::Text("Direction : ");
		ImGui::SameLine();
		if (Utils::DrawVec3(_dirLight->dir, "Dir"))
		{
			changed = true;
		}

		ImGui::Text("Ambient   : ");
		ImGui::SameLine();
		float ambientColor[3] = { _dirLight->ambient.x, _dirLight->ambient.y, _dirLight->ambient.z };
		if (ImGui::ColorEdit3("##Ambient", ambientColor, ImGuiColorEditFlags_Float))
		{
			_dirLight->ambient.x = ambientColor[0];
			_dirLight->ambient.y = ambientColor[1];
			_dirLight->ambient.z = ambientColor[2];

			changed = true;
		}

		ImGui::Text("Diffuse   : ");
		ImGui::SameLine();
		float diffuseColor[3] = { _dirLight->diffuse.x, _dirLight->diffuse.y, _dirLight->diffuse.z };
		if (ImGui::ColorEdit3("##Diffuse", diffuseColor, ImGuiColorEditFlags_Float))
		{
			_dirLight->diffuse.x = diffuseColor[0];
			_dirLight->diffuse.y = diffuseColor[1];
			_dirLight->diffuse.z = diffuseColor[2];

			changed = true;
		}

		ImGui::Text("Specular  : ");
		ImGui::SameLine();
		float specularColor[3] = { _dirLight->specular.x, _dirLight->specular.y, _dirLight->specular.z };
		if (ImGui::ColorEdit3("##Specular", specularColor, ImGuiColorEditFlags_Float))
		{
			_dirLight->specular.x = specularColor[0];
			_dirLight->specular.y = specularColor[1];
			_dirLight->specular.z = specularColor[2];

			changed = true;
		}
	}

	return changed;
}

bool InspectorInterface::DrawPointLight(PointLight* _pointLight)
{
	;
	bool changed = false;

	if (ImGui::CollapsingHeader("Point Light"))
	{
		ImGui::Text("Ambient  : ");
		ImGui::SameLine();
		float ambientColor[3] = { _pointLight->ambient.x, _pointLight->ambient.y, _pointLight->ambient.z };
		if (ImGui::ColorEdit3("##Ambient", ambientColor, ImGuiColorEditFlags_Float))
		{
			_pointLight->ambient.x = ambientColor[0];
			_pointLight->ambient.y = ambientColor[1];
			_pointLight->ambient.z = ambientColor[2];

			changed = true;
		}

		ImGui::Text("Diffuse  : ");
		ImGui::SameLine();
		float diffuseColor[3] = { _pointLight->diffuse.x, _pointLight->diffuse.y, _pointLight->diffuse.z };
		if (ImGui::ColorEdit3("##Diffuse", diffuseColor, ImGuiColorEditFlags_Float))
		{
			_pointLight->diffuse.x = diffuseColor[0];
			_pointLight->diffuse.y = diffuseColor[1];
			_pointLight->diffuse.z = diffuseColor[2];

			changed = true;
		}

		ImGui::Text("Specular  : ");
		ImGui::SameLine();
		float specularColor[3] = { _pointLight->specular.x, _pointLight->specular.y, _pointLight->specular.z };
		if (ImGui::ColorEdit3("##Specular", specularColor, ImGuiColorEditFlags_Float))
		{
			_pointLight->specular.x = specularColor[0];
			_pointLight->specular.x = specularColor[1];
			_pointLight->specular.x = specularColor[2];

			changed = true;
		}

		ImGui::Text("Constant  : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Constant", &_pointLight->constant, 0.5f, -FLT_MAX, FLT_MAX))
		{
			changed = true;
		};

		ImGui::Text("Linear    : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Linear", &_pointLight->linear, 0.5f, -FLT_MAX, FLT_MAX))
		{
			changed = true;
		}

		ImGui::Text("Quadratic : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Quadratic", &_pointLight->quadratic, 0.5f, -FLT_MAX, FLT_MAX))
		{
			changed = true;
		}

		ImGui::Text("Radius    : ");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Radius", &_pointLight->radius, 0.5f, -FLT_MAX, FLT_MAX))
		{
			changed = true;
		}
	}

	return changed;
}

bool InspectorInterface::DrawSpotLight(SpotLight* _spotLight)
{
	bool changed = false;

	if (ImGui::CollapsingHeader("Spot Light"))
	{
		ImGui::Text("Direction     : ");
		ImGui::SameLine();
		if (Utils::DrawVec3(_spotLight->dir, "Dir"))
		{
			changed = true;
		}

		ImGui::Text("Ambient       : ");
		ImGui::SameLine();
		float ambientColor[3] = { _spotLight->ambient.x, _spotLight->ambient.y, _spotLight->ambient.z };
		if (ImGui::ColorEdit3("##Ambient", ambientColor, ImGuiColorEditFlags_Float))
		{
			_spotLight->ambient.x = ambientColor[0];
			_spotLight->ambient.y = ambientColor[1];
			_spotLight->ambient.z = ambientColor[2];

			changed = true;
		}

		ImGui::Text("Diffuse       : ");
		ImGui::SameLine();
		float diffuseColor[3] = { _spotLight->diffuse.x, _spotLight->diffuse.y, _spotLight->diffuse.z };
		if (ImGui::ColorEdit3("##Diffuse", diffuseColor, ImGuiColorEditFlags_Float))
		{
			_spotLight->diffuse.x = diffuseColor[0];
			_spotLight->diffuse.y = diffuseColor[1];
			_spotLight->diffuse.z = diffuseColor[2];

			changed = true;
		}

		ImGui::Text("Specular      : ");
		ImGui::SameLine();
		float specularColor[3] = {_spotLight->specular.x , _spotLight->specular.y, _spotLight->specular.z };
		if (ImGui::ColorEdit3("##Specular", specularColor, ImGuiColorEditFlags_Float))
		{
			_spotLight->specular.x = specularColor[0];
			_spotLight->specular.y = specularColor[1];
			_spotLight->specular.z = specularColor[2];

			changed = true;
		}

		ImGui::Text("Cut Off       : ");
		ImGui::SameLine();
		float cutOff = RadToDeg(_spotLight->cutOff);
		if (ImGui::DragFloat("##CutOff", &cutOff, 1.0f, -FLT_MAX, FLT_MAX))
		{
			_spotLight->cutOff = DegToRad(cutOff);

			changed = true;
		}

		ImGui::Text("Outer Cut Off : ");
		ImGui::SameLine();
		float outerCutOff = RadToDeg(_spotLight->outerCutOff);
		if (ImGui::DragFloat("##OuterCutOff", &outerCutOff, 1.0f, -FLT_MAX, FLT_MAX))
		{
			_spotLight->outerCutOff = DegToRad(outerCutOff);

			changed = true;
		}
	}

	return changed;
}

void InspectorInterface::AddComponent(Entity* _entity, ECS* _ecs)
{

}
