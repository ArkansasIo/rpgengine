# ArcLight Engine - Niagara-inspired Particle System
# Developer: Stephen
# UE5 Niagara-inspired GPU particle simulation

set(PARTICLE_HEADERS
	NiagaraSystem.h
)

set(PARTICLE_SOURCES
	# Header-only for now
)

source_group("Particle" FILES ${PARTICLE_HEADERS} ${PARTICLE_SOURCES})

list(APPEND engineHeaders ${PARTICLE_HEADERS})
