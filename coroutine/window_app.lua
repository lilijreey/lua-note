--------------------------------------------------------------------------------
--         FILE:  window_app.lua
--      VERSION:  1.0
--      CREATED:  05/17/2012 03:05:57 PM CST
--------------------------------------------------------------------------------

local Window = require("event-handle")

-- Sample window message handler

local function SampleWindow(wnd, msg)
    wnd.Serial = (wnd.Serial or 0) + 1
    io.write("Window ", wnd,id, ", mssage ",
              msg, ", serial ", wnd.Serial, "\n")
    if msg == "ok" or msg == "cancel" then
	io.write("Calling Window Close on ", wnd.id, "\n")
	Window.Close(msg)
	io.write("Called Window Close on ", wnd.id, "\n")
    elseif msg == "button" or msg == "new" then
	local time = os.date("%X")
	io.write("Calling Window.Show form ", wnd.id, " (", time, ")\n")
	local status = Window.Show(SampleWindow)
	io.write("Calling window.Show from ", wnd.id, ", child returned ", status, " (", time, ")\n")

    end
end

io.write("Application: starting\n")
local status = Window.Show(SampleWindow)
io.write("Window returned ", status, "\n")
io.write("Application: ending\n")
