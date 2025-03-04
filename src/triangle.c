// ==================================================================
// Filename:    triangle.c
// Description: implementation of functional for:
//              1. rendering of triangles filled with solid color
//              2. rendering of triangles which are textured
// ==================================================================
#include "triangle.h"
#include "display.h"
#include "array.h"
#include "swap.h"
#include "light.h"

///////////////////////////////////////////////////////////

void ComputeReciprocalW(
    float* w0,               // w-component of point A
    float* w1,               // ... of point B
    float* w2)               // ... of point C
{
    // compute and write back reciprocal W for each input point
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
   const float lightIntensity,
   const int xStart,
   const int xEnd,
   const int y,
   const uint32_t color)
{
    float alpha = 0.0f;
    float beta = 0.0f;
    float gamma = 0.0f;

    // compute index of the pixel into z-buffer
    int pixelIdx = g_WindowWidth * y + xStart;

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
        if (depth < g_ZBuffer[pixelIdx])
        {
            // update the z-buffer value with the 1/w of this current pixel
            g_ZBuffer[pixelIdx] = depth;
            
            const uint32_t pixelColor = LightApplyIntensity(color, lightIntensity);

            g_ColorBuffer[pixelIdx] = pixelColor;
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
    const float lightIntensity,
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
               lightIntensity,
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
               lightIntensity,
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
void DrawTexelLine(
    const Vec2Int a,   // point A
    const Vec2Int b,   // point B
    const Vec2Int c,   // point C
    const Vec2Int ac,
    const Tex2 texA,
    const Tex2 texB,
    const Tex2 texC,
    const float invArea,
    const float w0,
    const float w1, 
    const float w2,
    const float lightIntensity,
    const int xStart,
    const int xEnd,
    const int y,
    const uint32_t* texture)
{
    float alpha = 0.0f;
    float beta  = 0.0f;
    float gamma = 0.0f;
   
    // compute index of the pixel into z-buffer
    int pixelIdx = g_WindowWidth * y + xStart;

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
        if (depth < g_ZBuffer[pixelIdx])
        {
            // update the z-buffer value with the 1/w of this current pixel
            g_ZBuffer[pixelIdx] = depth;

            // interpolate u/w and v/w coords using barycentric weights and a factor of 1/w
            float interpolatedU = (texA.u * alphaMulRecipW) + (texB.u * betaMulRecipW) + (texC.u * gammaMulRecipW);
            float interpolatedV = (texA.v * alphaMulRecipW) + (texB.v * betaMulRecipW) + (texC.v * gammaMulRecipW);

            float invInterpolatedReciprocalW = 1.0f / interpolatedReciprocalW;
            
            // now we can divide back both interpolated values by 1/w
            interpolatedU *= invInterpolatedReciprocalW;
            interpolatedV *= invInterpolatedReciprocalW;

            // map the UV coordinate to the full texture width and height
            int tx = abs((int)(interpolatedU * g_TextureWidth))  % g_TextureWidth;
            int ty = abs((int)(interpolatedV * g_TextureHeight)) % g_TextureHeight;
            
            const uint32_t texColor = texture[g_TextureWidth * ty + tx];
            const uint32_t pixelColor = LightApplyIntensity(texColor, lightIntensity);

            // so we draw the pixel at pos (x,y) with the color from the mapped texture only if the depth value is less that the previous one stored in the z-buffer
            DrawPixel(x, y, pixelColor);

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

void SwapTexCoords(Tex2* pTex1, Tex2* pTex2)
{
    Tex2 temp = *pTex1;
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
    const uint32_t* texture)
{
    Tex2 texA = { u0, v0 };
    Tex2 texB = { u1, v1 };
    Tex2 texC = { u2, v2 };

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
        if (y1 - y0 != 0) invSlope1 = (float)(x1 - x0) / abs(y1 - y0);
        if (y2 - y0 != 0) invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y0; y < y1; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // sample pixel color from the texture
            DrawTexelLine(
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
                texture);
        }
    }

    // ----------------------------------------------------
    // Render the bottom part of the triangle (flat-top)
    // ----------------------------------------------------
   
    if (y2 - y1 != 0)
    {
        invSlope1 = 0.0f;
        invSlope2 = 0.0f;
        if (y2 - y1 != 0) invSlope1 = (float)(x2 - x1) / abs(y2 - y1);
        if (y2 - y0 != 0) invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

        for (int y = y1; y <= y2; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd:
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            // sample pixel color from the texture
            DrawTexelLine(
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
                texture);      
        }
    }
}

