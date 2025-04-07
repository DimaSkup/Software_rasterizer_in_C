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
#include "geomtypes.h"

enum 
{
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

///////////////////////////////////////////////////////////

void InitFrustumPlanes(
    const float fovX, 
    const float fovY, 
    const float nearZ, 
    const float farZ);

Polygon CreatePolygonFromTriangle(
    const Vec3 v0,                    // vertex 0 
    const Vec3 v1, 
    const Vec3 v2,
    const Vec2 t0,
    const Vec2 t1, 
    const Vec2 t2);

void ClipPolygon(Polygon* pPolygon);
void ClipPolygonAgainstPlane(Polygon* pPolygon, const int planeType);

void CreateTrianglesFromPolygon(
    const Polygon* pPolygon, 
    Triangle* triangles, 
    int* pNumTriangles);

#endif
