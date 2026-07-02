local Planet = require("ogamebeyond.game.planet")
local Fleet = require("ogamebeyond.game.fleet")
local Economy = require("ogamebeyond.game.economy")
local Research = require("ogamebeyond.game.research")
local Combat = require("ogamebeyond.game.combat")

local Universe = {}
Universe.__index = Universe

function Universe.new()
  return setmetatable({
    galaxies = {},
    players = {},
    planets = {},
    fleets = {},
    economy = Economy.new(),
    research = Research.new(),
    combat = Combat.new(),
  }, Universe)
end

function Universe:Init()
  local homeworld = Planet.new(1, "Terra Prime", { x = 1, y = 72, z = 9 })
  homeworld.temperature = 15
  homeworld.diameter = 12742
  homeworld.population = 12547812
  homeworld:SetResource("metal", 36200000)
  homeworld:SetResource("crystal", 18700000)
  homeworld:SetResource("deuterium", 9400000)
  homeworld:SetResource("energy", 2800000)
  homeworld:SetResource("darkMatter", 124500)
  self.planets[#self.planets + 1] = homeworld

  local fleet = Fleet.new(1, "NovaPrime")
  fleet:AddShip("Battleship", 20)
  fleet:AddShip("Destroyer", 30)
  fleet:AddShip("Recycler", 20)
  self.fleets[#self.fleets + 1] = fleet

  self.economy:SetIncome("metal", 125000)
  self.economy:SetIncome("crystal", 98000)
  self.economy:SetIncome("deuterium", 73000)
  self.economy:SetIncome("energy", 0)
  self.economy:SetStorage("metal", 40000000)
  self.economy:SetStorage("crystal", 40000000)
  self.economy:SetStorage("deuterium", 20000000)
  self.economy:SetStorage("energy", 12450000)

  self.research:AddProject("energy", "Energy Technology", 4.2)
  self.research:AddProject("hyperspace", "Hyperspace Drive", 12.0)
end

function Universe:Update(dt)
  self.economy:Update(dt)
  self.research:Update(dt)
  self.combat:Update(dt)
  for _, planet in ipairs(self.planets) do
    planet:Update(dt)
  end
  for _, fleet in ipairs(self.fleets) do
    fleet:Update(dt)
  end
end

return Universe
