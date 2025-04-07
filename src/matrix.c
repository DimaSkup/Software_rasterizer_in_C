// ==================================================================
// Filename:     matrix.c
// Description:  implementation of all the matrix functions
// ==================================================================
#include "matrix.h"
#include "macros.h"
#include <stdio.h>
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

///////////////////////////////////////////////////////////

Matrix MatrixRotationAxis(const Vec3 u, const float theta)
{
    // return a rotation matrix by angle theta and the axis u;
    // NOTE: axis must be normalized

    const float s = sinf(theta);
    const float c = cosf(theta);
    const float sub = 1 - c;

    Matrix R = 
    {
        u.x*u.x * sub + c,     u.x*u.y * sub - u.z*s, u.x*u.z * sub + u.y*s, 0,
        u.x*u.y * sub + u.z*s, u.y*u.y * sub + c,     u.y*u.z * sub - u.x*s, 0,
        u.x*u.z * sub - u.y*s, u.y*u.z * sub + u.x*s, u.z*u.z * sub + c,     0,
        0, 0, 0, 1
    };

    return R;
}


// ==================================================================
// matrix-vector multiplication
// ==================================================================

Vec3 MatrixMulVec3(const Matrix* pM, const Vec3 v)
{
    // multiply Vec3 by a 4x4 matrix;
    //
    // the function assumes that the vector referes to a 4d homogeneous
    // vector, thus the func assumes that w == 1 to carry out the multiplication

    Vec3 vec = { 0,0,0 };            // the result vector

    // compute x
    vec.x += (v.x * pM->m[0][0]);
    vec.x += (v.y * pM->m[0][1]);
    vec.x += (v.z * pM->m[0][2]);
    vec.x += pM->m[0][3];

    // compute y
    vec.y += (v.x * pM->m[1][0]);
    vec.y += (v.y * pM->m[1][1]);
    vec.y += (v.z * pM->m[1][2]);
    vec.y += pM->m[1][3];
    
    // compute z
    vec.z += (v.x * pM->m[2][0]);
    vec.z += (v.y * pM->m[2][1]);
    vec.z += (v.z * pM->m[2][2]);
    vec.z += pM->m[2][3];
    
    return vec;
}

///////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////

void MatrixView(
    const Vec3 eye, 
    const Vec3 target, 
    const Vec3 up, 
    Matrix* pOutMatrix)
{
    // compute the forward (z), right(x), and up (y) vectors
    Vec3 z = Vec3Sub(target, eye);
    Vec3Normalize(&z);

    Vec3 x = Vec3Cross(up, z);
    Vec3Normalize(&x);

    Vec3 y = Vec3Cross(z, x);


    // | x.x   x.y   x.z   -dot(x,eye) |
    // | y.x   y.y   y.z   -dot(y,eye) |
    // | z.x   z.y   z.z   -dot(z,eye) |
    // |   0     0     0             1 |
    *pOutMatrix = (Matrix)
    {
        x.x, x.y, x.z, -Vec3Dot(x, eye),
        y.x, y.y, y.z, -Vec3Dot(y, eye),
        z.x, z.y, z.z, -Vec3Dot(z, eye),
          0,   0,   0,                1
    };
}


// ==================================================================
// common functions
// ==================================================================

void MatrixPrint(Matrix* pMat, const char* msg)
{
    if (msg != NULL)
        printf("%s\n", msg);

    for (int row = 0; row < 4; ++row)
        printf("%5.2f %5.2f %5.2f %5.2f\n", pMat->m[row][0], pMat->m[row][1], pMat->m[row][2], pMat->m[row][3]);

    printf("\n\n");
}

///////////////////////////////////////////////////////////

void MatrixZero(Matrix* pMat)
{
    if (pMat)
       memset((void*)pMat, 0, sizeof(Matrix));
}

///////////////////////////////////////////////////////////

void MatrixTranspose(const Matrix* pMatSrc, Matrix* pMatDst)
{
    assert(pMatSrc && pMatDst && "some input ptr is NULL");

    pMatDst->m00 = pMatSrc->m00;  pMatDst->m01 = pMatSrc->m10;
	pMatDst->m02 = pMatSrc->m20;  pMatDst->m03 = pMatSrc->m30;
	pMatDst->m10 = pMatSrc->m01;  pMatDst->m11 = pMatSrc->m11;
	pMatDst->m12 = pMatSrc->m21;  pMatDst->m13 = pMatSrc->m31;
	pMatDst->m20 = pMatSrc->m02;  pMatDst->m21 = pMatSrc->m12;
	pMatDst->m22 = pMatSrc->m22;  pMatDst->m23 = pMatSrc->m32;
	pMatDst->m30 = pMatSrc->m03;  pMatDst->m31 = pMatSrc->m13;
	pMatDst->m32 = pMatSrc->m22;  pMatDst->m33 = pMatSrc->m33;
}

