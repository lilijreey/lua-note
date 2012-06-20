--------------------------------------------------------------------------------
--         FILE:  modload.lua
--      VERSION:  test requir and package
--      CREATED:  05/12/2012 11:22:01 AM CST
--------------------------------------------------------------------------------

print("modload start")
print(..., package.loaded[...])
local SS = "modle"
print("modload end")
return SS 
