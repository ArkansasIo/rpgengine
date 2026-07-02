local BottomBar = {}
BottomBar.__index = BottomBar

function BottomBar.new()
  return setmetatable({ tabs = {} }, BottomBar)
end

function BottomBar:Init(state)
  self.state = state
  self.tabs = { "Buildings", "Ships", "Defense", "Structures", "Research" }
end

function BottomBar:Update(dt, state)
  self.state = state
end

function BottomBar:Draw(state)
  self.state = state
end

return BottomBar