///////////////////////////////////////////////////////////

float MatrixDeterminant3x3(const Matrix3x3* pM)
{
    if (pM == NULL)
        return 0;

    return (pM->m00 * (pM->m11 * pM->m22 - pM->m21 * pM->m12) - 
            pM->m01 * (pM->m10 * pM->m22 - pM->m20 * pM->m12) +
            pM->m02 * (pM->m10 * pM->m21 - pM->m20 * pM->m11));
}

///////////////////////////////////////////////////////////

float MatrixDeterminant(const Matrix* pM)
{
    if (pM == NULL)
        return 0;

    return (pM->m00 * (pM->m11 * pM->m22 - pM->m21 * pM->m12) -
            pM->m01 * (pM->m10 * pM->m22 - pM->m20 * pM->m12) +
            pM->m02 * (pM->m10 * pM->m21 - pM->m20 * pM->m11));
}
///////////////////////////////////////////////////////////

int MatrixInverse(const Matrix* pM, Matrix* pMi)
{
    // compute inverse matrix and return the result in pMi;
    // if the inverse matrix exists the function returns 1;
    // in another case it return 0, and the matrix pMi is a zero matrix;
    //
    // note: this function only works with matrices where last 
    // column is [0 0 0 1]t (transpose).

    // check input params
    if (pM == NULL)
    {
        printf("ERROR: input ptr to matrix == NULL");
        MatrixZero(pMi);
        return 0;
    }

    const float det = (pM->m00 * (pM->m11 * pM->m22 - pM->m21 * pM->m12) -
                       pM->m01 * (pM->m10 * pM->m22 - pM->m20 * pM->m12) +
                       pM->m02 * (pM->m10 * pM->m21 - pM->m20 * pM->m11));

    // test determinant == 0
    if (fabs(det) < EPSILON_E5)
    {
        MatrixZero(pMi);
        return 0;
    }

    const float det_inv = 1.0f / det;

    // compute inverse matrix: adjoint(m) / det(m)
    pMi->m00 =  det_inv * (pM->m11*pM->m22 - pM->m12*pM->m21);
    pMi->m01 = -det_inv * (pM->m01*pM->m22 - pM->m02*pM->m21);
    pMi->m02 =  det_inv * (pM->m01*pM->m12 - pM->m02*pM->m11);
    pMi->m03 = 0.0f;  // always 0

    pMi->m10 = -det_inv * (pM->m10*pM->m22 - pM->m12*pM->m20);
    pMi->m11 =  det_inv * (pM->m00*pM->m22 - pM->m02*pM->m20);
    pMi->m12 = -det_inv * (pM->m00*pM->m12 - pM->m02*pM->m10);
    pMi->m13 = 0.0f;  // always 0

    pMi->m20 =  det_inv * (pM->m10*pM->m21 - pM->m11*pM->m20);
    pMi->m21 = -det_inv * (pM->m00*pM->m21 - pM->m01*pM->m20);
    pMi->m22 =  det_inv * (pM->m00*pM->m11 - pM->m01*pM->m10);
    pMi->m23 = 0.0f;  // always 0

    pMi->m30 = -( pM->m30 * pMi->m00 + pM->m31 * pMi->m10 + pM->m32 * pMi->m20 );
    pMi->m31 = -( pM->m30 * pMi->m01 + pM->m31 * pMi->m11 + pM->m32 * pMi->m21 );
    pMi->m32 = -( pM->m30 * pMi->m02 + pM->m31 * pMi->m12 + pM->m32 * pMi->m22 );
    pMi->m33 = 1.0f;  // always 1

    return 1;
}
///////////////////////////////////////////////////////////

Matrix MatrixInverseTranspose(Matrix mat)
{	
    // we clear out any translation from the matrix because we use the inverse-transpose
	// to transform vectors, and translations only apply to points
    //mat.m03 = 0;
    //mat.m13 = 0;
    //mat.m23 = 0;
    //mat.m33 = 1;
    mat.m30 = 0;
    mat.m31 = 0;
    mat.m32 = 0;
    mat.m33 = 1;

    Matrix inverse;

    MatrixInverse(&mat, &inverse);
    MatrixTranspose(&inverse, &mat);

    return mat;
}
