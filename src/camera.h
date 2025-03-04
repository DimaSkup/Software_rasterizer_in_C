// ==================================================================
// Filename:     camera.h
// Description:  a camera data structure and functional declaration
//
// Created:      03.03.25  by DimaSkup
// ==================================================================
#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct 
{
    Vec3 position;
    Vec3 direction;
    Vec3 right;
    Vec3 velocity;
    float pitch;     // angle in radians 
    float yaw;       // angle in radians 
} Camera;

extern Camera g_Camera;


#endif
