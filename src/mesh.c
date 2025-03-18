#include "mesh.h"
#include "array.h"
#include "console_color.h"
#include "obj_loader.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// defines which are used for debugging of .obj files loading
#define PRINT_OBJ_VERTICES_DEBUG_INFO false
#define PRINT_OBJ_TEXTURES_DEBUG_INFO true
#define PRINT_OBJ_NORMALS_DEBUG_INFO  0
#define PRINT_OBJ_FACES_DEBUG_INFO    1

#define BUFFER_SIZE 64

#define MAX_NUM_MESHES 10
static Mesh s_Meshes[MAX_NUM_MESHES];
static int s_NumMeshes = 0;

///////////////////////////////////////////////////////////

void InitEmptyMesh(Mesh* pMesh)
{
    pMesh->vertices    = NULL;
    pMesh->texCoords   = NULL;
    pMesh->normals     = NULL;
    pMesh->faces       = NULL;
    pMesh->pTexture    = NULL;
    pMesh->scale       = (Vec3){ 1,1,1 };
    pMesh->rotation    = (Vec3){ 0,0,0 };
    pMesh->translation = (Vec3){ 0,0,0 };
}

///////////////////////////////////////////////////////////

Mesh* GetMeshPtrByIdx(const int meshIdx)
{
    if (meshIdx < 0 || meshIdx >= s_NumMeshes)
        return NULL;

    return &(s_Meshes[meshIdx]);
}

///////////////////////////////////////////////////////////

int GetNumMeshes(void)
{
    return s_NumMeshes;
}

///////////////////////////////////////////////////////////

void LoadMesh(
    const char* fileDataPath, 
    const char* texturePath,
    const Vec3 scale,
    const Vec3 translation,
    const Vec3 rotation)
{
    assert((fileDataPath != NULL) && (texturePath != NULL) && "invalid input args");

    Mesh* pMesh = &(s_Meshes[s_NumMeshes]);

    // load mesh data from the file
    int result = LoadObjFileData(pMesh, fileDataPath);
    if (result == -1)
    {
        printf("\nERROR: can't read in .obj file data: %s\n", fileDataPath);
        return;
    }

    // load mesh texture
    LoadPngTextureData(&(pMesh->pTexture), texturePath);

    // initialize scale, translation, and rotation
    pMesh->scale = scale;
    pMesh->translation = translation;
    pMesh->rotation = rotation;

    s_NumMeshes++;
}



// ==================================================================
// .obj loader functions (entry point: LoadObjFileData())
// ==================================================================
#if 0
Vec3* ReadVerticesData(FILE* pFile, char* buffer)
{
    // read in vertices data into arr and return a ptr to this arr

    Vec3* vertices = NULL;
    Vec3 vertex;
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

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

//#if PRINT_OBJ_VERTICES_DEBUG_INFO
    int lastIdx = ArrayLength(vertices) - 1;
    printf("first v: %f %f %f\n", vertices[0].x, vertices[0].y, vertices[0].z);
    printf("last  v: %f %f %f\n", vertices[lastIdx].x, vertices[lastIdx].y, vertices[lastIdx].z);
//#endif

    return vertices;
}

///////////////////////////////////////////////////////////

Tex2* ReadTexCoordsData(FILE* pFile, char* buffer)
{
    // read in textures data into arr and return a ptr to this arr

    //char buffer[64]   = {0};
    const int bufsize = BUFFER_SIZE;
    Tex2 tex          = {0,0};
    Tex2* texCoords   = NULL;

    // skip until the texture coords data
    //while (strncmp(buffer, "vt ", 3) != 0)
    //    fgets(buffer, bufsize, pFile);

    // while we're reading the texture coords data
    while (strncmp(buffer, "vt ", 3) == 0) 
    {
        sscanf(buffer, "vt %f %f", &tex.u, &tex.v);
        ArrayPush(texCoords, tex); 
        fgets(buffer, bufsize, pFile);
    } 

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

//#if PRINT_OBJ_TEXTURES_DEBUG_INFO
    int lastIdx = ArrayLength(texCoords) - 1;
    printf("first vt: %f %f \n", texCoords[0].u, texCoords[0].v);
    printf("last  vt: %f %f \n", texCoords[lastIdx].u, texCoords[lastIdx].v);
//#endif

    return texCoords;
}

///////////////////////////////////////////////////////////

Vec3* ReadNormalsData(FILE* pFile, char* buffer)
{
    // read in normals data into arr and return a ptr to this arr

    const int bufsize = BUFFER_SIZE;
    //char buffer[64] = {0};
    //char* result = buffer;
    Vec3* normals = NULL;
    Vec3 normal;

    // skip until the normals data block
    //while (strncmp(fgets(buffer, bufsize, pFile), "vn", 2) != 0)
    //while ((strncmp(buffer, "vn", 2) != 0) && result)
    //    result = fgets(buffer, bufsize, pFile);

    // while we're reading normals data
    while (strncmp(buffer, "vn", 2) == 0)
    {
        sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
        ArrayPush(normals, normal);
        fgets(buffer, bufsize, pFile);
    }

    fseek(pFile, ftell(pFile) - strlen(buffer), SEEK_SET);

//#if PRINT_OBJ_NORMALS_DEBUG_INFO
    int lastIdx = ArrayLength(normals) - 1;
    printf("first vn: %f %f %f \n", normals[0].x, normals[0].y, normals[0].z);
    printf("last  vn: %f %f %f \n", normals[lastIdx].x, normals[lastIdx].y, normals[lastIdx].z);
//#endif

    return normals;
}

