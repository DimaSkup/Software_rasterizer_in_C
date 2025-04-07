// ==================================================================
// Filename:    obj_loader.h
// Description: functional for loading .obj models data from file
//
// Created:     14.03.2025  by DimaSkup
// ==================================================================
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "../vector.h"
#include "../geomtypes.h"
#include <stdio.h>

Vec3* ReadVerticesData (FILE* pFile, char* buffer);
Vec2* ReadTexCoordsData(FILE* pFile, char* buffer);
Vec3* ReadNormalsData  (FILE* pFile, char* buffer);

Face* ReadFacesData(
    FILE* pFile,
    Vec3* vertices,
    Vec2* texCoords, 
    Vec3** normals,
    char* buffer,
    const int numVertices);

#endif
