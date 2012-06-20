--------------------------------------------------------------------------------
--         FILE:  producer_Consumer.lua
--      VERSION:  1.0
--      CREATED:  05/16/2012 01:30:17 PM CST
--------------------------------------------------------------------------------

local function LclProducer()
    print("Producer: initialize")
    --simulate event generation
    local List = {"m1", "m2", "m3", "m4"}
    for j, val in ipairs(List) do
	local evt = string.format("Event %d(%s)", j, val)
	print("Producer: ".. evt)
	coroutine.yield(evt) --return evt
    end
    print("Producer: finalize")
    return "end"  --return end
end

local function LclConsumer()
    local GetEvent = coroutine.wrap(LclProducer)
    local evt
    print("Consumer: initialize")
    while evt ~= "end" do
	evt = GetEvent() --it just as coroutine.resum(LclProducer)
	print("Consumer: " .. evt)
    end
    print("Consumer finalize")
end

LclConsumer()
