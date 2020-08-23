#pragma once
#include "Core\SLog.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/RefCountedPtr.h>

namespace SmolEngine
{
	void report_errors(lua_State* L, int status)
	{
		if (status != 0)
		{
			EDITOR_ERROR(lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		}
	}

	void Build(lua_State* L, std::string& path)
	{
		int lscript = luaL_dofile(L, path.c_str());

		if (lscript == 0)
		{
			lscript = lua_pcall(L, 0, 0, 0);
		}

		report_errors(L, lscript);
	}

	template<typename T>
	luabridge::RefCountedPtr<T> GetRefPtr()
	{
		static luabridge::RefCountedPtr<T> ref(new T);
		return ref;
	}
}