/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"ondk_dfb.h"
#include 	"ondk_gfx.h"




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


static	IDirectFB				*s_pDirectFB = NULL;
static	IDirectFBDisplayLayer	*s_pLayer = NULL;
//static	ISaWMan 				*s_pISaWMan = NULL;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

IDirectFB		*ONDK_DFB_GetIDirectFB(void){
	return s_pDirectFB;
}


IDirectFBDisplayLayer	*ONDK_DFB_GetIDirectFBDisplayLayer(void){
	return s_pLayer;
}



ONDK_Result_t  ONDK_DFB_Init(HINT32 argc, HCHAR **argv)
{
	ONDK_Result_t oErr = ONDK_RET_OK;
	DFBResult err = DR_OK;

	if (s_pDirectFB == NULL)
	{
		DFBCHECK(DirectFBInit(&argc, &argv));
		DFBCHECK(DirectFBCreate(&s_pDirectFB));
//		DFBCHECK(SaWManCreate(&s_pISaWMan));
		DFBCHECK(s_pDirectFB->SetCooperativeLevel(s_pDirectFB, DFSCL_NORMAL));
		DFBCHECK(s_pDirectFB->GetDisplayLayer(s_pDirectFB, DLID_PRIMARY, &s_pLayer));
		DFBCHECK(s_pLayer->SetCooperativeLevel(s_pLayer, DLSCL_ADMINISTRATIVE));
	}

	oErr = ONDK_DFB_COLOR_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_DFB_VIDEO_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_WINDOW_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_SURFACE_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);


	return ONDK_RET_OK;
}


ONDK_Result_t  ONDK_DFB_Destroy(void)
{
	if (s_pLayer)
		s_pLayer->Release(s_pLayer);
	s_pLayer = NULL;

	if (s_pDirectFB)
		s_pDirectFB->Release(s_pDirectFB);
	s_pDirectFB = NULL;

	return ONDK_RET_OK;
}


