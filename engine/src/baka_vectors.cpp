#include "baka_vectors.h"
#include <string>
#include <stdio.h>

Vec2 operator+(const Vec2 &a, const Vec2 &b)
{
    return {a.x + b.x, a.y + b.y};
}

Vec3 operator+(const Vec3 &a, const Vec3 &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vec4 operator+(const Vec4 &a, const Vec4 &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

void operator+=(Vec2 &a, const Vec2 &b)
{
    a = a + b;
}
void operator+=(Vec3 &a, const Vec3 &b)
{
    a = a + b;
}
void operator+=(Vec4 &a, const Vec4 &b)
{
    a = a + b;
}

Vec2 operator-(const Vec2 &a, const Vec2 &b)
{
    return {a.x - b.x, a.y - b.y};
}

Vec3 operator-(const Vec3 &a, const Vec3 &b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec4 operator-(const Vec4 &a, const Vec4 &b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

void operator-=(Vec2 &a, const Vec2 &b)
{
    a = a - b;
}
void operator-=(Vec3 &a, const Vec3 &b)
{
    a = a - b;
}
void operator-=(Vec4 &a, const Vec4 &b)
{
    a = a - b;
}