///////////////////////////////////////////////////////////

Face* ReadFacesData(FILE* pFile, Mesh* pMesh, char* buffer)
{
    // read in faces data into arr and return a ptr to this arr

    assert((pFile) && (pMesh) && (pMesh->texCoords));

//    char buffer[64] = {0};   // read in a full line into this buffer
    char* result = buffer;
    int bufsize = BUFFER_SIZE;
    int texIdxs[3];          // texture coords index
    int normIdxs[3];         // normal vector index
    Face* faces = NULL;
    Face face;


    // skip until the faces data (with reading of the first line)
//    while ((strncmp(buffer, "f ", 2) != 0) && result) 
//        result = fgets(buffer, bufsize, pFile);

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

        face.aUV = pMesh->texCoords[texIdxs[0] - 1];
        face.bUV = pMesh->texCoords[texIdxs[1] - 1];
        face.cUV = pMesh->texCoords[texIdxs[2] - 1];

        // flip the V component to account for inverted UV-coords (V grows downwards)
        face.aUV.v = 1.0f - face.aUV.v;
        face.bUV.v = 1.0f - face.bUV.v;
        face.cUV.v = 1.0f - face.cUV.v;

        face.color = 0xFFFFFFFF;
        ArrayPush(faces, face);
        result = fgets(buffer, bufsize, pFile);  // read in a new line
    }

//#if PRINT_OBJ_FACES_DEBUG_INFO
    const int lastIdx = ArrayLength(faces) - 1;
    printf("face first (vertices): %d %d %d\n", faces[0].a, faces[0].b, faces[0].c);
    printf("face last  (vertices): %d %d %d\n", faces[lastIdx].a, faces[lastIdx].b, faces[lastIdx].c);
//#endif

    return faces;
}

///////////////////////////////////////////////////////////
#endif 



int LoadObjFileData(Mesh* pMesh, const char* filepath)
{
    // read the contents of the .obj file
    // and load the vertices and faces in 
    // our mesh.vertices and mesh.faces

    printf("Try to load an .obj file: %s\n", filepath);

    FILE* pFile = fopen(filepath, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "error opening .obj file");
        return -1;
    }     

#if 1

    const int bufsize = BUFFER_SIZE;
    char buffer[BUFFER_SIZE];


    // read in whole .obj file line by line
    while (fgets(buffer, bufsize, pFile))
    {
        // if we came across the vertex data block
        if (strncmp(buffer, "v ", 2) == 0)
        {
            pMesh->vertices = ReadVerticesData2(pFile, buffer);
            printf("- vertices are loaded\n");
        }
        // ... the textures coords data block
        if (strncmp(buffer, "vt ", 3) == 0)
        {
            pMesh->texCoords = ReadTexCoordsData2(pFile, buffer);
            printf("- texture coords are loaded\n");
        }
        // ... the normals data block
#if 0
        if (strncmp(buffer, "vn ", 3) == 0)
        {
            pMesh->normals = ReadNormalsData(pFile);
            printf("- normals are loaded\n");
        }
#endif
        // ... face info
        if (strncmp(buffer, "f ", 2) == 0)
        {
            pMesh->faces = ReadFacesData2(pFile, pMesh->texCoords, buffer);
            printf("- faces are loaded\n");
        }
    }

    pMesh->numFaces = ArrayLength(pMesh->faces);
    printf("- number of faces is loaded:%d\n", pMesh->numFaces);
    
    // release memory from the temp texture coords data buffer
    ArrayFree((void**)&(pMesh->texCoords));

    fclose(pFile);

#else
    const int bufsize = BUFFER_SIZE;
    char buffer[BUFFER_SIZE];


    //pMesh->vertices = ReadVerticesData(pFile);
    pMesh->vertices = ReadVerticesData(pFile);
    printf("- vertices are loaded\n");

    pMesh->texCoords = ReadTexCoordsData(pFile);
    printf("- texture coords are loaded\n");

    pMesh->normals  = ReadNormalsData(pFile);
    printf("- normals are loaded\n");

    pMesh->faces = ReadFacesData(pFile, pMesh->texCoords);
    printf("- faces are loaded\n");

    pMesh->numFaces = ArrayLength(pMesh->faces);
    printf("- number of faces is loaded:%d\n", pMesh->numFaces);
    
    // release memory from the temp texture coords data buffer
    ArrayFree((void**)&(pMesh->texCoords));

    fclose(pFile);
#endif
    printf(".obj asset is successfully loaded: %s\n\n", filepath); 

    return 0; 
}
