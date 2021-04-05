local portf = require('portf')
local eq = assert.are.same

describe("portf", function()
  it("can get the version", function()
    eq(2, portf.get_version())
  end)
end)
