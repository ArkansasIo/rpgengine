local Fleet = {}
Fleet.__index = Fleet

function Fleet.new(id, owner)
  return setmetatable({
    id = id,
    owner = owner,
    ships = {},
    mission = "idle",
    origin = nil,
    destination = nil,
    eta = 0,
  }, Fleet)
end

function Fleet:AddShip(shipName, count)
  self.ships[shipName] = (self.ships[shipName] or 0) + count
end

function Fleet:SetMission(mission, origin, destination, eta)
  self.mission = mission
  self.origin = origin
  self.destination = destination
  self.eta = eta or 0
end

function Fleet:Update(dt)
  if self.eta > 0 then
    self.eta = math.max(0, self.eta - dt)
  end
end

return Fleet
