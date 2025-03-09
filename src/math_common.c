// ==================================================================
// Filename:  math_common.c
//  
// Created:   05.03.25  by DimaSkup
// ==================================================================
#include "math_common.h"


// ==================================================================
// GLOBALS
// ==================================================================
const float PI       = 3.14159f;
const float M_2PI    = 6.28318f; 
const float M_PIDIV2 = PI / 2;
const float M_PIDIV3 = PI / 3;
const float M_PIDIV4 = PI / 4;
const float M_PIDIV6 = PI / 6;


// ==================================================================
// FUNCTIONS
// ==================================================================
float RAD_TO_DEG(const float rads) 
{
    return rads * 57.29582f;     // rad * 180.0 / PI
}

float DEG_TO_RAD(const float ang)
{
    return ang * 0.0175;         // angle * PI / 180.0
}

///////////////////////////////////////////////////////////

float Lerp(const float a, const float b, const float t)
{
    // linear interpolation
    return a + t * (b - a);
}

