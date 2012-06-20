--------------------------------------------------------------------------------
--         FILE:  mulittaskMgr.lua
--      VERSION:  1.0
--      CREATED:  05/17/2012 09:33:59 AM CST
--------------------------------------------------------------------------------

local  function Multitask()
    local function LclWork(id, count) -- simulate some work
	for j = 1, count do
	    io.write(id,j)
	    io.flush()
	    local stop = os.time() + 1
	    while  os.time() < stop do end
	    coroutine.yield()
	end
    end

     --simulate check for user cancellation
     --return math.random(12) > 1
    local function LclContinue()
	return math.random(12) > 4
    end

    local WorkA = coroutine.create(function () LclWork('A',2) end)
    local WorkB = coroutine.create(function () LclWork('B',4) end)

    --set random gerearl zhongzhi like srand in C
    math.randomseed(os.time())

    local a, b, ok = true, true, true

    while (a or b) and ok do
	ok = LclContinue()
	if ok and a then
	    a = coroutine.resume(WorkA)
	    print("a resume:" .. tostring(a))
	end
	ok = LclContinue()
	if ok and b then
	    b = coroutine.resume(WorkB)
	    print("b resume:" .. tostring(b))
	end
    end
    io.write(ok and " done" or " cancel", "\n")
end
Multitask()
