local Class = VFS.Include("rts/Gameplay/NewFeatures/ScriptingSystems/OOScript/ooclass.lua")
local Runtime = VFS.Include("rts/Gameplay/NewFeatures/ScriptingSystems/OOScript/runtime.lua")

local GameController = Class.class("GameController")

function GameController:init()
  self.elapsed = 0
end

function GameController:onUpdate(dt)
  self.elapsed = self.elapsed + dt
end

Runtime.registerSystem("GameController", GameController:new())

return Runtime
