/*
 *  Description: 
 */
#include <stdio.h>
#include <stdlib.h>
#include "luahead.h"
#include "showvs.h"

//假设 table在top 
int  getfield(lua_State *L, const char *key);
void setfield(lua_State *l, const char *index, int value);

    //执行file_name
static int dofile(lua_State *LS, const char *file_name)  {
    int error;
    if ((error = luaL_dofile(LS,file_name)) != 0)
    {
	printf ( "lua_dofile ERROR[%d]:", error);
	// EE 这里如果有错只会打印数字，而没有描述下面的代码可以打印描述
	printf("%s\n", lua_tostring(LS, -1)); //错误会在top of stack 
    }
    return error;
}

static int l_add(lua_State *L) 
{
    if (0 == lua_isnumber(L,1))
	lua_error(L);
    double d = lua_tonumber(L, 1);

  //  double d = luaL_checknumber(L, 1); //可一输出有效的
  //  错误信息 比上面的要好
    lua_pushnumber(L, ++d);
    return 1; //return value count
}

int main() 
{
    //EE the lua_open is not surrposed use luaL_newstate instead it
    //EE 一个 lua_State 就是一个lua VM 
    //lua_State *LS = lua_open();
lua_State *LS = luaL_newstate();
//EE open libs can be instead with luaL_openlibs
luaL_openlibs(LS);

#if 0 //test vs op func
    //EE 读取lua中的全局变量
    //1把lua中的变量推倒stack中
    //执行test.lua
    dofile(LS, "test.lua");
    lua_settop(LS, 0);
    stackDump(LS);
    lua_getglobal(LS, "config_name"); //1
    stackDump(LS);
    lua_getglobal(LS, "config_value");//2
    stackDump(LS);

    SHOW_VS(lua_pushinteger(LS, 2));
//    int x = luaL_checkint(LS,-1);
    //printf("x:%d\n", x);
//    luaL_checkinteger(LS,-1);
    lua_tointeger(LS,-1);

    // 得到一个没有的变量
    lua_getglobal(LS, "cc");
    if (lua_isnil(LS, -1))
	printf ( "cc is nil\n" );
    else
	printf ( "cc not nil\n" );

    //2 check get virable tpye
    if (!lua_isstring(LS,1) || !lua_isstring(LS,2)) {
	printf ( "get lua virable error\n" );
	return 1;
    }
    //3 get lua virables
    const char *config_name = lua_tostring(LS,1);
    const char *config_value = lua_tostring(LS,2);
    printf ( "get name:%s, value%s\n", config_name, config_value );
    //4 pop virables
    lua_pop(LS,2);
    stackDump(LS);

    SHOW_VS(lua_settop(LS, 0));
    SHOW_VS(lua_pushboolean(LS, 1)); //1 is true
    SHOW_VS(lua_pushboolean(LS, 0)); //1 is true
    SHOW_VS(lua_pushinteger(LS, 1002));
    SHOW_VS(lua_pushstring(LS, "hello lua")); 
    SHOW_VS(lua_pushlstring(LS, "hello lua", 4)); 
    SHOW_VS(lua_pushnil(LS)); 
    SHOW_VS(lua_pushvalue(LS, -2)); 
    SHOW_VS(lua_replace(LS, 7)); 
    SHOW_VS(lua_remove(LS, -2)); 
    SHOW_VS(lua_insert(LS, 1)); 
    SHOW_VS(lua_settop(LS, 0)); //clear VS 
#endif

#if 0 //{{{
    //EE read table virable from lua op
    //1 set stack clean
    SHOW_VS(lua_settop(LS, 0));
    //2 push the table that you want get in lua of name on stack 
    SHOW_VS(lua_getglobal(LS, "bg"));
    //3 check type
    if (!lua_istable(LS,-1))
	luaL_error(LS, "not a table");

    //4 get 
    //the top stack is table name that you want get the K-V pair 
    int red = getfield(LS, "r");
    stackDump(LS);
    int green = getfield(LS, "g");
    stackDump(LS);
    int blue = getfield(LS, "b");
    stackDump(LS);
    printf("r:%d g:%d b:%d\n", red, green, blue);
#endif

#if 0
//EE set a table to lua
//设置table是需要先设置好table的各个field在把table pop VS
//当lua中
    SHOW_VS(lua_settop(LS, 0));
    SHOW_VS(lua_newtable(LS));
    //push key
    SHOW_VS(lua_pushstring(LS, "r"));
    SHOW_VS(lua_pushinteger(LS, 1));
    SHOW_VS(lua_settable(LS, -3));

    SHOW_VS(lua_setglobal(LS, "c_table"));
    dofile(LS, "test.lua");

#endif


#if 0 //{{{ set lua table field from C

    //set variable to lua
    lua_pushnumber(LS, 999);
    lua_setglobal(LS, "x");


    lua_settop(LS,0);
    stackDump(LS);
    //EE 调用lua中的函数
    lua_getglobal(LS, "ok"); //push func 
    stackDump(LS);
    lua_pushnumber(LS, 3);
    stackDump(LS);
    lua_pushstring(LS, "this is C");
    stackDump(LS);
    if (0 != lua_pcall(LS,2,1,0)) {
	stackDump(LS);
	printf("%s\n", lua_tostring(LS, -1)); //错误会在top of stack 
    }
    int x=lua_tonumber(LS, -1);
    lua_pop(LS,1);
    printf("x:%d\n",x);
#endif //}}}

//注册CFunc 在lua中调用
#if 0
//    lua_pushcfunction(LS, l_add); //push func
//    lua_setglobal(LS, "l_add"); // set func name in lua
     lua_register(LS, "l_add", l_add);// 等价与上面的
     stackDump(LS);
     lua_pushinteger(LS, 3);
     // 1 只是标记这个检测和谁有关
     luaL_argcheck(LS, 0, 1, "argcheck error");
#endif


// 设置注册表中的 userdata 元表
// 这里的设置并不和userdata有关系
// 只是操作注册表里的内容
#if 1
    dofile(LS, "test.lua");
    int x= luaL_newmetatable(LS, "llmm"); //llmm 在注册表中
    stackDump(LS);
    printf("x=%d\n", x);
    SHOW_VS(luaL_getmetatable(LS, "llmm")); //其实就是下面的Macro
    SHOW_VS(lua_getfield(LS, LUA_REGISTRYINDEX, "llmm"));

#endif

// EE use userdata
#if 0
    lua_settop(LS,0);
    SHOW_VS(lua_pushlightuserdata(LS, 0));
    SHOW_VS(lua_setglobal(LS, "lightUD"));
//    dofile(LS, "test.lua");

    typedef
    struct OO{
	int i;
	double db;
    }OO, *OOP;

    lua_settop(LS, 0);
    OOP ud=lua_newuserdata(LS, sizeof(struct OO));
    stackDump(LS);
    ud->i=2;
    ud->db=4;
    printf ( "i:%d, d:%f\n",ud->i, ud->db );
    if (lua_touserdata(LS, -1) == NULL)
	printf ( "top vS not a userdata\n" );
    else
	printf ( "top vS userdata adress:%p\n", lua_touserdata(LS, -1) );
    ud=0;
    SHOW_VS(lua_setglobal(LS, "cud"));
    //在lua中设置ud的member:w
    dofile(LS, "test.lua");

#endif

//使用注册表 
#if 0
    dofile(LS, "test.lua");
    SHOW_VS(lua_settop(LS, 0));
    //在注册表中压如一个值3
    SHOW_VS(lua_pushinteger(LS, 3));
    SHOW_VS(lua_setfield(LS, LUA_REGISTRYINDEX, "value"));

    //得到注册表中的值
//  SHOW_VS(lua_getfield(LS, LUA_REGISTRYINDEX, "value"));
    SHOW_VS(luaL_getmetatable(LS, "value")); //same above. this is a Marco

    printf ( "Registry value:%d\n", luaL_checkinteger(LS,-1) );
    SHOW_VS(lua_pop(LS,1));

    //int r = luaL_ref(LS, LUA_REGISTRYINDEX);

#endif
    //EE 释放资源
    lua_close(LS);
    return 0 ;
}


//get table field
int  getfield(lua_State *L, const char *key)
{
    int result = 0;
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) // value replase key
	luaL_error(L, "table field not a number");
    result = (int) lua_tonumber(L, -1);
    lua_pop(L, 1); // pop value
    return result;
}

//set table field
void setfield(lua_State *L, const char *index, int value)
{
    lua_pushstring(L, index);
    lua_pushnumber(L, 3);
    lua_settable(L, -3);
}

