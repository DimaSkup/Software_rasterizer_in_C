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
    float yaw;       // angle in radians 
    float pitch;     // angle in radians 
} Camera;

// ==================================================================

void InitCamera(const Vec3 position, const Vec3 direction);

Vec3 GetCameraPosition(void);
Vec3 GetCameraDirection(void);
Vec3 GetCameraForwardVelocity(void);
Vec3 GetCameraRight(void);

float GetCameraYaw(void);
float GetCameraPitch(void);

void SetCameraPosition(const Vec3 position);
void SetCameraDirection(const Vec3 direction);
void SetCameraForwardVelocity(const Vec3 forwardVelocity);

void RotateCameraYaw(const float angle);
void RotateCameraPitch(float angle);

float GetCameraMovementSpeed();
float GetCameraRotationSpeed();

Vec3 GetCameraLookAtTarget(void);

#endif
