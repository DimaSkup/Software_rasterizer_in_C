// ==================================================================
// Filename:    render.c
// Description: implementation of functional for:
//              1. rendering of triangles filled with solid color
//              2. rendering of triangles which are textured
// ==================================================================
#include "render.h"
#include "display.h"
#include "array.h"
#include "swap.h"
#include "light.h"
#include "display.h"
#include "macros.h"

void DrawPixel(int x, int y, Color color) 
{
    // if we have wrong input args we just do nothing
    if ((x < 0) || (x >= g_WindowWidth) || (y < 0) || (y >= g_WindowHeight))
        return;

    // set a pixel color at position (x,y) on the screen
    g_ColorBuffer[y * g_WindowWidth + x] = color;
}

///////////////////////////////////////////////////////////

void DrawPixelByIdx(const int pixelIdx, Color color)
{
    // set a color for a particular pixel by input index

    if (pixelIdx < 0 || (pixelIdx >= g_WindowArea))
        return;

    g_ColorBuffer[pixelIdx] = color;
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
            g_ColorBuffer[g_WindowWidth * posY + posX] = color;
        }
    }
}

///////////////////////////////////////////////////////////

void DrawCircle(int x, int y, int radius, Color color)
{
    const float radianStep = 0.0175f;    

    for (float rad = 0.0f; rad < PI_2; rad += radianStep)
    {
        int rx = (int)(cosf(rad) * radius) + x;
        int ry = (int)(sinf(rad) * radius) + y;

        g_ColorBuffer[(g_WindowWidth * ry) + rx] = color;
    }
}

///////////////////////////////////////////////////////////

void ComputeReciprocalW(
    float* w0,               // w-component of point A
    float* w1,               // ... of point B
    float* w2)               // ... of point C
{
    // compute and write back reciprocal W for each input point:
    // (using this we will perform perspective division)
    *w0 = 1.0f / *w0;
    *w1 = 1.0f / *w1;
    *w2 = 1.0f / *w2;
}

// ==================================================================
// Return the barycentric weights alpha, beta, and gamma
// for point p in the triangle ABC
// ==================================================================
/*
              B
             /|\
            / | \
           /  |  \
          /  (p)  \
         /  _/ \_  \
        / _/     \_ \
       /_/         \_\
      A---------------C
*/
// ==================================================================
void BarycentricWeights(
    const Vec2Int a,      // point A
    const Vec2Int b,      // point B
    const Vec2Int c,      // point C
    const Vec2Int p,      // point P
    const Vec2Int ac,     
    const float invArea,  // inverse area of doubled triangle                          
    float* alpha,
    float* beta,
    float* gamma)
{
    // find the vectors between the vertices ABC and point p
    const Vec2Int pc = {c.x - p.x, c.y - p.y}; // Vec2Sub(c, p);
    const Vec2Int pb = {b.x - p.x, b.y - p.y}; // Vec2Sub(b, p);
    const Vec2Int ap = {p.x - a.x, p.y - a.y}; // Vec2Sub(p, a);

    // alpha = area of parallelogram-PBC over the area of the full parallelogram-ABC
    *alpha = (pc.x * pb.y - pc.y * pb.x) * invArea;

    // beta = area of parallelogram-APC over the area of the full parallelogram-ABC
    *beta = (ac.x * ap.y - ac.y * ap.x) * invArea;

    // gamma can be easily found since barycentric coordinated always add up to 1.0
    *gamma = 1.0f - *alpha - *beta;
}

///////////////////////////////////////////////////////////

void DrawDepthLine(
   const Vec2Int a,
   const Vec2Int b,
   const Vec2Int c,
   const Vec2Int ac,
   const float invArea,
   const float w0, 
   const float w1, 
   const float w2,
   const int xStart,
   const int xEnd,
   const int y,
   const uint32_t color)
{
    float alpha = 0.0f;
    float beta  = 0.0f;
    float gamma = 0.0f;

    // compute index of the pixel into z-buffer
    int pixelIdx = GetWindowWidth() * y + xStart;

    // go through each pixel in horizontal line
    for (int x = xStart; x < xEnd; x++, pixelIdx++)
    {
        const Vec2Int p = { x, y };
        BarycentricWeights(a, b, c, p, ac, invArea, &alpha, &beta, &gamma);

        // multiply reciprocal W of each point by alpha/beta/gamma
        const float alphaMulRecipW = (w0 * alpha);
        const float betaMulRecipW  = (w1 * beta);
        const float gammaMulRecipW = (w2 * gamma);

        // also interpolate the value of 1/w for the current pixel
        float interpolatedReciprocalW = alphaMulRecipW + betaMulRecipW + gammaMulRecipW;

        const float depth = 1.0f - interpolatedReciprocalW;

        // immediately test if the pixel is closer or farther from the camera so we will be able to skip unnecessary computations (in case if farther)
        // NOTE: (1.0f - interpolated_recip_w): adjust 1/w so the pixels that are closer to the camera have smaller values (because bigger w gives us smaller 1/w so we failing z-test)    
        if (depth < GetZBufferByPixelIdx(pixelIdx))
        {
            // update the z-buffer value with the 1/w of this current pixel
            SetZBufferByPixelIdx(pixelIdx, depth);

            DrawPixelByIdx(pixelIdx, color);
        }
    }
}


