#ifndef FOLDER__H
#define FOLDER__H
#include <string>
#include <vector>

#include "Texture.h"
#include "BaseAssetVisitor.h"

struct Folder : public BaseAssetVisitor
{
	Texture* texture = nullptr;

	Folder* previousFolder = nullptr;

	std::string path;

	std::vector<BaseAssetVisitor*> m_assets;

	void Accept(AssetVisitor* _visitor) override;
};

#endif