#include "LuaInstance.h"

#include "Vector/Vec3.h"

LuaInstance& LuaInstance::Get()
{
	static LuaInstance instance;
	return instance;
}

void LuaInstance::Init()
{
	m_state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string,
		sol::lib::table, sol::lib::math);

	m_state.new_usertype<Vec3f>("Vec3f",
		sol::constructors<Vec3f(), Vec3f(float, float, float)>(),
		"x", &Vec3f::x, "y", &Vec3f::y, "z", &Vec3f::z,

		sol::meta_function::addition, [](const Vec3f& _a, const Vec3f& _b) { return _a + _b; },
		sol::meta_function::subtraction, [](const Vec3f& _a, const Vec3f& _b) { return _a - _b; },
		sol::meta_function::multiplication, sol::overload(
			[](const Vec3f& _a, const Vec3f& _b) { return _a * _b; },
			[](const Vec3f& _a, const float& _b) { return _a * _b; }),
		sol::meta_function::equal_to, [](const Vec3f& _a, const Vec3f& _b) { return _a == _b; }
		);
}

sol::state* LuaInstance::GetState()
{
	return &m_state;
}
