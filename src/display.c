#include "display.h"

// ==========================
// definitions
// ==========================
static enum CullMethod   g_CullMethod   = 0;
static enum RenderMethod g_RenderMethod = 0;

static SDL_Window*   g_pWindow = NULL;
static SDL_Renderer* g_pRenderer = NULL;
static SDL_Texture*  g_pColorBufferTexture = NULL;

const int     g_DefaultWindowWidth  = 320;
const int     g_DefaultWindowHeight = 180;

int    g_WindowWidth  = g_DefaultWindowWidth;
int    g_WindowHeight = g_DefaultWindowHeight;
int    g_WindowArea   = g_DefaultWindowWidth * g_DefaultWindowHeight;
u32*   g_ColorBuffer  = NULL;
float* g_ZBuffer      = NULL;


// ==================================================================
// implementations of functions
// ==================================================================
bool InitializeWindow(void) 
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;	
    }

    // use SDL to query what is the fullscreen max width and height
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    const int fullscreenWidth  = displayMode.w;
    const int fullscreenHeight = displayMode.h;

    //g_WindowArea = g_WindowWidth * g_WindowHeight;
#if 1

    // if we want to use "old school" style (image is pixelated)
    if (true)
    {
        g_WindowWidth  = fullscreenWidth / 2;
        g_WindowHeight = fullscreenHeight / 2;
        g_WindowArea = g_WindowWidth * g_WindowHeight;
    }
    else
    {
        g_WindowWidth = fullscreenWidth;
        g_WindowHeight = fullscreenHeight;
        g_WindowArea = g_WindowWidth * g_WindowHeight;
    }

    // create a SDL window
    g_pWindow = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        fullscreenWidth,
        fullscreenHeight,
        SDL_WINDOW_BORDERLESS);

    if (!g_pWindow)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // create a SDL renderer
    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
    if (!g_pRenderer) 
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(g_pWindow, SDL_WINDOW_FULLSCREEN);
#endif

    // allocate the required memory in bytes to hold the color buffer
    g_ColorBuffer = (u32*)malloc(sizeof(u32) * g_WindowWidth * g_WindowHeight);

    // ... and the z-buffer
    g_ZBuffer = (float*)malloc(sizeof(float) * g_WindowWidth * g_WindowHeight);

    ClearZBuffer();

    // creating a SDL texture that is used to display the color buffer
    g_pColorBufferTexture = SDL_CreateTexture(
        g_pRenderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);    

    // we successfully initialized SDL stuff
    return true;
}

///////////////////////////////////////////////////////////

SDL_Renderer* GetRenderer(void) { return g_pRenderer; }

///////////////////////////////////////////////////////////

int GetWindowWidth(void)  { return g_WindowWidth; }
int GetWindowHeight(void) { return g_WindowHeight; }

///////////////////////////////////////////////////////////

void SetRenderMethod(const int method) { g_RenderMethod = method; }
void SetCullMethod  (const int method) { g_CullMethod = method; }

bool IsCullBackface(void) { return g_CullMethod == CULL_BACK; }

///////////////////////////////////////////////////////////

bool ShouldRenderFilledTriangles(void)
{
    return 
        g_RenderMethod == RENDER_FILL_SOLID || 
        g_RenderMethod == RENDER_FILL_SOLID_WIRE;
}

///////////////////////////////////////////////////////////

bool ShouldRenderTexturedTriangles(void)
{
    return
        g_RenderMethod == RENDER_TEXTURED || 
        g_RenderMethod == RENDER_TEXTURED_WIRE;
}

///////////////////////////////////////////////////////////

bool ShouldRenderWireframe(void)
{
    return
        g_RenderMethod == RENDER_WIRE || 
        g_RenderMethod == RENDER_WIRE_VERTEX ||
        g_RenderMethod == RENDER_FILL_SOLID_WIRE ||
        g_RenderMethod == RENDER_TEXTURED_WIRE;
}

///////////////////////////////////////////////////////////

bool ShouldRenderWireVertices(void)
{
    return g_RenderMethod == RENDER_WIRE_VERTEX;
}

///////////////////////////////////////////////////////////

void RenderColorBuffer(void)
{
    SDL_UpdateTexture(
        g_pColorBufferTexture,
        NULL,
        g_ColorBuffer,
        (int)(g_WindowWidth * sizeof(u32)));

    SDL_RenderCopy(g_pRenderer, g_pColorBufferTexture, NULL, NULL);

    SDL_RenderPresent(g_pRenderer);
}

///////////////////////////////////////////////////////////

void ClearColorBuffer(const Color color)
{
    // set the entire color buffer with a specific color value
    for (int i = 0; i < g_WindowArea; ++i)
        g_ColorBuffer[i] = color;
}

//////////////////////////////////////////////////////////

void ClearZBuffer(void)
{
    // set the entire z-buffer with a specific value
    for (int i = 0; i < g_WindowArea; ++i)
        g_ZBuffer[i] = 1.0f;
}

//////////////////////////////////////////////////////////

void DestroyWindow()
{
    free(g_ColorBuffer);
    free(g_ZBuffer);

    SDL_DestroyRenderer(g_pRenderer);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();
}

//////////////////////////////////////////////////////////

u32 GetColorBufferByPixelIdx(const int pixelIdx)
{
    if (pixelIdx < 0 || (pixelIdx >= g_WindowArea))
        return 0xFFFFFFFF;

    return g_ColorBuffer[pixelIdx];
}

//////////////////////////////////////////////////////////

float GetZBufferAt(const int x, const int y)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y >= g_WindowHeight)
        return 1.0f;

    return g_ZBuffer[(g_WindowWidth * y) + x];
}

//////////////////////////////////////////////////////////

float GetZBufferByPixelIdx(const int pixelIdx)
{
    if (pixelIdx < 0 || (pixelIdx >= g_WindowArea))
        return 1.0f;

    return g_ZBuffer[pixelIdx];
}

//////////////////////////////////////////////////////////

void SetZBufferAt(const int x, const int y, const float value)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y >= g_WindowHeight)
        return;

    g_ZBuffer[(g_WindowWidth * y) + x] = value;
}

//////////////////////////////////////////////////////////

void SetZBufferByPixelIdx(const int pixelIdx, const float value)
{
    if (pixelIdx < 0 || (pixelIdx >= g_WindowArea))
        return;

    g_ZBuffer[pixelIdx] = value;
}

//////////////////////////////////////////////////////////

