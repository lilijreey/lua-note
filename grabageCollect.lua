--------------------------------------------------------------------------------
--         FILE:  grabageCollect.lua
--      VERSION:  1.0
--      CREATED:  05/18/2012 11:54:53 AM CST
--------------------------------------------------------------------------------

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


