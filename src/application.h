#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "camera.h"
#include "triangle.h"
#include "light.h"
#include "texture.h"
#include "math_common.h"
#include "clipping.h"
#include "upng.h"

// ==================================================================
// array of triangles that should be rendered frame by frame
// ==================================================================
extern Triangle g_TrianglesToRender[10000];


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

void FreeAssetResources(void);
void FreeResources(void);

#endif
