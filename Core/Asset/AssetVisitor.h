#ifndef ASSET_VISITOR__H
#define ASSET_VISITOR__H

#include "../LlynCore.h"

class AssetVisitor
{
public:
	void visit(Texture* _texture);
	void visit(const Material* _material);
	void visit(const Model* _model);
	void visit(Folder* _folder);
};

#endif