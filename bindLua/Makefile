#lua 链接时 -llua -ldl 需要动态库 -lm 需要数学库

cbind.so: Cbind.c
	gcc -Wall $< -shared -llua -ldl -lm -o $@
