/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace ArcLight {
	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable(const noncopyable&);
		const noncopyable& operator=(const noncopyable&);
	};
}

#endif // NONCOPYABLE_H
