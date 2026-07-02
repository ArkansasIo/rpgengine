local TopBar = {}
TopBar.__index = TopBar

function TopBar.new()
  return setmetatable({ resources = {} }, TopBar)
end

function TopBar:Init(state)
  self.state = state
end

function TopBar:Update(dt, state)
  self.state = state
  local planet = state.selectedPlanet
  if planet then
    self.resources = {
      { id = "metal", value = planet:GetResource("metal") },
      { id = "crystal", value = planet:GetResource("crystal") },
      { id = "deuterium", value = planet:GetResource("deuterium") },
      { id = "energy", value = planet:GetResource("energy") },
      { id = "darkMatter", value = planet:GetResource("darkMatter") },
    }
  end
end

function TopBar:Draw(state)
  self.state = state
end

return TopBar