// ==================================================================
// Draw a filled triangle with the flat-top/flat-bottom method.
// We split the original triangle in two, half flat-bottom and
// half flat-top
//===================================================================
/*
          (x0,y0)
            / \
           /   \
          /     \
         /       \
        /         \
   (x1,y1)------(Mx,My)
       \_           \
          \_         \
             \_       \
                \_     \
                   \    \
                     \_  \
                        \_\
                           \
                         (x2,y2)
*/
//===================================================================
void DrawFilledTriangle(
    int x0, int y0, float w0,
    int x1, int y1, float w1,
    int x2, int y2, float w2,
    const uint32_t color)
{
    // sort the vertices by Y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        SWAPF(&w0, &w1);
    }
    if (y1 > y2)
    {
        SWAPI(y1, y2);
        SWAPI(x1, x2);
        SWAPF(&w1, &w2);
    }
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        SWAPF(&w0, &w1);
    }

    ComputeReciprocalW(&w0, &w1, &w2);

    const Vec2Int a = { x0, y0 };
    const Vec2Int b = { x1, y1 };
    const Vec2Int c = { x2, y2 };

    // inverse area of full parallelogram (doubled triangle ABC) using the cross product
    const Vec2Int ac = {c.x - a.x, c.y - a.y};
    const Vec2Int ab = {b.x - a.x, b.y - a.y};
    float invArea = 1.0f / (ac.x * ab.y - ac.y * ab.x);  // 1.0f / Cross(AC, AB)

    // ----------------------------------------------------
    // Render the upper part of the triangle (flat-bottom)
    // ----------------------------------------------------
    float invSlope1 = 0.0f;
    float invSlope2 = 0.0f;

    
    if (y1 - y0 != 0)
    {
        invSlope1 = (float)(x1 - x0) / abs(y1 - y0);

        if (y2 - y0 != 0)
            invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y0; y < y1; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // draw a colored line where each pixel has its own depth
            DrawDepthLine(
               a, b, c,
               ac,
               invArea,
               w0, w1, w2,
               xStart,
               xEnd,
               y,
               color);
        }
    }

    // ----------------------------------------------------
    // Render the bottom part of the triangle (flat-top)
    // ----------------------------------------------------
   
    if (y2 - y1 != 0)
    {
        invSlope1 = (float)(x2 - x1) / abs(y2 - y1);

        if (y2 - y0 != 0) 
            invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y1; y <= y2; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd:
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // draw a colored line where each pixel has its own depth
            DrawDepthLine(
               a, b, c,
               ac,
               invArea,
               w0, w1, w2,
               xStart,
               xEnd,
               y,
               color);
        }
    }
}


// ==================================================================
// Function to draw the textured pixels at pos X and Y using interpolation
// ==================================================================
void DrawTexturedLine(
    const Vec2Int a,   // point A
    const Vec2Int b,   // point B
    const Vec2Int c,   // point C
    const Vec2Int ac,
    const Vec2 texA,   // texture coords for point A
    const Vec2 texB,
    const Vec2 texC,
    const float invArea,
    const float w0,
    const float w1, 
    const float w2,
    const float lightIntensity,
    const int xStart,
    const int xEnd,
    const int y,
    const int textureWidth,
    const int textureHeight,
    const uint32_t* textureBuffer)
{
    float alpha = 0.0f;
    float beta  = 0.0f;
    float gamma = 0.0f;
  

    // compute index of the pixel into z-buffer
    int pixelIdx = GetWindowWidth() * y + xStart;

    // go through each pixel in horizontal line
    for (int x = xStart; x <= xEnd; x++, pixelIdx++)
    {
        const Vec2Int p = { x, y };
        BarycentricWeights(a, b, c, p, ac, invArea, &alpha, &beta, &gamma);

        // multiply reciprocal W of each point by alpha/beta/gamma
        const float alphaMulRecipW = (w0 * alpha);
        const float betaMulRecipW  = (w1 * beta);
        const float gammaMulRecipW = (w2 * gamma);

        // also interpolate the value of 1/w for the current pixel
        float interpolatedReciprocalW = alphaMulRecipW + betaMulRecipW + gammaMulRecipW;

        const float depth = 1.0f - interpolatedReciprocalW;

        // immediately test if the pixel is closer or farther from the camera so we will be able to skip unnecessary computations (in case if farther)
        // NOTE: (1.0f - interpolated_recip_w): adjust 1/w so the pixels that are closer to the camera have smaller values (because bigger w gives us smaller 1/w so we failing z-test)    
        if (depth < GetZBufferByPixelIdx(pixelIdx))
        {
            // interpolate u/w and v/w coords using barycentric weights and a factor of 1/w
            float interpolatedU = (texA.x * alphaMulRecipW) + (texB.x * betaMulRecipW) + (texC.x * gammaMulRecipW);
            float interpolatedV = (texA.y * alphaMulRecipW) + (texB.y * betaMulRecipW) + (texC.y * gammaMulRecipW);

            float invInterpolatedReciprocalW = 1.0f / interpolatedReciprocalW;
            
            // now we can divide back both interpolated values by 1/w
            interpolatedU *= invInterpolatedReciprocalW;
            interpolatedV *= invInterpolatedReciprocalW;

            // map the UV coordinate to the full texture width and height
            int tx = abs((int)(interpolatedU * textureWidth))  % textureWidth;
            int ty = abs((int)(interpolatedV * textureHeight)) % textureHeight;

            uint32_t texColor = textureBuffer[textureWidth * ty + tx];


            // alpha clipping
            if (((texColor & 0xFF000000) >> 6) < 0.1f)
            {
                continue;
            }
            else
            {
                // update the z-buffer value with the 1/w of this current pixel
                SetZBufferByPixelIdx(pixelIdx, depth);

                const uint32_t pixelColor = LightApplyIntensity(texColor, lightIntensity);

                // draw the pixel at pos (x,y) with the color from the mapped texture only if the depth value is less that the previous one stored in the z-buffer
                DrawPixel(x, y, pixelColor);

            }
            
        } // if
    } // for
}

