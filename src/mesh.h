#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

// ==================================================================
// define a struct for dynamic size meshes, 
// with arr of vertices and faces
// ==================================================================
typedef struct 
{
    Vec3* vertices;                 // dynamic arr of vertices
    Tex2* texCoords;                 // texture UV coords
    Vec3* normals;                  // normal vectors
                                    
    Face* faces;                    // dynamic arr of faces
    upng_t* pTexture;                // PNG texture pointer for mesh                                    

    Vec3  scale;
    Vec3  rotation;                 
    Vec3  translation;
    int   numFaces;
} Mesh;


// ==================================================================
// functions prototypes
// ==================================================================
void InitEmptyMesh(Mesh* pMesh);

Mesh* GetMeshPtrByIdx(const int meshIdx);
int GetNumMeshes(void);

void LoadMesh(
    const char* fileDataPath, 
    const char* texturePath,
    const Vec3 scale,
    const Vec3 translation,
    const Vec3 rotation);

int  LoadObjFileData(Mesh* pMesh, const char* filepath);

void DebugVertices(Vec3* vertices);
void DebugTexCoords(Vec2* texCoords);
void DebugNormals(Vec3* normals);
void DebugFaces(Face* faces);

#endif
