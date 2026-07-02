/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef ArcLight_LUA_ALLOC_STATE_H
#define ArcLight_LUA_ALLOC_STATE_H

#include <atomic>

struct SLuaAllocState {
	std::atomic<uint64_t> allocedBytes;
	std::atomic<uint64_t> numLuaAllocs;
	std::atomic<uint64_t> luaAllocTime;
	std::atomic<uint64_t> numLuaStates;
};

#endif

