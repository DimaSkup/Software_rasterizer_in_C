// ==================================================================
// Filename:     camera.c
// Description:  impelementation of camera functional
//
// Created:      03.03.25  by DimaSkup
// ==================================================================
#include "camera.h"
#include "math_common.h"

Camera g_Camera = 
{
    .position  = { 0,0,0 },
    .direction = { 0,0,1 },
    .right     = { 1,0,0 },
    .velocity  = { 0,0,0 },
    .pitch = 0.0f,
    .yaw = 0.0f
};

///////////////////////////////////////////////////////////

void ComputeCameraAngles(
    const int deltaX, 
    const int deltaY, 
    const float deltaTime)
{
    g_Camera.pitch += (0.1f * deltaY * deltaTime);
    g_Camera.yaw   += (0.1f * deltaX * deltaTime);

    // limit the pitch value in range (-(PI/2)+0.01f < pitch < (PI/2)-0.01f)
    if (g_Camera.pitch > M_PIDIV2 - 0.01f)
    {
        g_Camera.pitch = M_PIDIV2 - 0.01f;
    }
    else if (g_Camera.pitch < -M_PIDIV2 + 0.01f)
    {
        g_Camera.pitch = M_PIDIV2 + 0.01f;
    }

    // limit the yaw value in range (-2PI < yaw < 2PI)
    g_Camera.yaw = (g_Camera.yaw > +M_2PI) ? -M_2PI : g_Camera.yaw;
    g_Camera.yaw = (g_Camera.yaw < -M_2PI) ? +M_2PI : g_Camera.yaw;
}

