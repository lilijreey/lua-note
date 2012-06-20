/*
 *  Description: 
 */
#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct {
    int val;
    int open;

} ex_t, *ex_t_ptr;


#define	STRING	"example"		/*  */
static ex_t_ptr ptrGet(lua_State *L, int stkPos)
{
    ex_t_ptr eptr = luaL_checkudata(L, stkPos, STRING);
    if (! eptr->open )
	luaL_error(L, "attempt to use a closed" STRING);
    return eptr;
}

static int exStr( lua_State *L)
{
    ex_t_ptr eptr;
    eptr = luaL_checkudata(L, 1, STRING);
    if (eptr->open)
	lua_pushfstring(L, STRING "(%d)", eptr->val);
    else
	lua_pushfstring(L, STRING "(%d, close)" ,eptr->val);
    return 1;
}

static int get(lua_State *L)
{
    ex_t_ptr eptr =  ptrGet(L, 1);
    lua_pushnumber(L, eptr->val);
    printf("Retrieving value of " STRING " (%d)\n", eptr->val);
    return 1;
}

static int set(lua_State *L)
{
    int val;
    ex_t_ptr eptr = ptrGet(L, 1);
    val = luaL_checkint(L, 2);
    printf("Setting value of " STRING " from %d to %d\n",
	    eptr->val, val);
    lua_pushnumber(L, eptr->val);
    eptr->val= val;
    return 1;
}

static int close(lua_State *L)
{
    ex_t_ptr eptr = ptrGet(L, 1);
    printf("Closing " STRING "(%d) explicitly\n", eptr->val);
    eptr->open = 0;
    return 0;
}

static int gc(lua_State *L)
{
    ex_t_ptr eptr = luaL_checkudata(L, 1, STRING);
    if (eptr->open) {
	printf("Collecting and closing " STRING " (%d)\n",
		eptr->val);
	eptr->open = 0;
    } else
	printf("Collecting " STRING "(%d), already closed\n", eptr->val);
    return 0;
}

static int open(lua_State *L)
{
    int val;
    ex_t_ptr eptr;
    val = luaL_checkint(L, 1);
    eptr = lua_newuserdata(L, sizeof(ex_t));
    printf("Opening " STRING " (%d)\n", val);
    eptr->val = val;
    eptr->open = 1;

    luaL_getmetatable(L, STRING);
    lua_setmetatable(L, -2);
    return 1;
}

int luaopen_udata(lua_State *L)
{
    static const luaL_Reg metaMap[] = {
	{"close", close},
	{"get", get},
	{"set", set},
	{"__tostring", exStr},
	{"__gc", gc},
	{NULL, NULL},
    };

    static const luaL_Reg map[] = {
	{"open", open},
	{NULL, NULL},
    };

    //create metatable for handlers of type STRING
    luaL_newmetatable(L, STRING);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_setfuncs(L, metaMap, 0);
    luaL_setfuncs(L, map, 0);
    printf("xx");
    return 1;
}

