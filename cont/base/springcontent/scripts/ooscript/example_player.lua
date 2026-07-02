local OOScript = VFS.Include("scripts/ooscript/ooclass.lua")
local Runtime = VFS.Include("scripts/ooscript/runtime.lua")

local class = OOScript.class

local Component = class("Component")
function Component:init(name)
  self.name = name
end

local PlayerMovement = class("PlayerMovement", Component)
function PlayerMovement:init(speed)
  Component.init(self, "PlayerMovement")
  self.speed = speed or 6.0
  self.distance = 0
end

function PlayerMovement:onStart(ctx)
  if ctx and ctx.log then
    ctx.log("[OOScript] PlayerMovement start")
  end
end

function PlayerMovement:onUpdate(dt, ctx)
  self.distance = self.distance + (self.speed * dt)
  if ctx and ctx.log and ctx.tick and ctx.tick % 300 == 0 then
    ctx.log("[OOScript] moved distance=" .. tostring(self.distance))
  end
end

Runtime.registerSystem("PlayerMovement", PlayerMovement(7.5))

return {
  Runtime = Runtime,
  PlayerMovement = PlayerMovement
}
