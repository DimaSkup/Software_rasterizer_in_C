#ifndef VECTOR_H
#define VECTOR_H

// ==================================================================
// declare a new types to hold 2D and 3D vectors
// ==================================================================
typedef struct 
{
    float x, y;
} Vec2;

typedef struct 
{
    int x, y;
} Vec2Int;

typedef struct
{
    float x, y, z;
} Vec3;

typedef struct
{
    float x, y, z, w;
} Vec4;


// ==================================================================
// Vector 2D functions
// ==================================================================

void Vec2Init(Vec2* v, const float x, const float y);

// vec2 add/sub/mul
Vec2 Vec2Add(const Vec2 a, const Vec2 b); 
Vec2 Vec2Sub(const Vec2 a, const Vec2 b);
Vec2 Vec2Mul(const Vec2 v, const float scalar);
Vec2 Vec2Div(const Vec2 v, const float scalar);
float Vec2Dot(const Vec2 a, const Vec2 b);

float Vec2Length(const Vec2 v);
void Vec2Normalize(Vec2* v);


// ==================================================================
// Vector 3D functions
// ==================================================================

void Vec3Init(Vec3* v, const float x, const float y, const float z);

// vec3 add/sub/mul
Vec3 Vec3Add(const Vec3 a, const Vec3 b);
Vec3 Vec3Sub(const Vec3 a, const Vec3 b);
Vec3 Vec3Mul(const Vec3 v, const float scalar);
Vec3 Vec3Div(const Vec3 v, const float scalar);

Vec3 Vec3Cross(const Vec3 a, const Vec3 b);
float Vec3Dot(const Vec3 a, const Vec3 b);

float Vec3Length(const Vec3 v);
void Vec3Normalize(Vec3* v);

// vec3 rotation
void Vec3RotateX(Vec3* v, const float angle);
void Vec3RotateY(Vec3* v, const float angle);
void Vec3RotateZ(Vec3* v, const float angle);


// ==================================================================
// Vector conversion functions
// ==================================================================
Vec4 Vec4FromVec3(const Vec3* v);
Vec3 Vec3FromVec4(const Vec4* v);

#endif
