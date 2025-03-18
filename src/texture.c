// ==================================================================
// Filename:    texture.c
// Description: initialization of textures related globals;
//              and implementation of textures related functions;
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#include "texture.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

///////////////////////////////////////////////////////////

void LoadPngTextureData(upng_t** ppTexture, const char* filename)
{
    assert((filename != NULL) && "invalid input args");

    // load a png texture image from the file by filename
    *ppTexture = upng_new_from_file(filename);

    if (*ppTexture != NULL)
    {
        upng_decode(*ppTexture);
        if (upng_get_error(*ppTexture) == UPNG_EOK)
        {
            //g_MeshTexture   = (uint32_t*)upng_get_buffer(*ppTexture);
            //g_TextureWidth  = upng_get_width(*ppTexture);
            //g_TextureHeight = upng_get_height(**ppTexture);
        }
    }
}


