#include "display.h"


// ==========================
// definitions
// ==========================
float PI = 3.141592653f;

SDL_Window*   g_pWindow = NULL;
SDL_Renderer* g_pRenderer = NULL;
SDL_Texture*  g_pColorBufferTexture = NULL;

int    g_WindowWidth  = 800;
int    g_WindowHeight = 600;
u32*   g_ColorBuffer = NULL;
float* g_ZBuffer = NULL;

	
// ============================
// implementations
// ============================
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

    g_WindowWidth  = displayMode.w;
    g_WindowHeight = displayMode.h;

    //g_WindowWidth = 800;
    //g_WindowHeight = 600;

    // create a SDL window
    g_pWindow = SDL_CreateWindow(
        NULL, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        g_WindowWidth,
        g_WindowHeight,
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
    
    // we successfully initialized SDL stuff
    return true;
}

///////////////////////////////////////////////////////////

void DrawPixel(int x, int y, Color color) 
{
    // set a pixel color at position (x,y) on the screen
    if (x >= 0 && x < g_WindowWidth && y >= 0 && y < g_WindowHeight)
        g_ColorBuffer[y * g_WindowWidth + x] = color;
}

///////////////////////////////////////////////////////////

void DrawLine(int x0, int y0, int x1, int y1, Color color)
{
    // DDA line drawing algorithm
    int dx = (x1 - x0);               // delta X
    int dy = (y1 - y0);               // delta Y

    // longest side length
    int sideLength = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    // find how much we should increment in both x and y each stem
    float xInc = dx / (float)sideLength;
    float yInc = dy / (float)sideLength;

    float currX = x0;
    float currY = y0;

    for (int i = 0; i <= sideLength; ++i)
    {
        DrawPixel(round(currX), round(currY), color);
        currX += xInc;
        currY += yInc;
    }
}

///////////////////////////////////////////////////////////

void DrawLine2(int x0, int y0, int x1, int y1, Color color)
{
    // Bresenham line drawing algorithm

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x1 >= x0) ? 1 : -1;
    int sy = (y1 >= y0) ? 1 : -1;

    // first case
    if (dy <= dx)
    {
        int d = (dy << 1) - dx;
        int d1 = dy << 1;
        int d2 = (dy - dx) << 1;

        // draw the first pixel of the line
        DrawPixel(x0, y0, color);

        for (int x = x0 + sx, y = y0, i = 1; i <= dx; i++, x+= sx)
        {
            if (d > 0)
            {  
                d += d2;
                y += sy;
            }
            else
            {
                d += d1;
            }

            DrawPixel(x, y, color);
        }
    }
    // second case
    else
    {
        int d = (dx << 1) - dy;
        int d1 = dx << 1;
        int d2 = (dx - dy) << 1;

        // draw the first pixel of the line
        DrawPixel(x0, y0, color);

        for (int y = y0 + sy, x = x0, i = 1; i <= dy; i++, y += sy)
        {
            if (d > 0)
            {
                d += d2;
                x += sx;
            }
            else
            {
                d += d1;
            }

            DrawPixel(x, y, color);
        }
    }
}

///////////////////////////////////////////////////////////

void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color color)
{
    // draw a line btw each pair of triangle's points
    DrawLine2(x0, y0, x1, y1, color);
    DrawLine2(x1, y1, x2, y2, color);
    DrawLine2(x0, y0, x2, y2, color);
}

///////////////////////////////////////////////////////////

void DrawGrid(void)
{
    const int multiple = 10;
    const u32 gridColor = 0xFF333333;

    for (int y = 0; y < g_WindowHeight; y += multiple)
    {
        for (int x = 0; x < g_WindowWidth; x += multiple)
        {
            g_ColorBuffer[(g_WindowWidth * y) + x] = gridColor;
        }
    }
}

//////////////////////////////////////////////////////////

void DrawRect(int x, int y, int width, int height, Color color)
{
    for (int posY = y; posY < (y + height); ++posY)
    {
        for (int posX = x; posX < (x + width); ++posX)
        {
            DrawPixel(posX, posY, color);
            //colorBuffer[windowWidth * posY + posX] = color;
        }
    }
}

///////////////////////////////////////////////////////////

void DrawCircle(int x, int y, int radius, Color color)
{
    const float radianStep = PI / 180.0f;    

    for (float rad = 0.0f; rad < 2*PI; rad += radianStep)
    {
        int rx = (int)(cosf(rad) * radius) + x;
        int ry = (int)(sinf(rad) * radius) + y;

        g_ColorBuffer[(g_WindowWidth * ry) + rx] = color;
    }
}

///////////////////////////////////////////////////////////

void RenderColorBuffer(void)
{
    SDL_UpdateTexture(
        g_pColorBufferTexture,
        NULL,
        g_ColorBuffer,
        (int)(g_WindowWidth * sizeof(u32)));

    SDL_RenderCopy(
        g_pRenderer,
        g_pColorBufferTexture,
        NULL,
        NULL);
}

///////////////////////////////////////////////////////////

void ClearColorBuffer(const Color color)
{
    // set the entire color buffer with a specific color value
    for (int i = 0; i < (g_WindowWidth * g_WindowHeight); ++i)
        g_ColorBuffer[i] = color;
}

//////////////////////////////////////////////////////////

void ClearZBuffer(void)
{
    // set the entire z-buffer with a specific value
    for (int i = 0; i < (g_WindowWidth * g_WindowHeight); ++i)
        g_ZBuffer[i] = 1.0f;
}

//////////////////////////////////////////////////////////

void DestroyWindow()
{
    SDL_DestroyRenderer(g_pRenderer);
    SDL_DestroyWindow(g_pWindow);
    SDL_Quit();
}

//////////////////////////////////////////////////////////


