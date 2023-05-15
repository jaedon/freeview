/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_extdev.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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
#include	<hlib.h>
#include	<apk.h>

#include <uapi.h>
#include <oapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	DxHotplugNotifier_t	pfNotifier;
}apkExtDev_Ctx_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static	apkExtDev_Ctx_t	s_ExtDevCtx;
/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/

#define	________LOCAL___Prototype______


#define	________LOCAL___Body______

apkExtDev_Ctx_t* apk_extdev_GetContext(void)
{
	return &s_ExtDevCtx;
}


// apArgv[1] = eNotitype
// apArgv[2] = p1
// apArgv[3] = p2
// apArgv[4] = p3
static void	apk_extdev_freeEvent(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;
//	HINT32	p1;
	DxHotplug_NotiType_e	eNotiType;

	eNotiType = (DxHotplug_NotiType_e)apArgV[1];
	switch (eNotiType)
	{
		case	eDxHOTPLUG_NOTI_DevChange:
			pvData = apArgV[3];
			if (pvData)
				HLIB_STD_MemFree(pvData);
			break;

		default:
			break;
	}
}

void	apk_extdev_Notifier(DxHotplug_NotiType_e eNotiType, HINT32 p1, HINT32 p2, HINT32 p3)
{
	apkExtDev_Ctx_t* pstCtx;
	DxHotplugNotifier_t pfNoti;
	APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};
	pstCtx = apk_extdev_GetContext();

	HxLOG_Warning("eNotiType : %d, connect : %s\n", eNotiType, (p1==1)?"TRUE":"FALSE");

	pfNoti = pstCtx->pfNotifier;
	if(pfNoti)
	{
		switch (eNotiType)
		{
			case	eDxHOTPLUG_NOTI_DevChange:
				p2 = (HINT32)HLIB_MEM_MemDup((void *)p2, p3);
				p3 = 0;
				break;

			default:
				break;
		}

		stApkEvtArgs.pfnFreeFunc = apk_extdev_freeEvent;
		stApkEvtArgs.ulArgCount = 5;
		stApkEvtArgs.apArgV[0] = (void *)pfNoti;
		stApkEvtArgs.apArgV[1] = (void *)eNotiType;
		stApkEvtArgs.apArgV[2] = (void *)p1;
		stApkEvtArgs.apArgV[3] = (void *)p2;
		stApkEvtArgs.apArgV[4] = (void *)p3;

		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#if defined(CONFIG_DEBUG)
HINT32 apk_extdev_cmdMicStart(void *pvArg)
{
	APK_EXTDEV_MIC_Start();
	return 0;
}
HINT32 apk_extdev_cmdMicStop(void *pvArg)
{
	APK_EXTDEV_MIC_Stop();
	return 0;
}
HINT32 apk_extdev_cmdSetMicVolume(void *pvArg)
{

	HCHAR	*aucArg=NULL;
	int		nValue = 0;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%d", &nValue) == 0 ) return HxCMD_ERR;

	APK_EXTDEV_MIC_SetVolume(nValue);
	return 0;
}

HINT32 apk_extdev_cmdGetMicVolume(void *pvArg)
{
	HINT32	nVolume;

	APK_EXTDEV_MIC_GetVolume(&nVolume);

	HLIB_CMD_Printf("MIC Volume : %d\n", nVolume);

	return 0;
}

HINT32 apk_extdev_cmdGetMicCount(void *pvArg)
{
	HUINT32	ulCnt;

	APK_EXTDEV_MIC_GetCount(&ulCnt);

	HLIB_CMD_Printf("MIC Cnt : %d\n", ulCnt);

	return 0;
}

void	apk_extdev_InitCommand(void)
{
#define	hCmdHandle		"APPKIT"
	HLIB_CMD_RegisterWord (hCmdHandle, apk_extdev_cmdMicStart, "micstart", "Start MIC", "micstart");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_extdev_cmdMicStop, "micstop", "Stop MIC", "micstop");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_extdev_cmdSetMicVolume, "setmv", "Set MIC volume", "setmv");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_extdev_cmdGetMicVolume, "getmv", "Get MIC volume", "getmv");
	HLIB_CMD_RegisterWord (hCmdHandle, apk_extdev_cmdGetMicCount, "getmc", "Get the number of MIC", "getmc");
}
#endif



#if 0	// for test
void test_noti(DxHotplug_NotiType_e eType, HINT32	p1, HINT32 p2, HINT32 p3)
{
	if (eDxSTORAGE_NOTI_DevChange == eType)
	{
		HBOOL	bConnect = (HBOOL)p1;
		DxHotplug_Info_t	*pstInfo = (DxHotplug_Info_t	*)p2;

		HxLOG_Warning("bConnect : %d, devtype : %d\n", bConnect, pstInfo->eDevType);
	}
}
#endif


#define ________GLOBAL________________
HERROR	APK_EXTDEV_Init(void)
{
#ifdef CONFIG_DEBUG
	apk_extdev_InitCommand();
#endif

// for test
//	APK_EXTDEV_RegisterNotifier(test_noti1);
	OAPI_EXTDEV_Init();

	return UAPI_HOTPLUG_Init();
}


void	APK_EXTDEV_RegisterNotifier(DxHotplugNotifier_t pfnObjectHandler)
{
	apkExtDev_Ctx_t* pstCtx;
	pstCtx = apk_extdev_GetContext();

	pstCtx->pfNotifier = pfnObjectHandler;

	UAPI_HOTPLUG_SetNotifier(apk_extdev_Notifier);
}

HERROR	APK_EXTDEV_MIC_Start(void)
{
	HxLOG_Trace();
	return OAPI_EXTDEV_MIC_Start();
}

HERROR	APK_EXTDEV_MIC_Stop(void)
{
	HxLOG_Trace();
	return OAPI_EXTDEV_MIC_Stop();
}

HERROR	APK_EXTDEV_MIC_SetVolume(HINT32 nValue)
{
	HxLOG_Trace();
	return OAPI_EXTDEV_MIC_SetVolume(nValue);
}

HERROR	APK_EXTDEV_MIC_GetVolume(HINT32 *pnValue)
{
	HxLOG_Trace();
	return OAPI_EXTDEV_MIC_GetVolume(pnValue);
}

// 연결된 MIC 개수
HERROR	APK_EXTDEV_MIC_GetCount(HUINT32 *pulCnt)
{
	HxLOG_Trace();
	return UAPI_HOTPLUG_GetDevCount(eDxHOTPLUG_DEVTYPE_MIC, pulCnt);
}

HERROR	APK_EXTDEV_MIC_GetStatus(HBOOL* pbStatus)
{
	HxLOG_Trace();
	return OAPI_EXTDEV_MIC_GetStatus(pbStatus);
}

