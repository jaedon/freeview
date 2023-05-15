/**************************************************************
*	@file	cmd_dms.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMS
**************************************************************/

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

#define	________________Header_Files_______________________________
#include <vkernel.h>
#include <hlib.h>

#include "homapi.h"
#include "hom_int.h"
#include "homapi_dms.h"
#include "dms.h"
#include "pl_dms.h"
#include "pl_ch_list.h"

#define ________________Internal_Functions_________________________
#if defined(CONFIG_DEBUG)
HINT32   cmd_dms_set_act(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if ( !HxSTD_StrCmp( "INIT", aucStr ) )
	{
		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
		PL_DMS_Init( NULL );
	}
	else if ( !HxSTD_StrCmp( "START", aucStr ) )
	{
		DMS_Config_t stConfig;
		HxSTD_MemSet(&stConfig, 0x00, sizeof(DMS_Config_t));
		HxSTD_StrNCpy( stConfig.szFriendlyName, "HMS 1000S DMS", sizeof(stConfig.szFriendlyName)-1 );

		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
		DMS_Start(&stConfig);
		//PL_DMS_Start();
	}
	else if ( !HxSTD_StrCmp( "STOP", aucStr ) )
	{
		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
		DMS_Stop();
		//PL_DMS_Stop();
	}
	else if ( !HxSTD_StrCmp( "ADDPATH", aucStr ) )
	{
	  	PL_DMS_ScanOpt_t scanOpt = { 0 };
        HCHAR szAll[256] = { 0 };
        HCHAR mediaPath[256] = { 0 };

		HLIB_CMD_Printf("DMS action : %s\n", aucStr);

        scanOpt.eDepth = ePL_DMS_ScanDepth_Recursive;
        scanOpt.pszUserData = NULL;
        scanOpt.pszFilter = NULL;

        HxSTD_snprintf( szAll, sizeof(szAll),  "%s%s%s", PL_DMS_DEFAULT_SCANFILTER_VIDEO, PL_DMS_DEFAULT_SCANFILTER_MUSIC, PL_DMS_DEFAULT_SCANFILTER_PHOTO);
        HxSTD_snprintf( mediaPath, sizeof(mediaPath),  "%s", "/mnt/hd2/Media");
        scanOpt.pszFilter = szAll;

        PL_DMS_AddScanDirectory( mediaPath, &scanOpt );
	}
	else if ( !HxSTD_StrCmp( "SCAN", aucStr ) )
	{
		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
 		PL_DMS_StartScan( PL_DMS_DEFAULT_RESCAN_INTERVAL, PL_DMS_DEFAULT_RESCAN_INTERVAL_FILE );
	}
	else if ( !HxSTD_StrCmp( "SCANSTOP", aucStr ) )
	{
		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
		PL_DMS_StopScan();
	}
	else if (!HxSTD_StrCmp( "CRASH", aucStr ))
	{
#if 0 // if you want to test crash, set to 1
	  HLIB_CMD_Printf("DMS action : %s\n", aucStr);
	  {
		volatile int* a = (int*)(NULL);
		*a = 1;
	  }
#endif
	}
  	else
	{
		HLIB_CMD_Printf("Not supported : %s\n", aucStr);
	}

	return HxCMD_OK;
}

