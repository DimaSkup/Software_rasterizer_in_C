#include "model_loader.h"


void LoadModel(
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
            pMesh->faces = ReadFacesData(
                pFile, 
                pMesh->vertices,
                pMesh->texCoords, 
                &pMesh->normals,
                buffer,
                ArrayLength(pMesh->vertices));
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
