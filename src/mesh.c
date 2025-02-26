#include "mesh.h"
#include "array.h"
#include "console_color.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PRINT_OBJ_VERTICES_DEBUG_INFO false
#define PRINT_OBJ_TEXTURES_DEBUG_INFO false
#define PRINT_OBJ_NORMALS_DEBUG_INFO  false
#define PRINT_OBJ_FACES_DEBUG_INFO    false

Mesh g_Mesh = 
{
    .vertices    = NULL,
    .tex         = NULL,
    .normals     = NULL,
    .faces       = NULL, 
    .scale       = {1,1,1},
    .rotation    = {0,0,0},
    .translation = {0,0,0},
    .numFaces    = 0,
};



// ==================================================================
//                       cube global data
// ==================================================================

Tex2* texCoords = NULL;

Vec3 gCubeVertices[N_CUBE_VERTICES] = 
{
    {-1, -1, -1},  // 1
    {-1,  1, -1},  // 2
    { 1,  1, -1},  // 3
    { 1, -1, -1},  // 4
    { 1,  1,  1},  // 5
    { 1, -1,  1},  // 6
    {-1,  1,  1},  // 7
    {-1, -1,  1},  // 8
};

Face gCubeFaces[N_CUBE_FACES] =
{
    // front
    {1,2,3, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFFFF0000}, 
    {1,3,4, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFFFF0000},

    // right
    {4,3,5, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFF00FF00}, 
    {4,5,6, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFF00FF00},

    // back
    {6,5,7, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFF0000FF}, 
    {6,7,8, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFF0000FF},

    // left
    {8,7,2, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFFFFFF00}, 
    {8,2,1, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFFFFFF00},

    // top
    {2,7,5, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFFFF00FF}, 
    {2,5,3, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFFFF00FF},

    // bottom
    {6,8,1, .aUV = {0,1}, .bUV = {0,0}, .cUV = {1,0}, .color = 0xFF00FFFF}, 
    {6,1,4, .aUV = {0,1}, .bUV = {1,0}, .cUV = {1,1}, .color = 0xFF00FFFF},
};

///////////////////////////////////////////////////////////

void InitEmptyMesh(Mesh* pMesh)
{
    pMesh->vertices = NULL;
    pMesh->tex      = NULL;
    pMesh->normals  = NULL;
    pMesh->faces    = NULL;
    pMesh->rotation = (Vec3){0,0,0};
}

///////////////////////////////////////////////////////////

void LoadCubeMeshData(void)
{
    // load vertices
    for (int i = 0; i < N_CUBE_VERTICES; i++)
        ArrayPush(g_Mesh.vertices, gCubeVertices[i]);

    // load faces
    for (int i = 0; i < N_CUBE_FACES; i++)
        ArrayPush(g_Mesh.faces, gCubeFaces[i]);

    g_Mesh.numFaces = N_CUBE_FACES;
}



// ==================================================================
// .obj loader functions (entry point: LoadObjFileData())
// ==================================================================

Vec3* ReadVerticesData(FILE* pFile)
{
    // read in vertices data into arr and return a ptr to this arr

    char buffer[64] = {0};
    const int bufsize = 64;
    Vec3* vertices = NULL;
    Vec3 vertex;


    // skip until the vertices data (with reading of the first line)
    while (strncmp(buffer, "v ", 2) != 0)
        fgets(buffer, bufsize, pFile);

    // read in vertices data
    while (strncmp(buffer, "v ", 2) == 0)
    {
        sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
        ArrayPush(vertices, vertex);
        fgets(buffer, bufsize, pFile);
    }

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_VERTICES_DEBUG_INFO
    int lastIdx = ArrayLength(vertices) - 1;
    printf("first v: %f %f %f\n", vertices[0].x, vertices[0].y, vertices[0].z);
    printf("last  v: %f %f %f\n", vertices[lastIdx].x, vertices[lastIdx].y, vertices[lastIdx].z);
#endif

    return vertices;
}

///////////////////////////////////////////////////////////

