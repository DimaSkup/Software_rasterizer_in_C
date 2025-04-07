// ==================================================================
// Filename:    texture.h
// Description: textures related typedefs and functions
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#ifndef TEXTURE_H
#define TEXTURE_H

#include "common_types.h"
#include "upng.h"

#define MAX_NUM_TEXTURES 64

//
// TYPEDEFS
//

typedef struct 
{
    TextureID id;
    int       textureWidth;       
    int       textureHeight;
    uint32_t* textureBuffer; 
    upng_t*   pPngTexture;

} Texture;


// 
// GLOBALS
//
extern Texture g_Textures[MAX_NUM_TEXTURES];
extern int     g_NumTextures;


//
// FUNCTIONS
//
TextureID LoadTexture(const char* filename);
void LoadPngTextureData(Texture* pTexture, const char* filename);

#endif
