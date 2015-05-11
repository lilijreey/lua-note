--------------------------------------------------------------------------------
--         FILE:  grabageCollect.lua
--      VERSION:  1.0
--      CREATED:  05/18/2012 11:54:53 AM CST
--------------------------------------------------------------------------------
--
--lua 是完全自动管理内存的
--所有的lua对象都是自动管理的包括userdata, functions thread .....
--使用mark-and-sweep GC
-- 分为两步
--  1. pause
--  2. multiplier
--  
--     pause 控制多长时间开始一个新的GC循环
--     小于100表示collector 不会等待，200表示
--     当使用的内存大于开始的两倍时，开始GC
--     
--     multiplier 控制分配内存的频率，越大越快

--当内存使用到达一定数量是，luaVM 就会检查可以回收
--的并回收
--回收函数
--collectgarbage("collect")
function GarbageTest(Len)
    -- Clean up any garbage:
    collectgarbage("collect")
    for I = 0, 9 do
	-- Create a Len-character string and put it in a variable
	-- that will go out of scope at the end of this iteration:
	local Junk = string.rep(I, Len)
	-- Print a line of a bar chart showing memory usage:
    -- 
    -- count 打印当前使用的内存大小(kb)
	print(collectgarbage("count")) -- This use
	-- of string.rep increases memory usage a bit too, but
	-- that can be ignored.
    end
end

--GarbageTest(20000)

f1 = function() end
f2 = function() end
f3 = function() end
x="xx"

--wt1 = { [f1]=1, [f2]=2, [f3]=3, x=1}
--setmetatable(wt1, {__mode = "k"})
--f2 = nil
--x=nil
--collectgarbage("collect")
--for _, v in pairs(wt1) do
--    print(_,v)
--end


wt1 = {f1,f2,f3,x}
--setmetatable(wt1, {__mode = "v"})
f2 = nil
x=nil
collectgarbage("collect")
for _, v in pairs(wt1) do
    print(_,v)
end

--collectgarbage("stop")  停止垃圾回收
--collectgarbage("restart")  重新开始GC


-- 避免内存泄露 和C++ 的不同 
-- 主要是没有使用local 定义变量
-- 非local 变量是不会被释放的；
collectgarbage("setpause", 100)
collectgarbage("setstepmul", 5000)
