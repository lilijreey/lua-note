--------------------------------------------------------------------------------
--         FILE:  callC.lua
--      VERSION:  1.0
--      CREATED:  05/22/2012 01:52:59 PM CST
--------------------------------------------------------------------------------

package.cpath="./?.so;"
local ud = require "bind"

print(type(ud))

-- 使用fulluserdata 的绑定
--create a new userdata x reference 
--the us_new pushed userd
--[[
x=ud.us_new()
print(type(x))
ud.us_set_a(x,1)
print(ud.us_get_a(x))
ud.us_set_d(x,3.32323)
print(ud.us_get_d(x))

t=ud.ot_new()
print(type(t))
ud.us_set_a(t,1) -- bad userdata type
]]

--use OO op userdata
--set usedata metatable in lua
--[[
s1=ud.us_new()
local sm=getmetatable(s1)
sm.__index=sm;
sm.us_set_a = ud.us_set_a
sm.us_set_d = ud.us_set_d
sm.us_get_a = ud.us_get_a
sm.us_get_d = ud.us_get_d
]]

s1:us_get_d()
s1:us_set_d(333)
print(s1:us_get_d())

s2=ud.us_new(2,2)
print(s2:us_get_a())



-- 使用ligtuserdata 的绑定
--[[
x=ud.us_new(1,2)
print(type(x))
print(ud.us_get_a(x))
print(ud.us_get_d(x))
ud.us_set_a(x,0)
ud.us_set_d(x,3.32323)
print(ud.us_get_d(x))

y=ud.us_new()
print(ud.us_get_a(y))
print(ud.us_get_d(y))

ud.us_free(x)
ud.us_free(y)
]]
