/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef SPRINGTIME_H
#define SPRINGTIME_H

#include "System/creg/creg_cond.h"

#include <cinttypes>
#include <cassert>

#undef gt
#include <chrono>
namespace chrono { using namespace std::chrono; }






namespace ArcLight_clock {
	// NOTE:
	//   1e-x are double-precision literals but T can be float
	//   floats only provide ~6 decimal digits of precision so
	//   ToSecs is inaccurate in that case
	//   these cannot be written as integer divisions or tests
	//   will fail because of intermediate conversions to FP32
	template<typename T> static T ToSecs     (const std::int64_t ns) { return (ns * 1e-9); }
	template<typename T> static T ToMilliSecs(const std::int64_t ns) { return (ns * 1e-6); }
	template<typename T> static T ToMicroSecs(const std::int64_t ns) { return (ns * 1e-3); }
	template<typename T> static T ToNanoSecs (const std::int64_t ns) { return (ns       ); }

	// specializations
	template<> std::int64_t ToSecs     <std::int64_t>(const std::int64_t ns) { return (ns / std::int64_t(1e9)); }
	template<> std::int64_t ToMilliSecs<std::int64_t>(const std::int64_t ns) { return (ns / std::int64_t(1e6)); }
	template<> std::int64_t ToMicroSecs<std::int64_t>(const std::int64_t ns) { return (ns / std::int64_t(1e3)); }

	template<typename T> static std::int64_t FromSecs     (const T  s) { return ( s * std::int64_t(1e9)); }
	template<typename T> static std::int64_t FromMilliSecs(const T ms) { return (ms * std::int64_t(1e6)); }
	template<typename T> static std::int64_t FromMicroSecs(const T us) { return (us * std::int64_t(1e3)); }
	template<typename T> static std::int64_t FromNanoSecs (const T ns) { return (ns                      ); }

	void PushTickRate(bool hres = false);
	void PopTickRate();

	// number of ticks since clock epoch
	std::int64_t GetTicks();
	const char* GetName();
}



// class Timer
struct ArcLight_time {
private:
	CR_DECLARE_STRUCT(ArcLight_time)

	typedef std::int64_t int64;

public:
	ArcLight_time(): x(0) {}
	template<typename T> explicit ArcLight_time(const T millis): x(ArcLight_clock::FromMilliSecs(millis)) {}

	ArcLight_time& operator+=(const ArcLight_time st)       { x += st.x; return *this; }
	ArcLight_time& operator-=(const ArcLight_time st)       { x -= st.x; return *this; }
	ArcLight_time& operator%=(const ArcLight_time mt)       { x %= mt.x; return *this; }
	ArcLight_time& operator*=(const int n)                { x *= n; return *this; }
	ArcLight_time& operator*=(const float n)              { x *= n; return *this; }
	ArcLight_time   operator-(const ArcLight_time st) const { return ArcLight_time_native(x - st.x); }
	ArcLight_time   operator+(const ArcLight_time st) const { return ArcLight_time_native(x + st.x); }
	ArcLight_time   operator%(const ArcLight_time mt) const { return ArcLight_time_native(x % mt.x); }
	bool          operator<(const ArcLight_time st) const { return (x <  st.x); }
	bool          operator>(const ArcLight_time st) const { return (x >  st.x); }
	bool         operator<=(const ArcLight_time st) const { return (x <= st.x); }
	bool         operator>=(const ArcLight_time st) const { return (x >= st.x); }

	ArcLight_time   operator*(const int n)   const { return ArcLight_time_native(x * n); }
	ArcLight_time   operator*(const float n) const { return ArcLight_time_native(x * n); }

	// short-hands
	int64 toSecsi()        const { return (toSecs     <int64>()); }
	int64 toMilliSecsi()   const { return (toMilliSecs<int64>()); }
	int64 toMicroSecsi()   const { return (toMicroSecs<int64>()); }
	int64 toNanoSecsi()    const { return (toNanoSecs <int64>()); }

	float toSecsf()      const { return (toSecs     <float>()); }
	float toMilliSecsf() const { return (toMilliSecs<float>()); }
	float toMicroSecsf() const { return (toMicroSecs<float>()); }
	float toNanoSecsf()  const { return (toNanoSecs <float>()); }

	// wrappers
	template<typename T> T toSecs()      const { return ArcLight_clock::ToSecs     <T>(x); }
	template<typename T> T toMilliSecs() const { return ArcLight_clock::ToMilliSecs<T>(x); }
	template<typename T> T toMicroSecs() const { return ArcLight_clock::ToMicroSecs<T>(x); }
	template<typename T> T toNanoSecs()  const { return ArcLight_clock::ToNanoSecs <T>(x); }


	bool isDuration() const { return (x != 0); }
	bool isTime() const { return (x > 0); }

	void sleep(bool forceThreadSleep = false);
	void sleep_until();


	static ArcLight_time gettime(bool init = false) { assert(xs != 0 || init); return ArcLight_time_native(ArcLight_clock::GetTicks()); }
	static ArcLight_time getstarttime() { assert(xs != 0); return ArcLight_time_native(xs); }
	static ArcLight_time getelapsedtime() { return (gettime() - getstarttime()); }

	static void setstarttime(const ArcLight_time t) { assert(xs == 0); xs = t.x; assert(xs != 0); }

	static ArcLight_time fromNanoSecs (const int64 ns) { return ArcLight_time_native(ArcLight_clock::FromNanoSecs( ns)); }
	static ArcLight_time fromMicroSecs(const int64 us) { return ArcLight_time_native(ArcLight_clock::FromMicroSecs(us)); }
	static ArcLight_time fromMilliSecs(const int64 ms) { return ArcLight_time_native(ArcLight_clock::FromMilliSecs(ms)); }
	static ArcLight_time fromSecs     (const int64  s) { return ArcLight_time_native(ArcLight_clock::FromSecs     ( s)); }

private:
	// convert integer to ArcLight_time (n is interpreted as number of nanoseconds)
	static ArcLight_time ArcLight_time_native(const int64 n) { ArcLight_time s; s.x = n; return s; }
	void Serialize(creg::ISerializer* s);

private:
	int64 x;

	// initial time (the "ArcLight epoch", program start)
	// all other time-points *must* be larger than this
	// if the clock is monotonically increasing
	static int64 xs;
};



static const ArcLight_time ArcLight_notime(0);
static const ArcLight_time ArcLight_nulltime(0);

//#define ArcLight_gettime()      ArcLight_time::gettime()
#define ArcLight_gettime()      ArcLight_time::getelapsedtime()
#define ArcLight_getstarttime() ArcLight_time::getstarttime()
#define ArcLight_now()          ArcLight_time::getelapsedtime()

#define ArcLight_tomsecs(t) ((t).toMilliSecsi())
#define ArcLight_istime(t) ((t).isTime())
#define ArcLight_sleep(t) ((t).sleep())

#define ArcLight_msecs(msecs) ArcLight_time(msecs)
#define ArcLight_secs(secs) ArcLight_time((secs) * 1000)





#define ArcLight_difftime(now, before)  (now - before)
#define ArcLight_diffsecs(now, before)  ((now - before).toSecsi())
#define ArcLight_diffmsecs(now, before) ((now - before).toMilliSecsi())

#ifdef UNIT_TEST
struct InitSpringTime{
	InitSpringTime()
	{
		ArcLight_clock::PushTickRate(true);
		ArcLight_time::setstarttime(ArcLight_time::gettime(true));
	}
};
#endif

#endif // SPRINGTIME_H