// ==================================================================
// Draw a texture triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom
// and half flat-top
// ==================================================================
/*
         A(x0,y0)
            / \
           /   \
          /     \
         /       \
        /         \
  B(x1,y1)         \
       \_           \
          \_         \
             \_       \
                \_     \
                   \    \
                     \_  \
                        \_\
                           \
                        C(x2,y2)
*/
//===================================================================

void SwapTexCoords(Vec2* pTex1, Vec2* pTex2)
{
    Vec2 temp = *pTex1;
    *pTex1 = *pTex2;
    *pTex2 = temp;
}

///////////////////////////////////////////////////////////

void DrawTexturedTriangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    float u0, float v0,
    float u1, float v1,
    float u2, float v2,
    float lightIntensity,
    const int textureWidth,
    const int textureHeight,
    const uint32_t* textureBuffer)
{
    Vec2 texA = { u0, v0 };
    Vec2 texB = { u1, v1 };
    Vec2 texC = { u2, v2 };

    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        //SWAPF(&z0, &z1);
        SWAPF(&w0, &w1);
        SwapTexCoords(&texA, &texB);
    }
    if (y1 > y2)
    {
        SWAPI(y1, y2);
        SWAPI(x1, x2);
        //SWAPF(&z1, &z2);
        SWAPF(&w1, &w2);
        SwapTexCoords(&texB, &texC);
    }
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        //SWAPF(&z0, &z1);
        SWAPF(&w0, &w1);
        SwapTexCoords(&texA, &texB);
    }

    ComputeReciprocalW(&w0, &w1, &w2);
   
    const Vec2Int a = { x0, y0 };
    const Vec2Int b = { x1, y1 };
    const Vec2Int c = { x2, y2 };

    // inverse area of the full parallelogram (doubled triangle ABC) using the cross product
    const Vec2Int ac = {c.x - a.x, c.y - a.y};            
    const Vec2Int ab = {b.x - a.x, b.y - a.y};            
    float invArea = 1.0f / (ac.x * ab.y - ac.y * ab.x);   // 1.0f / Cross(AC, AB)
    // ----------------------------------------------------
    // Render the upper part of the triangle (flat-bottom)
    // ----------------------------------------------------
    float invSlope1 = 0.0f;
    float invSlope2 = 0.0f;

    if (y1 - y0 != 0)
    { 
        invSlope1 = (float)(x1 - x0) / abs(y1 - y0);

        if (y2 - y0 != 0) 
            invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y0; y < y1; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // sample pixel color from the texture
            DrawTexturedLine(
                a, b, c,
                ac, 
                texA,
                texB,
                texC,
                invArea,
                w0, w1, w2,
                lightIntensity,
                xStart,
                xEnd,
                y,
                textureWidth,
                textureHeight,
                textureBuffer);
        }
    }

    // ----------------------------------------------------
    // Render the bottom part of the triangle (flat-top)
    // ----------------------------------------------------
   
    if (y2 - y1 != 0)
    {
        invSlope2 = 0.0f;
        invSlope1 = (float)(x2 - x1) / abs(y2 - y1);

        if (y2 - y0 != 0) 
            invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y1; y <= y2; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd:
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // sample pixel color from the texture
            DrawTexturedLine(
                a, b, c,
                ac, 
                texA,
                texB,
                texC,
                invArea,
                w0, w1, w2,
                lightIntensity,
                xStart,
                xEnd,
                y,
                textureWidth,
                textureHeight,
                textureBuffer);      
        }
    }
}

