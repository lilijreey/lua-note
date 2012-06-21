--------------------------------------------------------------------------------
--         FILE:  callC.lua
--      VERSION:  1.0
--      CREATED:  05/22/2012 01:52:59 PM CST
--------------------------------------------------------------------------------

package.cpath="./?.so;"
local ud = require "bind"

print(type(ud))

--create a new userdata x reference 
--the us_new pushed userd
x=ud.us_new()
print(type(x))
ud.us_set_a(x,1)
print(ud.us_get_a(x))
ud.us_set_d(x,3.32323)
print(ud.us_get_d(x))
