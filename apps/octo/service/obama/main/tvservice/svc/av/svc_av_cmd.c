/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_av_cmd.c
	@brief

	Description:  									\n
	Module: SVC / AV					\n

	Copyright (c) 2015 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <octo_common.h>

#include <pal_video.h>
#include <svc_av.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#if defined(CONFIG_DEBUG)

STATIC int svc_av_setccm(void *pvArgList)
{
	HCHAR	 	*aucArg = NULL;
	HINT32		nMode;

	if(pvArgList == NULL)
		return HxCMD_ERR;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArgList);
	if( aucArg == NULL )
	{ /* default is 'mute until first picture' */
		nMode = eChannelChangeMode_MuteUntilFirstPicture;
	}
	else
	{
		if( sscanf(aucArg, "%d", &nMode) == 0 )
		{
			return HxCMD_ERR;
		}
	}

	HLIB_CMD_Printf(HxANSI_COLOR_YELLOW("channel change mode = %s\n"), (0 == nMode) ? "Mute" : "Mute until first picture");
	SVC_AV_CMD_SetChannelChangeMode(nMode);

	return HxCMD_OK;
}

static HBOOL	s_bFccEnable;

STATIC int svc_av_togglefcc(void *pvArgList)
{
	if(pvArgList == NULL)
		return HxCMD_ERR;

	s_bFccEnable = (TRUE == s_bFccEnable) ? FALSE : TRUE;
	HLIB_CMD_Printf("** FCC %s\n", (TRUE == s_bFccEnable) ? "ENABLE" : "DISABLE");

	SVC_AV_CMD_SetFccEnable(s_bFccEnable);

	return HxCMD_OK;
}

#endif

void CMD_Register_SvcAv(void)
{
#ifdef CONFIG_DEBUG
#define	hCmdHandle		"svc_av"

#ifdef CONFIG_SUPPORT_FCC
	s_bFccEnable = TRUE;
#endif

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_av_setccm,
			"setccm",													/* keyword */
			"set channel change mode",									/*	help	*/
			"setccm [mode] (0:mute, not 0:mute until first picture");	/*	usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)svc_av_togglefcc,
			"togglefcc",	/* keyword */
			"togglefcc",	/*	help	*/
			"togglefcc");	/*	usage  */
#endif
}


/* end of file */
