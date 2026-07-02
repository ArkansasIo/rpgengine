local Economy = {}
Economy.__index = Economy

function Economy.new()
  return setmetatable({
    resources = {
      metal = 0,
      crystal = 0,
      deuterium = 0,
      energy = 0,
      darkMatter = 0,
    },
    income = {
      metal = 0,
      crystal = 0,
      deuterium = 0,
      energy = 0,
      darkMatter = 0,
    },
    storage = {
      metal = 0,
      crystal = 0,
      deuterium = 0,
      energy = 0,
      darkMatter = 0,
    },
  }, Economy)
end

function Economy:Update(dt)
  self.resources.metal = math.min(self.storage.metal, self.resources.metal + (self.income.metal * dt))
  self.resources.crystal = math.min(self.storage.crystal, self.resources.crystal + (self.income.crystal * dt))
  self.resources.deuterium = math.min(self.storage.deuterium, self.resources.deuterium + (self.income.deuterium * dt))
  self.resources.energy = math.min(self.storage.energy, self.resources.energy + (self.income.energy * dt))
end

function Economy:SetIncome(name, value)
  self.income[name] = value
end

function Economy:SetStorage(name, value)
  self.storage[name] = value
end

return Economy
