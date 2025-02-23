// ==================================================================
// Filename:    texture.h
// Description: textures related typedefs and declarations
//              of textures related functions
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "upng.h"

typedef struct
{
    float u;
    float v;
} Tex2;

extern int g_TextureWidth;
extern int g_TextureHeight;

extern upng_t* g_PngTexture;
extern uint32_t* g_MeshTexture;

void LoadPngTextureData(char* filename);

#endif
