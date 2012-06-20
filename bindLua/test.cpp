/*
 *  Description: 
 */
#include <stdio.h>
#include <stdlib.h>

#if 0
template<bool C, typename A, typename B> struct if_ {};
template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };
template <typename T> T* add_ptr(T&); 


//不是模版函数也可以是元函数，对类型进行处理
int vfnd_ptr_tester (const int*); 
double vfnd_ptr_tester (const double*); 
#endif

#if 1
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
	struct is_enum  //检测T是不是一个enum
	{ 
		static T arg; 
		// arg 必须是int兼容类型 && add_ptr(arg) 返回的是void*
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && 
			                    (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 
#endif

enum EN {a};
int main() {
    //只声明可以调用吗,可以在编译器只返回类型，是个元函数
    int x;
    printf("size:%d\n", sizeof(add_ptr(x))) ;
    int* p;
    printf ( "size:%d\n", sizeof(vfnd_ptr_tester(p)) );
    if (is_enum<int>::value)
	printf ( "is enum\n" );
    else
	printf ( "not enum\n" );





    return 0 ;
}


