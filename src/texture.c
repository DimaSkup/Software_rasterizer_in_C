// ==================================================================
// Filename:    texture.c
// Description: initialization of textures related globals;
//              and implementation of textures related functions;
//
// Created:     05.02.25 by DimaSkup
// ==================================================================
#include "texture.h"
#include <stdio.h>

int       g_TextureWidth  = 64;
int       g_TextureHeight = 64;

uint32_t* g_MeshTexture = NULL;
upng_t*   g_PngTexture = NULL;

///////////////////////////////////////////////////////////

void LoadPngTextureData(char* filename)
{
    // load a png texture image from the file by filename
    g_PngTexture = upng_new_from_file(filename);
    if (g_PngTexture != NULL)
    {
        upng_decode(g_PngTexture);
        if (upng_get_error(g_PngTexture) == UPNG_EOK)
        {
            g_MeshTexture   = (uint32_t*)upng_get_buffer(g_PngTexture);
            g_TextureWidth  = upng_get_width(g_PngTexture);
            g_TextureHeight = upng_get_height(g_PngTexture);
        }
    }
}

