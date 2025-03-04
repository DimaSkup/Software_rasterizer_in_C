// ==================================================================
// Filename:     camera.c
// Description:  impelementation of camera functional
//
// Created:      03.03.25  by DimaSkup
// ==================================================================
#include "camera.h"

Camera g_Camera = 
{
    .position  = { 0,0,0 },
    .direction = { 0,0,1 },
    .right     = { 1,0,0 },
    .velocity  = { 0,0,0 },
    .pitch = 0.0f,
    .yaw = 0.0f
};
