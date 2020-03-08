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

float vec2_magnitude(Vec2 v);
float vec3_magnitude(Vec3 v);
float vec4_magnitude(Vec4 v);

void vec2_normalize(Vec2 &v);
void vec3_normalize(Vec3 &v);
void vec4_normalize(Vec4 &v);

Vec3 vec3_cross_product(const Vec3 a, const Vec3 b);

#define vec2_dot_product(a, b) (a.x*b.x + a.y*b.y)
#define vec3_dot_product(a, b) ( vec2_dot_product(a,b) + a.z*b.z )
#define vec4_dot_product(a, b) ( vec3_dot_product(a,b) + a.w*b.w )

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