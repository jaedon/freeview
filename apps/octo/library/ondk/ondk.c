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




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/





/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC HBOOL	s_bIsONDKInit = FALSE;


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


ONDK_Result_t  ONDK_Init(HINT32 argc, HCHAR **argv)
{
	HERROR			hErr = ERR_OK;
	ONDK_Result_t 	oErr = ONDK_RET_OK;

	if(TRUE == s_bIsONDKInit)
	{
		return ONDK_RET_OK;
	}
	s_bIsONDKInit = TRUE;

	hErr = HLIB_CP_Init();
	if(ERR_OK != hErr)
	{
		ONDK_Assert(0);
		return ONDK_RET_FAIL;
	}

	oErr = ONDK_STDLIB_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

#if defined(CONFIG_DEBUG)
	oErr = ONDK_DEBUG_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);
#endif

	oErr = ONDK_DFB_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_IMG_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_FONT_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	oErr = ONDK_KEY_Init(argc, argv);
	ONDK_Assert(oErr == ONDK_RET_OK);

	//ONDK_GWM_RECV_Init(-1, 0);


	return oErr;
}

ONDK_Result_t  ONDK_Destroy(void)
{
	ONDK_Result_t err = ONDK_RET_OK;

	err = ONDK_DFB_Destroy();
	ONDK_Assert(err == ONDK_RET_OK);


	return err;
}


