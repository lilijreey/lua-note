    config_name = "name"
    config_value = "value"

    print("test.lua")
BLUE = {r=0, g=0, b=1}
bg= BLUE
--
--function oo()
--end
--
--function ok(x, str)
--    print("value:%d, str:%s\n", x, str);
--    return 888
--end

--调用C中注册的l_add
a=43
--print(l_add(a))

--read a table which use c create
--print("c_table: " .. type(c_table))
--print("c_table.r: " .. c_table.r)

-- use userdata
-- light userdata
--print("lightUD: " .. type(lightUD))

print("cud: " .. type(cud))
