// ==================================================================
// Filename:    clipping.h
// Description: frustum clipping stuff:
//              1. we discard vertices and triangles which are
//                 out of our frustum volume;
//              2. if triangle intersect frustum plane we split
//                 it into other triangle and discard that part 
//                 of the origin triangle which is out of frustum;
//              3. triangles which are inside the frustum volume
//                 remain the same
//
// Created:     05.03.25  by DimaSkup
// ==================================================================
#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"
#include "triangle.h"

#define MAX_NUM_POLYGON_VERTICES  10
#define MAX_NUM_POLYGON_TRIANGLES 8   // MAX_NUM_POLYGON_VERTICES - 2

enum 
{
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct
{
    Vec3 point;
    Vec3 normal;
} Plane;

typedef struct
{
    Vec3 vertices[MAX_NUM_POLYGON_VERTICES];
    Tex2 texCoords[MAX_NUM_POLYGON_VERTICES];
    int numVertices;
} Polygon;


void InitFrustumPlanes(
    const float fovX, 
    const float fovY, 
    const float nearZ, 
    const float farZ);

Polygon CreatePolygonFromTriangle(
    const Vec4 v0, 
    const Vec4 v1, 
    const Vec4 v2,
    const Tex2 t0,
    const Tex2 t1, 
    const Tex2 t2);

void ClipPolygon(Polygon* pPolygon);
void ClipPolygonAgainstPlane(Polygon* pPolygon, const int planeType);

void CreateTrianglesFromPolygon(
    const Polygon* pPolygon, 
    Triangle* triangles, 
    int* pNumTriangles);

#endif
