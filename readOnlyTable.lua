--------------------------------------------------------------------------------
--         FILE:  readOnlyTable.lua
--         DEC:  create a readOnly table
--      VERSION:  1.0
--      CREATED:  05/30/2012 01:20:46 PM CST
--------------------------------------------------------------------------------


--有table后设施为只读

local tt = {}

tt.one = 1
tt.tow= 2
tt.three= 3

--EE 方法1直接
function setReadOnlyMode1(t)
    if "table" ~= type(t) then
	error("passed argument not a table", 2)
    end
    local mt = {
	__newindex = function(t, k, v)
	    error("attempt to update a read-only table", 2)
	end
    }
    setmetatable(t, mt)
end

--setReadOnlyMode1(tt)
--for _, v in pairs(tt) do
--    print(_, v)
--end

--EE 2 使用代理，返回一个loacl table，
--这个table 的__index 指向指定的talbe，
--	__newindex 屏蔽掉,这样写就可一不用改变原始
--	table的元表，只是得到一个只读的引用
--注意使用这样的代理就不能使用pairs 这种迭代其了

function setReadOnlyMode2(tt)
    if "table" ~= type(tt) then
	error("passed argument not a table", 2)
    end
    local proxy = {}
    local mt = {
	__index = tt, -- 通过这个代理访问
	__newindex = function(t, k, v)
	    error("attempt to update a read-only table", 2)
	end
    }
    setmetatable(proxy, mt)
    return proxy
end

--e.g. 比如有个func希望输入的table是只读的，
--这样不太会产生BUG,可以这样写
local function someFunc(tt)
    local RO_table = setReadOnlyMode2(tt)
    tt=nil

end

--e.g
--map = setReadOnlyMode2{"sunday", "Monday", "Tuesday"}
someFunc(tt)
print("xxxx")
    for _, v in pairs(tt) do
	print(_, v)
    end
