// ==================================================================
// Filename:     light.h
// Description:  typedefs for light sources and 
//               declarations of related functions
// 
// Created:      05.02.25
// ==================================================================
#ifndef LIGHT_H
#define LIGHT_H

#include "common_types.h"
#include "vector.h"


// 
// Typedefs
//
typedef struct 
{
    Vec3 direction;
} Light;



//
// Functions declarations
//
u32 LightApplyIntensity(
    const u32 origColor, 
    const float lightMagnitude);

void InitDirectedLight(const Vec3 direction);
Vec3 GetDirectedLightDirection(void);

#endif
