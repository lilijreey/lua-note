--------------------------------------------------------------------------------
--         FILE:  event-handle.lua
--      VERSION:  1.0
--      CREATED:  05/17/2012 01:58:30 PM CST
--------------------------------------------------------------------------------

local Window, Lcl = {}, {}

-- queue of posted event message

Lcl.msgQueue = {}

--Parent: window object of parent
--Id: string identifier, e.g. 1.2.5 for 5th child of 2nd child of window 1
--Co: coroutine(Fnc) blocks until terminating event
--ChildCount: number of active children
--ChildSerial: used for naming new children
--ChildList: associative array keyed by child window objects
--Close: assigned the terminating message (usually “cancel” or “ok”)
--User: table passed to handler to hold data that must persist from event to event

Lcl.wList = {}
Lcl.wActive= nil -- Currently active window
Lcl.msgReturn = "\000"

function Lcl.Show()
    local list = {}
    for id, wnd in pairs(Lcl.wList) do
	table.insert(list, wnd.Close and (id .. "pending closure)") or id)
    end
    table.sort(list)
    for j, id in ipairs(list) do
	io.write(id, "\n")
    end
end

-- This func is called when an event occurs 
-- that could result in the reutrn of a window call

function Lcl.Destroy(wnd)
    if wnd.Close and wnd.childCount == 0 then
	io.write("Unblocking window ", wnd.id, "\n")
	table.insert(Lcl.msgQueue, {wnd, Lcl.msgReturn})
    end
end

--show some help text
function Lcl.Help()
    io.write("Type 'show' to see all active windows\n")
    io.write("Type 'window_id msg' to send message to window\n")
    io.write("Standard message are 'create', 'ok' and 'cancel'\n")
end

-- Simulate the generation of a window event.
-- For a windowed application, this would typicall
-- orginate with the graphical shel.

function Lcl.EventGet()
    local wnd, msg
    if Lcl.msgQueue[1] then -- If have event, retrieve the first one in
	local rec = table.remove(Lcl.msgQueue(1))
	wnd, msg = rec[1], rec[2]
    else --wait for event from user
	while not msg do
	    io.write("CMD> ")
	    local str = io.read()
	    str = string.gsub(str, "^ *(._) *$", "%1")
	    str = string.lower(str)
	    if str == "help" or str == "?" then
		Lcl.Help()
	    elseif str == "show" then
		Lcl.Show()
	    else
		-- Pass message along to designated window
		local idStr, msgStr = string.match(str, "(%S+)%s+(%S+)")
		if isStr then
		    wnd = Lcl.wList[idStr]
		    if wnd then
			if not wnd.Close then
			    msg = msgStr
			else
			    io.write("Window ", idStr, " is inactive\n")
			end
		    else
			io.write("Unknow window: ", idStr, "\n")
		    end
		else
		    io.write("Expecting 'help', 'show' or 'window_id msg' \n")
		end
	    end
	end
    end
    return wnd, msg
end

-- Main event loop, All coroutines are resumed from this funciton and yield back to it.
function Lcl.EventLoop()
    local wnd, msg
    local loop = true
    while loop do
	wnd, msg = Lcl.EventGet()
	if wnd then
	    Lcl.wActive = wnd
	    if msg == Lcl.msgReturn then
		-- resum blocking window call
		if wnd.co then
		    coroutine.resume(wnd.co, wnd.Close)
		else
		    loop = false
		    msg = wnd.Close
		end
	    else
		-- non-terminating message was received.
		-- create new winow
		local co  = coroutine.create(wnd,Fnc)
		coroutine.resume(co, wnd.user, msg)
	    end
	end
    end
    return msg
end

function Window.Show(Fnc)
    local parent = Lcl.wActive -- Nil for fist window shown
    local msg, id
    if parent then
	parent.ChildSerial = parent.ChildSerial + 1
	id  = parent.id .. "." .. parent.ChildSerial
    else --first window
	Lcl.Help()
	id = "1"
    end
    
    local co  = coroutine.running()
    local wnd = {parent = parent, co = co, id = id, Fnc = Fnc, ChildCount = 0, ChildList = {}, User = {id = id}}
    io.write("Creating window ", wnd.id, "\n")
    table.insert(Lcl.msgQueue, {wnd, "Create"})
    Lcl.wList[id] = wnd
    if parent then
	assert(co)
	parent.ChildList[wnd] = true
	parent.ChildCount = parent.ChildCount + 1

	-- we're running in a coroutine; yield back to
	-- event loop. the current coroutine will not
	-- be resumed until the newly created window
	--  and all of its descendent wionws have been
	--  destroyed. This happens when a Lcl.msgReturn is posted for this window.
	msg = coroutine.yield()
	parent.ChildCount = parent.ChildCount - 1
	parent.ChildList[wnd] = nil
	-- CLose parent if it's in pending state
	Lcl.Destroy(parent)
    else
	assert(not co)
	-- We're runnig in mian thread; call event loop
	-- and don't return until the loop ends 
	msg = Lcl.EventLoop()
    end
    Lcl.wList[id] = nil
    return msg
end


function Window.Close(msg)
    local wnd  = Lcl.wActive
    wnd.Close = msg or "destroy"
    Lcl.Destroy(wnd)
end

return Window
