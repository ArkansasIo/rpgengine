local Sidebar = {}
Sidebar.__index = Sidebar

function Sidebar.new()
  return setmetatable({ entries = {} }, Sidebar)
end

function Sidebar:Init(state)
  self.state = state
  self.entries = {
    "Overview",
    "Messages",
    "Events",
    "Technology",
    "Shipyard",
    "Defense",
    "Fleet",
    "Galaxy",
    "Alliance",
    "Market",
  }
end

function Sidebar:Update(dt, state)
  self.state = state
end

function Sidebar:Draw(state)
  self.state = state
end

return Sidebar
