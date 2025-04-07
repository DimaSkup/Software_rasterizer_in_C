// ==================================================================
// Filename:    texture.c
// Description: initialization of textures related globals;
//              and implementation of textures related functions;
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#include "texture.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

// GLOBALS
Texture g_Textures[MAX_NUM_TEXTURES];
int     g_NumTextures = 0;

///////////////////////////////////////////////////////////

TextureID LoadTexture(const char* filename)
{
    // load a texture from the file according to its extension

    // TODO: add different types (not only png)

    LoadPngTextureData(&g_Textures[g_NumTextures], filename);

    const TextureID id = g_NumTextures;
    g_NumTextures++;

    return id;
}

///////////////////////////////////////////////////////////

void LoadPngTextureData(Texture* pTexture, const char* filename)
{
    assert((filename != NULL) && "invalid input args");

    // load a png texture image from the file by filename
    upng_t* pPngTex = upng_new_from_file(filename);

    if (pPngTex != NULL)
    {
        upng_decode(pPngTex);
        if (upng_get_error(pPngTex) == UPNG_EOK)
        {
            pTexture->textureBuffer = (uint32_t*)upng_get_buffer(pPngTex);
            pTexture->textureWidth  = upng_get_width(pPngTex);
            pTexture->textureHeight = upng_get_height(pPngTex);
        }
    }

    // store a pointer to the png texture
    pTexture->pPngTexture = pPngTex;
    pPngTex = NULL;
}


