local Widgets = {}
Widgets.__index = Widgets

function Widgets.new()
  return setmetatable({ buttons = {}, panels = {} }, Widgets)
end

function Widgets:Init(state)
  self.state = state
end

function Widgets:Update(dt, state)
  self.state = state
end

function Widgets:Draw(state)
  self.state = state
end

return Widgets
