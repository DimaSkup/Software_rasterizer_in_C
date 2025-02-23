#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

// ==================================================================
// Typedefs
// ==================================================================

// stores vertex index
typedef struct 
{
    int a;
    int b;
    int c;
    Tex2 aUV;
    Tex2 bUV;
    Tex2 cUV;
    uint32_t color;
} Face;

// stores the actual Vec2 points of the triangle in the screen
typedef struct 
{
    Vec4 points[3];
    Tex2 texCoords[3];
    uint32_t color;
    float avgDepth;     // average depth of 3 vertices
} Triangle;


// ==================================================================
// Functions declarations
// ==================================================================
void DrawFilledTriangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    const uint32_t color);

// sort triangles by average depth
void SortTriangles(
    Triangle* arr, 
    const int numTriangles, 
    const int low, 
    const int high);


void DrawTexel(
    const int x, const int y, 
    const uint32_t* texture,
    const Vec4 pointA,
    const Vec4 pointB,
    const Vec4 pointC,
    const Tex2 tex0,
    const Tex2 tex1,
    const Tex2 tex2);


void DrawTexturedTriangle(
    int x0, int y0, float z0, float w0,     // vec4: xyzw of a projected triangle
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    float u0, float v0,                     // uv texture coords of the 1st triangle vertex    
    float u1, float v1,                     // ... of the 2nd triangle vertex
    float u2, float v2,                     // ... and of the 3rd triangle vertex
    const uint32_t* texture);

#endif
