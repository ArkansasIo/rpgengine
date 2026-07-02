local Research = {}
Research.__index = Research

function Research.new()
  return setmetatable({
    queue = {},
    completed = {},
    points = {
      energy = 0,
      laser = 0,
      ion = 0,
      plasma = 0,
      computer = 0,
      logistics = 0,
      espionage = 0,
      astronomy = 0,
      astrophysics = 0,
    },
  }, Research)
end

function Research:AddProject(id, name, cost)
  self.queue[#self.queue + 1] = { id = id, name = name, cost = cost, progress = 0 }
end

function Research:Update(dt)
  if #self.queue == 0 then
    return
  end
  local project = self.queue[1]
  project.progress = project.progress + dt
  if project.progress >= project.cost then
    self.completed[project.id] = true
    table.remove(self.queue, 1)
  end
end

return Research
