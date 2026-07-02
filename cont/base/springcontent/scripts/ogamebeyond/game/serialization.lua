local Serialization = {}

function Serialization.Save(state)
  return {
    activeTab = state.activeTab,
    selectedPlanet = state.selectedPlanet and state.selectedPlanet.name or nil,
    notifications = state.notifications,
  }
end

function Serialization.Load(snapshot, state)
  if snapshot.activeTab then
    state.activeTab = snapshot.activeTab
  end
  if snapshot.notifications then
    state.notifications = snapshot.notifications
  end
end

return Serialization
