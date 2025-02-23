// ==================================================================
// Filename:     light.h
// Description:  typedefs for light sources and 
//               declarations of related functions
// 
// Created:      05.02.25
// ==================================================================
#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include <stdint.h>

// 
// Typedefs
//
typedef struct 
{
    Vec3 direction;
} Light;

typedef uint32_t u32;


//
// Globals
//
extern Light g_LightDir;   // directed light source (sun)


//
// Functions declarations
//
u32 LightApplyIntensity(const u32 origColor, float lightMagnitude);


#endif
