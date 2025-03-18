// ==================================================================
// Filename:     camera.c
// Description:  impelementation of camera functional
//
// Created:      03.03.25  by DimaSkup
// ==================================================================
#include "camera.h"
#include "math_common.h"
#include "matrix.h"
#include <stdio.h>

static Camera s_Camera = 
{
    .position  = { 0,0,0 },
    .direction = { 0,0,1 },
    .right     = { 1,0,0 },
    .velocity  = { 0,0,0 },
    .pitch = 0.0f,
    .yaw = 0.0f
};

static float s_CameraMovementSpeed = 10.0f;
static float s_CameraRotationSpeed = 0.4f;

///////////////////////////////////////////////////////////

void InitCamera(const Vec3 position, const Vec3 direction)
{
    s_Camera.position = position;
    s_Camera.direction = direction;
    s_Camera.velocity = (Vec3){ 0, 0, 0 };
    s_Camera.yaw   = 0.0f;
    s_Camera.pitch = 0.0f;
}

///////////////////////////////////////////////////////////

Vec3 GetCameraPosition(void)        { return s_Camera.position; }
Vec3 GetCameraDirection(void)       { return s_Camera.direction; }
Vec3 GetCameraForwardVelocity(void) { return s_Camera.velocity; }
Vec3 GetCameraRight(void)           { return s_Camera.right; }

///////////////////////////////////////////////////////////

float GetCameraYaw(void)            { return s_Camera.yaw; }
float GetCameraPitch(void)          { return s_Camera.pitch; }

///////////////////////////////////////////////////////////

void SetCameraPosition(const Vec3 position) 
{ 
    s_Camera.position = position; 
}

void SetCameraDirection(const Vec3 direction)
{
    s_Camera.direction = direction;
}

void SetCameraForwardVelocity(const Vec3 forwardVelocity)
{
    s_Camera.velocity = forwardVelocity;
}

///////////////////////////////////////////////////////////

void RotateCameraYaw(const float angle)
{
    s_Camera.yaw += angle;

    // limit the yaw value in range (-2PI < yaw < 2PI)
    s_Camera.yaw = (s_Camera.yaw > +M_2PI) ? -M_2PI : s_Camera.yaw;
    s_Camera.yaw = (s_Camera.yaw < -M_2PI) ? +M_2PI : s_Camera.yaw;

    // compute the RIGHT direction vector of the camera
    const Vec3 up = {0,1,0};
    s_Camera.right = Vec3Cross(up, s_Camera.direction);
    Vec3Normalize(&s_Camera.right);
    
    //
    // rotate the camera's direction around right vector by input angle
    //
    Vec4 target = Vec4FromVec3(&s_Camera.direction);

    // compute rotation matrix around world Y-axis
    Matrix cameraRotationMatY;
    MatrixRotationY(angle, &cameraRotationMatY);

    MatrixMulVec4(&cameraRotationMatY, target, &target);

    Vec3 dir = { target.x, target.y, target.z };
    Vec3Normalize(&dir);

    SetCameraDirection(Vec3Init(dir.x, dir.y, dir.z));
}

///////////////////////////////////////////////////////////

void RotateCameraPitch(float angle)
{
    // rotate the camera's direction around camera's right vector by input angle

    s_Camera.pitch += angle;

    // limit the pitch value
    if (s_Camera.pitch > M_PIDIV2 - 0.1f)
    {
        s_Camera.pitch = M_PIDIV2 - 0.1f;
        angle = 0.0f;
    }
    
    if (s_Camera.pitch < -M_PIDIV2 + 0.1f)
    {
        s_Camera.pitch = -M_PIDIV2 + 0.1f;
        angle = 0.0f;
    }

    if (angle == 0.0f)
        return;
    // transform camera's direction vector
    //
    Vec4 target  = Vec4FromVec3(&s_Camera.direction);

    Matrix R = MatrixRotationAxis(GetCameraRight(), angle);
    MatrixMulVec4(&R, target, &target);

    Vec3 dir = { target.x, target.y, target.z };
    Vec3Normalize(&dir);

    SetCameraDirection(Vec3Init(dir.x, dir.y, dir.z));
}

///////////////////////////////////////////////////////////

float GetCameraMovementSpeed(void) { return s_CameraMovementSpeed; }
float GetCameraRotationSpeed(void) { return s_CameraRotationSpeed; }

///////////////////////////////////////////////////////////

Vec3 GetCameraLookAtTarget(void)
{
    // compute the new camera rotation and translation for the FPS camera movement
     // offset the camera position in the direction where the camera is pointing at
    return Vec3Add(s_Camera.position, s_Camera.direction);
}
