#!/usr/bin/env lua

print("Hello, world!")
--local doc = xml.parse_xml("test.xml")
local doc = xml.parse_xml("<root><name>Hello</name></root>")
print(xml.dump_xml(doc))
