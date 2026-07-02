local function class(name, base)
  local cls = {}
  cls.__name = name or "AnonymousClass"
  cls.__index = cls
  cls.__base = base

  setmetatable(cls, {
    __index = base,
    __call = function(c, ...)
      local instance = setmetatable({}, c)
      if instance.init then
        instance:init(...)
      end
      return instance
    end
  })

  function cls:isA(typeName)
    local cursor = cls
    while cursor do
      if cursor.__name == typeName then
        return true
      end
      cursor = cursor.__base
    end
    return false
  end

  return cls
end

return {
  class = class
}
