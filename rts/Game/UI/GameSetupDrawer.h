/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef GAME_SETUP_DRAWER
#define GAME_SETUP_DRAWER

#include "InputReceiver.h"
#include "System/Misc/SpringTime.h"


class GameSetupDrawer : public CInputReceiver
{
public:
	static void Enable();
	static void Disable();

	static void StartCountdown(unsigned time);

private:
	GameSetupDrawer();
	~GameSetupDrawer();

	virtual void Draw() override;

	static GameSetupDrawer* instance;

	ArcLight_time readyCountdown;
	ArcLight_time lastTick;
};


#endif // GAME_SETUP_DRAWER
