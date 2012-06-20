--------------------------------------------------------------------------------
--         FILE:  ENV.lua
--         test evn
--      VERSION:  1.0
--      CREATED:  05/25/2012 10:29:32 AM CST
--------------------------------------------------------------------------------

--可以修改upvalue
function aa() 
    ok = 3
    return function() ok = ok + 1 print(ok)  end

end
--oo = aa()
--show _ENV

--_ENV he _G 的地址一样，但是_ENV 包括 _G
--_ENV 是一个upvalue,er _G不是
--_G 中没有_ENV
print(_ENV);
print(_G);

_ENV=nil
print("33333333333");
for _, v  in pairs(_G) do
    print(_, v)
end
