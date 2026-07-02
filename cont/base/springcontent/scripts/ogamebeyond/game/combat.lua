local Combat = {}
Combat.__index = Combat

function Combat.new()
  return setmetatable({
    battles = {},
  }, Combat)
end

function Combat:StartBattle(attacker, defender)
  self.battles[#self.battles + 1] = {
    attacker = attacker,
    defender = defender,
    time = 0,
    finished = false,
  }
end

function Combat:Update(dt)
  for _, battle in ipairs(self.battles) do
    if not battle.finished then
      battle.time = battle.time + dt
      if battle.time > 3.0 then
        battle.finished = true
      end
    end
  end
end

return Combat
