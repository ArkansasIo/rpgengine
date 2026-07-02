local Class = {}

function Class.class(name, base)
  local cls = {}
  cls.__name = name
  cls.__index = cls
  cls.__base = base

  if base then
    setmetatable(cls, { __index = base })
  end

  function cls:new(...)
    local instance = setmetatable({}, cls)
    if instance.init then
      instance:init(...)
    end
    return instance
  end

  return cls
end

return Class
