/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#include "ExplosionListener.h"
#include "System/ContainerUtil.h"


std::vector<IExplosionListener*> CExplosionCreator::explosionListeners;


IExplosionListener::~IExplosionListener()
{
	CExplosionCreator::RemoveExplosionListener(this);
}

void CExplosionCreator::AddExplosionListener(IExplosionListener* listener)
{
	ArcLight::VectorInsertUnique(explosionListeners, listener, true);
}

void CExplosionCreator::RemoveExplosionListener(IExplosionListener* listener)
{
	ArcLight::VectorErase(explosionListeners, listener);
}

void CExplosionCreator::FireExplosionEvent(const CExplosionParams& event)
{
	for (auto& expList: explosionListeners) {
		expList->ExplosionOccurred(event);
	}
}

