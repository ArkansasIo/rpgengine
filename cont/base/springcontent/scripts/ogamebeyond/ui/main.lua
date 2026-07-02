local TopBar = require("ogamebeyond.ui.topbar")
local Sidebar = require("ogamebeyond.ui.sidebar")
local BottomBar = require("ogamebeyond.ui.bottombar")
local Overview = require("ogamebeyond.ui.overview")
local Galaxy = require("ogamebeyond.ui.galaxy")
local Fleet = require("ogamebeyond.ui.fleet")
local Widgets = require("ogamebeyond.ui.widgets.widgets")

local UI = {}
UI.__index = UI

function UI.new()
  return setmetatable({
    topBar = TopBar.new(),
    sidebar = Sidebar.new(),
    bottomBar = BottomBar.new(),
    overview = Overview.new(),
    galaxy = Galaxy.new(),
    fleet = Fleet.new(),
    widgets = Widgets.new(),
  }, UI)
end

function UI:Init(state)
  self.topBar:Init(state)
  self.sidebar:Init(state)
  self.bottomBar:Init(state)
  self.overview:Init(state)
  self.galaxy:Init(state)
  self.fleet:Init(state)
  self.widgets:Init(state)
end

function UI:Update(dt, state)
  self.topBar:Update(dt, state)
  self.sidebar:Update(dt, state)
  self.bottomBar:Update(dt, state)
  self.overview:Update(dt, state)
  self.galaxy:Update(dt, state)
  self.fleet:Update(dt, state)
  self.widgets:Update(dt, state)
end

function UI:Draw(state)
  self.topBar:Draw(state)
  self.sidebar:Draw(state)
  self.bottomBar:Draw(state)
  self.overview:Draw(state)
  self.galaxy:Draw(state)
  self.fleet:Draw(state)
  self.widgets:Draw(state)
end

return UI
