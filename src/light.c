// ==================================================================
// Filename:    light.c
// Description: initialization of light globals and
//              implementation of functions related to light sources
// Created:     05.02.25
// ==================================================================
#include "light.h"


static Light s_DirLight = 
{
    .direction = { 0, 0, 10 }
};

///////////////////////////////////////////////////////////

void InitDirectedLight(const Vec3 direction)
{
    s_DirLight.direction = direction;
}

///////////////////////////////////////////////////////////

Vec3 GetDirectedLightDirection(void)
{
    return s_DirLight.direction;
}

///////////////////////////////////////////////////////////

u32 LightApplyIntensity(
    const u32 originalColor, 
    float lightMagnitude)
{
    // change color base on a light magnitude to
    // represent light intensity over a face

    // clamp in range [0, 1]
    lightMagnitude = (lightMagnitude < 0) ? 0 : lightMagnitude;
    lightMagnitude = (lightMagnitude > 1) ? 1 : lightMagnitude;
    
    u32 a = (originalColor & 0xFF000000);
    u32 r = (originalColor & 0x00FF0000) * lightMagnitude;
    u32 g = (originalColor & 0x0000FF00) * lightMagnitude;
    u32 b = (originalColor & 0x000000FF) * lightMagnitude;

    // return an ARGB color
    return (a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF));
}

///////////////////////////////////////////////////////////
