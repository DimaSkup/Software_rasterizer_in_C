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
        float mat[9];
        float m[3][3];

        struct 
        {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };    
    };
} Matrix3x3;

///////////////////////////////////////////////////////////

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

// ==================================================================
// Create transformation matrices 
// ==================================================================
void MatrixScaling    (const float sx, const float sy, const float sz, Matrix* outMat);
void MatrixTranslation(const float tx, const float ty, const float tz, Matrix* outMat);
void MatrixRotationX  (const float angle, Matrix* outMat);
void MatrixRotationY  (const float angle, Matrix* outMat);
void MatrixRotationZ  (const float angle, Matrix* outMat);

Matrix MatrixRotationAxis(const Vec3 axis, const float angleInRad);

void MatrixView(const Vec3 eye, const Vec3 target, const Vec3 up, Matrix* pOutMatrix);

Vec3 MatrixMulVec3(const Matrix* pM, const Vec3 v);
void   MatrixMulVec4(const Matrix* m, const Vec4 v, Vec4* outVec);
Matrix MatrixMulMatrix(const Matrix* m1, const Matrix* m2);
//void   MatrixMulMatrix(const Matrix* m1, const Matrix* m2, Matrix* m);


// ==================================================================
// projection matrix stuff
// ==================================================================
Matrix MatrixInitPerspective(
    const float fov, 
    const float aspectRatio,
    const float nearZ,
    const float farZ);

void MatrixMulVec4Project(
    const Matrix* matProj, 
    const Vec4 origVec,
    Vec4* projectedVec);


// ==================================================================
// common functions prototypes
// ==================================================================
void MatrixPrint(Matrix* pMat, const char* msg);
void MatrixZero(Matrix* pMat);

void MatrixTranspose(const Matrix* pMatSrc, Matrix* pMatDst);

float MatrixDeterminant3x3(const Matrix3x3* pMat);
float MatrixDeterminant(const Matrix* pMat);
int MatrixInverse(const Matrix* pMat, Matrix* pMi);
Matrix MatrixInverseTranspose(Matrix mat);

#endif
