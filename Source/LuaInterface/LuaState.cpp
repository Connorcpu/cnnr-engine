#include "pch.h"
#include "LuaState.h"
#include "RequireLoader.h"
#include "LuaMath.h"

#define LS_PTR "___LuaState_ptr"

int LuaStoreUserDataForPtr(lua_State *L);

LuaState::LuaState()
{
    L = lua_open();

    lua_pushlightuserdata(L, this);
    lua_setfield(L, LUA_GLOBALSINDEX, "LS_PTR");

    luaL_openlibs(L);
    RegisterRequireLoader(L);

    lua_pushcclosure(L, LuaStoreUserDataForPtr, 0);
    lua_setfield(L, LUA_GLOBALSINDEX, "__helper_LuaStoreUserDataForPtr");

    lua_math_init(L);
}

LuaState::~LuaState()
{
    for (auto &on_close : on_closes)
    {
        on_close();
    }
    lua_close(L);
}

lua_State *LuaState::state()
{
    return L;
}

void LuaState::load(const String &file)
{
    lua_getfield(L, LUA_GLOBALSINDEX, "require");
    file.push_lua(L);
    if (lua_pcall(L, 1, 0, 0))
    {
        auto msg = String::from_lua(L, -1).into_stdstring();
        throw std::runtime_error{ msg };
    }
}

void LuaState::call(int nresults, int nargs, const String &func, std::optional<const String&> module)
{
    if (module)
        require(1, *module);
    else
        lua_pushvalue(L, LUA_GLOBALSINDEX);


}

void LuaState::require(int nresults, const String &file)
{
    lua_getfield(L, LUA_GLOBALSINDEX, "require");
    file.push_lua(L);
    if (lua_pcall(L, 1, nresults, 0))
    {
        auto msg = String::from_lua(L, -1);
        throw std::runtime_error{ msg.as_owned() };
    }
}

LuaState *LuaState::from_raw(lua_State *L)
{
    lua_assertstack(L, 1);
    lua_getfield(L, LUA_GLOBALSINDEX, LS_PTR);
    auto ptr = (LuaState *)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return ptr;
}
