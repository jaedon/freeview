/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_matrix.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/14/10 2:32p $
 *
 * Module Description: Matrix entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_matrix.c $
 * 
 * Hydra_Software_Devel/5   8/14/10 2:32p nissen
 * SW7400-2378: Fixed problems with bad input.
 * 
 * Hydra_Software_Devel/4   4/7/09 1:12p nissen
 * PR 53913: Switched error in glPopMatrix to underflow instead of
 * overflow.
 * 
 * Hydra_Software_Devel/3   10/27/06 10:04a nissen
 * PR 12814: Added api function to query matrix.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:45p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
GL_P_Matrix s_IdentityMatrix = 
{
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}
};

/***************************************************************************/
#define GL_P_SET_MATRIX_ARRAY_ROW( array, row, m0, m1, m2, m3 ) \
{ \
	(array)[row + 0] = (m0); (array)[row +  4] = (m1); \
	(array)[row + 8] = (m2); (array)[row + 12] = (m3); \
}

/***************************************************************************/
#define GL_P_COPY_MATRIX_ARRAY( po, pi ) \
{ \
	(po)[0]  = (pi)[0];  (po)[1]  = (pi)[1];  (po)[2]  = (pi)[2];  (po)[3]  = (pi)[3]; \
	(po)[4]  = (pi)[4];  (po)[5]  = (pi)[5];  (po)[6]  = (pi)[6];  (po)[7]  = (pi)[7]; \
	(po)[8]  = (pi)[8];  (po)[9]  = (pi)[9];  (po)[10] = (pi)[10]; (po)[11] = (pi)[11]; \
	(po)[12] = (pi)[12]; (po)[13] = (pi)[13]; (po)[14] = (pi)[14]; (po)[15] = (pi)[15]; \
}
	
/***************************************************************************/
void GL_P_MultiplyMatrix( 
	GL_P_Matrix *pMatOut,
	GL_P_Matrix *pMatIn, 
	GLfloat *pfInA )
{
	GLfloat *pfOut = pMatOut->a;
	GLfloat *pfInB = pMatIn->a;
	pfOut[0]  = ((pfInB[0] * pfInA[0] )) + ((pfInB[4] * pfInA[1] )) + ((pfInB[8]  * pfInA[2] )) + ((pfInB[12] * pfInA[3] ));
	pfOut[1]  = ((pfInB[1] * pfInA[0] )) + ((pfInB[5] * pfInA[1] )) + ((pfInB[9]  * pfInA[2] )) + ((pfInB[13] * pfInA[3] ));
	pfOut[2]  = ((pfInB[2] * pfInA[0] )) + ((pfInB[6] * pfInA[1] )) + ((pfInB[10] * pfInA[2] )) + ((pfInB[14] * pfInA[3] ));
	pfOut[3]  = ((pfInB[3] * pfInA[0] )) + ((pfInB[7] * pfInA[1] )) + ((pfInB[11] * pfInA[2] )) + ((pfInB[15] * pfInA[3] ));
	pfOut[4]  = ((pfInB[0] * pfInA[4] )) + ((pfInB[4] * pfInA[5] )) + ((pfInB[8]  * pfInA[6] )) + ((pfInB[12] * pfInA[7] ));
	pfOut[5]  = ((pfInB[1] * pfInA[4] )) + ((pfInB[5] * pfInA[5] )) + ((pfInB[9]  * pfInA[6] )) + ((pfInB[13] * pfInA[7] ));
	pfOut[6]  = ((pfInB[2] * pfInA[4] )) + ((pfInB[6] * pfInA[5] )) + ((pfInB[10] * pfInA[6] )) + ((pfInB[14] * pfInA[7] ));
	pfOut[7]  = ((pfInB[3] * pfInA[4] )) + ((pfInB[7] * pfInA[5] )) + ((pfInB[11] * pfInA[6] )) + ((pfInB[15] * pfInA[7] ));
	pfOut[8]  = ((pfInB[0] * pfInA[8] )) + ((pfInB[4] * pfInA[9] )) + ((pfInB[8]  * pfInA[10])) + ((pfInB[12] * pfInA[11]));
	pfOut[9]  = ((pfInB[1] * pfInA[8] )) + ((pfInB[5] * pfInA[9] )) + ((pfInB[9]  * pfInA[10])) + ((pfInB[13] * pfInA[11]));
	pfOut[10] = ((pfInB[2] * pfInA[8] )) + ((pfInB[6] * pfInA[9] )) + ((pfInB[10] * pfInA[10])) + ((pfInB[14] * pfInA[11]));
	pfOut[11] = ((pfInB[3] * pfInA[8] )) + ((pfInB[7] * pfInA[9] )) + ((pfInB[11] * pfInA[10])) + ((pfInB[15] * pfInA[11]));
	pfOut[12] = ((pfInB[0] * pfInA[12])) + ((pfInB[4] * pfInA[13])) + ((pfInB[8]  * pfInA[14])) + ((pfInB[12] * pfInA[15]));
	pfOut[13] = ((pfInB[1] * pfInA[12])) + ((pfInB[5] * pfInA[13])) + ((pfInB[9]  * pfInA[14])) + ((pfInB[13] * pfInA[15]));
	pfOut[14] = ((pfInB[2] * pfInA[12])) + ((pfInB[6] * pfInA[13])) + ((pfInB[10] * pfInA[14])) + ((pfInB[14] * pfInA[15]));
	pfOut[15] = ((pfInB[3] * pfInA[12])) + ((pfInB[7] * pfInA[13])) + ((pfInB[11] * pfInA[14])) + ((pfInB[15] * pfInA[15]));
}

