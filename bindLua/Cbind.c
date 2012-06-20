/*
 *  Description: 一个Lua 绑定C的框架
 */
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

// print expersion
static void StackPrint(lua_State *L, const char *str)
{
    int j, top;
    printf("%-26s [", str);
    top = lua_gettop(L);
    for (j = 1; j <= top; ++j) {
//    printf ( "[%s] ",lua_typename(L, lua_type(L, j)));
	if (lua_isnil(L, j))
	    printf(" - ");
	else
	    printf(" %d ", lua_tointeger(L, j));
    }
    printf("]\n");
}
#define LUA_STACK_PRINT(code) (code), StackPrint(L, #code)

static int StackLook(lua_State *L)
{
    int i;
    int top = lua_gettop(L);
    printf ( "vs ele:%d\n",top );

    for (i=1; i <= top; ++i) 
	luaL_checkinteger(L, i);
      StackPrint(L, "Iitial stack");
      LUA_STACK_PRINT(lua_settop(L,0));
      LUA_STACK_PRINT(lua_pushinteger(L, 1));
      LUA_STACK_PRINT(lua_pushinteger(L, 2));
      LUA_STACK_PRINT(lua_pushinteger(L, 3));
//    LUA_STACK_PRINT(lua_settop(L,3));
//    LUA_STACK_PRINT(lua_settop(L,5));
//    LUA_STACK_PRINT(lua_pushinteger(L,5));
//    LUA_STACK_PRINT(lua_pushinteger(L,4));
//    LUA_STACK_PRINT(lua_replace(L,-4));
//    LUA_STACK_PRINT(lua_replace(L,5));
//    LUA_STACK_PRINT(lua_remove(L,3));
//    LUA_STACK_PRINT(lua_pushinteger(L,3));
//    LUA_STACK_PRINT(lua_insert(L,-3));
//    LUA_STACK_PRINT(lua_pushvalue(L,2));
//    LUA_STACK_PRINT(lua_pop(L, 1));
    return lua_gettop(L);
}

int luaopen_cbind(lua_State *L)
{
    static const luaL_Reg Map[] = {
	{"StackLook", StackLook},
	{NULL, NULL}
    };

    lua_newtable(L); //5.2中没有luaL_register 使用luaL_setfuncs,但是要先把一个table压入VS，然后，luaL_setfuncs就会
    //把Map中所用的func存到table中
    luaL_setfuncs(L, Map, 0);
    return 1;
}


