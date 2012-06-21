#lua 链接时 -llua -ldl 需要动态库 -lm 需要数学库
a.out: test.c showvs.h showvs.c
	gcc -Wall test.c showvs.c -llua -ldl -lm
