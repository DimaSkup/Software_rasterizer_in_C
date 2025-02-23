#include "vector.h"
#include <math.h>


// ==================================================================
// Implementations Vector 2D functions
// ==================================================================

void Vec2Init(Vec2* v, const float x, const float y)
{
    v->x = x;
    v->y = y;
}

void Vec2MulScalar(Vec2* v, const float scalar)
{
    v->x *= scalar;
    v->y *= scalar;
}

Vec2 Vec2Add(const Vec2 a, const Vec2 b)
{
    Vec2 vec = { a.x+b.x, a.y+b.y };
    return vec;
}

Vec2 Vec2Sub(const Vec2 a, const Vec2 b)
{
    Vec2 vec = { a.x-b.x, a.y-b.y };
    return vec;
}

Vec2 Vec2Mul(const Vec2 v, const float scalar)
{
    Vec2 vec = { v.x * scalar, v.y * scalar };
    return vec;
}

Vec2 Vec2Div(const Vec2 v, const float scalar)
{
    float invScalar = 1.0f / scalar;
    Vec2 vec = { v.x * invScalar, v.y * invScalar };
    return vec;
}

///////////////////////////////////////////////////////////

float Vec2Length(const Vec2 v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

void Vec2Normalize(Vec2* v)
{
    const float invLen = 1.0f / sqrt((v->x * v->x) + (v->y * v->y));
    
    v->x *= invLen;
    v->y *= invLen;     
}

///////////////////////////////////////////////////////////

float Vec2Dot(const Vec2 a, const Vec2 b)
{
    return (a.x*b.x) + (a.y*b.y);
}


// ==================================================================
// Implementations Vector 3D functions
// ==================================================================

void Vec3Init(Vec3* v, const float x, const float y, const float z)
{
    v->x = x; 
    v->y = y;
    v->z = z;
}

///////////////////////////////////////////////////////////

void Vec3AddScalar(Vec3* v, const float scalar)
{
    v->x += scalar;
    v->y += scalar;
    v->z += scalar;
}

///////////////////////////////////////////////////////////

void Vec3RotateX(Vec3* v, const float angle)
{
    // rotate the input vector around X-axis by angle
    // (x value remains the same)
    const float vy = v->y;
    const float vz = v->z;

    v->y = (vy * cosf(angle)) - (vz * sinf(angle));
    v->z = (vy * sinf(angle)) + (vz * cosf(angle));
}

///////////////////////////////////////////////////////////

void Vec3RotateY(Vec3* v, float angle)
{
    // rotate the input vector around Y-axis by angle
    // (y value remains the same)
    const float vx = v->x;
    const float vz = v->z;

    v->x = (vx * cosf(angle)) - (vz * sinf(angle));
    v->z = (vx * sinf(angle)) + (vz * cosf(angle));
}

///////////////////////////////////////////////////////////

void Vec3RotateZ(Vec3* v, const float angle)
{
    // rotate the input vector around z-axis by angle
    // (z value remains the same)
    const float vx = v->x;
    const float vy = v->y;

    v->x = (vx * cosf(angle)) - (vy * sinf(angle));
    v->y = (vx * sinf(angle)) + (vy * cosf(angle));
}

///////////////////////////////////////////////////////////

Vec3 Vec3Add(const Vec3 a, const Vec3 b)
{ 
    Vec3 vec = { a.x+b.x, a.y+b.y, a.z+b.z };
    return vec;
}

Vec3 Vec3Sub(const Vec3 a, const Vec3 b)
{
    Vec3 vec = { a.x-b.x, a.y-b.y, a.z-b.z };
    return vec;
}

Vec3 Vec3Mul(const Vec3 v, const float scalar)
{
    Vec3 vec = { v.x * scalar, v.y * scalar, v.z * scalar };
    return vec;
}
Vec3 Vec3Div(const Vec3 v, const float scalar)
{
    const float invScalar = 1.0f / scalar;
    Vec3 vec = { v.x * invScalar, v.y * invScalar, v.z * invScalar };
    return vec;
}

///////////////////////////////////////////////////////////

Vec3 Vec3Cross(const Vec3 a, const Vec3 b)
{
    Vec3 v = 
    {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x,
    };
    return v;
}

float Vec3Dot(const Vec3 a, const Vec3 b)
{
    return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
}

///////////////////////////////////////////////////////////

float Vec3Length(const Vec3 v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void Vec3Normalize(Vec3* v)
{
    const float invLen = 1.0f / sqrt((v->x*v->x) + (v->y*v->y) + (v->z*v->z));
    
    v->x *= invLen;
    v->y *= invLen;     
    v->z *= invLen;
}



// ==================================================================
// Implementations of vector conversion functions
// ==================================================================
Vec4 Vec4FromVec3(const Vec3* v)
{
    Vec4 vec = { v->x, v->y, v->z, 1.0f };
    return vec;
}

Vec3 Vec3FromVec4(const Vec4* v)
{
    Vec3 vec = { v->x, v->y, v->z };
    return vec;
}


