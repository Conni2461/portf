local ffi = require("ffi")
local clib = ffi.load("libportf.so")

ffi.cdef[[
  typedef struct {
    float change;
    float change_percent;
    float price;
    float time;
  } value_t;

  typedef struct {
    const char *symbol;
    const char *currency;
    const char *display_name;

    // fifty day stuff
    float fifty_day_average;
    float fifty_day_change;

    // pre
    // Are there no pre market information in the request response?
    /* value_t pre; */

    // regular
    value_t regular;

    float regular_high;
    float regular_low;
    float regular_open;
    float regular_prev_close;

    // post
    value_t post;

    // region
    // date
  } stock_t;

  const char *get_version();
  const char *get_api_key();

  void init_shares(stock_t *out, char** symbols, int count);
  char *stock_to_string(const stock_t *stock);
  int get_shares(stock_t *out, int count);
]]

local m = {}

m.get_version = function()
  return ffi.string(clib.get_version())
end

m.get_share = function(shares)
  local count = #shares
  local s = ffi.new("stock_t[?]", count)
  local names = {}
  for i, v in ipairs(shares) do
    local c_str = ffi.new("char[?]", #v + 1)
    ffi.copy(c_str, v)
    names[i] = c_str
  end
  local c_names = ffi.new("char *[?]", count, names)

  clib.init_shares(s, c_names, count)
  clib.get_shares(s, count)


  local results = {}
  for i = 0, (count - 1) do
    print(ffi.string(clib.stock_to_string(s[i])))
    results[i + 1] = {
      symbol = ffi.string(s[i].symbol),
      display_name = ffi.string(s[i].display_name),
      currency = ffi.string(s[i].currency),
      regular_price = s[i].regular.price,
      regular_change = s[i].regular.change,
      regular_change_percent = s[i].regular.change_percent
    }
  end

  return results
end

return m
