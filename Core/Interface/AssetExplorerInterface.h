#ifndef ASSET_EXPLORER_INTERFACE__H
#define ASSET_EXPLORER_INTERFACE__H

#include "../Asset/AssetVisitor.h"

class AssetExplorerInterface
{
private:
	AssetVisitor m_assetVisitor;

	Folder* m_actualFolder = nullptr;
};

#endif