// ==================================================================
// Filename:     math_common.h
// Description:  declaration of some common 
//               math constants and functions
//
// Created:      05.03.25  by DimaSkup
// ==================================================================
#ifndef MATH_COMMON_H
#define MATH_COMMON_H

//extern float M_PI;
extern const float M_2PI;
extern const float M_PIDIV2;
extern const float M_PIDIV3;
extern const float M_PIDIV4;
extern const float M_PIDIV6;

extern float RAD_TO_DEG(const float radian);
extern float DEG_TO_RAD(const float degrees);

float Lerp(const float a, const float b, const float t);

#endif

