local Runtime = {
  systems = {}
}

function Runtime.registerSystem(name, system)
  assert(type(name) == "string" and #name > 0, "system name is required")
  assert(type(system) == "table", "system must be a table")

  Runtime.systems[name] = system
end

function Runtime.start(ctx)
  for _, system in pairs(Runtime.systems) do
    if type(system.onStart) == "function" then
      system:onStart(ctx)
    end
  end
end

function Runtime.update(dt, ctx)
  for _, system in pairs(Runtime.systems) do
    if type(system.onUpdate) == "function" then
      system:onUpdate(dt, ctx)
    end
  end
end

function Runtime.shutdown(ctx)
  for _, system in pairs(Runtime.systems) do
    if type(system.onShutdown) == "function" then
      system:onShutdown(ctx)
    end
  end
end

return Runtime
