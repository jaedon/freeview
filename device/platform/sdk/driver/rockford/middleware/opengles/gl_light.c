/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_light.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 7/29/10 6:24p $
 *
 * Module Description: Light entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_light.c $
 * 
 * Hydra_Software_Devel/10   7/29/10 6:24p nissen
 * SW7405-4657: Added support for using color array with lighting enabled.
 * 
 * Hydra_Software_Devel/9   10/22/08 7:58p nissen
 * PR 41194: Fixed spot attenuation calculation.
 * 
 * Hydra_Software_Devel/8   8/21/08 9:07p nissen
 * PR 42433: Reverted back to previous fix.
 * 
 * Hydra_Software_Devel/6   5/5/08 11:45a nissen
 * PR 42433: Fixed problem color material when using lighting.
 * 
 * Hydra_Software_Devel/5   9/14/06 3:22p nissen
 * PR 22858: Switched from a separate specular color to single color with
 * specular added to diffuse.
 * 
 * Hydra_Software_Devel/4   10/28/05 9:37a nissen
 * PR 17656: Fixed problem with specular lighting.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:27p nissen
 * PR 9132: Fixed lighting problems related to spot attenuation and spot
 * direction.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:34p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_AddVector( 
	GL_P_Coord3f *pVOut, 
	GL_P_Coord3f *pVIn0, 
	GL_P_Coord3f *pVIn1 )
{
	pVOut->fX = pVIn0->fX + pVIn1->fX;
	pVOut->fY = pVIn0->fY + pVIn1->fY;
	pVOut->fZ = pVIn0->fZ + pVIn1->fZ;
}

/***************************************************************************/
GLfloat GL_P_NormalizeVector( 
	GL_P_Coord3f *pV )
{
	GLfloat fSize = (GLfloat) sqrt( pV->fX * pV->fX + pV->fY * pV->fY + pV->fZ * pV->fZ );
	if( fSize != 0.0f )
	{
		pV->fX /= fSize;
		pV->fY /= fSize;
		pV->fZ /= fSize;
	}

	return fSize;
}

/***************************************************************************/
GLfloat GL_P_LightVector(
	GL_P_Coord3f *pVOut, 
	GL_P_Coord3f *pVIn0, 
	GL_P_Coord4f *pVIn1 )
{
	if( pVIn1->fW == 0.0f )
	{
		pVOut->fX = pVIn1->fX;
		pVOut->fY = pVIn1->fY;
		pVOut->fZ = pVIn1->fZ;
	}
	else
	{
		pVOut->fX = pVIn1->fX - pVIn0->fX;
		pVOut->fY = pVIn1->fY - pVIn0->fY;
		pVOut->fZ = pVIn1->fZ - pVIn0->fZ;
	}

	return GL_P_NormalizeVector( pVOut );
}

/***************************************************************************/
GLfloat GL_P_LightDotProduct( 
	GL_P_Coord3f *pVIn0, 
	GL_P_Coord3f *pVIn1 )
{
	GLfloat fDotProduct = pVIn0->fX * pVIn1->fX + pVIn0->fY * pVIn1->fY + pVIn0->fZ * pVIn1->fZ;

	return GL_P_MAX(fDotProduct, 0.0);
}

