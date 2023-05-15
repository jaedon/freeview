/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
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

/**************************************************************************************************/
#define _________PAL_MMC_Private_Include_________________________________________________
/**************************************************************************************************/
#include "umma_fs_pal_fs.h"

#include "umma_fs_pal_mmc.h"
#include "di_mmc.h"

/**************************************************************************************************/
#define _________PAL_MMC_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PAL_MMC_Private_Struct_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PAL_MMC_Private_Value_________________________________________________
/**************************************************************************************************/
static PalMmc_Callback_t	s_PalMMC_Notifier;

/**************************************************************************************************/
#define _________PAL_MMC_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PAL_MMC_Private_Init__________________________________________________________
/**************************************************************************************************/
/**************************************************************************************************/
#define _________PAL_MMC_Private_Destroy__________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________PAL_MMC_Private_Functions__________________________________________________________
/**************************************************************************************************/
static void pal_mmc_ExecuteMwCallback (PalMmc_Event_e eEvt, HINT32 Port, HCHAR* pszName)
{
	if (s_PalMMC_Notifier != NULL)
	{
		(s_PalMMC_Notifier)(eEvt, Port, pszName);
	}
	else
	{
		HxLOG_Critical("\n\n");
	}
}

// Hot Plug Event callback..
static DI_ERR_CODE	pal_mmc_HotPlugEventCallback(DI_MMC_EVT_E evt, void *mmc_info)
{
	DI_MMC_HP_EVT_T	*pInfo = (DI_MMC_HP_EVT_T*)mmc_info;
	PalMmc_Event_e eEvent = ePAL_MMC_EVT_START;

	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC) Hot-Event[%d], name[%s] port[%d]\n"),evt,	pInfo->dev_name,pInfo->portnum);

	switch(evt)
	{
	case DI_MMC_EVT_CONNECTED:
		eEvent = ePAL_MMC_EVT_CONNECTED;
		break;
	case DI_MMC_EVT_DISCONNECTED:
		eEvent = ePAL_MMC_EVT_DISCONNECTED;
		break;
	default:
		eEvent = ePAL_MMC_EVT_MAX;
		break;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC)Hot-Event[%d]->[%d]\n"),evt,eEvent);

	pal_mmc_ExecuteMwCallback(eEvent,pInfo->portnum,pInfo->dev_name);

	return DI_ERR_OK;
}

// DI_MMC_SVC_FORMAT / DI_MMC_SVC_MOUNT / DI_MMC_SVC_UMOUNT   service callback..
static DI_ERR_CODE	pal_mmc_SvcCallback(DI_MMC_SVC_EVT_E evt)
{
	PalMmc_Event_e eEvent = ePAL_MMC_EVT_START;

	switch(evt)
	{
	case DI_MMC_SVC_EVT_FORMAT_DONE:
		eEvent = ePAL_MMC_EVT_FORMAT_DONE;
		break;
	case DI_MMC_SVC_EVT_FORMAT_FAIL:
		eEvent = ePAL_MMC_EVT_FORMAT_FAIL;
		break;
	case DI_MMC_SVC_EVT_MOUNT_DONE:
		eEvent = ePAL_MMC_EVT_MOUNT_DONE;
		break;
	case DI_MMC_SVC_EVT_MOUNT_FAIL:
		eEvent = ePAL_MMC_EVT_MOUNT_FAIL;
		break;
	case DI_MMC_SVC_EVT_UMOUNT_DONE:
		eEvent = ePAL_MMC_EVT_UMOUNT_DONE;
		break;
	case DI_MMC_SVC_EVT_UMOUNT_FAIL:
		eEvent = ePAL_MMC_EVT_UMOUNT_FAIL;
		break;
	case DI_MMC_SVC_EVT_MAX :
		eEvent = ePAL_MMC_EVT_MAX;
		break;
	}


	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC) SVC-Event[%d]->[%d]\n"),evt,eEvent);

	pal_mmc_ExecuteMwCallback(eEvent,-1,NULL);
	return DI_ERR_OK;
}




#if defined(CONFIG_DEBUG)

