#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include <stdbool.h>


#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)                // 6 cube faces, 2 triangle per face

extern Vec3 gCubeVertices[N_CUBE_VERTICES]; 
extern Face gCubeFaces[N_CUBE_FACES];       

// ==================================================================
// define a struct for dynamic size meshes, 
// with arr of vertices and faces
// ==================================================================
typedef struct 
{
    Vec3* vertices;                 // dynamic arr of vertices
    Vec2* tex;                      // texture UV coords
    Vec3* normals;                  // normal vectors
    Face* faces;                    // dynamic arr of faces
    Vec3  scale;
    Vec3  rotation;                 // rotation with x,y,z values
    Vec3  translation;
    int   numFaces;
} Mesh;

extern Mesh g_Mesh;


// ==================================================================
// functions prototypes
// ==================================================================
void InitEmptyMesh(Mesh* pMesh);
void LoadCubeMeshData(void);
bool LoadObjFileData(const char* filepath);

void DebugVertices(Vec3* vertices);
void DebugTexCoords(Vec2* texCoords);
void DebugNormals(Vec3* normals);
void DebugFaces(Face* faces);

#endif
