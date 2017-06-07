#!/usr/bin/env lua

local lpeg = require("lpeg")

local word = lpeg.R("az")
local em = lpeg.P("//")

local good = "//emphasis//"
local bad = "missing //em"

local em_patt = em * word^1 * em

print("good = ", em_patt:match(good))
print("bad  = ", em_patt:match(bad))