/**************************************************************************************************/
#define _________PAL_MMC_CMD___________________________________________________________
/**************************************************************************************************/
STATIC int pal_mmc_CMD_mount (void *szArgList)
{
	DI_ERR_CODE err;
	HUINT8			*aucArg_1=NULL,*aucArg_2=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;


	HxLOG_Trace();
	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}
	else
		return HxCMD_ERR;

	/* 2st argument */
	aucArg_2 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_2 == NULL )
		return HxCMD_ERR;




	err = DI_MMC_Mount(arg1,aucArg_2);

	HLIB_CMD_Printf("MMC Mount Err[%d]\n", aucArg_2,err);
	return HxCMD_OK;
}



STATIC int pal_mmc_CMD_umount (void *szArgList)
{
	DI_ERR_CODE err;
	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;


	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}else
		return HxCMD_ERR;


	err = DI_MMC_Umount(arg1);

	HLIB_CMD_Printf("MMC DI_MMC_Umount Err[%d]\n",err);
	return HxCMD_OK;
}



STATIC int pal_mmc_CMD_getinfo(void *szArgList)
{
	DI_ERR_CODE 	err;
	DI_MMC_INFO_T tMmcInfo;

	HUINT8			*aucArg_1=NULL;
	HUINT8		   aucStr1[20];
	unsigned long   arg1 = 0;
	char 		   *szCmd = (char *)szArgList;


	/* 1st argument */
	aucArg_1 = HLIB_CMD_GetNextArg((HCHAR**)&szCmd);
	if( aucArg_1 != NULL )
	{
		if( sscanf(aucArg_1, "%s", (char *)&aucStr1) == 0 )
			return HxCMD_ERR;
		arg1 = atoi(aucStr1);
	}else
		return HxCMD_ERR;

	err = DI_MMC_GetMMCInfo(arg1, &tMmcInfo);

	if(err != DI_ERR_OK)
	{
		HLIB_CMD_Printf("PAL_MMC_GetMMCInfo Fail....[%d] Err[%d]\n", arg1,err);
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("MMC Info.....\n");
	HLIB_CMD_Printf("Port [%d]\n",tMmcInfo.nPort);
	HLIB_CMD_Printf("Mounted [%d]\n",tMmcInfo.nMounted);
	HLIB_CMD_Printf("DiskSize [%d]\n",tMmcInfo.uiDiskSize);
	HLIB_CMD_Printf("uiAvailableSize [%d]\n",tMmcInfo.uiAvailableSize);
	HLIB_CMD_Printf("uiUsedSize [%d]\n",tMmcInfo.uiUsedSize);

	return HxCMD_OK;
}






STATIC void pal_mmc_cmdInit(void)
{
#define	hCmdHandle		"FS"
	HLIB_CMD_RegisterWord (hCmdHandle,
							pal_mmc_CMD_mount,
							"mmc_mount", 						/* keyword */
							"MMC mount",			/*	 help  */
							"[port],[Mount path]"
							);

	HLIB_CMD_RegisterWord (hCmdHandle,
							pal_mmc_CMD_umount,
							"mmc_umount", 						/* keyword */
							"MMC umount",			/*	 help  */
							"[port]"
							);

	HLIB_CMD_RegisterWord (hCmdHandle,
						pal_mmc_CMD_getinfo,
						"mmc_info", 						/* keyword */
						"MMC getinfo",			/*	 help  */
						"[port]"
						);
}
#endif

/**************************************************************************************************/
#define _________PAL_MMC_Public_Functions__________________________________________________________
/**************************************************************************************************/
HERROR PAL_MMC_Initialize (void)
{
	DI_MMC_Init();
	if(DI_MMC_RegisterCallbackFunc(pal_mmc_HotPlugEventCallback) !=DI_ERR_OK)
  		HxLOG_Critical("PAL-MMC Hot plug RegisterCallback Error!!\n");

  	if(DI_MMC_RegisterSvcCallbackFunc(pal_mmc_SvcCallback) !=DI_ERR_OK)
  		HxLOG_Critical("PAL-MMC Service RegisterCallback Error!!\n");

#if defined(CONFIG_DEBUG)
	pal_mmc_cmdInit();
#endif

  return ERR_OK;
}


void PAL_MMC_RegisterNotifier (PalMmc_Callback_t fsCallback)
{
	if (fsCallback != NULL)
	{
		s_PalMMC_Notifier = fsCallback;
	}
	else
	{
		HxLOG_Critical("\n\n\n");
	}
}


HERROR PAL_MMC_Mount(HINT32 Port,HCHAR* pszMountath)
{
	DI_ERR_CODE err;
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC)Mount[%d][%s]\n"), Port,pszMountath);
	err = DI_MMC_Mount(Port,pszMountath);

	if(err != DI_ERR_OK)
	{
		HxLOG_Error(HxANSI_COLOR_YELLOW("PAL-MMC Mount Fail..[%s] Err[%d]\n"), pszMountath,err);
		return ERR_FAIL;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC Mount OK.. [%s]\n"), pszMountath);
	return ERR_OK;
}


HERROR PAL_MMC_UnMount(HINT32 Port)
{
	DI_ERR_CODE err;
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC)UnMount[%d]\n"), Port);
	err = DI_MMC_Umount(Port);

	if(err != DI_ERR_OK)
	{
		HxLOG_Error("PAL-MMC UnMount Fail..[%d] Err[%d]\n", Port,err);
		return ERR_FAIL;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC UnMount OK.. [%d]\n"), Port);
	return ERR_OK;
}



HERROR PAL_MMC_GetMMCInfo(HINT32 Port,PalMmc_Info_e *pInfo)
{
	DI_MMC_INFO_T tMmcInfo;
	DI_ERR_CODE err;

	err = DI_MMC_GetMMCInfo(Port, &tMmcInfo);

	if(err != DI_ERR_OK)
	{
		HxLOG_Error("PAL_MMC_GetMMCInfo Fail....[%d] Err[%d]\n", Port,err);
		return ERR_FAIL;
	}
	pInfo->nPort			= tMmcInfo.nPort;
	pInfo->nMounted 		= tMmcInfo.nMounted;
	pInfo->uiDiskSize		= tMmcInfo.uiDiskSize;
	pInfo->uiAvailableSize 	= tMmcInfo.uiAvailableSize;
	pInfo->uiUsedSize		= tMmcInfo.uiUsedSize;

	HxLOG_Print("tMmcInfo.szUuid : %s\n",tMmcInfo.szUuid);

	HxSTD_StrNCpy(pInfo->szUuid, tMmcInfo.szUuid, HxSTD_StrLen(tMmcInfo.szUuid));
	return ERR_OK;
}

HERROR PAL_MMC_Format(HINT32 Port)
{
	DI_ERR_CODE err;
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MMC)Format[%d]\n"), Port);
	err = DI_MMC_Format(Port);
	if(err != DI_ERR_OK)
	{
		HxLOG_Error("PAL-MMC Format Fail..[%d] Err[%d]\n", Port,err);
		return ERR_FAIL;
	}
	HxLOG_Print(HxANSI_COLOR_YELLOW("PAL-MOUNT Format OK.. [%d]\n"), Port);
	return ERR_OK;
}

HERROR PAL_MMC_GetVolumeName(HINT32 nPort, HUINT8 *pszName, HUINT32 ulsize)
{
	return	DI_MMC_GetVolumeName(nPort, pszName, ulsize);
}

HERROR PAL_MMC_SetVolumeName(HINT32 nPort, HUINT8 *pszName, HUINT32 ulsize)
{
	return	DI_MMC_SetVolumeName(nPort, pszName, ulsize);
}

HERROR PAL_MMC_PowerOn(void)
{
	return DI_MMC_PowerControl(DI_MMC_PORT_1, DI_MMC_PWR_ON);
}
HERROR PAL_MMC_PowerOff(void)
{
	return DI_MMC_PowerControl(DI_MMC_PORT_1, DI_MMC_PWR_OFF);
}


/**************************************************************************************************/
#define _________PAL_MMC_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/
#ifdef	CONFIG_DEBUG

#endif


/**************************************************************************************************/
#define _________PAL_MMC_Public_Proc____________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________PAL_MMC_End_Of_File____________________________________________________
/**************************************************************************************************/

