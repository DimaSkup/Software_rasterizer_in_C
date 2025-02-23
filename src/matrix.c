// ==================================================================
// Filename:     matrix.c
// Description:  implementation of all the matrix functions
// ==================================================================
#include "matrix.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>    // for using memcpy
#include <math.h>      // for using trigonometry functions


// global 4x4 identity matrix
Matrix gMatrixIdentity = 
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

///////////////////////////////////////////////////////////

void MatrixInitIdentity(Matrix* m)
{
    // init input matrix with identity 4x4 matrix
    assert(m != NULL && "ptr to the input matrix == NULL");
    memcpy((void*)(m), (void*)&gMatrixIdentity, sizeof(Matrix));
}

///////////////////////////////////////////////////////////

void MatrixInitWorld(
    const Vec3* scale,
    const Vec3* rotation,
    const Vec3* translation,
    Matrix* outWorld)
{
    Matrix scaleMat;
    Matrix rotationMatZ;
    Matrix rotationMatY;
    Matrix rotationMatX;
    Matrix translationMat;

    MatrixInitIdentity(outWorld);
   
    // create transformation matrices
    MatrixScaling(scale->x, scale->y, scale->z, &scaleMat);
    MatrixRotationX(rotation->x, &rotationMatZ);
    MatrixRotationY(rotation->y, &rotationMatY);
    MatrixRotationZ(rotation->z, &rotationMatX);
    MatrixTranslation(translation->x, translation->y, translation->z, &translationMat);  

    // compute world matrix based on transformation matrices
    *outWorld = MatrixMulMatrix(&scaleMat, outWorld);
    *outWorld = MatrixMulMatrix(&rotationMatZ, outWorld);
    *outWorld = MatrixMulMatrix(&rotationMatY, outWorld);
    *outWorld = MatrixMulMatrix(&rotationMatX, outWorld);
    *outWorld = MatrixMulMatrix(&translationMat, outWorld);
}

// ==================================================================
// make scaling/translation/rotation matrix
// ==================================================================

void MatrixScaling(
    const float sx, 
    const float sy, 
    const float sz, 
    Matrix* outMat)
{
    // return a scaling matrix in outMat
    *outMat = (Matrix)
    {
        sx,  0,  0,  0,
         0, sy,  0,  0,
         0,  0, sz,  0,
         0,  0,  0,  1
    };
}

///////////////////////////////////////////////////////////

void MatrixTranslation(
    const float tx,
    const float ty,
    const float tz,
    Matrix* outMat)
{
    // return a translation matrix in outMat
    *outMat = (Matrix) 
    {
        1, 0, 0, tx,
        0, 1, 0, ty,
        0, 0, 1, tz,
        0, 0, 0,  1
    };
}

///////////////////////////////////////////////////////////

void MatrixRotationX(const float angle, Matrix* outMat)
{       
    // return a rotation matrix in counter-clockwise direction by angle
    const float c = cosf(angle);
    const float s = sinf(angle);

    *outMat = (Matrix)
    {
        1,  0,  0,  0,
        0,  c, -s,  0,
        0,  s,  c,  0,
        0,  0,  0,  1
    };
}

///////////////////////////////////////////////////////////

void MatrixRotationY(const float angle, Matrix* outMat)
{
    // return a rotation matrix in CLOCKWISE (!) direction by angle
    const float c = cosf(angle);
    const float s = sinf(angle);

    *outMat = (Matrix) 
    {
         c,  0,  s,  0,
         0,  1,  0,  0,
        -s,  0,  c,  0,
         0,  0,  0,  1
    };
}

///////////////////////////////////////////////////////////

void MatrixRotationZ(const float angle, Matrix* outMat)
{
    // return a rotation matrix in counter-clockwise direction by angle
    const float c = cosf(angle);
    const float s = sinf(angle);

    *outMat = (Matrix)
    {
        c, -s,  0,  0,
        s,  c,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1
    };
}



// ==================================================================
// matrix-vector multiplication
// ==================================================================

void MatrixMulVec4(const Matrix* m, const Vec4 v, Vec4* outVec)
{
    // transform input vec4 (v) with matrix and 
    // return the result in outVec
    assert((m != NULL) & (outVec != NULL));

    outVec->x = (m->m00*v.x) + (m->m01*v.y) + (m->m02*v.z) + (m->m03*v.w);
    outVec->y = (m->m10*v.x) + (m->m11*v.y) + (m->m12*v.z) + (m->m13*v.w);
    outVec->z = (m->m20*v.x) + (m->m21*v.y) + (m->m22*v.z) + (m->m23*v.w);
    outVec->w = (m->m30*v.x) + (m->m31*v.y) + (m->m32*v.z) + (m->m33*v.w);
}

///////////////////////////////////////////////////////////

Matrix MatrixMulMatrix(const Matrix* m1, const Matrix* m2)
{
    // multiply input matrix m1 by m2 and 
    // return the result as a new Matrix

    Matrix result;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.m[i][j] = 
                m1->m[i][0] * m2->m[0][j] + 
                m1->m[i][1] * m2->m[1][j] +
                m1->m[i][2] * m2->m[2][j] +
                m1->m[i][3] * m2->m[3][j];
        }
    }

    return result;
}

///////////////////////////////////////////////////////////

void MatrixMulMatrixRetProd(
    const Matrix* m1, 
    const Matrix* m2, 
    Matrix* m)
{
    // multiply input matrix m1 by m2 and
    // return the product result in m
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m->m[i][j] = 
                m1->m[i][0] * m2->m[0][j] + 
                m1->m[i][1] * m2->m[1][j] +
                m1->m[i][2] * m2->m[2][j] +
                m1->m[i][3] * m2->m[3][j];
        }
    }
}


// ==================================================================
// projection matrix stuff
// ==================================================================
Matrix MatrixInitPerspective(
    const float fov, 
    const float aspectRatio,
    const float nearZ,
    const float farZ)
{
    // | (h/w)*1/tan(fov/2)             0          0                0 |
    // |                  0 1/tanf(fov/2)          0                0 |
    // |                  0             0 zf/(zf-zn) (-zf*zn)/(zf-zn) |
    // |                  0             0          1                0 |
    Matrix m;
    memset(m.m, 0, sizeof(Matrix));

    m.m00 = aspectRatio * (1.0f / tanf(fov * 0.5f));
    m.m11 = 1.0f / tanf(fov * 0.5f);
    m.m22 = farZ / (farZ - nearZ);
    m.m23 = (-farZ * nearZ) / (farZ - nearZ);
    m.m32 = 1.0f;
    return m;
}

///////////////////////////////////////////////////////////

void MatrixMulVec4Project(
    const Matrix* matProj, 
    const Vec4 origVec,
    Vec4* projVec)
{
    // multiply the projection matrix by our original vector
    MatrixMulVec4(matProj, origVec, projVec);

    // perform perspective divide with original z-value 
    // that is now stored in w
    if (projVec->w != 0.0f)
    {
        float invZ = 1.0f / projVec->w;
        projVec->x *= invZ;
        projVec->y *= invZ;
        projVec->z *= invZ;
    }    
}
