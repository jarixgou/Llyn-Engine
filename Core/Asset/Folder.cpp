#include "Folder.h"

#include "AssetVisitor.h"

void Folder::Accept(AssetVisitor* _visitor)
{
	_visitor->visit(this);
}
