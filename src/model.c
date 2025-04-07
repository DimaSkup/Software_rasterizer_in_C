// ==================================================================
// Filename:    model.c
// Description: implementation of the model related functions
// ==================================================================
#include "model.h"
#include <string.h>

void InitEmptyMesh(Mesh* pMesh)
{
    memset(pMesh->name, 0, 32);
    pMesh->vertexStart = 0;
    pMesh->vertexCount = 0;
    pMesh->indexStart  = 0;
    pMesh->indexCount  = 0;

    pMesh->texID = 0;

    pMesh->scale       = (Vec3){ 1,1,1 };
    pMesh->rotation    = (Vec3){ 0,0,0 };
    pMesh->translation = (Vec3){ 0,0,0 };
}

///////////////////////////////////////////////////////////

void InitEmptyModel(Model* pModel)
{

}


