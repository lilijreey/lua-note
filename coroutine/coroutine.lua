--------------------------------------------------------------------------------
--         FILE:  coroutine.lua
--      VERSION:  1.0
--      CREATED:  05/15/2012 01:18:41 PM CST
--------------------------------------------------------------------------------

local function KnightFnc()
    print [[KnightFnc1:
    The name of the song is caleed "HADDC."
    ]]
    coroutine.yield() --//do what?
    print [[KnightFnc2, looking a little vexed:
    NO, you don't understand, that's what the name is CALLD.
    The name really Is "THE AGED AGED MAN."
    ]]
    coroutine.yield() 
    print [[KnightFnc3:
    but that's only what it's CALS, your konw!
    ]]
    coroutine.yield() 
    print [[KnightFnc4:
    I was coming to  Ths song reall.
    ]]
end

local function Alice()
    local KnightFnc = coroutine.create(KnightFnc)
    print("createed");
    coroutine.resume(KnightFnc)
    print [[Alice1: trying to feel interseted:
    Oh, that's the name of the song. it it?
    ]]
    coroutine.resume(KnightFnc)
    print [[Alice2: 
    Then I ought to have said "Thais waht the SONG"?
    ]]
    coroutine.resume(KnightFnc)
    print [[Alice3: 
    Well, what IS the song, then?
    ]]
    coroutine.resume(KnightFnc)
end

--start Alice
--Alice()

local function main()
    print("A1")
    coroutine.yield()
    print("A2")
end

print(type(coroutine.create(main)))
print(type(coroutine.wrap(main)))
