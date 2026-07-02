local Universe = require("ogamebeyond.game.universe")

local GameState = {}
GameState.__index = GameState

function GameState.new()
  return setmetatable({
    universe = nil,
    selectedPlanet = nil,
    selectedFleet = nil,
    activeTab = "overview",
    notifications = {},
  }, GameState)
end

function GameState:Init(universe)
  self.universe = universe or Universe.new()
  if #self.universe.planets > 0 then
    self.selectedPlanet = self.universe.planets[1]
  end
end

function GameState:Update(dt, bus)
  self.universe:Update(dt)
  if bus then
    bus:emit("game:update", dt, self)
  end
end

function GameState:SetTab(tabName)
  self.activeTab = tabName
end

function GameState:AddNotification(text)
  self.notifications[#self.notifications + 1] = text
end

return GameState
