/* ArcLight Engine - Minimal types for ECS components.
 * Self-contained - no external dependencies.
 */

#pragma once

#include <cmath>
#include <cstdint>

namespace arclight {

struct float3 {
	float x = 0, y = 0, z = 0;
	float3() = default;
	float3(float x, float y, float z) : x(x), y(y), z(z) {}
	float3 operator+(const float3& o) const { return {x+o.x, y+o.y, z+o.z}; }
	float3 operator-(const float3& o) const { return {x-o.x, y-o.y, z-o.z}; }
	float3 operator*(float s) const { return {x*s, y*s, z*s}; }
	float3 operator/(float s) const { return {x/s, y/s, z/s}; }
	float3 operator-() const { return {-x, -y, -z}; }
	float3& operator+=(const float3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
	float3& operator-=(const float3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
	float3& operator*=(float s) { x*=s; y*=s; z*=s; return *this; }
	float Length() const { return sqrtf(x*x + y*y + z*z); }
	float3 Normalize() const { float l=Length(); return l>0 ? float3(x/l, y/l, z/l) : float3(); }
	float Dot(const float3& o) const { return x*o.x + y*o.y + z*o.z; }
	float3 Cross(const float3& o) const { return {y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x}; }
};

static const float3 ZeroVector(0, 0, 0);

struct float2 {
	float x = 0, y = 0;
	float2() = default;
	float2(float x, float y) : x(x), y(y) {}
	float2 operator+(const float2& o) const { return {x+o.x, y+o.y}; }
	float2 operator-(const float2& o) const { return {x-o.x, y-o.y}; }
	float2 operator*(float s) const { return {x*s, y*s}; }
	float Length() const { return sqrtf(x*x + y*y); }
};

struct Quaternion {
	float x = 0, y = 0, z = 0, w = 1;
	Quaternion() = default;
	Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) {}
	Quaternion(float3 axis, float angle) {
		float s = sinf(angle * 0.5f);
		x = axis.x * s; y = axis.y * s; z = axis.z * s; w = cosf(angle * 0.5f);
	}
	Quaternion operator*(const Quaternion& o) const {
		return Quaternion(
			w*o.x + x*o.w + y*o.z - z*o.y,
			w*o.y - x*o.z + y*o.w + z*o.x,
			w*o.z + x*o.y - y*o.x + z*o.w,
			w*o.w - x*o.x - y*o.y - z*o.z
		);
	}
	Quaternion Normalized() const {
		float l = sqrtf(x*x + y*y + z*z + w*w);
		return l > 0 ? Quaternion(x/l, y/l, z/l, w/l) : Quaternion();
	}
};

struct Matrix44f {
	float m[16] = {0};
	void LoadIdentity() {
		for (int i = 0; i < 16; i++) m[i] = 0;
		m[0] = m[5] = m[10] = m[15] = 1;
	}
	void Scale(float sx, float sy, float sz) { m[0]*=sx; m[5]*=sy; m[10]*=sz; }
	void SetPos(const float3& p) { m[12]=p.x; m[13]=p.y; m[14]=p.z; }
	Matrix44f operator*(const Matrix44f&) const { return {}; }
};

} // namespace arclight
