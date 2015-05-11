--------------------------------------------------------------------------------
--         FILE:  ENV.lua
--         test evn
--      VERSION:  1.0
--      CREATED:  05/25/2012 10:29:32 AM CST
--------------------------------------------------------------------------------

--5.2
--所有的全局引用都会被转换为对_ENV table中var 的引用
--也就是是 a=32 其实会被编译为 _ENV.a=32

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

a=323
if a == _ENV.a then
    print("a == _ENV.a")
end
--_ENV=nil
print("33333333333");
for _, v  in pairs(_G) do
    print(_, v)
end
