// ==================================================================
// Filename: obj_loader.c
//
// Created:  14.03.2025  by DimaSkup
// ==================================================================
#include "obj_loader.h"
#include "array.h"
#include <string.h>
#include <assert.h>

// debug flags (are used to show the first and the last element of data block; for instance: print the first and last vertices values)
#define PRINT_OBJ_VERTICES_DEBUG_INFO 0
#define PRINT_OBJ_TEXTURES_DEBUG_INFO 0
#define PRINT_OBJ_NORMALS_DEBUG_INFO 0
#define PRINT_OBJ_FACES_DEBUG_INFO 0
#define BUFFER_SIZE 64

// TODO: debug functions prototypes
void DebugVertices(Vec3* vertices);
void DebugTexCoords(Vec2* texCoords);
void DebugNormals(Vec3* normals);
void DebugFaces(Face* faces);

///////////////////////////////////////////////////////////

Vec3* ReadVerticesData(FILE* pFile, char* buffer)
{
    // read in vertices data into arr and return a ptr to this arr

    Vec3* vertices = NULL;
    Vec3 vertex;
    const int bufsize = BUFFER_SIZE;


    // read in vertices data
    while (strncmp(buffer, "v ", 2) == 0)
    {
        sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
        ArrayPush(vertices, vertex);
        fgets(buffer, bufsize, pFile);
    }

    // we're read a line which isn't vertex data but maybe some another data so we just step back by the length of this line
    //fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_VERTICES_DEBUG_INFO
    int lastIdx = ArrayLength(vertices) - 1;
    printf("first v: %f %f %f\n", vertices[0].x, vertices[0].y, vertices[0].z);
    printf("last  v: %f %f %f\n", vertices[lastIdx].x, vertices[lastIdx].y, vertices[lastIdx].z);
#endif

    return vertices;
}

///////////////////////////////////////////////////////////

Vec2* ReadTexCoordsData(FILE* pFile, char* buffer)
{
    // read in textures data into arr and return a ptr to this arr

    const int bufsize = BUFFER_SIZE;
    Vec2  tex          = {0,0};
    Vec2* texCoords   = NULL;


    // while we're reading the texture coords data
    while (strncmp(buffer, "vt ", 3) == 0) 
    {
        sscanf(buffer, "vt %f %f", &tex.x, &tex.x);
        ArrayPush(texCoords, tex); 
        fgets(buffer, bufsize, pFile);
    } 

    // we're read a line which isn't texture coords data but maybe some another data so we just step back by the length of this line
    //fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_TEXTURES_DEBUG_INFO
    int lastIdx = ArrayLength(texCoords) - 1;
    printf("first vt: %f %f \n", texCoords[0].u, texCoords[0].v);
    printf("last  vt: %f %f \n", texCoords[lastIdx].u, texCoords[lastIdx].v);
#endif

    return texCoords;
}

///////////////////////////////////////////////////////////

Vec3* ReadNormalsData(FILE* pFile, char* buffer)
{
    // read in normals data into arr and return a ptr to this arr

    const int bufsize = BUFFER_SIZE;
    Vec3* normals = NULL;
    Vec3 normal;


    // while we're reading normals data
    while (strncmp(buffer, "vn", 2) == 0)
    {
        sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
        ArrayPush(normals, normal);
        fgets(buffer, bufsize, pFile);
    }

    // we're read a line which isn't normals data but maybe some another data so we just step back by the length of this line
    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_NORMALS_DEBUG_INFO
    int lastIdx = ArrayLength(normals) - 1;
    printf("first vn: %f %f %f \n", normals[0].x, normals[0].y, normals[0].z);
    printf("last  vn: %f %f %f \n", normals[lastIdx].x, normals[lastIdx].y, normals[lastIdx].z);
#endif

    return normals;
}

///////////////////////////////////////////////////////////

void ComputeNormals( 
    const Vec3* vertices, 
    const int numVertices,
    Vec3** normals)
{
    assert((vertices != NULL) && "input ptr to vertices == NULL");
    assert((numVertices > 0) && "input number of vertices must be > 0");
    printf("num vertices == %d\n", numVertices);
    assert((numVertices % 3 == 0) && "invalid input args");    

    // go through vertices and for each 3 vertices (triangle) compute normal vector
    for (int i = 0; i < numVertices;)
    {
        const Vec3 v0 = vertices[i++];
        const Vec3 v1 = vertices[i++];
        const Vec3 v2 = vertices[i++];

#if 0
        // find triangle edge vectors
        Vec3 vec01 = { v1.x - x0.x, v1.y - v0.y, v1.z - v0.z };
        Vec3 vec02 = { v2.x - x0.x, v2.y - v0.y, v2.z - v0.z };

        // compute inverse length of vectors
        const float invLength01 = 1.0f / sqrtf(vec01.x*vec01.x + vec01.y*vec01.y + vec01.z*vec01.z);
        const float invLength02 = 1.0f / sqrtf(vec02.x*vec02.x + vec02.y*vec02.y + vec02.z*vec02.z);

        // normalize edge vectors
        vec01.x *= invLen01;
        vec01.y *= invLen01;
        vec01.z *= invLen01;

        vec02.x *= invLen02;
        vec02.y *= invLen02;
        vec02.z *= invLen02;
#endif
        // compute vectors: v1-v0 and v2-v0
        Vec3 vec01 = Vec3Sub(v1, v0);
        Vec3 vec02 = Vec3Sub(v2, v0);

        // compute the triangle's normal vector
        Vec3 normal = Vec3Cross(vec01, vec02);
        Vec3Normalize(&normal);

        ArrayPush(*normals, normal);
    }
}


///////////////////////////////////////////////////////////

Face* ReadFacesData(
    FILE* pFile,
    Vec3* vertices, 
    Vec2* texCoords, 
    Vec3** normals,
    char* buffer,
    const int numVertices)
{
    // read in faces data into arr and return a ptr to this arr

    assert(pFile     && "input ptr to file == NULL");
    assert(vertices  && "input ptr to vertices == NULL");
    assert(texCoords && "input ptr to texture coords == NULL");
    //assert(normals   && "input ptr to normals == NULL");
    assert(buffer    && "input ptr to buffer == NULL");

    // if we didn't load any normals data before we compute them manually
    if (*normals == NULL)
    {
        //ComputeNormals(vertices, numVertices, normals);
    }

    char* result = buffer;
    int bufsize = BUFFER_SIZE;
    int texIdxs[3];          // texture coords index
    int normIdxs[3];         // normal vector index
    Face* faces = NULL;
    //int *normalIdxs = NULL;  // array of indices to normal-vectors
    Face face;

    // while we're reading the faces data
    while ((strncmp(buffer, "f ", 2) == 0) && result)
    {
        sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &face.a, &texIdxs[0], &normIdxs[0],  // the first point of the face
            &face.b, &texIdxs[1], &normIdxs[1],  // the second point
            &face.c, &texIdxs[2], &normIdxs[2]); // the third point

        // make index correction
        face.a--;
        face.b--;
        face.c--;

        face.aUV = texCoords[texIdxs[0] - 1];
        face.bUV = texCoords[texIdxs[1] - 1];
        face.cUV = texCoords[texIdxs[2] - 1];

        face.normal = (*normals)[normIdxs[0]]; 

        // flip the V component to account for inverted UV-coords (V grows downwards)
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

