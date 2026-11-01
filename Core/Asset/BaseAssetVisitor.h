#ifndef BASE_ASSET_VISITOR__H
#define BASE_ASSET_VISITOR__H

class AssetVisitor;

class BaseAssetVisitor
{
public:
	virtual void Accept(AssetVisitor* _visitor) = 0;
};

#endif