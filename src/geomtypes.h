// ==================================================================
// Filename:    geomtypes.h
// Description: declarations of different geometry primitives
//
// Created:     02.04.2025 by DimaSkup
// ==================================================================

#ifndef GEOMTYPES_H
#define GEOMTYPES_H

#include "vector.h"
#include <stdint.h>

// attributes of polygons and surfaces
#define FACE_ATTR_2SIDED             0x0001
#define FACE_ATTR_TRANSPARENT        0x0002
#define FACE_ATTR_8BITCOLOR          0x0004
#define FACE_ATTR_RGB16              0x0008
#define FACE_ATTR_RGB24              0x0010

#define FACE_ATTR_SHADE_MODE_PURE    0x0020
#define FACE_ATTR_SHADE_MODE_FLAT    0x0040
#define FACE_ATTR_SHADE_MODE_GOURAUD 0x0080
#define FACE_ATTR_SHADE_MODE_PHONG   0x0100

// states of polygons and sufraces
#define FACE_STATE_ACTIVE            0x0001
#define FACE_STATE_CLIPPED           0x0002
#define FACE_STATE_BACKFACE          0x0004

#define MAX_NUM_POLYGON_VERTICES     10
#define MAX_NUM_POLYGON_TRIANGLES    8   // MAX_NUM_POLYGON_VERTICES - 2


typedef struct 
{
    // stores 3 vertices data
    uint32_t state;          // info about face state
    uint32_t attr;           // physical attributes of the face
    uint32_t color;

    uint32_t  idxs[3];       // indices to the vertices array
    Vec2      texCoords[3];  // texture coordinates
    Vec3      normals[3];    // normal vectors
} Face;

///////////////////////////////////////////////////////////

typedef struct 
{
    // stores the actual Vec4 points of the triangle which will be rendered
    Vec4 points[3];
    Vec2 texCoords[3];
    uint32_t color;
    float lightIntensity;   // over the triangle
    uint32_t textureID; 
} Triangle;

///////////////////////////////////////////////////////////

typedef struct
{
    Vec3 point;
    Vec3 normal;
} Plane;

///////////////////////////////////////////////////////////

typedef struct
{
    Vec3 vertices[MAX_NUM_POLYGON_VERTICES];
    Vec2 texCoords[MAX_NUM_POLYGON_VERTICES];
    int numVertices;
} Polygon;

///////////////////////////////////////////////////////////

#endif
