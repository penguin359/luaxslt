#!/usr/bin/env lua

local xml = require('xml')

print("Hello, world!")
--local doc = xml.parse_xml("test.xml")
local doc = xml.parse_xml("<root><name>Hello</name></root>")
local status, error = pcall(function() print('Parse XML Document:', xml.dump_xml(doc)     ) end) if not status then print('Lua error: '..error) end
local status, error = pcall(function() print('Parse io.stdin:',     xml.dump_xml(io.stdin)) end) if not status then print('Lua error: '..error) end
local status, error = pcall(function() print('io.stdin:',           io.stdin              ) end) if not status then print('Lua error: '..error) end
local status, error = pcall(function() print('Parse empty string:', xml.dump_xml('')      ) end) if not status then print('Lua error: '..error) end
local status, error = pcall(function() print('Parse nil:',          xml.dump_xml(nil)     ) end) if not status then print('Lua error: '..error) end

local doc2 = xml.parse_xml_file('test.xml')
local style = xml.parse_xml_file('test.xsl')
local output = xml.transform_xml(doc2, style)
print('Transformed XML:', xml.dump_xml(output))
