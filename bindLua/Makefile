#lua 链接时 -llua -ldl 需要动态库 -lm 需要数学库

# $^ 所有必要文件名，
# $@ 目标文件名
# $< 第一个必要文件名
bind.so: bind.cpp ../showvs.h ../showvs.c
	g++ -Wall $^ -shared -llua -ldl -lm -o $@

clean:
	rm -f bind.so
