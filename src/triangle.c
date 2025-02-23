#include "triangle.h"
#include "display.h"
#include "array.h"
#include "swap.h"

//===================================================================
// draw a filled triangle with a flat bottom
//===================================================================
/* 
           (x0,y0) 
             / \
            /   \
           /     \
          /       \
         /         \
     (x1,y1)------(x2,y2) 
*/
//===================================================================
void FillFlatBottomTriangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    const u32 color)
{
    // find the two slopes (from p0->p1 and p0->p2)
    float invSlope1 = (float)(x1 - x0) / (y1 - y0);
    float invSlope2 = (float)(x2 - x0) / (y2 - y0); 

    // start x_start and x_end from the top vertex (x0,y0)
    float xStart = x0;
    float xEnd = x0;

    // loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; ++y)
    {
        DrawLine(xStart, y, xEnd, y, color);

        xStart += invSlope1;
        xEnd += invSlope2;           
    }
}

//===================================================================
// draw a filled triangle with a flat top
//===================================================================
/*
     (x0,y0)------(x1,y1)
         \         /
          \       /
           \     /
            \   /
             \ /
           (x2,y2)
*/
//===================================================================
void FillFlatTopTriangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    const u32 color)
{
    // find the two slopes (from p2->p0 and p2->p1)
    float invSlope1 = (float)(x2 - x0) / (y2 - y0);
    float invSlope2 = (float)(x2 - x1) / (y2 - y1);

    // start x_start and x_end from the bottom vertex (x2,y2)
    float xStart = x2;
    float xEnd = x2;

    // loop all the scanlines from bottom to top
    for (int y = y2; y > y0; --y)
    {
        DrawLine(xStart, y, xEnd, y, color);
        
        xStart -= invSlope1;
        xEnd -= invSlope2;
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
    int x0, int y0,
    int x1, int y1,
    int x2, int y2,
    const u32 color)
{
    // sort the vertices by Y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
    }
    if (y1 > y2)
    {
        SWAPI(y1, y2);
        SWAPI(x1, x2);
    }
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
    }

    // avoid division by zero (when delta_y == 0)
    if (y1 == y2)
    {
        // we can simple draw the flat-bottom triangle and go out
        FillFlatBottomTriangle(x0, y0, x1, y1, x2, y2, color);
    }
    else if (y0 == y1)
    {
        // we can simple draw the flat-top triangle and go out
        FillFlatTopTriangle(x0, y0, x1, y1, x2, y2, color);
    }
    // a common case
    else
    {
        // calculate the new vertex (Mx,My) using triangle similarity
        int My = y1;
        int Mx = x0 + ((float)(x2-x0)*(y1-y0)) / (float)(y2-y0);

        // draw flat-bottom triangle
        FillFlatBottomTriangle(x0, y0, x1, y1, Mx, My, color);

        // draw flat-top triangle
        FillFlatTopTriangle(x1, y1, Mx, My, x2, y2, color);
    }
}


