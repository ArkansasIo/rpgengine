local Plugin = {
  name = "SampleGameplayPlugin"
}

function Plugin:onLoad(ctx)
  if ctx and ctx.log then
    ctx.log("[SampleGameplayPlugin] loaded")
  end
end

function Plugin:onUpdate(dt, ctx)
  if ctx and ctx.tick and ctx.tick % 600 == 0 and ctx.log then
    ctx.log("[SampleGameplayPlugin] heartbeat dt=" .. tostring(dt))
  end
end

function Plugin:onUnload(ctx)
  if ctx and ctx.log then
    ctx.log("[SampleGameplayPlugin] unloaded")
  end
end

return Plugin