/***************************************************************************/
void GL_P_CalculateLightValues(
	EGL_P_Context *pContext, 
	GLsizei iCount )
{
	BP3D_Vertex *pVertices = pContext->pVertices;
	GL_P_Light *apLights[GL_P_LIGHT_MAX];
	GL_P_Coord4f EyePoint = { 0.0f, 0.0f, 1.0f, 0.0f };
	GL_P_Color *pAmbientColor = pContext->bColorMaterial ? (&pContext->Color) : (&pContext->Material.AmbientColor);
	GL_P_Color *pDiffuseColor = pContext->bColorMaterial ? (&pContext->Color) : (&pContext->Material.DiffuseColor);
	bool bColorArray = pContext->bColorMaterial && pContext->bColorArray;
	GLsizei iLightCount = 0;
	GLsizei ii, jj;

	for( ii = 0; ii < GL_P_LIGHT_MAX; ++ii )
	{
		if( pContext->bLight[ii] )
			apLights[iLightCount++] = &pContext->aLights[ii];
	}

	for( jj = 0; jj < iCount; ++jj )
	{
		GL_P_Coord3f *pEyeCoord = (GL_P_Coord3f *) (&pVertices->fX);
		GL_P_Coord3f *pNormal = (GL_P_Coord3f *) (&pVertices->fW);
		GL_P_Color SpecColor = { 0, 0, 0, 0 };
		GL_P_Color DiffColor;

		if( bColorArray )
		{
			pAmbientColor = (GL_P_Color *) (&pVertices->fS);
			pDiffuseColor = (GL_P_Color *) (&pVertices->fS);
		}

		GL_P_MUL_LIGHT_COLOR( &DiffColor, &pContext->AmbientColor, pAmbientColor );
		GL_P_ADD_LIGHT_COLOR( &DiffColor, &DiffColor, &pContext->Material.EmissionColor );

		for( ii = 0; ii < iLightCount; ++ii )
		{
			GL_P_Light *pLight = apLights[ii];
			GL_P_Coord3f LightVector;
			GL_P_Color AmbientColor;
			GL_P_Color DiffuseColor;
			GL_P_Color SpecularColor;
			GL_P_Color Color0;
			GL_P_Color Color1;
			GL_P_Color Color2;
			
			GLfloat fLightVectorSize = GL_P_LightVector( &LightVector, pEyeCoord, &pLight->Position );
			GLfloat fVectorDotProduct = GL_P_LightDotProduct( pNormal, &LightVector );
			GLfloat fAttenuation = 1.0;
			GLfloat fSpotAttenuation = 1.0;

			if( (pLight->Position.fW != 0.0) && ((pLight->fConstantAttenuation != 1.0) || 
				(pLight->fLinearAttenuation != 0.0) || (pLight->fQuadraticAttenuation != 0.0)) )
			{
				fAttenuation = 1.0 / (pLight->fConstantAttenuation + fLightVectorSize * 
					(pLight->fLinearAttenuation + pLight->fQuadraticAttenuation * fLightVectorSize));
			}

			if( pLight->fSpotCutoffAngle != 180.0 )
			{
				GL_P_Coord3f SpotVector;
				GLfloat fSpotDotProduct;

				SpotVector.fX = -LightVector.fX;
				SpotVector.fY = -LightVector.fY;
				SpotVector.fZ = -LightVector.fZ;

				fSpotDotProduct = GL_P_LightDotProduct( &SpotVector, &pLight->SpotDirection );
				if( (fSpotDotProduct > 0.0) && (fSpotDotProduct >= pLight->fSpotCutoffAngleCos) )
					fSpotAttenuation = (GLfloat) exp( pLight->fSpotExponent * log( fSpotDotProduct ) );
				else
					fSpotAttenuation = 0.0;
			}

			GL_P_MUL_LIGHT_COLOR( &AmbientColor, pAmbientColor, &pLight->AmbientColor );
			GL_P_CPY_LIGHT_COLOR( &Color0, &AmbientColor );

			if( fVectorDotProduct != 0.0 )
			{
				GL_P_MUL_LIGHT_COLOR( &DiffuseColor, pDiffuseColor, &pLight->DiffuseColor );
				GL_P_MUL_LIGHT_CONSTANT( &Color2, &DiffuseColor, fVectorDotProduct );
				GL_P_ADD_LIGHT_COLOR( &Color0, &Color0, &Color2 );
				GL_P_MUL_LIGHT_COLOR( &SpecularColor, &pContext->Material.SpecularColor, &pLight->SpecularColor );

				if( pContext->Material.fSpecularExponent != 0.0 )
				{
					GL_P_Coord3f EyeVector;
					GL_P_Coord3f SpecVector;
					GLfloat fSpotDot;
					GLfloat fShininess;

					GL_P_LightVector( &EyeVector, pEyeCoord, &EyePoint );
					GL_P_AddVector( &SpecVector, &LightVector, &EyeVector );
					GL_P_NormalizeVector( &SpecVector );

					fSpotDot = GL_P_LightDotProduct( pNormal, &SpecVector );
					fShininess = (GLfloat) exp( pContext->Material.fSpecularExponent * log( fSpotDot ) );
					GL_P_MUL_LIGHT_CONSTANT( &Color1, &SpecularColor, fShininess );
				}
				else
				{
					GL_P_CPY_LIGHT_COLOR( &Color1, &SpecularColor );
				}
			}
			else
			{
				GL_P_SET_LIGHT_COLOR( &Color1, 0.0, 0.0, 0.0, 0.0 );
			}

#if 1
			/* GL_LIGHT_MODEL_COLOR_CONTROL = GL_SINGLE_COLOR */
			GL_P_ADD_LIGHT_COLOR( &Color0, &Color0, &Color1 );
			GL_P_MUL_LIGHT_CONSTANT( &Color0, &Color0, fAttenuation * fSpotAttenuation );
			GL_P_ADD_LIGHT_COLOR( &DiffColor, &DiffColor, &Color0 );
#else
			/* GL_LIGHT_MODEL_COLOR_CONTROL = GL_SEPARATE_SPECULAR_COLOR (unsupported by OpenGL ES) */
			GL_P_MUL_LIGHT_CONSTANT( &Color0, &Color0, fAttenuation * fSpotAttenuation );
			GL_P_ADD_LIGHT_COLOR( &DiffColor, &DiffColor, &Color0 );

			GL_P_MUL_LIGHT_CONSTANT( &Color1, &Color1, fAttenuation * fSpotAttenuation );
			GL_P_ADD_LIGHT_COLOR( &SpecColor, &SpecColor, &Color1 );
#endif
		}

		DiffColor.fA = pDiffuseColor->fA;
		SpecColor.fA = 0.0f;

		pVertices->ulDiffuse = GL_P_CONVERT_COLOR_FtoI( DiffColor.fA, DiffColor.fR, DiffColor.fG, DiffColor.fB );
		pVertices->ulSpecular = GL_P_CONVERT_COLOR_FtoI( SpecColor.fA, SpecColor.fR, SpecColor.fG, SpecColor.fB );
		pVertices++;
	}
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glLightModelf( 
	GLenum pname, 
	GLfloat param )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pname != GL_LIGHT_MODEL_TWO_SIDE )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pContext->bLightModelTwoSide = (param == 0.0) ? GL_FALSE : GL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY glLightModelfv( 
	GLenum pname, 
	const GLfloat *params )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	switch( pname )
	{
	case GL_LIGHT_MODEL_TWO_SIDE:
		glLightModelf( pname, params[0] );
		break;
	case GL_LIGHT_MODEL_AMBIENT:
		GL_P_SET_LIGHT_COLOR( &pContext->AmbientColor, params[0], params[1], params[2], params[3] );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI void APIENTRY glLightModelx( 
	GLenum pname, 
	GLfixed param )
{
	glLightModelf( pname, (GLfloat) param );
}

/***************************************************************************/
GLAPI void APIENTRY glLightModelxv( 
	GLenum pname, 
	const GLfixed *params )
{
	GLfloat afParams[4];

	switch( pname )
	{
	case GL_LIGHT_MODEL_TWO_SIDE:
		glLightModelf( pname, GL_P_CONVERT_XtoF(params[0]) );
		break;
	case GL_LIGHT_MODEL_AMBIENT:
		afParams[0] = GL_P_CONVERT_XtoF(params[0]);
		afParams[1] = GL_P_CONVERT_XtoF(params[1]);
		afParams[2] = GL_P_CONVERT_XtoF(params[2]);
		afParams[3] = GL_P_CONVERT_XtoF(params[3]);
		glLightModelfv( pname, (const GLfloat *) afParams );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI void APIENTRY glLightf( 
	GLenum light, 
	GLenum pname, 
	GLfloat param )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Light *pLight;

	if( (light - GL_LIGHT0) >= GL_P_LIGHT_MAX )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pLight = pContext->aLights + (light - GL_LIGHT0);

	switch( pname )
	{
	case GL_SPOT_EXPONENT:
		pLight->fSpotExponent = param;
		break;
	case GL_SPOT_CUTOFF:
		pLight->fSpotCutoffAngle = param;
		pLight->fSpotCutoffAngleCos = (GLfloat) cos( (GL_P_PI * param) / 180.0f );
		break;
	case GL_CONSTANT_ATTENUATION:
		pLight->fConstantAttenuation = param;
		break;
	case GL_LINEAR_ATTENUATION:
		pLight->fLinearAttenuation = param;
		break;
	case GL_QUADRATIC_ATTENUATION:
		pLight->fQuadraticAttenuation = param;
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI void APIENTRY glLightfv( 
	GLenum light, 
	GLenum pname, 
	const GLfloat *params )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GL_P_Light *pLight;
	GL_P_Matrix *pMat;
	GL_P_Coord4f *pPos;
	GL_P_Coord3f *pDir;
	GLfloat fSize;

	if( (light - GL_LIGHT0) >= GL_P_LIGHT_MAX )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	pLight = pContext->aLights + (light - GL_LIGHT0);

	switch( pname )
	{
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
		glLightf( light, pname, params[0] );
		break;

	case GL_AMBIENT:
		GL_P_SET_LIGHT_COLOR( &pLight->AmbientColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_DIFFUSE:
		GL_P_SET_LIGHT_COLOR( &pLight->DiffuseColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_SPECULAR:
		GL_P_SET_LIGHT_COLOR( &pLight->SpecularColor, params[0], params[1], params[2], params[3] );
		break;

	case GL_POSITION:
		pMat = &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop];
		pPos = &pLight->Position;
		
		pPos->fX = pMat->a[0] * params[0] + pMat->a[4] * params[1] + pMat->a[8]  * params[2] + pMat->a[12] * params[3];
		pPos->fY = pMat->a[1] * params[0] + pMat->a[5] * params[1] + pMat->a[9]  * params[2] + pMat->a[13] * params[3];
		pPos->fZ = pMat->a[2] * params[0] + pMat->a[6] * params[1] + pMat->a[10] * params[2] + pMat->a[14] * params[3];
		pPos->fW = pMat->a[3] * params[0] + pMat->a[7] * params[1] + pMat->a[11] * params[2] + pMat->a[15] * params[3];

		if( pPos->fW != 0.0f )
		{
			pPos->fX /= pPos->fW;
			pPos->fY /= pPos->fW;
			pPos->fZ /= pPos->fW;
			pPos->fW = 1.0f;
		}
		else
		{
			fSize = (GLfloat) sqrt( pPos->fX * pPos->fX + pPos->fY * pPos->fY + pPos->fZ * pPos->fZ );
			if (fSize != 0.0f)
			{
				pPos->fX /= fSize;
				pPos->fY /= fSize;
				pPos->fZ /= fSize;
			}
		}
		break;

	case GL_SPOT_DIRECTION:
		pMat = &pContext->NormalMatrix;
		pDir = &pLight->SpotDirection;

		GL_P_SetNormalMatrix( pMat, &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop] );

		pDir->fX = pMat->a[0] * params[0] + pMat->a[4] * params[1] + pMat->a[8]  * params[2];
		pDir->fY = pMat->a[1] * params[0] + pMat->a[5] * params[1] + pMat->a[9]  * params[2];
		pDir->fZ = pMat->a[2] * params[0] + pMat->a[6] * params[1] + pMat->a[10] * params[2];

		fSize = (GLfloat) sqrt( pDir->fX * pDir->fX + pDir->fY * pDir->fY + pDir->fZ * pDir->fZ );
		if( fSize != 0.0f )
		{
			pDir->fX /= fSize;
			pDir->fY /= fSize;
			pDir->fZ /= fSize;
		}
		break;
	
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI void APIENTRY glLightx( 
	GLenum light, 
	GLenum pname, 
	GLfixed param )
{
	glLightf( light, pname, GL_P_CONVERT_XtoF(param) );
}

/***************************************************************************/
GLAPI void APIENTRY glLightxv( 
	GLenum light, 
	GLenum pname, 
	const GLfixed *params )
{
	GLfloat afParams[4];

	switch( pname )
	{
	case GL_SPOT_EXPONENT:
	case GL_SPOT_CUTOFF:
	case GL_CONSTANT_ATTENUATION:
	case GL_LINEAR_ATTENUATION:
	case GL_QUADRATIC_ATTENUATION:
		glLightf( light, pname, GL_P_CONVERT_XtoF(params[0]) );
		break;
	case GL_AMBIENT:
	case GL_DIFFUSE:
	case GL_SPECULAR:
	case GL_POSITION:
		afParams[0] = GL_P_CONVERT_XtoF(params[0]);
		afParams[1] = GL_P_CONVERT_XtoF(params[1]);
		afParams[2] = GL_P_CONVERT_XtoF(params[2]);
		afParams[3] = GL_P_CONVERT_XtoF(params[3]);
		glLightfv( light, pname, (const GLfloat *) afParams );
		break;
	case GL_SPOT_DIRECTION:
		afParams[0] = GL_P_CONVERT_XtoF(params[0]);
		afParams[1] = GL_P_CONVERT_XtoF(params[1]);
		afParams[2] = GL_P_CONVERT_XtoF(params[2]);
		glLightfv( light, pname, (const GLfloat *) afParams );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/* End of File */
