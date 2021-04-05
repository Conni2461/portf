local ffi = require("ffi")
local clib = ffi.load("libportf.so")

ffi.cdef[[
  int get_number();
]]

local m = {}

m.get_version = function()
    return clib.get_number()
end

return m
