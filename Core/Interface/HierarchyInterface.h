#ifndef HIERARCHY_INTERFACE__H
#define HIERARCHY_INTERFACE__H

#include <vector>

#include "../LlynCore.h"

class HierarchyInterface
{
private:
	std::vector<Entity>* m_hierarchy = nullptr;
	Entity* m_selectedEntity = nullptr;
public:
	void Init(std::vector<Entity>* _hierarchy);

	void Update();

	Entity* GetSelectedEntity();
	Entity** GetSelectedEntityPtr();
private:
	void DrawTreeNode(Entity* _entity);
};

#endif