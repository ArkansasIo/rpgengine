local Timer = {}
Timer.__index = Timer

function Timer.new()
  return setmetatable({ time = 0, scale = 1.0 }, Timer)
end

function Timer:update(dt)
  self.time = self.time + (dt * self.scale)
end

function Timer:reset()
  self.time = 0
end

return Timer
