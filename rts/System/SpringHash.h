/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef _ArcLight_HASH_H_
#define _ArcLight_HASH_H_

#include "Sync/HsiehHash.h"
#include <string>

namespace ArcLight {
	template<typename T>
	struct synced_hash {
		std::uint32_t operator()(const T& s) const;
	};


	#define ArcLightDefaultHash(T)                          \
	template<>                                            \
	struct synced_hash<T>                                 \
	{                                                     \
		std::uint32_t operator()(const T& t) const {      \
			return static_cast<std::uint32_t>(t);         \
		}                                                 \
	};

	ArcLightDefaultHash(std::int8_t)
	ArcLightDefaultHash(std::uint8_t)
	ArcLightDefaultHash(std::int16_t)
	ArcLightDefaultHash(std::uint16_t)
	ArcLightDefaultHash(std::int32_t)
	ArcLightDefaultHash(std::uint32_t)
	#undef ArcLightDefaultHash

	template<>
	struct synced_hash<std::int64_t> {
	public:
		std::uint32_t operator()(const std::int64_t& i) const
		{
			return static_cast<std::uint32_t>(i) ^ static_cast<std::uint32_t>(i >> 32);
		}
	};

	template<>
	struct synced_hash<std::uint64_t> {
	public:
		std::uint32_t operator()(const std::int64_t& i) const
		{
			return static_cast<std::uint32_t>(i) ^ static_cast<std::uint32_t>(i >> 32);
		}
	};


	template<>
	struct synced_hash<std::string> {
	public:
		std::uint32_t operator()(const std::string& s) const
		{
			return HsiehHash(&s.data()[0], s.size(), 0);
		}
	};
}

namespace spring {
	template<typename T>
	using synced_hash = ArcLight::synced_hash<T>;
}

#endif //_ArcLight_HASH_H_
