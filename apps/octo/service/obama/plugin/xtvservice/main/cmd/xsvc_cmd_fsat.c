/********************************************************************
 * $Workfile:   cmd_channel.c  $
 * Project    : COMMON(ST)
 * Author     : Jino Lee
 * Description:
 *
 *                                 Copyright (c) 2002 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/
 #if defined(CONFIG_OP_FREESAT)

/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>

#include <htype.h>
#include <vkernel.h>
#include <hlib.h>

//#include <mgr_svclist.h>
//#include <xmgr_spec.h>
#include "octo_common.h"
#include <bus.h>
#include <db_param.h>
#include <svc_si.h>

#include <xsvc_cmd.h>
/********************************************************************
 Macro Definition
 ********************************************************************/



extern HUINT16 FSAT_GetBouquetId(void);
extern HUINT16 FSAT_GetRegiontCode(void);

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/
extern HUINT32	g_ulDebugEitSvcId;

#define _______FUNCTIONS______________________________
/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
static void local_cmd_PrintUsage(void)
{
	HLIB_CMD_Printf("\n");
	HLIB_CMD_Printf("Usage : fsat [all|bat|region|postcode_r|postcode_w|genre] value \n");
	HLIB_CMD_Printf("\n");
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
#if defined(CONFIG_DB_SVC_SVCGROUP)
extern void MGR_SVCLIST_InitSvcAttribute(DbSvc_Attr_t * pstAttr);

HERROR	local_cmd_PrintAllFreesatGenre(void)
{
	HUINT32			ulCnt, i, j;
	HERROR			hError;
	DbSvc_GroupInfo_t	*pstGroupInfo = NULL;
	DbSvc_Attr_t		stSvcAttr;
	HUINT32			ulChlistCount = 0;
	Handle_t			*phSvcList		= NULL;
	DbSvc_Info_t		stSvcInfo;

	HLIB_CMD_Printf("[[[[[[[[[[[[ FREESAT GENRE ]]]]]]]]]]]]\n");

	ulCnt = 0;
	hError = DB_SVC_GetAllGroupInfoList(&ulCnt, &pstGroupInfo);
	HLIB_CMD_Printf("[[[ Genre Count  = %d ]]]\n", ulCnt);
	if (hError == ERR_OK&& ulCnt != 0)
	{
		for (i = 0; i < ulCnt; i++)
		{
			HLIB_CMD_Printf("=== usGroupIdx : (%d) ===\n", pstGroupInfo[i].usGroupIdx);
			HLIB_CMD_Printf("\t Freesat Group ID : (%d)\n", pstGroupInfo[i].ulGroupId);
			HLIB_CMD_Printf("\t Non Destructive Tune Flag : (%d)\n", XdbSvc_FreeSatGroupGetNondestructiveTuneFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Return Channel Access Flag : (%d)\n", XdbSvc_FreeSatGroupGetReturnChannelAccessFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Group Type : (%d)\n", DB_SVC_GroupGetGroupType(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Group Name : (%s)\n", pstGroupInfo[i].szGroupName);
			HLIB_CMD_Printf("\t Language Code : (%s)\n", XdbSvc_FreeSatGroupGetIso639LangCode(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t G2 Extension Flag : (%d)\n", XdbSvc_FreeSatGroupGetG2ExtensionFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Default Visable Flag : (%d)\n", XdbSvc_FreeSatGroupGetDefaultVisibleFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Adult Channels Group Flag : (%d)\n", XdbSvc_FreeSatGroupGetAdultChannelsGroupFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Ip Channels Group Flag : (%d)\n", XdbSvc_FreeSatGroupGetIpChannelsGroupFlag(pstGroupInfo[i]));
			HLIB_CMD_Printf("\t Ca Channels Group Flag: (%d)\n", XdbSvc_FreeSatGroupGetCaChannelsGroupFlag(pstGroupInfo[i]));

			phSvcList = NULL;
			MGR_SVCLIST_InitSvcAttribute(&stSvcAttr);
			stSvcAttr.eSvcType = eDxSVC_TYPE_All;
			stSvcAttr.eOrgDeliType = eDxDELIVERY_TYPE_SAT;
			stSvcAttr.eSortType = eSortType_NUMBER;
			stSvcAttr.nGenreIdx = pstGroupInfo[i].usGroupIdx;

			hError = DB_SVC_FindServiceList(eSvcGroup_All,
									&stSvcAttr,
									0,
									HANDLE_NULL,
									eSvcFindDirection_FowardFromStart,
									&ulChlistCount, &phSvcList);
			if(hError == ERR_OK && phSvcList != NULL)
			{
				HLIB_CMD_Printf("\t == Service List Count (%d) ==\n", ulChlistCount);

				for(j = 0; j < ulChlistCount; ++j)
				{
					hError = DB_SVC_GetServiceInfo(phSvcList[j], &stSvcInfo);
					if (hError != ERR_OK)
						continue;

					HLIB_CMD_Printf("\t\t LCN : (%d)\n", DbSvc_GetLcn(&stSvcInfo));
					HLIB_CMD_Printf("\t\t\t Service Name : (%s)\n", DbSvc_GetName(&stSvcInfo));
				}
			}
			else
			{
				HLIB_CMD_Printf("\t == Service List of genre failed to get.\n");
			}

			DB_SVC_FreeServiceList(phSvcList);
		}

		DB_SVC_FreeGroupInfoList(pstGroupInfo);
	}

	return ERR_OK;
}
#endif

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
int local_cmd_fsat(void *szArgs)
{
	int					result = HxCMD_ERR;
	char					*szArg = NULL;
	char					szBuffer1[HxCMD_MAX_WORD_LEN];
	char					szBuffer2[HxCMD_MAX_WORD_LEN];
	int						cmdType = 0;
	int						value;
	char					postcode[HxCMD_MAX_WORD_LEN];
	int						batId, regionId;

	memset(szBuffer1, 0x00, sizeof(char)*HxCMD_MAX_WORD_LEN);
	memset(szBuffer2, 0x00, sizeof(char)*HxCMD_MAX_WORD_LEN);

	// get 1st argument
	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
	{
		local_cmd_PrintUsage();
		return 0;
	}

	result = sscanf(szArg, "%s", szBuffer1);
	if (result == 0)
	{
		local_cmd_PrintUsage();
		return 0;
	}

	if (!strncmp(szBuffer1, "bat", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 0;
	}
	else if (!strncmp(szBuffer1, "region", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 1;
	}
	else if (!strncmp(szBuffer1, "postcode_r", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 2;
	}
	else if (!strncmp(szBuffer1, "postcode_w", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 3;
	}
	else if (!strncmp(szBuffer1, "all", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 4;
	}
	else if (!strncmp(szBuffer1, "eit", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 5;
	}
	else if (!strncmp(szBuffer1, "adult", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 6;
	}
	else if (!strncmp(szBuffer1, "dy", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 7;
	}
	else if (!strncmp(szBuffer1, "genre", HxCMD_MAX_WORD_LEN))
	{
		cmdType = 8;
	}
	else
	{
		local_cmd_PrintUsage();
		return 0;
	}

	switch (cmdType)
	{
		case 0:
			value = (int) FSAT_GetBouquetId();
			HLIB_CMD_Printf("[%s][%d]: BouquetId=(%03d) \n", __FUNCTION__, __LINE__, value);
			break;
		case 1:
			value = FSAT_GetRegiontCode();
			HLIB_CMD_Printf("[%s][%d]: RegiontId=(%05d) \n", __FUNCTION__, __LINE__, value);
			break;
		case 2:
			postcode[0] = 0;
//			result = MWC_PARAM_GetPostcode(postcode, HxCMD_MAX_WORD_LEN-1);
			result = DB_PARAM_SetItem(eDB_PARAM_ITEM_POSTCODE, (HUINT32)postcode, HxCMD_MAX_WORD_LEN-1);
			HLIB_CMD_Printf("[%s][%d]: Postcode=(%s) \n", __FUNCTION__, __LINE__, postcode);
			break;
		case 3:
			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
			if (szArg != NULL)
			{
				int nLen, i;

				result = sscanf(szArg, "%s", szBuffer2);
				if (result == 0)
				{
					VK_strncpy(szBuffer2, "RH1 1XX", 9);
				}
				VK_strncpy(postcode, szBuffer2, 9);

				nLen = strlen(postcode);
				for (i = 0; i < nLen; i++)
				{
					if (postcode[i] == '_')
						postcode[i] = ' ';
				}
			}
//			MWC_PARAM_SetPostcode(postcode);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_POSTCODE, (HUINT32)postcode, 0);
			HLIB_CMD_Printf("[%s][%d]: Postcode is set =(%s) \n", __FUNCTION__, __LINE__, postcode);
			break;
		case 4:
			postcode[0] = 0;
//			result = MWC_PARAM_GetPostcode(postcode, HxCMD_MAX_WORD_LEN - 1);
			result = DB_PARAM_SetItem(eDB_PARAM_ITEM_POSTCODE, (HUINT32)postcode, HxCMD_MAX_WORD_LEN-1);

			batId = (int) FSAT_GetBouquetId();
			regionId = FSAT_GetRegiontCode();
			HLIB_CMD_Printf("[%s][%d]: RegionId=(%05d),BatId=(%03d),Postcode=(%s) \n", __FUNCTION__, __LINE__, regionId, batId, postcode);
			break;

		case 5:
			value = 0xFFFF + 1;

			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
			if (szArg != NULL)
			{
				result = sscanf(szArg, "%x", &value);
				g_ulDebugEitSvcId = value;

				HLIB_CMD_Printf("[%s][%d]: Debug Eit SvcID=(0x%04X) \n", __FUNCTION__, __LINE__, g_ulDebugEitSvcId);

			}
			break;
		case 6:
			value = 0xFFFF + 1;

			szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
			if (szArg != NULL)
			{
				result = sscanf(szArg, "%x", &value);
				if (value > 0)
				{
					value = 1;
				}
				else
				{
					value = 0;
				}
				DB_PARAM_SetItem(eDB_PARAM_ITEM_ADULT_CHANNEL, (HUINT32)value, 0);
				XMGR_SPEC_ProcessAdultService_Freesat(FSAT_ADULT_GENER_ID);
			}
			break;

		case 7:
			{
				extern HBOOL g_bFsatDynamicUpdate;

				if (g_bFsatDynamicUpdate == TRUE)
				{
					g_bFsatDynamicUpdate = FALSE;
					HLIB_CMD_Printf("[%s][%d]: Freesat Dynamic Update is OFF \n", __FUNCTION__, __LINE__);
				}
				else
				{
					g_bFsatDynamicUpdate = TRUE;
					HLIB_CMD_Printf("[%s][%d]: Freesat Dynamic Update is ON \n", __FUNCTION__, __LINE__);
				}
			}
			break;

		case 8:
#if defined(CONFIG_DB_SVC_SVCGROUP)
			local_cmd_PrintAllFreesatGenre();
#endif
			break;

		default:
			return HxCMD_ERR;
	}

	return 0;
}

/****************************************************************************
Description	:
Parameters	:
Output		:
Return		:
****************************************************************************/
void XCMD_RegisterFsat(void)
{
#define	hCmdHandle		"FSAT"
	HLIB_CMD_RegisterWord(hCmdHandle,local_cmd_fsat,
			"fsat",															/* keyword */
			"set or get information of freesat",							/*   help  */
			"....");
}
#endif