// ==================================================================
// painter algorithm: sort triangles by average depth
// ==================================================================
void SortTriangles(
    Triangle* arr, 
    const int numTriangles, 
    const int low, 
    const int high)
{
    // sort triangles by its average depth

    if ((numTriangles > 0) && (low < high))
    {
        // simple bubble sort O_o
        for (int i = 0; i < numTriangles; ++i)
        {
            for (int j = i; j < numTriangles; ++j)
            {
                if (arr[i].avgDepth < arr[j].avgDepth)
                {
                    // swap the triangle position in the array
                    Triangle temp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = temp;
                }
            }
        }
    }
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
Vec3 BarycentricWeights(
    const Vec2 a, 
    const Vec2 b, 
    const Vec2 c, 
    const Vec2 p)
{
    // find the vectors between the vertices ABC and point p
    const Vec2 ac = Vec2Sub(c, a);
    const Vec2 ab = Vec2Sub(b, a);
    const Vec2 pc = Vec2Sub(c, p);
    const Vec2 pb = Vec2Sub(b, p);
    const Vec2 ap = Vec2Sub(p, a);

    // inverse area of the full parallelogram (doubled triangle ABC) using the cross product
    float invArea = 1.0f / (ac.x * ab.y - ac.y * ab.x);   // 1.0f / Cross(AC, AB)

    // alpha = area of parallelogram-PBC over the area of the full parallelogram-ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) * invArea;

    // beta = area of parallelogram-APC over the area of the full parallelogram-ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) * invArea;

    // gamma can be easily found since barycentric coordinated always add up to 1.0
    float gamma = 1.0f - alpha - beta;

    Vec3 weights = { alpha, beta, gamma };
    return weights;
}

// ==================================================================
// Function to draw the textured pixel at pos X and Y using interpolation
// ==================================================================
void DrawTexel(
    const int x, const int y, 
    const uint32_t* texture,
    const Vec4 pointA,
    const Vec4 pointB,
    const Vec4 pointC,
    const Tex2 texA,
    const Tex2 texB,
    const Tex2 texC)
{
    const Vec2 p = { x, y };
    const Vec2 a = { pointA.x, pointA.y };
    const Vec2 b = { pointB.x, pointB.y };
    const Vec2 c = { pointC.x, pointC.y };

    const Vec3 weights = BarycentricWeights(a, b, c, p);

    float alpha = weights.x;
    float beta  = weights.y;
    float gamma = weights.z;

    // multiply reciprocal W of each point by alpha/beta/gamma
    const float alphaMulRecipW = (pointA.w * alpha);
    const float betaMulRecipW  = (pointB.w * beta);
    const float gammaMulRecipW = (pointC.w * gamma);


    // also interpolate the value of 1/w for the current pixel
    float interpolatedReciprocalW = alphaMulRecipW + betaMulRecipW + gammaMulRecipW;

    // compute index of the pixel into z-buffer
    const int idx = (g_WindowWidth * y) + x;

    // immediately test if the pixel is closer or farther from the camera so we will be able to skip unnecessary computations (in case if farther)
    // NOTE: (1.0f - interpolated_recip_w): adjust 1/w so the pixels that are closer to the camera have smaller values (because bigger w gives us smaller 1/w so we failing z-test)    
    if ((1.0f - interpolatedReciprocalW) < g_ZBuffer[idx])
    {
        // update the z-buffer value with the 1/w of this current pixel
        g_ZBuffer[idx] = 1.0f - interpolatedReciprocalW;

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

        // so we draw the pixel at pos (x,y) with the color from the mapped texture only if the depth value is less that the previous one stored in the z-buffer
        DrawPixel(x, y, texture[g_TextureWidth * ty + tx]);
    }
}

///////////////////////////////////////////////////////////

void ComputeReciprocalW(
    Vec4* pPointA,
    Vec4* pPointB,
    Vec4* pPointC)
{
    // compute and write back reciprocal W for each input point
    pPointA->w = 1.0f / pPointA->w;
    pPointB->w = 1.0f / pPointB->w;
    pPointC->w = 1.0f / pPointC->w;
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
void DrawTexturedTriangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    float u0, float v0,
    float u1, float v1,
    float u2, float v2,
    const uint32_t* texture)
{

    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        SWAPF(&z0, &z1);
        SWAPF(&w0, &w1);
        SWAPF(&u0, &u1);
        SWAPF(&v0, &v1);   
    }
    if (y1 > y2)
    {
        SWAPI(y1, y2);
        SWAPI(x1, x2);
        SWAPF(&z1, &z2);
        SWAPF(&w1, &w2);
        SWAPF(&u1, &u2);
        SWAPF(&v1, &v2);
    }
    if (y0 > y1)
    {
        SWAPI(y0, y1);
        SWAPI(x0, x1);
        SWAPF(&z0, &z1);
        SWAPF(&w0, &w1);
        SWAPF(&u0, &u1);
        SWAPF(&v0, &v1);
    }

    // flip the V component to account for inverted UV-coords (V grows downwards)
    v0 = 1.0f - v0;
    v1 = 1.0f - v1;
    v2 = 1.0f - v2;

    Vec4 pointA = { x0, y0, z0, w0 };
    Vec4 pointB = { x1, y1, z1, w1 };
    Vec4 pointC = { x2, y2, z2, w2 };
    const Tex2 texA = { u0, v0 };
    const Tex2 texB = { u1, v1 };
    const Tex2 texC = { u2, v2 };

    ComputeReciprocalW(&pointA, &pointB, &pointC);

    // ----------------------------------------------------
    // Render the upper part of the triangle (flat-bottom)
    // ----------------------------------------------------
    float invSlope1 = 0.0f;
    float invSlope2 = 0.0f;

    if (y1 - y0 != 0) invSlope1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) invSlope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0)
    {
        for (int y = y0; y <= y1; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            for (int x = xStart; x < xEnd; x++)
            {
                // sample pixel color from the texture
                DrawTexel(
                    x, y, texture,
                    pointA, 
                    pointB,
                    pointC,
                    texA,
                    texB,
                    texC);
            }
        }
    }


    // ----------------------------------------------------
    // Render the bottom part of the triangle (flat-top)
    // ----------------------------------------------------

    if (y2 - y1 != 0)
    {
        invSlope1 = (float)(x2 - x1) / abs(y2 - y1);
        
        for (int y = y1; y <= y2; y++)
        {
            int xStart = x1 + (y - y1) * invSlope1;
            int xEnd   = x0 + (y - y0) * invSlope2;

            // swap if xStart is to the right of xEnd
            if (xEnd < xStart)
                SWAPI(xStart, xEnd);

            for (int x = xStart; x < xEnd; x++)
            {
                // sample pixel color from the texture
                DrawTexel(
                    x, y, texture,
                    pointA, 
                    pointB,
                    pointC,
                    texA,
                    texB,
                    texC); 
            }
        }
    }
}

