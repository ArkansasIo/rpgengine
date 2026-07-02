/* ArkLight Beyond - Core Types
 * Developer: Stephen
 * Fundamental math and utility types.
 */
#pragma once
#include <cmath>
#include <cstdint>
#include <string>

namespace ogb {
struct Vec3 {
    float x=0, y=0, z=0;
    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
    Vec3 operator/(float s) const { return {x/s, y/s, z/s}; }
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
    Vec3& operator*=(float s) { x*=s; y*=s; z*=s; return *this; }
    float Length() const { return sqrtf(x*x + y*y + z*z); }
    Vec3 Normalized() const { float l=Length(); return l>0?Vec3(x/l,y/l,z/l):Vec3(); }
    float Dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    Vec3 Cross(const Vec3& o) const { return {y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x}; }
};
static const Vec3 VEC3_ZERO(0,0,0);

struct Color {
    float r=1, g=1, b=1, a=1;
    Color() = default;
    Color(float r, float g, float b, float a=1) : r(r), g(g), b(b), a(a) {}
    static Color White() { return {1,1,1}; }
    static Color Black() { return {0,0,0}; }
    static Color Red() { return {1,0,0}; }
    static Color Green() { return {0,1,0}; }
    static Color Blue() { return {0,0,1}; }
    static Color Yellow() { return {1,1,0}; }
    static Color Cyan() { return {0,1,1}; }
};

struct Rect { float x=0, y=0, w=0, h=0; bool Contains(float px, float py) const { return px>=x && px<=x+w && py>=y && py<=y+h; } };

using Tick = uint64_t;
constexpr Tick TICKS_PER_SECOND = 60;
constexpr float DT = 1.0f / TICKS_PER_SECOND;
} // namespace ogb
