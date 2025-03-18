// ==================================================================
// Filename: obj_loader.c
//
// Created:  14.03.2025  by DimaSkup
// ==================================================================
#include "obj_loader.h"
#include "array.h"
#include <string.h>
#include <assert.h>

#define PRINT_OBJ_VERTICES_DEBUG_INFO 0
#define PRINT_OBJ_TEXTURES_DEBUG_INFO 0
#define PRINT_OBJ_NORMALS_DEBUG_INFO 0
#define PRINT_OBJ_FACES_DEBUG_INFO 0
#define BUFFER_SIZE 64


Vec3* ReadVerticesData(FILE* pFile)
{
    // read in vertices data into arr and return a ptr to this arr

    Vec3* vertices = NULL;
    Vec3 vertex;
    char buffer[BUFFER_SIZE] = {0};
    const int bufsize = BUFFER_SIZE;

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

Tex2* ReadTexCoordsData(FILE* pFile)
{
    // read in textures data into arr and return a ptr to this arr

    char buffer[BUFFER_SIZE] = {0};
    const int bufsize        = BUFFER_SIZE;
    Tex2 tex                 = {0,0};
    Tex2* texCoords          = NULL;

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

    return texCoords;
}
///////////////////////////////////////////////////////////

Vec3* ReadNormalsData(FILE* pFile)
{
    // read in normals data into arr and return a ptr to this arr

    const int bufsize = BUFFER_SIZE;
    char buffer[BUFFER_SIZE] = {0};
    char* result = buffer;
    Vec3* normals = NULL;
    Vec3 normal;

    // skip until the normals data block
    while ((strncmp(buffer, "vn", 2) != 0) && result)
        result = fgets(buffer, bufsize, pFile);

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

Face* ReadFacesData(FILE* pFile, Tex2* texCoords)
{
    // read in faces data into arr and return a ptr to this arr

    assert((pFile) && (texCoords));

    char buffer[BUFFER_SIZE] = {0};   // read in a full line into this buffer
    char* result = buffer;
    int bufsize = BUFFER_SIZE;
    int texIdxs[3];          // texture coords index
    int normIdxs[3];         // normal vector index
    Face* faces = NULL;
    Face face;


    // skip until the faces data (with reading of the first line)
    while ((strncmp(buffer, "f ", 2) != 0) && result) 
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

        face.aUV = texCoords[texIdxs[0] - 1];
        face.bUV = texCoords[texIdxs[1] - 1];
        face.cUV = texCoords[texIdxs[2] - 1];

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


// ==================================================================
// ==================================================================
// ==================================================================
// ==================================================================

Vec3* ReadVerticesData2(FILE* pFile, char* buffer)
{
    // read in vertices data into arr and return a ptr to this arr

    Vec3* vertices = NULL;
    Vec3 vertex;
    //char buffer[BUFFER_SIZE] = {0};
    const int bufsize = BUFFER_SIZE;

    // skip until the vertices data (with reading of the first line)
    //while (strncmp(buffer, "v ", 2) != 0)
    //    fgets(buffer, bufsize, pFile);

    // read in vertices data
    while (strncmp(buffer, "v ", 2) == 0)
    {
        sscanf(buffer, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
        ArrayPush(vertices, vertex);
        fgets(buffer, bufsize, pFile);
    }

    //fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_VERTICES_DEBUG_INFO
    int lastIdx = ArrayLength(vertices) - 1;
    printf("first v: %f %f %f\n", vertices[0].x, vertices[0].y, vertices[0].z);
    printf("last  v: %f %f %f\n", vertices[lastIdx].x, vertices[lastIdx].y, vertices[lastIdx].z);
#endif

    return vertices;
}

///////////////////////////////////////////////////////////

Tex2* ReadTexCoordsData2(FILE* pFile, char* buffer)
{
    // read in textures data into arr and return a ptr to this arr

    ///char buffer[BUFFER_SIZE] = {0};
    const int bufsize        = BUFFER_SIZE;
    Tex2 tex                 = {0,0};
    Tex2* texCoords          = NULL;

    // skip until the texture coords data
    //while (strncmp(buffer, "vt ", 3) != 0)
    //    fgets(buffer, bufsize, pFile);

    printf("TEX %s\n", buffer);

    // while we're reading the texture coords data
    while (strncmp(buffer, "vt ", 3) == 0) 
    {
        sscanf(buffer, "vt %f %f", &tex.u, &tex.v);
        ArrayPush(texCoords, tex); 
        fgets(buffer, bufsize, pFile);
    } 

    //fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

#if PRINT_OBJ_TEXTURES_DEBUG_INFO
    int lastIdx = ArrayLength(texCoords) - 1;
    printf("first vt: %f %f \n", texCoords[0].u, texCoords[0].v);
    printf("last  vt: %f %f \n", texCoords[lastIdx].u, texCoords[lastIdx].v);
#endif

    return texCoords;
}

///////////////////////////////////////////////////////////

Face* ReadFacesData2(FILE* pFile, Tex2* texCoords, char* buffer)
{
    // read in faces data into arr and return a ptr to this arr

    assert((pFile) && (texCoords));

//    char buffer[BUFFER_SIZE] = {0};   // read in a full line into this buffer
    char* result = buffer;
    int bufsize = BUFFER_SIZE;
    int texIdxs[3];          // texture coords index
    int normIdxs[3];         // normal vector index
    Face* faces = NULL;
    Face face;


    // skip until the faces data (with reading of the first line)
    //while ((strncmp(buffer, "f ", 2) != 0) && result) 
    //    result = fgets(buffer, bufsize, pFile);

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

        face.aUV = texCoords[texIdxs[0] - 1];
        face.bUV = texCoords[texIdxs[1] - 1];
        face.cUV = texCoords[texIdxs[2] - 1];

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

