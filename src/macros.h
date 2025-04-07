// ==================================================================
// Filename:     macros.h
// Description:  common helper macroses
//
// Created:      07.04.2025 by DimaSkup
// ==================================================================
#ifndef MACROS_H
#define MACROS_H

// bit manipulation macros
#define SET_BIT  (word, bit_flag) ((word) = (word | bit_flag))
#define RESET_BIT(word, bit_flag) ((word) = (word & (~bit_flag)))

// used for swapping algorithm
#define SWAP(a,b,t) { t=a; a=b; b=t; }

// ==================================================================
// math
// ==================================================================

// PI number and related stuff
#define PI       ((float)3.141592654f)
#define PI_2     ((float)6.283185307f)  // PI * 2
#define PI_DIV_2 ((float)1.570796327f)
#define PI_DIV_3 ((float)1.047197551f)
#define PI_DIV_4 ((float)0.785398163f)
#define PI_DIV_6 ((float)0.523598776f)
#define PI_INV   ((float)0.318309886f)  // 1.0f / PI

// definitions of small numbers
#define EPSILON_E4 (float)(1E-4)
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)


#define RAND_RANGE(x,y) ((x) + (rand()%((y)-(x)+1)))

#define RAD_TO_DEG(rads) (rads * 57.29582f);    // rad * 180.0 / PI
#define DEG_TO_RAD(angle) (angle * 0.0175f);    // angle * PI / 180.0

// linear interpolation (params are expected to be float or double)
#define LERP(a,b,t) (a + t * (b-a))


#endif