void ReadTexCoordsData(FILE* pFile)
{
    // read in textures data into arr and return a ptr to this arr

    char buffer[64]   = {0};
    const int bufsize = 64;
    Tex2 tex          = {0,0};         // tex coords

    // skip until the texture coords data
    while (strncmp(buffer, "vt ", 3) != 0)
        fgets(buffer, bufsize, pFile);

    // while we're reading the texture coords data
    while (strncmp(buffer, "vt ", 3) == 0) 
    {
        sscanf(buffer, "vt %f %f", &tex.u, &tex.v);
        ArrayPush(texCoords, tex); 
        fgets(buffer, bufsize, pFile);
    } 

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_TEXTURES_DEBUG_INFO
    int lastIdx = ArrayLength(texCoords) - 1;
    printf("first vt: %f %f \n", texCoords[0].u, texCoords[0].v);
    printf("last  vt: %f %f \n", texCoords[lastIdx].u, texCoords[lastIdx].v);
#endif

    //return texCoords;
}

///////////////////////////////////////////////////////////

Vec3* ReadNormalsData(FILE* pFile)
{
    // read in normals data into arr and return a ptr to this arr

    char buffer[64] = {0};
    const int bufsize = 64;
    Vec3* normals = NULL;
    Vec3 normal;

    // skip until the normals data block
    while (strncmp(buffer, "vn", 2) != 0)
        fgets(buffer, bufsize, pFile);

    // while we're reading normals data
    while (strncmp(buffer, "vn", 2) == 0)
    {
        sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
        ArrayPush(normals, normal);
        fgets(buffer, bufsize, pFile);
    }

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_NORMALS_DEBUG_INFO
    int lastIdx = ArrayLength(normals) - 1;
    printf("first vn: %f %f %f \n", normals[0].x, normals[0].y, normals[0].z);
    printf("last  vn: %f %f %f \n", normals[lastIdx].x, normals[lastIdx].y, normals[lastIdx].z);
#endif

    return normals;
}

///////////////////////////////////////////////////////////

Face* ReadFacesData(FILE* pFile)
{
    // read in faces data into arr and return a ptr to this arr

    assert((pFile != NULL) && (texCoords != NULL));

    char* result = NULL;
    char buffer[64] = {0};   // read in a full line into this buffer
    int bufsize = 64;
    int texIdxs[3];          // texture coords index
    int normIdxs[3];         // normal vector index
    Face* faces = NULL;
    Face face;


    // skip until the faces data (with reading of the first line)
    while (strncmp(buffer, "f ", 2) != 0) 
        result = fgets(buffer, bufsize, pFile);

    // while we're reading the faces data
    while ((strncmp(buffer, "f ", 2) == 0) && result)
    {
        sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &face.a, &texIdxs[0], &normIdxs[0],  // the first point of the face
            &face.b, &texIdxs[1], &normIdxs[1],  // the second point
            &face.c, &texIdxs[2], &normIdxs[2]); // the third point

        face.a--;
        face.b--;
        face.c--;

        // flip the V component to account for inverted UV-coords (V grows downwards)
//    v0 = 1.0f - v0;
 //   v1 = 1.0f - v1;
   // v2 = 1.0f - v2;

        face.aUV = texCoords[texIdxs[0] - 1];
        face.bUV = texCoords[texIdxs[1] - 1];
        face.cUV = texCoords[texIdxs[2] - 1];

        face.aUV.v = 1.0f - face.aUV.v;
        face.bUV.v = 1.0f - face.bUV.v;
        face.cUV.v = 1.0f - face.cUV.v;

        face.color = 0xFFFFFFFF;
        ArrayPush(faces, face);
        result = fgets(buffer, bufsize, pFile);  // read in a new line
    }

#if PRINT_OBJ_FACES_DEBUG_INFO
    const int lastIdx = ArrayLength(faces) - 1;
    printf("face first (vertices): %d %d %d\n", faces[0].a, faces[0].b, faces[0].c);
    printf("face last  (vertices): %d %d %d\n", faces[lastIdx].a, faces[lastIdx].b, faces[lastIdx].c);
#endif

    return faces;
}

///////////////////////////////////////////////////////////

bool LoadObjFileData(const char* filepath)
{
    // read the contents of the .obj file
    // and load the vertices and faces in 
    // our mesh.vertices and mesh.faces

    printf("Try to load an .obj file: %s\n", filepath);

    FILE* pFile = fopen(filepath, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "error opening .obj file");
        return false;
    }     

    g_Mesh.vertices = ReadVerticesData(pFile);
    ReadTexCoordsData(pFile);
    g_Mesh.normals  = ReadNormalsData(pFile);
    g_Mesh.faces    = ReadFacesData(pFile);
    g_Mesh.numFaces = ArrayLength(g_Mesh.faces);
    
    //if (texCoords != NULL)
        ArrayFree((void**)&texCoords);

    fclose(pFile);

    return true; 
}
