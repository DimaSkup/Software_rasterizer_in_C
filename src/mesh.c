#include "mesh.h"
#include "array.h"
#include "console_color.h"
#include "obj_loader.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BUFFER_SIZE 64

#define MAX_NUM_MESHES 10
static Mesh s_Meshes[MAX_NUM_MESHES];
static int s_NumMeshes = 0;

///////////////////////////////////////////////////////////

void InitEmptyMesh(Mesh* pMesh)
{
    memset(pMesh->name, 0, 32);
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
    const Vec3 translation,
    const Vec3 rotation,
    const Vec3 scale)
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

///////////////////////////////////////////////////////////

int LoadObjFileData(Mesh* pMesh, const char* filepath)
{
    // read the contents of the .obj file
    // and load it into the input mesh

    printf("Try to load an .obj file: %s\n", filepath);

    FILE* pFile = fopen(filepath, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "error opening .obj file");
        return -1;
    }     

    const int bufsize = BUFFER_SIZE;
    char buffer[BUFFER_SIZE];


    // read in whole .obj file line by line
    while (fgets(buffer, bufsize, pFile))
    {
        // if we came across the vertex data block
        if (strncmp(buffer, "v ", 2) == 0)
        {
            pMesh->vertices = ReadVerticesData(pFile, buffer);
            //printf("- vertices are loaded\n");
        }
        if (strncmp(buffer, "vt ", 3) == 0)
        {
            pMesh->texCoords = ReadTexCoordsData(pFile, buffer);
            //printf("- texture coords are loaded\n");
        }
        if (strncmp(buffer, "vn ", 3) == 0)
        {
            pMesh->normals = ReadNormalsData(pFile, buffer);
            //printf("- normals are loaded\n");
        }
        if (strncmp(buffer, "f ", 2) == 0)
        {
            pMesh->faces = ReadFacesData(pFile, pMesh->texCoords, buffer);
            //printf("- faces are loaded\n");
        }
    }

    // set and print the number of faces in this mesh
    pMesh->numFaces = ArrayLength(pMesh->faces);
    printf("- the number of loaded faces:%d\n", pMesh->numFaces);
   
    // set a name for the mesh
    const int nameLength = (strlen(filepath) > 32) ? 32 : strlen(filepath);
    strncpy(pMesh->name, filepath, nameLength);


    // release memory from the temp texture coords data buffer
    ArrayFree((void**)&(pMesh->texCoords));

    fclose(pFile);


    printf(".obj asset is successfully loaded: %s\n\n", filepath); 

    return 0; 
}
