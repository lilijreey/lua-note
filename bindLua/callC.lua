--------------------------------------------------------------------------------
--         FILE:  callC.lua
--      VERSION:  1.0
--      CREATED:  05/22/2012 01:52:59 PM CST
--------------------------------------------------------------------------------

package.cpath="./?.so;"
local ud = require "bind"

print(type(ud))

-- 使用fulluserdata 的绑定
--[[ 
--create a new userdata x reference 
--the us_new pushed userd
x=ud.us_new()
print(type(x))
ud.us_set_a(x,1)
print(ud.us_get_a(x))
ud.us_set_d(x,3.32323)
print(ud.us_get_d(x))
]]

-- 使用ligtuserdata 的绑定
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
