local Planet = {}
Planet.__index = Planet

function Planet.new(id, name, coords)
  return setmetatable({
    id = id,
    name = name,
    coords = coords or { x = 0, y = 0, z = 0 },
    temperature = 0,
    diameter = 0,
    population = 0,
    resources = { metal = 0, crystal = 0, deuterium = 0, energy = 0, darkMatter = 0 },
    buildings = {},
    fleetQueue = {},
  }, Planet)
end

function Planet:SetResource(name, value)
  self.resources[name] = value
end

function Planet:GetResource(name)
  return self.resources[name] or 0
end

function Planet:Update(dt)
  self.population = self.population + (dt * 0.05)
end

return Planet
