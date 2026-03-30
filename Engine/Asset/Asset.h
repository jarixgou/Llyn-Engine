#ifndef IASSET__H
#define IASSET__H

#include <string>

namespace Llyn
{
	class Asset
	{
	private:
		std::string m_path;
	public:
		Asset() = default;
		virtual ~Asset() = default;

		virtual bool Load(const char* _path) = 0;
		virtual bool Save() = 0;

		void SetPath(const char* _path);
		std::string GetPath();
	};
}

#endif