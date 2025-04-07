// ==================================================================
// Filename:    model.h
// Description: declaration of the game object (model) type and
//              related basic functional
//
// Created:     04.04.2025 by DimaSkup
// ==================================================================
#ifndef MODEL_H
#define MODEL_H

#include "common_types.h"
#include "vector.h"
#include "geomtypes.h"
#include <stdint.h>


// defines for objects
#define OBJECT_MAX_NUM_VERTICES      1024
#define OBJECT_MAX_NUM_FACES         1024

// states for objects
#define OBJECT_STATE_ACTIVE          0x0001
#define OBJECT_STATE_VISIBLE         0x0002
#define OBJECT_STATE_CULLED          0x0004


// ==================================================================
// define a struct for dynamic size meshes, 
// with arr of vertices and faces
// ==================================================================
typedef struct 
{
    char name[32];
    uint32_t vertexStart;
    uint32_t vertexCount;
    uint32_t indexStart;
    uint32_t indexCount;
                                    
    TextureID texID;                                                     

    Vec3  scale;
    Vec3  rotation;                 
    Vec3  translation;
} Mesh;

// ==================================================================
// functions prototypes
// ==================================================================
typedef struct
{
    // object, based on the vertices array and indices array

    uint32_t id;              // numerical ID of the object
    char     name[64];        // ASCII-name of the object
    uint32_t state;       
    uint32_t attr;            // attributes of the object
    float    avgRadius;       // average radius of the object to detect collision
    float    maxRadius;       // maximal radius of the object

    Vec4     worldPos;        // position of the object in the world
    Vec4     dir;             // angles which defines an object orientation in local space, or normalized direction vector set by user
    Vec4     ux,uy,uz;        // local axis to define orientation. When we call rotation functions these params are automatically updated

    uint32_t numVertices;     // number of vertices
    uint32_t numIndices;

    Vec3     verticesLocal[OBJECT_MAX_NUM_VERTICES];  // array of vertices in local space
    Vec3     verticesTrans[OBJECT_MAX_NUM_VERTICES];  // array of vertices after transformation                                                      

    uint32_t numFaces;                                // number of faces
    Face     faces[OBJECT_MAX_NUM_FACES];             // array of faces
                                                      //
} Model;


// ==================================================================
// functions prototypes
// ==================================================================
void InitEmptyMesh(Mesh* pMesh);
void InitEmptyModel(Model* pModel);


#endif
