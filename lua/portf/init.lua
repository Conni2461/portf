local ffi = require("ffi")
local clib = ffi.load("libportf.so")

ffi.cdef[[
  typedef struct {
    const char *name;
    float value;
  } share_t;

  const char* get_version();
  const char* get_api_key();

  int get_share(share_t *out, int count);
]]

local m = {}

m.get_version = function()
  return ffi.string(clib.get_version())
end

m.get_share = function(shares)
  local count = #shares
  local s = ffi.new("share_t[?]", count)
  for i, v in ipairs(shares) do
    s[i - 1].name = v
    s[i - 1].value = -1
  end

  clib.get_share(s, count)

  local results = {}
  for i = 0, (count - 1) do
    results[i + 1] = { name = ffi.string(s[i].name), value = s[i].value }
  end

  return results
end

return m
