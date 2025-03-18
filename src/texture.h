// ==================================================================
// Filename:    texture.h
// Description: textures related typedefs and declarations
//              of textures related functions
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#ifndef TEXTURE_H
#define TEXTURE_H

#include "upng.h"

typedef struct
{
    float u;
    float v;
} Tex2;

void LoadPngTextureData(upng_t** ppTexture, const char* filename);

#endif
