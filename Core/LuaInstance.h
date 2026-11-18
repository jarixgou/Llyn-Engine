#ifndef LUA_INSTANCE__H
#define LUA_INSTANCE__H

#include "sol/sol.hpp"

class LuaInstance
{
private:
	sol::state m_state;
public:
	static LuaInstance& Get();

	void Init();

	sol::state* GetState();
};

#endif