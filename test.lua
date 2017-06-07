#!/usr/bin/env lua

--[[ My First lua program ! --]]
print("hello, world!")

io.write("Hello, World, from ",_VERSION,"!\n")

local lpeg = require("lpeg")
print(lpeg)

local a, b = 1, 2
c, d, e = 3, 4
print(a*b*c*d)
print(d," = 24")
print(e," = nil")
local f, g = 3 a = 9
print(f*a," = 27")
