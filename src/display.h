#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#define FPS 500
#define FRAME_TARGET_TIME (1000 / FPS)

// ============================
// typedefs
// ============================
typedef uint32_t u32;
typedef uint32_t Color;

enum CullMethod
{
    CULL_NONE,              // disable backface-culling
    CULL_BACK,              // enable backface-culling
};

enum RenderMethod 
{
    RENDER_WIRE,            // render only wireframe
    RENDER_WIRE_VERTEX,     // render both wireframe and vertices
    RENDER_FILL_SOLID,      // render filled triangle with some color
    RENDER_FILL_SOLID_WIRE, // render both filled triangle and wireframe
    RENDER_TEXTURED,        // render textured triangle
    RENDER_TEXTURED_WIRE,   // render textured triangle with wireframe on top
};

// =============================
// functions prototypes
// =============================
bool InitializeWindow(void);

int GetWindowWidth(void);
int GetWindowHeight(void);
bool IsCullBackface(void);

void SetRenderMethod(const int method);
void SetCullMethod(const int method);

bool ShouldRenderFilledTriangles(void);
bool ShouldRenderTexturedTriangles(void);
bool ShouldRenderWireframe(void);
bool ShouldRenderWireVertices(void);

void DrawPixel     (int x, int y, Color color);
void DrawPixelByIdx(const int pixelIdx, Color color);
void DrawLine      (int x0, int y0, int x1, int y1, Color color);
void DrawLine2     (int x0, int y0, int x1, int y1, Color color);
void DrawTriangle  (int x0, int y0, int x1, int y1, int x2, int y2, Color color);
void DrawGrid      (void);
void DrawRect      (int x, int y, int width, int height, Color color);
void DrawCircle    (int x, int y, int radius, Color color);

void RenderColorBuffer(void);
void ClearColorBuffer(Color color);
void ClearZBuffer(void);
void DestroyWindow(void);

u32 GetColorBufferByPixelIdx(const int pixelIdx);

// z-buffer setters/getters
float GetZBufferAt(const int x, const int y);
float GetZBufferByPixelIdx(const int pixelIdx);
void SetZBufferAt(const int x, const int y, const float value);
void SetZBufferByPixelIdx(const int pixelIdx, const float value);

#endif