HINT32   cmd_dms_get_act(void *szArgList)
{
	HCHAR	*aucArg=NULL, aucStr[20];
	void	*scanData = NULL;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	HLIB_CMD_Printf("%s\n", aucStr);
	if ( !HxSTD_StrCmp( "STATUS", aucStr ) )
	{
		HLIB_CMD_Printf("DMS action : %s\n", aucStr);
	}
	else if ( !HxSTD_StrCmp( "SCANLIST", aucStr ) )
	{
		HINT32 scanCnt = 0;
		HCHAR *scanPath = NULL;

		HxList_t *pathList = NULL;

		HLIB_CMD_Printf("** DMS action : %s\n", aucStr);

		if(PL_DMS_GetScanPathList( &scanCnt, &pathList ) != ERR_OK)
		{
			HLIB_CMD_Printf("** DMS is not started. FSS is NULL \n");
			return HxCMD_OK;
		}

		HLIB_CMD_Printf("** Scan List Count  : %d\n", scanCnt);

		while ( pathList )
		{
			scanData = (void *)HLIB_LIST_Data( pathList);
			(void)PL_DMS_GetScanPath( scanData, &scanPath );
			HLIB_CMD_Printf("PATH - %s \n", scanPath);
			pathList = pathList->next;
		}

		(void)PL_DMS_FreeScanPathList( pathList );
	}
	else if (VK_strcmp(aucStr, "LIVESTREAMINFO") == 0)
	{
		HINT32 nSvcUid = 0;

		HUINT32 nCount = 0;
		HOMxDMS_LiveStreamInfo_t *pResultData = NULL;

		aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
		if( aucArg == NULL )
			return HxCMD_ERR;

		nSvcUid = atoi(aucArg);

		HLIB_CMD_Printf("nSvcUid %d\n", nSvcUid);

		HOMAPI_DMS_GetLiveStreamInfo(nSvcUid, &nCount, &pResultData);

		if(pResultData != NULL)
		{
			HINT32 i;

			for(i = 0; i < nCount; i++) {
				HLIB_CMD_Printf("%d) szURL[%d] : %s, szMIME[%d] : %s\n",
					nSvcUid, i, pResultData[i].szURI, i, pResultData[i].szMIME);
			}

			HOMAPI_DMS_FreeLiveStreamInfo( nCount, pResultData );
		}
	}
	else
	{

	}

	return HxCMD_OK;
}

HINT32   cmd_dms_live_act(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

#if 0
	if(VK_strcmp(aucStr, "START") == 0)
	{
		APKS_DLNA_LIVESTREAM_SETUP_t setup;
		APK_DLNA_OBAMA_LiveStream_Start( &setup );
	}
	if(VK_strcmp(aucStr, "STOP") == 0)
	{
		HINT32	handle = 123;
		APK_DLNA_OBAMA_LiveStream_Stop( handle );
	}
	if(VK_strcmp(aucStr, "STATUS") == 0)
	{
		HINT32 	handle = 1;
		HINT32	type = 2;
		APKS_DLNA_LIVESTREAM_STATUS_t status;
		printf ( "[%s:%s:%d] \n", __FILE__, __FUNCTION__, __LINE__ );
		APK_DLNA_OBAMA_LiveStream_Status(handle, type, &status);
		printf("[%s:%d] LiveStream Status Result : rating(%d) transcode(%d) \n", __func__, __LINE__, status.rating, status.is_transcode);
	}
	if(VK_strcmp(aucStr, "PROFILE") == 0)
	{
		HINT32 index = -1;
		printf ( "[%s:%s:%d] \n", __FILE__, __FUNCTION__, __LINE__ );

		APK_DLNA_OBAMA_LiveStream_ItemindexByProfile("TEST_CODEC_PROFILE", &index);
		printf("[%s:%d] LiveStream Index Result : %d \n", __func__, __LINE__, index);
	}
	else
	{

	}
#endif

	return HxCMD_OK;
}

HINT32   cmd_dms_channel_list_act(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "UPDATE") == 0)
	{
		HLIB_CMD_Printf("\n** channel list update \n\n");
        PL_COMMON_UpdateChannelList();
	}
	if(VK_strcmp(aucStr, "SHOW") == 0)
	{
	}
	else
	{

	}

	return HxCMD_OK;
}
#endif

#define _____PUBLIC_FUNCTIONS____
void	HOMMA_CMD_Register_DMS(void)
{
#if defined (CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( "dms", cmd_dms_set_act, "set", "init | start | stop | crash ", "dms set action");
	HLIB_CMD_RegisterWord( "dms", cmd_dms_get_act, "get", "status | scanlist | livestreaminfo", "dms get action");

	HLIB_CMD_RegisterWord( "dms", cmd_dms_live_act, "live", "start | stop | status | profile ", "dms livestream action");
	HLIB_CMD_RegisterWord( "dms", cmd_dms_channel_list_act, "channel", "update | show | ", "dms channel list action");
#endif
}

#define _____END_OF_FILE_________
