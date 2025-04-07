#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "clipping.h"
#include "upng.h"
#include "model.h"
#include "render.h"


// ==================================================================
#define MAX_NUM_MODELS 10
#define RENDERLIST_MAX_SIZE 10000

typedef struct
{
    // array of triangles that should be rendered onto the screen

    uint32_t  state;                              // render list state
    uint32_t  attr;                               // render list attributes

    Triangle* trianglesPtrs[RENDERLIST_MAX_SIZE]; // visualization list (a.k.a. indexed vertex buffers) - array of pointers to actual triangles
    Triangle  triangles[RENDERLIST_MAX_SIZE];     // real data of triangle to render
    int       numTriangles;                       // actual number of tringles to render
} RenderList;

extern RenderList g_RenderList;
extern Model      g_Models[MAX_NUM_MODELS];
extern int        g_NumModels;

// ==================================================================
// global variables for execution status and game loop
// ==================================================================
extern bool   g_IsRunning;	
extern int    g_PrevFrameTime;

extern Matrix g_ProjMatrix;

// ==================================================================
// main functions
// ==================================================================
void Initialize(void);
void Run(void);
void Shutdown(void);

void ProcessInput(void);
void ProcessKeydown(const int keycode);

void Update(void);
void Render(void);

void FreeAssetResources(Mesh* pMesh);
void FreeResources(void);

#endif