/***************************************************************************/
void GL_P_SetCombinedMatrix( 
	GL_P_Matrix *pCombinedMat,
	GL_P_Matrix *pProjectionMat,
	GL_P_Matrix *pModelViewMat )
{
	GL_P_MultiplyMatrix( pCombinedMat, pProjectionMat, pModelViewMat->a );
}

/***************************************************************************/
void GL_P_SetNormalMatrix( 
	GL_P_Matrix *pNormalMat,
	GL_P_Matrix *pModelViewMat )
{
	GLfloat x00 = pModelViewMat->a[0];
	GLfloat x01 = pModelViewMat->a[1];
	GLfloat x02 = pModelViewMat->a[2];
	GLfloat x10 = pModelViewMat->a[4];
	GLfloat x11 = pModelViewMat->a[5];
	GLfloat x12 = pModelViewMat->a[6];
	GLfloat x20 = pModelViewMat->a[8];
	GLfloat x21 = pModelViewMat->a[9];
	GLfloat x22 = pModelViewMat->a[10];
	GLfloat x30 = -pModelViewMat->a[12];
	GLfloat x31 = -pModelViewMat->a[13];
	GLfloat x32 = -pModelViewMat->a[14];

	GLfloat z00 = x11 * x22 - x12 * x21;
	GLfloat z01 = x20 * x12 - x10 * x22;
	GLfloat z02 = x10 * x21 - x20 * x11;
	GLfloat z10 = x21 * x02 - x01 * x22;
	GLfloat z11 = x00 * x22 - x20 * x02;
	GLfloat z12 = x20 * x01 - x00 * x21;
	GLfloat z20 = x01 * x12 - x11 * x02;
	GLfloat z21 = x10 * x02 - x00 * x12;
	GLfloat z22 = x00 * x11 - x10 * x01;

	GLfloat rcp = x20 * z20 + x10 * z10 + x00 * z00;
	if( rcp == 0.0 )
	    return;
	rcp = (GLfloat) 1.0 / rcp;

	pNormalMat->a[0]  = z00 * rcp;
	pNormalMat->a[1]  = z10 * rcp;
	pNormalMat->a[2]  = z20 * rcp;
	pNormalMat->a[3]  = 0;
	pNormalMat->a[4]  = z01 * rcp;
	pNormalMat->a[5]  = z11 * rcp;
	pNormalMat->a[6]  = z21 * rcp;
	pNormalMat->a[7]  = 0;
	pNormalMat->a[8]  = z02 * rcp;
	pNormalMat->a[9]  = z12 * rcp;
	pNormalMat->a[10] = z22 * rcp;
	pNormalMat->a[11] = 0;
	pNormalMat->a[12] = pNormalMat->a[0] * x30 + pNormalMat->a[4] * x31 + pNormalMat->a[8]  * x32;
	pNormalMat->a[13] = pNormalMat->a[1] * x30 + pNormalMat->a[5] * x31 + pNormalMat->a[9]  * x32;
	pNormalMat->a[14] = pNormalMat->a[2] * x30 + pNormalMat->a[6] * x31 + pNormalMat->a[10] * x32;
	pNormalMat->a[15] = 1;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glMatrixMode( 
	GLenum mode )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	switch( mode )
	{
	case GL_MODELVIEW:
		pContext->pMatrixStack = &pContext->ModelViewMatrixStack;
		break;
	case GL_PROJECTION:
		pContext->pMatrixStack = &pContext->ProjectionMatrixStack;
		break;
	case GL_TEXTURE:
		pContext->pMatrixStack = &pContext->TextureMatrixStack;
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->eMatrixMode = mode;
	pContext->pMatrix = &pContext->pMatrixStack->aMatrices[pContext->pMatrixStack->uiTop];
}

/***************************************************************************/
GLAPI void APIENTRY glPushMatrix( void )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pContext->pMatrixStack->uiTop == 0 )
	{
		GL_P_SET_ERROR(GL_STACK_OVERFLOW);
		return;
	}

	pContext->pMatrixStack->uiTop--;
	pContext->pMatrix = &pContext->pMatrixStack->aMatrices[pContext->pMatrixStack->uiTop];

	GL_P_COPY_MATRIX_ARRAY( pContext->pMatrix->a, pContext->pMatrixStack->aMatrices[pContext->pMatrixStack->uiTop + 1].a );
}

