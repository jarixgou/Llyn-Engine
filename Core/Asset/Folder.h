#ifndef FOLDER__H
#define FOLDER__H
#include <vector>

#include "BaseAssetVisitor.h"

struct Folder : public BaseAssetVisitor
{
	Folder* previousFolder = nullptr;

	std::vector<BaseAssetVisitor*> m_assets;

	void Accept(AssetVisitor* _visitor) override;
};

#endif