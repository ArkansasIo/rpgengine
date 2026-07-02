local EventBus = require("ogamebeyond.engine.event")
local Timer = require("ogamebeyond.engine.timer")
local Universe = require("ogamebeyond.game.universe")
local GameState = require("ogamebeyond.game.state")
local UI = require("ogamebeyond.ui.main")

local Engine = {}
Engine.__index = Engine

function Engine.new()
  return setmetatable({
    bus = EventBus.new(),
    timer = Timer.new(),
    running = false,
    universe = Universe.new(),
    state = GameState.new(),
    ui = UI.new(),
  }, Engine)
end

function Engine:Init()
  self.universe:Init()
  self.state:Init(self.universe)
  self.ui:Init(self.state)
  self.running = true
end

function Engine:Update(dt)
  self.timer:update(dt)
  self.state:Update(dt, self.bus)
  self.ui:Update(dt, self.state)
end

function Engine:Draw()
  self.ui:Draw(self.state)
end

function Engine:Run()
  while self.running do
    local dt = 1 / 60
    self:Update(dt)
    self:Draw()
    self.running = false
  end
end

return Engine
