local portf = require('portf')
local eq = assert.are.same

describe("portf", function()
  it("can get the version", function()
    eq("0.1", portf.get_version())
  end)

  -- it("can get shares", function()
  --   local shares = portf.get_share({ "AMC", "AAPL" })
  --   eq("AMC", shares[1].name)
  --   eq("AAPL", shares[2].name)

  --   assert.is.True(shares[1].value > -1)
  --   assert.is.True(shares[2].value > -1)
  -- end)
end)
