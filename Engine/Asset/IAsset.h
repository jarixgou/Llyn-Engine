#ifndef IASSET__H
#define IASSET__H

namespace Llyn
{
	class IAsset
	{
	public:
		IAsset() = default;
		virtual ~IAsset() = default;

		virtual bool Load(const char* _path) = 0;
		virtual void* Get() = 0;
	};
}

#endif