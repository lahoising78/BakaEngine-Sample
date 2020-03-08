#include "baka_vectors.h"
#include <string>
#include <stdio.h>
#include <math.h>

float vec2_magnitude(Vec2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float vec3_magnitude(Vec3 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vec4_magnitude(Vec4 v)
{
    return std::sqrt( vec4_dot_product(v, v) );
}


void vec2_normalize(Vec2 &v)
{
    float m = vec2_magnitude(v);

    if(m == 0.0f)
        return;

    m = 1/m;
    v.x *= m;
    v.y *= m;
}

void vec3_normalize(Vec3 &v)
{
    float m = vec3_magnitude(v);

    if(m == 0.0f)
        return;

    m = 1/m;
    v.x *= m;
    v.y *= m;
    v.z *= m;
}

void vec4_normalize(Vec4 &v)
{
    float m = vec4_magnitude(v);

    if(m == 0.0f) return;

    m = 1/m;
    v.x *= m;
    v.y *= m;
    v.z *= m;
    v.w *= m;
}

Vec3 vec3_cross_product(const Vec3 a, const Vec3 b)
{
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

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