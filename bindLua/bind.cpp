/*
 * Description: 一个Lua 绑定的框架
 */

#include <stdio.h>
#include <lua.hpp>
#include  "../showvs.h"

//Fiset step use userdata
#if 0
class US {

public: 
    int a;
    double d;
};


//create a newa US vairable push to L 
static int us_new(lua_State *L) 
{
    SHOW_VSL(lua_newuserdata(L,sizeof(US)));
    return 1; //return the pushed userdata
}

static int us_get_a(lua_State *L)
{
    //check argument is user data
    US *us = (US*)lua_touserdata(L, -1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    SHOW_VSL(lua_settop(L,0)); //clear argument
    SHOW_VSL(lua_pushinteger(L, us->a));
    return 1;
}

static int us_get_d(lua_State *L)
{
    //check argument is user data
    US *us = (US*)lua_touserdata(L, -1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    SHOW_VSL(lua_settop(L,0)); //clear argument
    SHOW_VSL(lua_pushnumber(L, us->d));
    return 1;
}

static int us_set_a(lua_State *L)
{
    //check argument is user data
    US *us = (US*)lua_touserdata(L, 1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    int a = luaL_checkint(L,2);	
    us->a=a;
    printf ( "us_set_a a:%d\n", a);
    return 0;
}

static int us_set_d(lua_State *L)
{
    //check argument is user data
    US *us = (US*)lua_touserdata(L, 1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    double d = luaL_checknumber(L,2);	
    us->d=d;
    printf ( "us_set_d d:%f\n", d);
    return 0;
}

//注意在C++下load接口用“C” 否则经过编译器后luaopen_bind 的符合
//就会改变，是的lua查找不到luaopen_bind 函数
extern "C"
int luaopen_bind(lua_State *L)
{
    static const luaL_Reg Map[] = {
	{"us_new", us_new},
	{"us_get_a", us_get_a},
	{"us_get_d", us_get_d},
	{"us_set_a", us_set_a},
	{"us_set_d", us_set_d},
	{NULL, NULL}
    };

    lua_newtable(L); 
    //5.2中没有luaL_register 使用luaL_setfuncs,但是要先把一个table压入VS，然后，luaL_setfuncs就会
    //把Map中所用的func存到table中
    luaL_setfuncs(L, Map, 0);
    return 1;
}
#endif

// 使用使用lightuserdata 绑定c++对象
// lua不会管理对象的生死，生成的对象要手动释放
#if 1
class US {
public: 
    US(int x=0, double y=0.0) 
	:a(x), d(y){}
    ~US() { DEBUG_LOG( printf ( "Destruction US\n" )) }

    int get_a() const {return a;}
    double get_d() const {return d;}

    void set_a(int x) {a = x;}
    void set_d(double y) {d=y;}

private:
    int a;
    double d;
};

static int us_new(lua_State *L)
{
    //实现默认参数
    DEBUG_LOG(printf ( "--------us_new\n" ))
    stackDump(L);
    int x = luaL_optint(L, 1, 0);
    double d = luaL_optnumber(L, 2, 0);
    DEBUG_LOG(printf ( "x%d,d%f\n",x, d ))

    //clean VS 可以不用，lua会忽略VS中其他的值
//    SHOW_VSL(lua_settop(L,0));

    //?? 如何更好的实现默认参数 HACKING
    US *usPtr = new US(x,d);

    //push lightuserdata to lua
    SHOW_VSL(lua_pushlightuserdata(L, usPtr));

    DEBUG_LOG(printf ( "--------us_new\n" ))
    return 1; 
}

static int us_free(lua_State *L)
{
    DEBUG_LOG(printf ( "---us_free \n" ))
    SHOW_VSL(luaL_argcheck(L, lua_islightuserdata(L, -1)
	          ,-1, "us_free is not a US lightuserdata"));
    delete (US*) lua_touserdata(L, -1);
    DEBUG_LOG(printf ( "---us_free \n" ))
    return 0;
}


//和fulluserdata 几乎一样
static int us_get_a(lua_State *L)
{
    DEBUG_LOG(printf ( "---us_get_a\n" ))
    //check argument is user data
    US *us = (US*)lua_touserdata(L, -1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    SHOW_VSL(lua_settop(L,0)); //clear argument
    SHOW_VSL(lua_pushinteger(L, us->get_a()));
    return 1;
}

static int us_get_d(lua_State *L)
{
    DEBUG_LOG(printf ( "---us_get_d\n" ))
    //check argument is user data
    US *us = (US*)lua_touserdata(L, -1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    SHOW_VSL(lua_settop(L,0)); //clear argument
    SHOW_VSL(lua_pushnumber(L, us->get_d()));
    return 1;
}

static int us_set_a(lua_State *L)
{
    DEBUG_LOG(printf ( "---us_set_a\n" ))
    //check argument is user data
    US *us = (US*)lua_touserdata(L, 1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    int a = luaL_checkint(L,2);	
    us->set_a(a);
    DEBUG_LOG(printf ( "us_set_a a:%d\n", a))
    return 0;
}

static int us_set_d(lua_State *L)
{
    DEBUG_LOG(printf ( "---us_set_d\n" ))
    //check argument is user data
    US *us = (US*)lua_touserdata(L, 1);
    // if not a userdata print error msg
    SHOW_VSL(luaL_argcheck(L, us != NULL, -1, "not a userdata"));
    double d = luaL_checknumber(L,2);	
    us->set_d(d);
    DEBUG_LOG(printf ( "us_set_d d:%f\n", d))
    return 0;
}

extern "C"
int luaopen_bind(lua_State *L)
{
    static const luaL_Reg Map[] = {
	{"us_new", us_new},
	{"us_free", us_free},
	{"us_get_a", us_get_a},
	{"us_get_d", us_get_d},
	{"us_set_a", us_set_a},
	{"us_set_d", us_set_d},
	{NULL, NULL}
    };

    lua_newtable(L); 
    //5.2中没有luaL_register 使用luaL_setfuncs,但是要先把一个table压入VS，然后，luaL_setfuncs就会
    //把Map中所用的func存到table中
    luaL_setfuncs(L, Map, 0);
    return 1;
}
#endif
