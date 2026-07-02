local Runtime = {
  systems = {},
}

function Runtime.registerSystem(name, system)
  Runtime.systems[name] = system
end

function Runtime.getSystem(name)
  return Runtime.systems[name]
end

function Runtime.start(context)
  Runtime.context = context
end

function Runtime.update(dt)
  for _, system in pairs(Runtime.systems) do
    if system and system.onUpdate then
      system:onUpdate(dt, Runtime.context)
    end
  end
end

return Runtime
