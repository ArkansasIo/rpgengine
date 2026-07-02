local EventBus = {}
EventBus.__index = EventBus

function EventBus.new()
  return setmetatable({ listeners = {} }, EventBus)
end

function EventBus:on(name, handler)
  local bucket = self.listeners[name]
  if not bucket then
    bucket = {}
    self.listeners[name] = bucket
  end
  bucket[#bucket + 1] = handler
end

function EventBus:emit(name, ...)
  local bucket = self.listeners[name]
  if not bucket then
    return
  end
  for i = 1, #bucket do
    bucket[i](...)
  end
end

return EventBus
