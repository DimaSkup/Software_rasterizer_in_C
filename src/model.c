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

///////////////////////////////////////////////////////////

float ComputeModelRadius(Model* pModel)
{
    // compute average and maximum radius for set model
    // and update the model data

    // reset in case there's any other values
    pModel->avgRadius = 0;
    pModel->maxRadius = 0;

    Vec3* vertices = pModel->verticesLocal;

    // loop through and compute radius
    for (int vIdx = 0; vIdx < pModel->numVertices; ++vIdx)
    {
        // update the average and maximum radius
        const float x = vertices[vIdx].x;
        const float y = vertices[vIdx].y;
        const float z = vertices[vIdx].z;

        const float distToVertex = (x*x + y*y + z*z);

        // accumulate total radius
        pModel->avgRadius += distToVertex;

        // update maximum radius
        if (distToVertex > pModel->maxRadius)
            pModel->maxRadius = distToVertex;
    }

    // finalize average radius computation
    pModel->avgRadius /= pModel->numVertices;

    // return max radius
    return pModel->maxRadius;
}
