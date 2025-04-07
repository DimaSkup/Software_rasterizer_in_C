// ==================================================================
// Filename:    render.h
// Description: functional for rendering:
//              1. rendering of triangles filled with solid color
//              2. rendering of triangles which are textured
//
// Create:      05.04.2025 by DimaSkup              
// ==================================================================
#ifndef RENDER_H
#define RENDER_H

#include "common_types.h"
#include "vector.h"
#include "geomtypes.h"
#include <stdint.h>

// ==================================================================
// Functions declarations
// ==================================================================
void DrawPixel     (int x, int y, Color color);
void DrawPixelByIdx(const int pixelIdx, Color color);
void DrawLine      (int x0, int y0, int x1, int y1, Color color);
void DrawLine2     (int x0, int y0, int x1, int y1, Color color);
void DrawTriangle  (int x0, int y0, int x1, int y1, int x2, int y2, Color color);
void DrawGrid      (void);
void DrawRect      (int x, int y, int width, int height, Color color);
void DrawCircle    (int x, int y, int radius, Color color);

// ----------------------------------------------

void DrawFilledTriangle(
    int x0, int y0, float w0,
    int x1, int y1, float w1,
    int x2, int y2, float w2,
    const uint32_t color);

// ----------------------------------------------

void DrawTexturedLine(
    const Vec2Int a,            // point A
    const Vec2Int b,            // point B
    const Vec2Int c,            // point C
    const Vec2Int ac,
    const Vec2 texA,            // texture coords for point A
    const Vec2 texB,
    const Vec2 texC,
    const float invArea,
    const float w0,             // w-component of point A
    const float w1,             // w-component of point B
    const float w2,             // w-component of point C
    const float lightIntensity,
    const int xStart,
    const int xEnd,
    const int y,
    const int textureWidth,
    const int textureHeight,
    const uint32_t* textureBuffer);

// ----------------------------------------------

void RenderTexturedTriangle(
    int x0, int y0, float z0, float w0,     // vec4: xyzw of a projected triangle
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    float u0, float v0,                     // uv texture coords of the 1st triangle vertex    
    float u1, float v1,                     // ... of the 2nd triangle vertex
    float u2, float v2,                     // ... and of the 3rd triangle vertex
    float lightIntensity,                                            
    const int textureWidth,
    const int textureHeight,
    const uint32_t* textureBuffer);



#endif

