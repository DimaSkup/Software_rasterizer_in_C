#include "clipping.h"
#include "math_common.h"
#include <math.h>


#define NUM_PLANES 6
Plane g_FrustumPlanes[NUM_PLANES];


void InitFrustumPlanes(
    const float fovX,
    const float fovY,
    const float nearZ, 
    const float farZ)
{
    // each frustum plane is defined by a point and a normal vector;
    // so here we compute each frustum plane params;

    // horizontal (by X)
    const float sx = sinf(fovX / 2);
    const float cx = cosf(fovX / 2);

    // vertical (by Y)
    const float sy = sinf(fovY / 2);
    const float cy = sinf(fovY / 2);

    const Vec3 origin = { 0,0,0 };

    // left plane: P=(0,0,0), N=(cos(fov/2), 0, sin(fov/2))
    g_FrustumPlanes[LEFT_FRUSTUM_PLANE].point    = origin;
    g_FrustumPlanes[LEFT_FRUSTUM_PLANE].normal   = Vec3Init(cx, 0, sx);

    // right plane: P=(0,0,0), N=(-cos(fov/2), 0, sin(fov/2))
    g_FrustumPlanes[RIGHT_FRUSTUM_PLANE].point   = origin;
    g_FrustumPlanes[RIGHT_FRUSTUM_PLANE].normal  = Vec3Init(-cx, 0, sx);

    // top plane: P=(0,0,0), N=(0, -cos(fov/2), sin(fov/2))
    g_FrustumPlanes[TOP_FRUSTUM_PLANE].point     = origin;
    g_FrustumPlanes[TOP_FRUSTUM_PLANE].normal    = Vec3Init(0, -cy, sy);

    // bottom plane: P=(0,0,0), N=(0, cos(fov/2), sin(fov/2))
    g_FrustumPlanes[BOTTOM_FRUSTUM_PLANE].point  = origin;
    g_FrustumPlanes[BOTTOM_FRUSTUM_PLANE].normal = Vec3Init(0, cy, sy);

    // near plane: P=(0,0,nearZ), N=(0,0,1)
    g_FrustumPlanes[NEAR_FRUSTUM_PLANE].point    = Vec3Init(0, 0, nearZ);
    g_FrustumPlanes[NEAR_FRUSTUM_PLANE].normal   = Vec3Init(0, 0, 1);

    // far plane: P=(0,0,farZ), N=(0,0,-1)
    g_FrustumPlanes[FAR_FRUSTUM_PLANE].point     = Vec3Init(0, 0, farZ);
    g_FrustumPlanes[FAR_FRUSTUM_PLANE].normal    = Vec3Init(0, 0, -1);
}

///////////////////////////////////////////////////////////

Polygon CreatePolygonFromTriangle(
    const Vec4 v0, 
    const Vec4 v1, 
    const Vec4 v2,
    const Tex2 t0,
    const Tex2 t1, 
    const Tex2 t2)
{
    Polygon polygon = 
    {
        .vertices = 
        {
            (Vec3){ v0.x, v0.y, v0.z },
            (Vec3){ v1.x, v1.y, v1.z },
            (Vec3){ v2.x, v2.y, v2.z },
        },
        .texCoords = { t0, t1, t2 },
        .numVertices = 3
    };

    return polygon;
}

