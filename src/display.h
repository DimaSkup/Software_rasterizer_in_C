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
extern float PI;

extern enum CullMethod
{
    CULL_NONE,              // disable backface-culling
    CULL_BACK,              // enable backface-culling
} g_CullMethod;

extern enum RenderMethod 
{
    RENDER_WIRE,            // render only wireframe
    RENDER_WIRE_VERTEX,     // render both wireframe and vertices
    RENDER_FILL_SOLID,      // render filled triangle with some color
    RENDER_FILL_SOLID_WIRE, // render both filled triangle and wireframe
    RENDER_TEXTURED,        // render textured triangle
    RENDER_TEXTURED_WIRE,   // render textured triangle with wireframe on top
} g_RenderMethod;

// ============================
// globals
// ===========================
extern SDL_Window*   g_pWindow;
extern SDL_Renderer* g_pRenderer;
extern SDL_Texture*  g_pColorBufferTexture;

extern int    g_WindowWidth;
extern int    g_WindowHeight;
extern u32*   g_ColorBuffer;
extern float* g_ZBuffer;

// =============================
// functions prototypes
// =============================
bool InitializeWindow(void);

void DrawPixel   (int x, int y, Color color);
void DrawLine    (int x0, int y0, int x1, int y1, Color color);
void DrawLine2   (int x0, int y0, int x1, int y1, Color color);
void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color);
void DrawGrid    (void);
void DrawRect    (int x, int y, int width, int height, Color color);
void DrawCircle  (int x, int y, int radius, Color color);

void RenderColorBuffer(void);
void ClearColorBuffer(Color color);
void ClearZBuffer(void);
void DestroyWindow(void);

#endif
