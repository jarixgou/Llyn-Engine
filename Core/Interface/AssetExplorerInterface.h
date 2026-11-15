#ifndef ASSET_EXPLORER_INTERFACE__H
#define ASSET_EXPLORER_INTERFACE__H

#include "../Asset/AssetVisitor.h"

class AssetExplorerInterface
{
private:
	AssetVisitor m_assetVisitor;

	Folder* m_actualFolder = nullptr;

public:
	void Init();
	void Update();
	void Cleanup();

private:
	void RecursiveFolder(Folder* _actualFolder, const std::string& _path);
};

#endif