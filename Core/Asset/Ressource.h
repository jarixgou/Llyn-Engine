#ifndef RESSOURCE__H
#define RESSOURCE__H
#include <string>

class IRessource
{
public:
	IRessource() = default;
	virtual ~IRessource() = default;
	virtual bool Load(const std::string& _fileName) = 0;
	virtual void Unload() = 0;

	virtual bool Add(const std::string& _filePath, IRessource* _ressource) = 0;

	IRessource(const IRessource&) = default;
	IRessource& operator=(const IRessource&) = default;
	IRessource(IRessource&&) = default;
	IRessource& operator=(IRessource&&) = default;
};

#endif