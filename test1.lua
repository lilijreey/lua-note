--------------------------------------------------------------------------------
--         FILE:  test1.lua
--      VERSION:  1.0
--      CREATED:  04/14/2012 11:57:30 AM CST
--------------------------------------------------------------------------------

--[[
function ShowT(x)
    return type(x)
end

tt1 = { name = "Evan",
	address = "shangxi",
	age ="10",
	sex = "male"
}

tt2 = {}
for _, v in pairs(tt1) do 
    print(_, v);
    --每次插入一个table
    --_ 就相当一个名字，两个_ 不同
    --v 一样
    --tt2 的每个index都是一个pair table
    table.insert(tt2, {_ = _, v = v})
end

tt2 = {11, [3]=11}
local i = 1
while i < 10 do
    table.insert(tt2, i);
    i =  i + 1
end


xx = { 1, 2, 3, 4 ,3}
table.sort(xx, function(a, b) return a < b end);

print(table.concat(xx,"##"))

print(table.remove({1,2,3}))
]]--

--[[
tt2 = {11, [3]=11}
table.insert(tt2, 20, 1);
for i, v in pairs(tt2) do
    print( i ,v )
end

print(#{[-1]=2, [-2]=2});
--eq 0

print(table.remove(tt2))
--print(table.remove(tt2))
]]--

--[[
function MM(n)
    --this just a definition not involed
    local function Get()
	return n
    end

    local function Inc(m)
	n = n + m
    end
    -- Get and Inc are not a same things
    -- is a name the other a value
    -- get two local functions
    --return Get, Inc;
    return {Get = Get, Inc = Inc}
end

aa = MM(33)
print(aa.Get())
aa.Inc(100)
print(aa["Get"]())
]]--

--[[
--使用do可以形成一个作用于，可以使得func似有化
do
    local function Get(obj)
	print(obj.N)
    end

    local function Inc(obj, M)
	obj.N  = obj.N + M
    end

    --not a local func
    function MM(n)
	return {N = n, Get = Get, Inc = Inc, ss =ss}
    end

end

x=MM(3);
x:Inc(10); --same as x.Inc(x, m)
x:Get();
]]--

--[[
--另一个使用self
--只有table有self
do 
    --T is only used as a container
    local T = {}

    --this function neither local nor global they are field of a table
    -- T:SS()
    function T.SS(self)
	print(self.N .. "ss")
    end

    -- T.Get(self)
    function T:Get()
	--self same T
	print(self.N)
    end

    function T:Inc(m)
	self.N = self.N + m
    end

    --constructor function
    function S1(n) 
	return {N=n, Get = T.Get, Inc = T.Inc, SS = T.SS}
    end
end

x2 = S1(4)
--same as xx2.Get(xx2)
x2:Get()
--same as xx2.Inc(xx2, 1)
x2:Inc(1)
x2:SS()
]]--


--[[
aa = {"bb", "cc", "aa"}
function so(x)
    table.sort(x)
    return x
end
function ss(x)
    for i , v in ipairs(x) do 
	print(i , v)
    end
end

ss(so(aa))
ss(aa)
]]--


--[[
do 
    local ss = {Str = "aa"}
-- x is reference not a copy
    a1= {x= ss, Str = ss.Str}
    a2= {x= ss, Str = ss.Str}
end

a1.x.Str="bnb"
print(a2.x.Str)
a1.Str="ob"
print(a2.Str)

--do ring
do
    local a = {s="a"}
    local b = {s="b"}
    local c = {s="c"}
    a.next=b;
    b.next=c
    c.next =a;
    function get()
     return a;
    end
end


x = get();
    
while x.next do
    print(x.s)
    x=x.next
end
]]--


--[[
function ss(t)
for k , v in pairs(t) do
    if type(v) == "table" and v ~= _G then
	print(k .." velue is table")	
	ss(v);
    else
	print(k , v)
end
end
end

print(#_G)
a = _G;
if a ~= _G then
    print("==")
end

]]--
--5.20 is not surppot
--setfenv(xx,{})

--str = "ABCdefg"

--print(string.lower(str))
--print(str)
--
--print(string.sub(str,1,3))

--line =  io.read()
--print("xx" ..  line)

--EE io
--[[
fd , err = io.open("usefile.txt", "r+")
print(fd, err);
if err ~= nil then
    print("error")
else

fd:write("Line 1\nLine 2\n Line 3\n")
fd:close();

fd, err = io.open("usefile.txt");
print(fd, err);

print(fd:read())
print(fd:read())
print(fd:read())
print(fd:read())
fd:close();

--like rm
print(os.remove("usefile.txt"));
]]--

--[[
--EE string.format
--like c printf
name = "Evan"
age = 10
print(string.format("posener name:%10s, age:%d", name, age));

function Report(users)
    print("USERNAME	NEW POSTS RELIES")
    for username, user in pairs(users) do
	print(string.format("%-15s, %6d	%6d",
	                     username, user.NewPosts, user.Replies)) 
    end
end

--call func
Report({
    ak = {NewPosts= 39, Replies = 19},
    os = {NewPosts = 22, Replies = 21},
    lee = {NewPosts = 232, Replies=0}})

]]--

--EE also can use [[ ..,. ]] formaat stirng
-- not need "
--print(string.format([[
-->%5.0f<-- width: 5; pre : 0
-->%5.1f<-- width: 5; pre : 1
-->%5.2f<-- width: 5; pre : 2]], 9.33,9.33,9.33 ))

--print([[hello world]])
--print(string.format("%q", [[back: \; double quote: ]]))
--print(string.format("%q",[[" fe \ xx \n]]))
--[[

--EE output a file
local iter = io.lines("test1.lua")
for ll in iter do
    print(ll)
end
]]--


--print(string.gsub([[patches/"  oo]], "^patches/", "", 1))
--print(string.gsub("fef f.png", "%.png$", "", 1))
--print(string.gsub("12-34,334/2-22", "[/,.-]", ""))
--print(string.gsub("12 value", "%d", "*"))


--function Squeeze(str)
--    return (string.gsub(str, "%s+", "-"))
--end
--
--testStr = {
--    "nospcase",
--    "alpha brave charlie",
--    "	alpha	brave charlie",
--    "\nalpha\tbrave\tcharlie\na\tb\tc",
--    [[
--    alpha
--    brave
--    charlie]]
--}
--
--for _, str in ipairs(testStr) do
--    io.write("UNSQUEEZED: <", str, ">\n");
--    io.write("  SQUEEZED: <", Squeeze(str), ">\n");
--end

--[[
local str = "abc def"
local patt ="%a+"
print(string.match(str, patt, 2))
print(string.find(str, patt, 2))
print(string.find("mgc %^&*", "%^", 1, true ))
返回匹配的字符串在text中的开始index和结束index
[i,l] 只返回第一个匹配的
找不到返回 nil

Frmt="%10s\n"
for _, name in ipairs({"Lynn", "Jeremy", "Sally"}) do
    io.write(string.format(Frmt, name))
end

print(assert(1, "assert failed"))
error("nihe");

]]--

--[[
function ok(a, b, c)
    local a , b, c = tonumber(a), tonumber(b), tonumber(c)
    print(a and b and c and x+y+z
            or "expected")
end

function pp()
    print("ok1")

    error("fef");
    print("ok2")

    return 1
end
local a , b =pcall(pp)
print(a, b)

local a , b =xpcall(pp, debug.traceback)
print(a, b)

--]]


--[[
--print(package.path)
require("common")
local aa = {"ni", "ss" ,343}
ObjectShow(aa, "aa")
]]--

--[[
local ModStr= "mm"
print("Main, before require", package.loaded[ModStr])
local Val = require(ModStr)
print("Main, after 1st require", package.loaded[ModStr], Val)
Val = require(ModStr)
print("Main, after 2nd require", package.loaded[ModStr], Val)
Val = require(ModStr)
print("Main, after reset", package.loaded[ModStr], Val)

package.loaded["ok"]=true;
print("requie ok ", require("ok"));
]]--

--require("namespaseTest")
--print(nn.Quote("ni"))

-- EE metamethods
--a, b = {}, {}
--print(a .. b)
-- splicing two arrays with the concatenation opreator
--[[
do
    local SpliceMeta 
    -- SpliceMeta needs to be in
    -- SpliceMeta.__concat’s scope.
    SpliceMeta = {
    -- Makes a new array by splicing two arrays together:
	__concat = function(ArrA, ArrB)
	    assert(type(ArrA) == "table" and type(ArrB) == "table")
	    local Ret = setmetatable({}, SpliceMeta)
	    for I, Elem in ipairs(ArrA) do
		Ret[I] = Elem
	    end
	    local LenA = #ArrA
	    for I, Elem in ipairs(ArrB) do
		Ret[LenA + I] = Elem
	    end
	    return Ret
	end ,
    -- Make +
    __add = function(ArrA, ArrB)
	print("++")
	return ArrB
    end ,
    __tostring = function(TT)
	print("TTStrng")
	return "TTStrng";
    end,
    --table have alreay defind __len
    __len = function(T)
	print("M LEN")
	return 11
    end ,
    __eq = function(ArrA, ArrB)
	print("Eq")
	return 5
    end,
    __lt = function(ArrA, ArrB)
	print("LT")
	return nil
    end,


    }
    -- Takes an array, returns that array after giving it a
    -- metamethod that makes it “spliceable” with the
    -- concatenation operator:
    function MakeSpliceArr(Arr)
	return setmetatable(Arr, SpliceMeta)
    end
end

A = MakeSpliceArr{"a1", "a2", "a3"}
--B = MakeSpliceArr{"b1", "b2", "b3"}
B = {"b1", "b2", "b3"} --is ok bacause cheick A metatable
--and use A __concat
C = A .. B -- So it can concatenation with ..
for i, elem in ipairs(C) do
    print(i, elem)
end

for i, elem in ipairs(A+B) do
    print(i, elem)
end

--tet __len
print(#A)
--test __eq
print(A == B)
debug.setmetatable(1,{__len=math.abs})
print(#9)

]]--

--[[
print(getmetatable("i"))
print(getmetatable(3))
print(getmetatable(nil))
print(getmetatable(true))

local a = 3;
local a = function()
    print("a");
end

print(a)
a()
]]

--[[
--genereatic for
local function ReverseIt(t)
    local function Iter()
	for J = #t, 1, -1 do
	    --return J, t[J]
	    coroutine.yield(J, t[J])
	end
    end
    return coroutine.wrap(Iter)
end

for j, str in ReverseIt({"one", "w", "t"}) do
    print(j, str)
    for k, str1 in ReverseIt({"1", "2", "3"}) do
	print("XXXX: ",k, str1)
    end
end

function xx(str)
    print(str)
end

bb = string.dump(xx)
loadstring(bb)("xx")
]]

--print(string.rep("a", 3,"#"))
print(_VERSION)

function aaa()
    if a then
        print(nil)
    end
end

aaa()

--local f ; f =function() print(type(f)) end
