// ==================================================================
// Filename:    obj_loader.h
// Description: functional for loading .obj models data from file
//
// Created:     14.03.2025  by DimaSkup
// ==================================================================
#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "vector.h"
#include "texture.h"
#include "triangle.h"
#include <stdio.h>

Vec3* ReadVerticesData(FILE* pFile, char* buffer);
Tex2* ReadTexCoordsData(FILE* pFile, char* buffer);
Vec3* ReadNormalsData(FILE* pFile, char* buffer);
Face* ReadFacesData(FILE* pFile, Tex2* texCoords, char* buffer);

#endif