/***************************************************************************/
GLAPI void APIENTRY glPopMatrix( void )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pContext->pMatrixStack->uiTop == GL_P_MATRIX_STACK_DEPTH - 1 )
	{
		GL_P_SET_ERROR(GL_STACK_UNDERFLOW);
		return;
	}

	pContext->pMatrixStack->uiTop++;
	pContext->pMatrix = &pContext->pMatrixStack->aMatrices[pContext->pMatrixStack->uiTop];
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glLoadIdentity( void )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_COPY_MATRIX_ARRAY( pContext->pMatrix->a, s_IdentityMatrix.a );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glLoadMatrixf( 
	const GLfloat *m )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	GL_P_COPY_MATRIX_ARRAY( pContext->pMatrix->a, m );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glLoadMatrixx( 
	const GLfixed *m )
{
	GLfloat af[16];
	GLint ii;

	for( ii = 0; ii < 16; ++ii )
		af[ii] = GL_P_CONVERT_XtoF(m[ii]);

	glLoadMatrixf( (const GLfloat *) af );
}

/***************************************************************************/
GLAPI void APIENTRY glMultMatrixf( 
	const GLfloat *m )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Matrix matrix = *pContext->pMatrix;

	GL_P_MultiplyMatrix( pContext->pMatrix, &matrix, (GLfloat *) m );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glMultMatrixx( 
	const GLfixed *m )
{
	GLfloat af[16];
	GLint ii;

	for( ii = 0; ii < 16; ++ii )
		af[ii] = GL_P_CONVERT_XtoF(m[ii]);

	glMultMatrixf( (const GLfloat *) af );
}

