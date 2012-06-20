#lua 链接时 -llua -ldl 需要动态库 -lm 需要数学库
a.out: test.c
	gcc -Wall test.c -llua -ldl -lm
