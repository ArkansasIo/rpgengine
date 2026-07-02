local FleetPanel = {}
FleetPanel.__index = FleetPanel

function FleetPanel.new()
  return setmetatable({ ships = {} }, FleetPanel)
end

function FleetPanel:Init(state)
  self.state = state
end

function FleetPanel:Update(dt, state)
  self.state = state
end

function FleetPanel:Draw(state)
  self.state = state
end

return FleetPanel
