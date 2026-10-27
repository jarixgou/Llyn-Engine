#ifndef ASSET_VISITOR__H
#define ASSET_VISITOR__H

#include "../LlynCore.h"

class AssetVisitor
{
	void visit(const Texture* _texture);
	void visit(const Material* _material);
	void visit(const Model* _model);
};

#endif