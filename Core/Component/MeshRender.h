#ifndef MESH_RENDER__H
#define MESH_RENDER__H

#include "Component.h"
#include "../LlynCore.h"

class MeshRender : public IComponent
{
private:
	RessourceID m_materialID = 0;
	// TODO: Add lighting parameters
public:
	void SetMaterial(const std::string& _name);
	void SetMaterial(const RessourceID& _ressourceID);
	RessourceID GetMaterialID() const;
};

#endif