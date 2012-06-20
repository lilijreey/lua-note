--------------------------------------------------------------------------------
--         FILE:  common.lua
--      DESCRIP:  有用的lua函数
--      VERSION:  1.0
--      CREATED:  04/14/2012 11:54:16 AM CST
--------------------------------------------------------------------------------

function ObjectDescribe(Tbl, Val, Key, TruncLen)
    LclRender(Tbl, Val, LclRenderStr(Key, TruncLen), TruncLen or 0, 1, {}, "")
end

--- show a object info is very wonderful
function ObjectShow(Val, Key, TruncLen)
    local Tbl = {}
    ObjectDescribe(Tbl, Val, Key, TruncLen)
    for J, Rec in ipairs(Tbl) do
       io.write(string.rep(" ", Rec[1] - 1), Rec[2], "\n")
    end
end


local function LclRenderStr(Obj, TruncLen)
    local TpStr = type(Obj)
    if TpStr == "string" then
	Obj = string.gsub(Obj, "[^%w %p]", function(Ch)
			  return "\\" .. string.format("%03d", string.byte(Ch)) end )
	if TruncLen and TruncLen > 0 and string.len(Obj) > TruncLen + 3 then
    -- This could misleadingly truncate numeric escape value
	Obj = string.sub(Obj, 1, TruncLen) .. "..."
    end
    Obj = '"' .. Obj .. '"'
    elseif TpStr == "boolean" then
    Obj = "boolean: " .. tostring(Obj)
    else
    Obj = tostring(Obj)
    end
    return Obj
end

--print(LclRenderStr("34aaf\n \t*32 a ()"))

-- This function replaces ["x"]["y"] stubble with x.y Keys are assumed to be
-- identifier-compatible.
local function LclShave(Str)
    local Count
    Str, Count = string.gsub(Str, '^%[%"(.+)%"%]$', '%1')
    if Count == 1 then
    Str = string.gsub(Str, '%"%]%[%"', '.')
    end
    return Str
end

local function macthA(str)
    local t = type(str)
    local count
    if t == "string" then   -- subtrac this [ ]
	str ,count = string.gsub(str, '^%[%"(.+)%"%]$', '%1')
	if count == 1 then -- if have substitutioned sub "][" with .
	    str = string.gsub(str, '%"%]%[%"', '.')
	end
    else
	print(tostring(str) .. " is not a string");
    end
    return str
end

--print(macthA('["x"]["y"]'))

local function LclRender(Tbl, Val, KeyStr, TruncLen, Lvl, Visited, KeyPathStr)
    local VtpStr, ValStr
    VtpStr = type(Val)
    if Visited[Val] then
	ValStr = "same as " .. Visited[Val]
    else
	ValStr = LclRenderStr(Val, TruncLen)
	if VtpStr == "function" then -- Display function's environment
	    local Env = _ENV.Val
	    Env = Visited[Env] or Env
	    ValStr = string.gsub(ValStr, "(function:%s*.*)$", "%1 (env " ..
	    string.gsub(tostring(Env), "table: ", "") .. ")")
	elseif VtpStr == "table" then
	    ValStr = ValStr .. string.format(" (n = %d)", #Val)
	end
    end
    KeyPathStr = KeyPathStr .. "[" .. KeyStr .. "]"
    table.insert(Tbl, { Lvl, 
                        string.format('[%s] %s',KeyStr, ValStr) })
    if VtpStr == "table" and not Visited[Val] then
	Visited[Val] = LclShave(KeyPathStr)
	local SrtTbl = {}
	for K, V in pairs(Val) do
	    table.insert(SrtTbl, { LclRenderStr(K, TruncLen), V, K, type(K) })
	end
	local function LclCmp(A, B)
	    local Cmp
	    local Ta, Tb = A[4], B[4]
	    if Ta == "number" then
		if Tb == "number" then
		Cmp = A[3] < B[3]
		else
		Cmp = true -- Numbers appear first
		end
	    else -- A is not a number
		if Tb == "number" then
		Cmp = false -- Numbers appear first
		else
		Cmp = A[1] < B[1]
		end
	    end
	return Cmp
	end
	table.sort(SrtTbl, LclCmp)
	for J, Rec in ipairs(SrtTbl) do
	    LclRender(Tbl, Rec[2], Rec[1], TruncLen, Lvl + 1, Visited, KeyPathStr)
	end
    end
end


