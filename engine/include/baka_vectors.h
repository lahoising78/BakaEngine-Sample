#ifndef _VECTORS_H_
#define _VECTORS_H_

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    float x, y, z;
} Vec3;

typedef struct
{
    float x, y, z, w;
} Vec4;

Vec2 operator+(const Vec2 &a, const Vec2 &b);
Vec3 operator+(const Vec3 &a, const Vec3 &b);
Vec4 operator+(const Vec4 &a, const Vec4 &b);

void operator+=(Vec2 &a, const Vec2 &b);
void operator+=(Vec3 &a, const Vec3 &b);
void operator+=(Vec4 &a, const Vec4 &b);

Vec2 operator-(const Vec2 &a, const Vec2 &b);
Vec3 operator-(const Vec3 &a, const Vec3 &b);
Vec4 operator-(const Vec4 &a, const Vec4 &b);

void operator-=(Vec2 &a, const Vec2 &b);
void operator-=(Vec3 &a, const Vec3 &b);
void operator-=(Vec4 &a, const Vec4 &b);

#endif