/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#include "LuaSky.h"

#include "Rendering/GlobalRendering.h"
#include "System/EventHandler.h"

void CLuaSky::Draw(Game::DrawMode mode)
{
	if (!globalRendering->drawSky)
		return;

	eventHandler.DrawSky();
}

void CLuaSky::DrawSun(Game::DrawMode mode)
{
	if (!globalRendering->drawSky)
		return;

	eventHandler.DrawSun();
}

