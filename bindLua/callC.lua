--------------------------------------------------------------------------------
--         FILE:  callC.lua
--      VERSION:  1.0
--      CREATED:  05/22/2012 01:52:59 PM CST
--------------------------------------------------------------------------------

package.cpath="./?.so;"
local stack = require "cbind"

print(type(stack.StackLook))

print(stack.StackLook(5,6,7,8,9));
