--------------------------------------------------------------------------------
--         FILE:  status.lua
--      VERSION:  1.0
--      CREATED:  05/16/2012 02:42:27 PM CST
--------------------------------------------------------------------------------
local A, B, C
local function Status(Str)
    io.write(string.format("%-8s A is %-10s C is %-10s",
			   Str, coroutine.status(A), coroutine.status(C)))
    local recRun , bb = coroutine.running()
    io.write(bb and "main one\n" or "other\n")
end

function A()
    Status("A")
end
function B()
    Status("B")
end

function C()
    Status("C 1")
    coroutine.resume(A) --start running A
    Status("C 2")
    B() --start running B
    Status("C 3")
    coroutine.yield()
    Status("C 4")
end

A = coroutine.create(A)
print("a create")
B = coroutine.wrap(B)
print("b warp")
C = coroutine.create(C)
print("c create")
Status("Main 1")
coroutine.resume(A)
Status("Main 2")