///////////////////////////////////////////////////////////
/*
                        * Q1        dotQ1 = dot(n, (Q1-P))
         _ N            |           dotQ2 = dot(n, (Q2-P))
        /|\             |
         |              | I
  -------*--------------*---------- plane
         P              |
                        |
                        |
                        * Q2
*/
///////////////////////////////////////////////////////////
void ClipPolygonAgainstPlane(Polygon* pPolygon, const int planeType)
{
    // clip an input polygon agains a frustum plane of input type
    // and return modified polygon stored in the input polygon param; 

    const Vec3 planePoint  = g_FrustumPlanes[planeType].point;
    const Vec3 planeNormal = g_FrustumPlanes[planeType].normal;

    // the array of inside vertices (are in positive half space) that will be part of the final polygon returned via parameter
    Vec3 insideVertices[MAX_NUM_POLYGON_VERTICES];
    Tex2 insideTexCoords[MAX_NUM_POLYGON_VERTICES];
    int numInsideVertices = 0;

    // start current and previous vertex with the first and last polygon vertices and do the same for texture coords 
    Vec3* pCurrVertex   = &pPolygon->vertices[0];
    Vec3* pPrevVertex   = &pPolygon->vertices[pPolygon->numVertices - 1];
    Tex2* pCurrTexCoord = &pPolygon->texCoords[0];
    Tex2* pPrevTexCoord = &pPolygon->texCoords[pPolygon->numVertices - 1];

    // calculate the dotQ1 (for the previous vertex)
    float prevDot = Vec3Dot(Vec3Sub(*pPrevVertex, planePoint), planeNormal);

    // loop while the current vertex is different that the last vertex
    while (pCurrVertex != &pPolygon->vertices[pPolygon->numVertices])
    {
        // calcute the dotQ2 (for the current vertex)
        float currDot = Vec3Dot(Vec3Sub(*pCurrVertex, planePoint), planeNormal);

        // if we changed from inside to outside (positive/negative half space) or vise-versa
        if (currDot * prevDot < 0)
        {
            // calculate the interpolation factor: t = dotQ1 / (dotQ1 - dotQ2)
            const float t = prevDot / (prevDot - currDot);
            
            // calculate the intersection point:   I = Q1 + t(Q2-Q1)
            Vec3 intersectionPoint = 
            {
                Lerp(pPrevVertex->x, pCurrVertex->x, t),
                Lerp(pPrevVertex->y, pCurrVertex->y, t),
                Lerp(pPrevVertex->z, pCurrVertex->z, t),
            };

            // use the lerp formula to get the interpolated U and V
            Tex2 interpolatedTexCoord = 
            {
                .u = Lerp(pPrevTexCoord->u, pCurrTexCoord->u, t),
                .v = Lerp(pPrevTexCoord->v, pCurrTexCoord->v, t)
            };
            
            // insert the new intersection point to the list of "inside vertices"
            insideVertices[numInsideVertices] = intersectionPoint;
            insideTexCoords[numInsideVertices] = interpolatedTexCoord;
            numInsideVertices++;
        }

        // if current point is inside the plane
        if (currDot > 0)
        {
            // insert current vertex in the list of "inside vertices"
            insideVertices[numInsideVertices]  = *pCurrVertex;
            insideTexCoords[numInsideVertices] = *pCurrTexCoord;
            numInsideVertices++;
        }

        // move to the next vertex
        prevDot = currDot;
        pPrevVertex = pCurrVertex;
        pPrevTexCoord = pCurrTexCoord;
        pCurrVertex++;
        pCurrTexCoord++;
    }

    // copy all the vertices/textures from the insideVertices/insideTexCoords into the destination polygon parameter
    for (int i = 0; i < numInsideVertices; ++i)
        pPolygon->vertices[i] = insideVertices[i];

    for (int i = 0; i < numInsideVertices; ++i)
        pPolygon->texCoords[i] = insideTexCoords[i];

    pPolygon->numVertices = numInsideVertices;
}

///////////////////////////////////////////////////////////

void ClipPolygon(Polygon* pPolygon)
{
    // clip input polygon agains all the 6 frustum planes
    
    ClipPolygonAgainstPlane(pPolygon, LEFT_FRUSTUM_PLANE);
    ClipPolygonAgainstPlane(pPolygon, RIGHT_FRUSTUM_PLANE);

    ClipPolygonAgainstPlane(pPolygon, TOP_FRUSTUM_PLANE);
    ClipPolygonAgainstPlane(pPolygon, BOTTOM_FRUSTUM_PLANE);

    ClipPolygonAgainstPlane(pPolygon, NEAR_FRUSTUM_PLANE);
    ClipPolygonAgainstPlane(pPolygon, FAR_FRUSTUM_PLANE);
}

///////////////////////////////////////////////////////////

void CreateTrianglesFromPolygon(
    const Polygon* pPolygon, 
    Triangle* triangles, 
    int* pNumTriangles)
{
    // split input polygon into triangles;
    //
    // out: triangles   -- array of created triangles;
    //      numTriangle -- the number of created triangles

    *pNumTriangles = pPolygon->numVertices - 2;

    for (int i = 0; i < *pNumTriangles; ++i)
    {
        const Vec3 v0 = pPolygon->vertices[0];
        const Vec3 v1 = pPolygon->vertices[i + 1];
        const Vec3 v2 = pPolygon->vertices[i + 2];

        triangles[i].points[0] = (Vec4){ v0.x, v0.y, v0.z, 1.0f };
        triangles[i].points[1] = (Vec4){ v1.x, v1.y, v1.z, 1.0f };
        triangles[i].points[2] = (Vec4){ v2.x, v2.y, v2.z, 1.0f };

        triangles[i].texCoords[0] = pPolygon->texCoords[0];
        triangles[i].texCoords[1] = pPolygon->texCoords[i + 1];
        triangles[i].texCoords[2] = pPolygon->texCoords[i + 2];
    }
}
