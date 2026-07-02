local Overview = {}
Overview.__index = Overview

function Overview.new()
  return setmetatable({ title = "Empire Overview" }, Overview)
end

function Overview:Init(state)
  self.state = state
end

function Overview:Update(dt, state)
  self.state = state
end

function Overview:Draw(state)
  self.state = state
end

return Overview