/***************************************************************************/
GLAPI void APIENTRY glFrustumf( 
	GLfloat left, 
	GLfloat right, 
	GLfloat bottom, 
	GLfloat top, 
	GLfloat zNear, 
	GLfloat zFar )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Matrix matrix;
	GLfloat afFrustum[16];
	GLfloat fX, fY, fA, fB, fC, fD;

	if( (left == right) || (top == bottom) || (zNear == zFar) || (zNear < 0) || (zFar < 0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	fX = (zNear * 2) / (right - left);
	fY = (zNear * 2) / (top - bottom);
	fA = (right + left) / (right - left);
	fB = (top + bottom) / (top - bottom);
	fC = -(zFar + zNear) / (zFar - zNear);
	fD = -(zFar * zNear * 2) / (zFar - zNear);

	GL_P_SET_MATRIX_ARRAY_ROW( afFrustum, 0,   fX, 0.0f,    fA, 0.0f );
	GL_P_SET_MATRIX_ARRAY_ROW( afFrustum, 1, 0.0f,   fY,    fB, 0.0f );
	GL_P_SET_MATRIX_ARRAY_ROW( afFrustum, 2, 0.0f, 0.0f,    fC,   fD );
	GL_P_SET_MATRIX_ARRAY_ROW( afFrustum, 3, 0.0f, 0.0f, -1.0f, 0.0f );

	GL_P_COPY_MATRIX_ARRAY( matrix.a, pContext->pMatrix->a );
	GL_P_MultiplyMatrix( pContext->pMatrix, &matrix, afFrustum );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glFrustumx( 
	GLfixed left, 
	GLfixed right, 
	GLfixed bottom, 
	GLfixed top, 
	GLfixed zNear, 
	GLfixed zFar )
{
	glFrustumf( GL_P_CONVERT_XtoF(left), GL_P_CONVERT_XtoF(right), GL_P_CONVERT_XtoF(bottom), 
		GL_P_CONVERT_XtoF(top), GL_P_CONVERT_XtoF(zNear), GL_P_CONVERT_XtoF(zFar) );
}

/***************************************************************************/
GLAPI void APIENTRY glOrthof( 
	GLfloat left, 
	GLfloat right, 
	GLfloat bottom, 
	GLfloat top, 
	GLfloat zNear, 
	GLfloat zFar )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Matrix matrix;
	GLfloat afOrtho[16];

	GLfloat fX =  2.0f / (right - left);
	GLfloat fY =  2.0f / (top - bottom);
	GLfloat fZ = -2.0f / (zFar - zNear);
	GLfloat fTX = -(right + left) / (right - left);
	GLfloat fTY = -(top + bottom) / (top - bottom);
	GLfloat fTZ = -(zFar + zNear) / (zFar - zNear);

	GL_P_SET_MATRIX_ARRAY_ROW( afOrtho, 0,   fX, 0.0f, 0.0f,  fTX );
	GL_P_SET_MATRIX_ARRAY_ROW( afOrtho, 1, 0.0f,   fY, 0.0f,  fTY );
	GL_P_SET_MATRIX_ARRAY_ROW( afOrtho, 2, 0.0f, 0.0f,   fZ,  fTZ );
	GL_P_SET_MATRIX_ARRAY_ROW( afOrtho, 3, 0.0f, 0.0f, 0.0f, 1.0f );

	GL_P_COPY_MATRIX_ARRAY( matrix.a, pContext->pMatrix->a );
	GL_P_MultiplyMatrix( pContext->pMatrix, &matrix, afOrtho );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glOrthox( 
	GLfixed left, 
	GLfixed right, 
	GLfixed bottom, 
	GLfixed top, 
	GLfixed zNear, 
	GLfixed zFar )
{
	glOrthof( GL_P_CONVERT_XtoF(left), GL_P_CONVERT_XtoF(right), GL_P_CONVERT_XtoF(bottom), 
		GL_P_CONVERT_XtoF(top), GL_P_CONVERT_XtoF(zNear), GL_P_CONVERT_XtoF(zFar) );
}

/***************************************************************************/
GLAPI void APIENTRY glRotatef( 
	GLfloat angle, 
	GLfloat x, 
	GLfloat y, 
	GLfloat z )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Matrix matrix;
	GLfloat afRotate[16];

	GLfloat fMag = (GLfloat) sqrt( x * x + y * y + z * z );
	GLfloat fX = (fMag != 0.0f) ? x / fMag : 0;
	GLfloat fY = (fMag != 0.0f) ? y / fMag : 0;
	GLfloat fZ = (fMag != 0.0f) ? z / fMag : 0;
	GLfloat fXX = fX * fX;
	GLfloat fYY = fY * fY;
	GLfloat fZZ = fZ * fZ;
	GLfloat fXY = fX * fY;
	GLfloat fYZ = fY * fZ;
	GLfloat fXZ = fZ * fX;
	GLfloat fAngle = angle * 3.1415926 / 180.0;
	GLfloat fSin = (GLfloat) sin( fAngle );
	GLfloat fCos = (GLfloat) cos( fAngle );
	GLfloat f1Cos = 1.0f - fCos;
	GLfloat fSX = fSin * fX;
	GLfloat fSY = fSin * fY;
	GLfloat fSZ = fSin * fZ;

	GL_P_SET_MATRIX_ARRAY_ROW( afRotate, 0, fXX * f1Cos + fCos, fXY * f1Cos - fSZ,  fXZ * f1Cos + fSY,  0.0f );
	GL_P_SET_MATRIX_ARRAY_ROW( afRotate, 1, fXY * f1Cos + fSZ,  fYY * f1Cos + fCos, fYZ * f1Cos - fSX,  0.0f );
	GL_P_SET_MATRIX_ARRAY_ROW( afRotate, 2, fXZ * f1Cos - fSY,  fYZ * f1Cos + fSX,  fZZ * f1Cos + fCos, 0.0f );
	GL_P_SET_MATRIX_ARRAY_ROW( afRotate, 3, 0.0f, 0.0f, 0.0f, 1.0f );

	GL_P_COPY_MATRIX_ARRAY( matrix.a, pContext->pMatrix->a );
	GL_P_MultiplyMatrix( pContext->pMatrix, &matrix, afRotate );
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glRotatex( 
	GLfixed angle, 
	GLfixed x, 
	GLfixed y, 
	GLfixed z )
{
	glRotatef( GL_P_CONVERT_XtoF(angle), GL_P_CONVERT_XtoF(x), GL_P_CONVERT_XtoF(y), GL_P_CONVERT_XtoF(z) );
}

/***************************************************************************/
GLAPI void APIENTRY glScalef( 
	GLfloat x, 
	GLfloat y, 
	GLfloat z )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->pMatrix->a[0]  *= x;
	pContext->pMatrix->a[1]  *= x;
	pContext->pMatrix->a[2]  *= x;
	pContext->pMatrix->a[3]  *= x;
	pContext->pMatrix->a[4]  *= y;
	pContext->pMatrix->a[5]  *= y;
	pContext->pMatrix->a[6]  *= y;
	pContext->pMatrix->a[7]  *= y;
	pContext->pMatrix->a[8]  *= z;
	pContext->pMatrix->a[9]  *= z;
	pContext->pMatrix->a[10] *= z;
	pContext->pMatrix->a[11] *= z;
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glScalex( 
	GLfixed x, 
	GLfixed y, 
	GLfixed z )
{
	glScalef( GL_P_CONVERT_XtoF(x), GL_P_CONVERT_XtoF(y), GL_P_CONVERT_XtoF(z) );
}

/***************************************************************************/
GLAPI void APIENTRY glTranslatef( 
	GLfloat x, 
	GLfloat y, 
	GLfloat z )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->pMatrix->a[12] += pContext->pMatrix->a[0] * x + pContext->pMatrix->a[4] * y + pContext->pMatrix->a[8]  * z;
	pContext->pMatrix->a[13] += pContext->pMatrix->a[1] * x + pContext->pMatrix->a[5] * y + pContext->pMatrix->a[9]  * z;
	pContext->pMatrix->a[14] += pContext->pMatrix->a[2] * x + pContext->pMatrix->a[6] * y + pContext->pMatrix->a[10] * z;
	pContext->pMatrix->a[15] += pContext->pMatrix->a[3] * x + pContext->pMatrix->a[7] * y + pContext->pMatrix->a[11] * z;
	pContext->pMatrixStack->bDirty = GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glTranslatex( 
	GLfixed x, 
	GLfixed y, 
	GLfixed z )
{
	glTranslatef( GL_P_CONVERT_XtoF(x), GL_P_CONVERT_XtoF(y), GL_P_CONVERT_XtoF(z) );
}

/***************************************************************************/
GLAPI void APIENTRY glQueryMatrixf( 
	const GLfloat **m )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pContext->ModelViewMatrixStack.bDirty || pContext->ProjectionMatrixStack.bDirty )
	{
		GL_P_Matrix *pModelViewMatrix = &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop];
		GL_P_Matrix *pProjectionMatrix = &pContext->ProjectionMatrixStack.aMatrices[pContext->ProjectionMatrixStack.uiTop];
		
		GL_P_SetCombinedMatrix( &pContext->CombinedMatrix, pProjectionMatrix, pModelViewMatrix );
		pContext->bNormalMatrixDirty = GL_TRUE;
	}

	*m = (const GLfloat *) (&pContext->CombinedMatrix.a);
*m = (const GLfloat *) (&pContext->pMatrix->a);
}

/* End of File */
