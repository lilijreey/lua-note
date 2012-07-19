// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

extern "C" {
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

#include <new>
#include <cassert>
#include <string.h>

namespace lua_tinker
{
	// init LuaTinker
	// 
	void	init(lua_State *L);

	//设置unsignal int64 支持
	void	init_s64(lua_State *L);
	//设置signal int64 支持
	void	init_u64(lua_State *L);

	// excution
	void	dofile(lua_State *L, const char *filename);
	void	dostring(lua_State *L, const char* buff);
	void	dobuffer(lua_State *L, const char* buff, size_t sz);

	// debug helpers
	void	enum_stack(lua_State *L);
	int		on_error(lua_State *L);
	void	print_error(lua_State *L, const char* fmt, ...);

	// dynamic type extention
	struct lua_value
	{
		virtual void to_lua(lua_State *L) = 0;
	};

	// type trait
	template<typename T> struct class_name;
	struct table;

	//一个编译期if，如果C是true，type就是A类型，反之type就是B类型
	template<bool C, typename A, typename B> struct if_ {};
	template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
	template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

	template<typename A>
	struct is_ptr { static const bool value = false; };
	template<typename A>
	struct is_ptr<A*> { static const bool value = true; };

	template<typename A>
	struct is_ref { static const bool value = false; };
	template<typename A>
	struct is_ref<A&> { static const bool value = true; };

	template<typename A>
	struct remove_const { typedef A type; };
	template<typename A>
	struct remove_const<const A> { typedef A type; };

	template<typename A>
	struct base_type { typedef A type; };
	template<typename A>
	struct base_type<A*> { typedef A type; };
	template<typename A>
	struct base_type<A&> { typedef A type; };

	template<typename A>
	struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };
	
	/////////////////////////////////
	enum { no = 1, yes = 2 }; 
	typedef char (& no_type )[no]; 
	typedef char (& yes_type)[yes]; 

	//一个接受int类型的struct,间接的把int类型转为int_conv_type
	struct int_conv_type { int_conv_type(int); }; 

	// 这个使用SFINE 来train不同的参数，返回不同的类型.关于SFINE 请google。 
	// 如果调用int_conv_type返回的是no_type 说明调用的是第一个函数，进一步说明传入的参数不能转换为int类型 
	// 如果返回yes_type，同上道理则说明传入的参数是int兼容性
	no_type int_conv_tester (...); 
	yes_type int_conv_tester (int_conv_type);  

	// 类型tran void*返回yes_type,其他返回no_type
	no_type vfnd_ptr_tester (const volatile char *); 
	no_type vfnd_ptr_tester (const volatile short *); 
	no_type vfnd_ptr_tester (const volatile int *); 
	no_type vfnd_ptr_tester (const volatile long *);
	no_type vfnd_ptr_tester (const volatile double *); 
	no_type vfnd_ptr_tester (const volatile float *); 
	no_type vfnd_ptr_tester (const volatile bool *); 
	yes_type vfnd_ptr_tester (const volatile void *); 

	//add_ptr计算类型时可以使用(不用定义）
	//e.g. int x; sizeof(add_ptr(x));
	//但是add_ptr(x)就不行
	template <typename T> T* add_ptr(T&); 

	template <bool C> struct bool_to_yesno { typedef no_type type; }; 
	template <> struct bool_to_yesno<true> { typedef yes_type type; }; 

	template <typename T> 
	struct is_enum  //检测T是不是一个enum:原理：
	// 如果能通过第一个测试，说明T类型是个int兼容型，
	// 第二个类型屏蔽所有不是enum的类型的，通过返回值。由于enum类型的指针不能通过vfndPptr_tester的重载，
	// 只能匹配void*的重载。（所有类型的指针都能赋值给void*）
	{ 
		static T arg; 
		// arg 必须是int兼容类型 && add_ptr(arg) 返回的是void*
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && 
			                    (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 
	/////////////////////////////////

	// from lua
	template<typename T>
	struct void2val { static T invoke(void* input){ return *(T*)input; } };
	template<typename T>
	struct void2ptr { static T* invoke(void* input) { if (input == 0) return 0; else return (T*)input; } };
	template<typename T>
	struct void2ref { static T& invoke(void* input){ return *(T*)input; } };

	template<typename T>  
	struct void2type
	{
		static T invoke(void* ptr)
		{
		    //如果T是个ptr就返回1,
		    //要不然T是ref就返回2
		    //其他返回3
		    //然后调用返回类型的invoke方法
			return	if_<is_ptr<T>::value
				    ,void2ptr<typename base_type<T>::type> //1
				    ,typename if_<is_ref<T>::value
						 ,void2ref<typename base_type<T>::type>//2
						 ,void2val<typename base_type<T>::type>//3
						 >::type
				   >::type::invoke(ptr);
		}
	};

    // 保存一个userdata,使用virtual 保证distroy 真正的对象
	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
	};

	template<typename T>  
	struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

	template<typename T>
	struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

// 	template<typename T>
// 	struct lua2object
// 	{ 
// 		static T invoke(lua_State *L, int index) 
// 		{ 
// 			if(!lua_isuserdata(L,index))
// 			{
// 				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
// 				lua_error(L);
// 			}
// 			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
// 		} 
// 	};
    template<typename T>
    struct lua2object
    { 
        static T invoke(lua_State *L, int index) 
        { 
            if(lua_isnil(L,index))
            {
                return void2type<T>::invoke(0);
            }
            else if(!lua_isuserdata(L,index))
            {
                lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
                lua_error(L);
            }
            return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
        } 
    };

	template<typename T>
	T lua2type(lua_State *L, int index)
	{
		return	if_<is_enum<T>::value
					,lua2enum<T>
					,lua2object<T> 
				>::type::invoke(L, index);
	}

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) {}

		template<typename T1>
		val2user(T1 t1) : user(new T(t1)) {}

		template<typename T1, typename T2>
		val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

		template<typename T1, typename T2, typename T3>
		val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

		template<typename T1, typename T2, typename T3, typename T4>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4) : user(new T(t1, t2, t3,t4)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : user(new T(t1, t2, t3,t4,t5)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) : user(new T(t1, t2, t3,t4,t5, t6)) {}

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) : user(new T(t1, t2, t3,t4,t5, t6, t7)) {}

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) : user(new T(t1, t2, t3,t4,t5, t6, t7, t8)) {}

		~val2user() { delete ((T*)m_p); }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
	};

	// to lua
	template<typename T>
	struct val2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input); } };
	template<typename T>
	struct ptr2lua { static void invoke(lua_State *L, T* input){ if(input) new(lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input); else lua_pushnil(L); } };
	template<typename T>
	struct ref2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input); } };

	template<typename T>
	struct enum2lua { static void invoke(lua_State *L, T val) { lua_pushnumber(L, (int)val); } };

	template<typename T>
	struct object2lua 
	{ 
		static void invoke(lua_State *L, T val) 
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
					,ref2lua<typename base_type<T>::type>
					,val2lua<typename base_type<T>::type>
				>::type
			>::type::invoke(L, val);

			push_meta(L, class_name<typename class_type<T>::type>::name());
			lua_setmetatable(L, -2);
		} 
	};

	template<typename T>
	void type2lua(lua_State *L, T val)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val);
	}

	// get value from cclosure
	template<typename T>  
	T upvalue_(lua_State *L)
	{
		return user2type<T>::invoke(L, lua_upvalueindex(1));
	}

	// read a value from lua stack 
	template<typename T>  
	T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

	template<>	char*				read(lua_State *L, int index);
	template<>	const char*			read(lua_State *L, int index);
	template<>	char				read(lua_State *L, int index);
	template<>	unsigned char		read(lua_State *L, int index);
	template<>	short				read(lua_State *L, int index);
	template<>	unsigned short		read(lua_State *L, int index);
	template<>	long				read(lua_State *L, int index);
	template<>	unsigned long		read(lua_State *L, int index);
	template<>	int					read(lua_State *L, int index);
	template<>	unsigned int		read(lua_State *L, int index);
	template<>	float				read(lua_State *L, int index);
	template<>	double				read(lua_State *L, int index);
	template<>	bool				read(lua_State *L, int index);
	template<>	void				read(lua_State *L, int index);
	template<>	long long			read(lua_State *L, int index);
	template<>	unsigned long long	read(lua_State *L, int index);
	template<>	table				read(lua_State *L, int index);

	// push a value to lua stack 
	template<typename T>  
	void push(lua_State *L, T ret)					{ type2lua<T>(L, ret); }
	
	template<>	void push(lua_State *L, char ret);
	template<>	void push(lua_State *L, unsigned char ret);
	template<>	void push(lua_State *L, short ret);
	template<>	void push(lua_State *L, unsigned short ret);
	template<>	void push(lua_State *L, long ret);
	template<>	void push(lua_State *L, unsigned long ret);
	template<>	void push(lua_State *L, int ret);
	template<>	void push(lua_State *L, unsigned int ret);
	template<>	void push(lua_State *L, float ret);
	template<>	void push(lua_State *L, double ret);
	template<>	void push(lua_State *L, char* ret);
	template<>	void push(lua_State *L, const char* ret);
	template<>	void push(lua_State *L, bool ret);
	template<>	void push(lua_State *L, lua_value* ret);
	template<>	void push(lua_State *L, long long ret);
	template<>	void push(lua_State *L, unsigned long long ret);
	template<>	void push(lua_State *L, table ret);

	// pop a value from lua stack
	template<typename T>  
	T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }
	
	template<>
	void lua_tinker::pop(lua_State *L)
	{
		lua_pop(L, 1);
	}

	template<>	
	lua_tinker::table lua_tinker::pop(lua_State *L)
	{
		return table(L, lua_gettop(L));
	}

	// functor
	template<typename RVal, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void>
	struct functor
	{
        static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5), read<T6>(L,6), read<T7>(L,7), read<T8>(L,8))); return 1; }
	};

    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    struct functor<RVal, T1, T2, T3, T4, T5, T6, T7>
    {
        static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5), read<T6>(L,6), read<T7>(L,7))); return 1; }
    };

    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    struct functor<RVal, T1, T2, T3, T4, T5, T6>
    {
        static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5), read<T6>(L,6))); return 1; }
    };

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<RVal, T1, T2, T3, T4, T5>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return 1; }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	struct functor<RVal,T1,T2,T3,T4> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4))); return 1; }
	};

	template<typename RVal, typename T1, typename T2, typename T3>
	struct functor<RVal,T1,T2,T3> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3))); return 1; }
	};

	template<typename RVal, typename T1, typename T2>
	struct functor<RVal,T1,T2> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2))); return 1; }
	};

	template<typename RVal, typename T1>
	struct functor<RVal,T1> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1)>(L)(read<T1>(L,1))); return 1; }
	};

	template<typename RVal>
	struct functor<RVal>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)()>(L)()); return 1; }
	};

	//
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    struct functor<void, T1, T2, T3, T4, T5, T6, T7>
    {
        static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5), read<T6>(L,6), read<T7>(L,7)); return 0; }
    };


	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<void, T1, T2, T3, T4, T5, T6>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5), read<T6>(L,6)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<void, T1, T2, T3, T4, T5>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4>
	struct functor<void, T1, T2, T3, T4>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)); return 0; }
	};

	template<typename T1, typename T2, typename T3>
	struct functor<void, T1, T2, T3>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)); return 0; }
	};

	template<typename T1, typename T2>
	struct functor<void, T1, T2>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)); return 0; }
	};

	template<typename T1>
	struct functor<void, T1>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L,1)); return 0; }
	};

	template<>
	struct functor<void>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
	};

	// push_functor
	template<typename RVal> 
	void push_functor(lua_State *L, RVal (*func)())
	{
		lua_pushcclosure(L, functor<RVal>::invoke, 1);
	}

	template<typename RVal, typename T1> 
	void push_functor(lua_State *L, RVal (*func)(T1))
	{ 
		lua_pushcclosure(L, functor<RVal,T1>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6>::invoke, 1);
	}

    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> 
    void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7))
    { 
        lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7>::invoke, 1);
    }

	// member variable
	struct var_base
	{
		virtual void get(lua_State *L) = 0;
		virtual void set(lua_State *L) = 0;
	};

	template<typename T, typename V>
	struct mem_var : var_base
	{
		V T::*_var;
		mem_var(V T::*val) : _var(val) {}
		void get(lua_State *L)	{ push(L, read<T*>(L,1)->*(_var));		}
		void set(lua_State *L)	{ read<T*>(L,1)->*(_var) = read<V>(L, 3);	}
	};

	// member function
	template<typename RVal, typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void>
	struct mem_functor
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7), read<T7>(L,8), read<T8>(L,9)));; return 1; }
	};

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    struct mem_functor<RVal, T, T1, T2, T3, T4, T5, T6, T7>
    {
        static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6), read<T6>(L,7), read<T7>(L,8)));; return 1; }
    };

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    struct mem_functor<RVal, T, T1, T2, T3, T4, T5, T6>
    {
        static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6), read<T6>(L,7)));; return 1; }
    };


	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<RVal, T, T1, T2, T3, T4, T5>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)));; return 1; }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4> 
	struct mem_functor<RVal,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5))); return 1; }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3> 
	struct mem_functor<RVal,T,T1,T2,T3>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4))); return 1; }
	};

	template<typename RVal, typename T, typename T1, typename T2> 
	struct mem_functor<RVal,T,T1, T2>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3))); return 1; }
	};

	template<typename RVal, typename T, typename T1> 
	struct mem_functor<RVal,T,T1>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2))); return 1; }
	};

	template<typename RVal, typename T> 
	struct mem_functor<RVal,T>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)()>(L))()); return 1; }
	};

    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8>
    {
        static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9)); return 0; }
    };

    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7>
    {
        static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)); return 0; }
    };

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<void,T,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	struct mem_functor<void,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3>
	struct mem_functor<void,T,T1,T2,T3>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)); return 0; }
	};

	template<typename T, typename T1, typename T2>
	struct mem_functor<void,T,T1,T2>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)); return 0; }
	};

	template<typename T, typename T1>
	struct mem_functor<void,T,T1>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2)); return 0; }
	};

	template<typename T>
	struct mem_functor<void,T>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)()>(L))(); return 0; }
	};

	// push_functor
	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)()) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)() const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6)) 
    { 
        lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
    }

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
	}

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7)) 
    { 
        lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
    }


    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const) 
    { 
        lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
    }

    template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const) 
    { 
        lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1); 
    }


	// constructor
    template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    int constructor(lua_State *L) 
    { 
        new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8));
        push_meta(L, class_name<typename class_type<T>::type>::name());
        lua_setmetatable(L, -2);

        return 1; 
    }

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	// destroyer call ~class
	template<typename T>
	int destroyer(lua_State *L) 
	{ 
		((user*)lua_touserdata(L, 1))->~user();
		return 0;
	}

	// global function
	template<typename F> 
	void def(lua_State* L, const char* name, F func)
	{ 
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, (void*)func);
		push_functor(L, func);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// global variable
	template<typename T>
	void set(lua_State* L, const char* name, T object)
	{
		lua_pushstring(L, name);
		push(L, object);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	template<typename T>
	T get(lua_State* L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		return pop<T>(L);
	}

	template<typename T>
	void decl(lua_State* L, const char* name, T object)
	{
		set(L, name, object);
	}

	// call
	template<typename RVal>
	RVal call(lua_State* L, const char* name)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			if(lua_pcall(L, 0, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1>
	RVal call(lua_State* L, const char* name, T1 arg)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg);
			if(lua_pcall(L, 1, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			if(lua_pcall(L, 2, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			if(lua_pcall(L, 3, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			if(lua_pcall(L, 4, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			if(lua_pcall(L, 5, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			if(lua_pcall(L, 6, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        if(lua_isfunction(L,-1))
        {
            push(L, arg1);
            push(L, arg2);
            push(L, arg3);
            push(L, arg4);
            push(L, arg5);
            push(L, arg6);
            push(L, arg7);
            if(lua_pcall(L, 7, 1, errfunc) != 0)
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, -2);
        return pop<RVal>(L);
    }

    template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
    {
        lua_pushcclosure(L, on_error, 0);
        int errfunc = lua_gettop(L);

        lua_pushstring(L, name);
        lua_gettable(L, LUA_GLOBALSINDEX);
        if(lua_isfunction(L,-1))
        {
            push(L, arg1);
            push(L, arg2);
            push(L, arg3);
            push(L, arg4);
            push(L, arg5);
            push(L, arg6);
            push(L, arg7);
            push(L, arg8);
            if(lua_pcall(L, 8, 1, errfunc) != 0)
            {
                lua_pop(L, 1);
            }
        }
        else
        {
            print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
        }

        lua_remove(L, -2);
        return pop<RVal>(L);
    }



	// class helper
	int meta_get(lua_State *L);
	int meta_set(lua_State *L);
	void push_meta(lua_State *L, const char* name);

	// class init
    // 每个class就是一个table __newindex __name __index 都是table
    // key , class table 的metatabel ???
    //
	template<typename T>
	void class_add(lua_State* L, const char* name) 
	{ 
		class_name<T>::name(name); //存入name 作用??

        //下面的table的key
		lua_pushstring(L, name);

		lua_newtable(L); //meta table -3 注册到LUA_GLOBALSINDEX)
		lua_pushstring(L, "__name"); //注册name
		lua_pushstring(L, name);
		lua_rawset(L, -3);

		lua_pushstring(L, "__index"); 
		lua_pushcclosure(L, meta_get, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcclosure(L, meta_set, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__gc");
		lua_pushcclosure(L, destroyer<T>, 0);
		lua_rawset(L, -3);

		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// Tinker Class Inheritence
	template<typename T, typename P>
	void class_inh(lua_State* L)
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, "__parent");
			push_meta(L, class_name<P>::name());
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Constructor
	template<typename T, typename F>
	void class_con(lua_State* L,F func)
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_newtable(L);
			lua_pushstring(L, "__call");
			lua_pushcclosure(L, func, 0);
			lua_rawset(L, -3);
			lua_setmetatable(L, -2);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Functions 在Lua中注册一个class
	template<typename T, typename F>
	void class_def(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(F))) F(func);
			push_functor(L, func);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Variables
	template<typename T, typename BASE, typename VAR>
	void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

    //存入一个指定name,没有指定返回""
	template<typename T>
	struct class_name
	{
		// global name
		static const char* name(const char* name = NULL)
		{
			static char temp[256] = "";
			if(name) strcpy(temp, name);
			return temp;
		}
	};

	// Table Object on Stack
	// lua VS 中table造作的c++封装类,通过table_obj就可以在VS中操作table
	struct table_obj
	{
	        //取得VS中index指向的table的地址
		table_obj(lua_State* L, int index);
		~table_obj();

		void inc_ref(); //引用计数，为Table包装器提供支持
		void dec_ref();

		bool validate(); //检测是否是一个table类型，或userdata，

		//把table中key=index的value设为object 
		template<typename T>
		void set(int index, T object)
		{
			if(validate())
			{
				lua_pushinteger(m_L, index);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		//得到table 中制定index 的值,如果index 无效返回nil
	    	template<typename T>
		T get(int index)
		{
			if(validate())
			{
				lua_pushinteger(m_L, index);
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		//以string为key的set
		template<typename T>
		void set(const char* name, T object)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		//以string为key的get
		template<typename T>
		T get(const char* name)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		//common by evan [2012/7/2]
		//add by szd [2010/04/06]
		//在table中压如obj，不是用元方法
		template <typename T>
		void put(T object)
		{
			if (validate())
			{
				push(m_L, object);
				lua_rawseti(m_L, m_index, size() + 1);
			}
		}

		//得到table的大小，和#table的值相同
		int size()
		{
			if (validate())
			{
				lua_pushinteger(m_L, luaL_getn(m_L, m_index));	//only use in table here
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<int>(m_L);
		}

		//去的一个table的value，不是用元方法
		template <typename T>
		T fetch(int index)
		{
			//index from 1 default
			assert(index>=1 && index<=size());
			if (validate())
			{
				lua_rawgeti(m_L, m_index, index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		lua_State*		m_L; //lua State
		int				m_index; //table 在VS中的index
		const void*		m_pointer; //基本没啥用
		int				m_ref;//引用计数 
	};

	// Table Object Holder
	struct table
	{
		table(lua_State* L);
		table(lua_State* L, int index);
		table(lua_State* L, const char* name);
		table(const table& input);
		~table();

		template<typename T>
		void set(int index, T object)
		{
			m_obj->set(index, object);
		}

		template<typename T>
		T get(int index)
		{
			return m_obj->get<T>(index);
		}

		template<typename T>
		void set(const char* name, T object)
		{
			m_obj->set(name, object);
		}

		template<typename T>
		T get(const char* name)
		{
			return m_obj->get<T>(name);
		}


		//////////////////////////////////////////////////////////////////////////
		int size() 
		{
			return m_obj->size();
		}

        bool empty()
        {
            return m_obj->size() == 0;
        }

		template <typename T>
		void put(T object)
		{
			m_obj->put(object);
		}

		template <typename T>
		T fetch(int index)
		{
			assert(index>=1 && index<=size());
			return m_obj->fetch<T>(index);
		}

		table_obj*		m_obj;
	};

} // namespace lua_tinker

typedef lua_tinker::table Table;

#endif //_LUA_TINKER_H_
