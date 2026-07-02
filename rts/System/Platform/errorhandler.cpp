/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

/**
 * @brief error messages
 * Error handling based on platform.
 */


#include "errorhandler.h"

#include <string>
#include <functional>

#include "System/SpringExitCode.h"
#include "System/Log/ILog.h"
#include "System/Log/LogSinkHandler.h"
#include "System/Threading/SpringThreading.h"

#if !defined(DEDICATED)
	#include "System/SpringApp.h"
	#include "System/Platform/Threading.h"
#endif
#if !defined(DEDICATED) && !defined(HEADLESS)
	#include "System/Platform/MessageBox.h"
#endif
#ifdef DEDICATED
	#include "Net/GameServer.h"
	#include "System/SafeUtil.h"
#endif


static void ExitArcLightProcessAux(bool waitForExit, bool exitSuccess)
{
	// wait a bit before forcing the kill
	if (waitForExit)
		ArcLight::this_thread::sleep_for(std::chrono::seconds(5));

	logSinkHandler.SetSinking(false);

#ifdef _MSC_VER
	if (!exitSuccess)
		TerminateProcess(GetCurrentProcess(), ArcLight::EXIT_CODE_CRASHED);
#endif

	exit(ArcLight::EXIT_CODE_CRASHED);
}


#ifdef DEDICATED
static void ExitArcLightProcess(const char* msg, const char* caption, unsigned int flags)
{
	LOG_L(L_ERROR, "[%s] errorMsg=\"%s\" msgCaption=\"%s\"", __func__, msg, caption);

	ArcLight::SafeDelete(gameServer);
	ExitArcLightProcessAux(false, true);
}

#else

static void ExitArcLightProcess(const char* msg, const char* caption, unsigned int flags)
{
	LOG_L(L_ERROR, "[%s] errorMsg=\"%s\" msgCaption=\"%s\" mainThread=%d", __func__, msg, caption, Threading::IsMainThread());

	switch (SpringApp::PostKill(Threading::Error(caption, msg, flags))) {
		case -1: {
			// main thread; either gets to ESPA first and cleans up our process or exit is forced by this
			std::function<void()> forcedExitFunc = [&]() { ExitArcLightProcessAux(true, false); };
			ArcLight::thread forcedExitThread = std::move(ArcLight::thread(forcedExitFunc));

			// .join can (very rarely) throw a no-such-process exception if it runs in parallel with exit
			assert(forcedExitThread.joinable());
			forcedExitThread.detach();

			SpringApp::Kill(false);
		} break;
		case 0: { assert(false); } break; // [unreachable] thread failed to post, ESPA
		case 1: {        return; } break; // thread posted successfully
	}

	ExitArcLightProcessAux(false, false);
}
#endif


void ErrorMessageBox(const char* msg, const char* caption, unsigned int flags)
{
	#if (!defined(DEDICATED) && !defined(HEADLESS))
	if (Threading::IsMainThread()) {
		// the thread that throws up this message-box will be blocked
		// until it is clicked away which can cause spurious detected
		// hangs, so deregister it here (by forwarding an empty error)
		// note: main also does the talking for other threads via Run
		SpringApp::PostKill({});
		Platform::MsgBox(msg, caption, flags);
	}
	#endif

	ExitArcLightProcess(msg, caption, flags);
}

