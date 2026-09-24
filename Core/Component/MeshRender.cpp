#include "MeshRender.h"

#include "../Asset/Material.h"
#include "../Asset/RessourceManager.h"

void MeshRender::SetMaterial(const std::string& _name)
{
	m_materialID = RessourceManager::Get().GetRessourceID<Material>(_name);
}

void MeshRender::SetMaterial(const RessourceID& _ressourceID)
{
	m_materialID = _ressourceID;
}

RessourceID MeshRender::GetMaterialID() const
{
	return m_materialID;
}
