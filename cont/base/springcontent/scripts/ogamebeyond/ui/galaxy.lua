local Galaxy = {}
Galaxy.__index = Galaxy

function Galaxy.new()
  return setmetatable({ systems = {} }, Galaxy)
end

function Galaxy:Init(state)
  self.state = state
end

function Galaxy:GenerateSystem(id)
  return {
    id = id,
    name = "System " .. id,
    x = id * 1000,
    y = id * 500,
    planets = {},
  }
end

function Galaxy:Update(dt, state)
  self.state = state
end

function Galaxy:Draw(state)
  self.state = state
end

return Galaxy
