#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

// ===================================================================
//                           TYPEDEFS
// ===================================================================
typedef struct 
{
    union
    {
        float mat[16];
        float m[4][4];

        struct 
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };    
    };
} Matrix;

// ==================================================================
//                       IDENTITY MATRICES
// ==================================================================

extern Matrix gMatrixIdentity;

void MatrixInitIdentity(Matrix* m);
void MatrixInitWorld(const Vec3* scale, const Vec3* rotation, const Vec3* translation, Matrix* world);

void MatrixScaling(const float sx, const float sy, const float sz, Matrix* outMat);
void MatrixTranslation(const float tx, const float ty, const float tz, Matrix* outMat);
void MatrixRotationX(const float angle, Matrix* outMat);
void MatrixRotationY(const float angle, Matrix* outMat);
void MatrixRotationZ(const float angle, Matrix* outMat);
Matrix MatrixRotationAxis(const Vec3 axis, const float angleInRad);

void   MatrixMulVec4(const Matrix* m, const Vec4 v, Vec4* outVec);
Matrix MatrixMulMatrix(const Matrix* m1, const Matrix* m2);
void   MatrixMulMatrixRetProd(const Matrix* m1, const Matrix* m2, Matrix* m);

// projection matrix stuff
Matrix MatrixInitPerspective(
    const float fov, 
    const float aspectRatio,
    const float nearZ,
    const float farZ);

void MatrixMulVec4Project(
    const Matrix* matProj, 
    const Vec4 origVec,
    Vec4* projectedVec);

void MatrixView(const Vec3 eye, const Vec3 target, const Vec3 up, Matrix* pOutMatrix);

#endif
