/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>

#include <linkedlist.h>
#include <pal_pipe.h>
#include <pal_sef.h>
#include <pal_sys.h>
#include <pal_param.h>

#include <svc_si.h>
#include <util.h>
#include <svc_download.h>

/** @brief local header file  */
#include <_svc_download.h>
#include <svc_swup.h>
#include "../../local_include/_xsvc_download.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SECTION_HEADER_LEN					(8)
#define DDB_HEADER_LEN						(12)
#define SECTION_MESSAGEID_POS					(10)
#define MAX_FILTERING_NUM					(3)
#define INVALID_INFO_INDEX					(99)
#define MAX_CACHE_NUM						(12)

#define ONE_BYTE_FILTER						0x000000FF
#define BASE_BIT_POSITION					(24)
#define ONE_BYTE_POSITION					(8)


#define MAX_PRE_RECEIVE_SIZE				818400//409200//204800

#define DDB_DOWNLOAD_TIMEOUT				120


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagOTA_META_Header
{
	HUINT32			ulDownloadCompleted;
	HUINT32			ulFileNumTotal;
	HUINT32			ulFileSize;
	HUINT32			ulReceivecTotal;
	HUINT32			ulSignalVersion;
	HUINT32			ulProgress;
	HUINT32			ulProgressBackup;
	HULONG			ulFp;
} XsvcDownload_OtaMetaHeader_t;

typedef struct tagOTA_META_INFO
{
	HBOOL				 bStarted;
	XsvcDownload_OtaMetaHeader_t	 stOTAMetaHeader;
	HUINT8				*pucOTAMetaStatus;
} XsvcDownload_OtaMetaInfo_t;

typedef struct tagDOWNLOAD_OTA_DBINFO
{
	HBOOL		bDownloadCompleted;
	HUINT32		ulDownloadBlockNumTotal;
	HUINT32		ulDownloadBlockSize;
	HUINT32		ulDownloadedVersion;
} XsvcDownload_OtaDbInfo_t;

typedef struct
{
	//HUINT16				usCacheIndex;
	HUINT16				usPid;
	HUINT16				usDsiFilterId, usDdbFilterId;
	HUINT32				ulSignalVersion;

	// DVB-SSU Simple 다운로드 시에는 Group 1, Module 1이다.
	// 빠른 계산을 위해 여기에 GroupInfo와 ModuleInfo의 pointer를 놓도록 한다.
	SvcDownload_DnGrp_t		*pstGroupInfo;
	SvcDownload_DnMod_t		*pstModuleInfo;

	HBOOL				bDsiParsed, bDiiParsed;
	HBOOL				bDdbZeroBlockParsed;
	HBOOL				bDdbAllReceived;
} XsvcDownload_DsmccDnInfo_t;

typedef struct tagDnldDvbssuUpgrade
{
	Handle_t			hTargetStorage;
	HUINT32				ulDataSize;
	HUINT32				ulPeiceSize;
	XsvcDownload_OtaMetaInfo_t		stMetaInfo;
} XsvcDownload_UpgradeInfo_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XsvcDownload_UpgradeInfo_t	s_stSsuUpgradeInfo;
STATIC UNIXTIME			 			s_timeDdbTimeout = 0;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC XsvcDownload_OtaMetaInfo_t *xsvc_download_GetMetaInfo_Dvbssu(void);
STATIC XsvcDownload_UpgradeInfo_t *xsvc_download_GetUpgradeInfo_Dvbssu(void);

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_AllocDownloadInfo_Dvbssu (void);
STATIC void						  xsvc_download_FreeDownloadInfo_Dvbssu (XsvcDownload_DsmccDnInfo_t *pstInfo);
STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_GetDownloadInfo_Dvbssu (SvcDownload_DsmccContents_t *pstContents);


STATIC HERROR 		xsvc_download_StartDsiDiiFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents);		/* Index가 필요 없다. 파라미터로 전달된 모든 PID에 대해 filtering을 한다. */
STATIC HERROR 		xsvc_download_StartDdbFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents);
STATIC HERROR 		xsvc_download_StopDsiDiiFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents);
STATIC HERROR 		xsvc_download_StopDdbFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents);
STATIC HERROR 		xsvc_download_StopFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents);			/* Index가 필요 없다. 현재 Filtering되어 있는 모든 DII/DSI, DDB Stop */
STATIC void 		xsvc_download_CallbackDsiDii_Dvbssu (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC void 		xsvc_download_CallbackDdb_Dvbssu (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
STATIC HERROR 		xsvc_download_ParseCompability_Dvbssu (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound, HUINT32 *pulSystemVer);
STATIC void			xsvc_download_ParseDsi_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId, HUINT32 ulTsUniqId);
STATIC void 		xsvc_download_ParseDii_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);
STATIC void 		xsvc_download_ParseDdb_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId);

STATIC HBOOL 		xsvc_download_CheckOTAInfo_Dvbssu(HUINT32 ulSignalVersion);
STATIC void 		xsvc_download_WriteOtaDbInfo_Dvbssu(XsvcDownload_OtaMetaHeader_t *pstMetaHeader);

STATIC HERROR 		xsvc_download_WriteDdb1Block_Dvbssu(Handle_t hUpgrade, HUINT8 *pucData, HUINT32 size, HUINT32 index, HUINT32 ulBlockSize, HBOOL *pbComplete);


STATIC HERROR 		xsvc_download_StartDownload_Dvbssu (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg);
STATIC char			*xsvc_download_GetProgressString_Dvbssu(HUINT32 ulBlockNo);

STATIC HERROR		xsvc_download_UpgradeInit_Dvbssu(void);
STATIC HERROR	 	xsvc_download_UpgradeGetUserData_Dvbssu(void **ppvUserData);
STATIC HERROR		xsvc_download_UpgradeSetUserData_Dvbssu(void  *pvUserData);
STATIC HBOOL		xsvc_download_UpgradeCheckComplete_Dvbssu(void *pvUserData);
STATIC Handle_t		xsvc_download_UpgradeCreateHandle_Dvbssu(HUINT32 ulTag, void *pvUserData);
STATIC HERROR		xsvc_download_UpgradeDestoryHandle_Dvbssu(void *pvUserData);
STATIC HINT32		xsvc_download_UpgradeRead_Dvbssu(void *pvUserData, HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize);
STATIC HINT32		xsvc_download_UpgradeWrite_Dvbssu(void *pvUserData, const HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize);

#define _____LOCAL_FUNCTIONS_____

STATIC XsvcDownload_OtaMetaInfo_t *xsvc_download_GetMetaInfo_Dvbssu(void)
{
	return &s_stSsuUpgradeInfo.stMetaInfo;
}

STATIC XsvcDownload_UpgradeInfo_t *xsvc_download_GetUpgradeInfo_Dvbssu(void)
{
	return &s_stSsuUpgradeInfo;
}

HERROR xsvc_download_InitMetaInfo_Dvbssu(void)
{
	XsvcDownload_OtaMetaInfo_t *pstMetaInfo = xsvc_download_GetMetaInfo_Dvbssu();

	if (pstMetaInfo)
	{
		HxSTD_memset(pstMetaInfo, 0x00, sizeof(XsvcDownload_OtaMetaInfo_t));
	}
	return ERR_OK;
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_AllocDownloadInfo_Dvbssu (void)
{
	XsvcDownload_DsmccDnInfo_t *pstDnInfo;

	pstDnInfo = (XsvcDownload_DsmccDnInfo_t *)OxSI_Malloc (sizeof(XsvcDownload_DsmccDnInfo_t));
	if (pstDnInfo != NULL)
	{
		HxSTD_memset (pstDnInfo, 0, sizeof(XsvcDownload_DsmccDnInfo_t));
	}

	return pstDnInfo;

}

STATIC void xsvc_download_FreeDownloadInfo_Dvbssu (XsvcDownload_DsmccDnInfo_t *pstInfo)
{
	XsvcDownload_OtaMetaInfo_t *pstMetaInfo = xsvc_download_GetMetaInfo_Dvbssu();

	if (pstInfo != NULL)
	{
		OxSI_Free(pstInfo);
	}

	if (pstMetaInfo)
	{
		if (pstMetaInfo->pucOTAMetaStatus != NULL)
		{
			OxSI_Free(pstMetaInfo->pucOTAMetaStatus);
			pstMetaInfo->pucOTAMetaStatus = NULL;
		}
	}
}

STATIC XsvcDownload_DsmccDnInfo_t *xsvc_download_GetDownloadInfo_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		return (XsvcDownload_DsmccDnInfo_t *)pstContents->pvDownloadInfo;
	}

	return NULL;
}

#define _____START_FUNCTIONS_____

STATIC HERROR xsvc_download_SetDownloadContents_Dvbssu (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HUINT32							ulDemuxId;
	HUINT32				 			ulActId;
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo;
	HERROR							hErr;

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	if (pstMsg == NULL)
	{
		return ERR_DOWN_SOURCE_NULL;
	}

	// Get the demux ID
	ulDemuxId = RxRSCID_NULL;
	ulActId = PAL_PIPE_GetActionId (pstMsg->hActHandle);
	hErr = PAL_PIPE_GetResourceId (ulActId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	pstContents->hAction		= pstMsg->hActHandle;
	pstContents->eActType		= (SvcDownload_ActionType_e)pstMsg->unArg.stStartDown.ulActType;
	pstContents->usTsUniqId		= pstMsg->unArg.stStartDown.usTsUniqId;
	pstContents->usPid			= pstMsg->unArg.stStartDown.usPid;
	pstContents->ulDemuxId		= ulDemuxId;

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo != NULL)
	{
		pstDnInfo->ulSignalVersion = pstMsg->unArg.stStartDown.ulExtId1;
		pstDnInfo->usPid = pstMsg->unArg.stStartDown.usPid;
		pstDnInfo->usDsiFilterId = FILTER_ID_NULL;
		pstDnInfo->usDdbFilterId = FILTER_ID_NULL;
	}

	return ERR_DOWN_RESULT_OK;

}

#define _____FILTERING_FUNCTIONS_____
STATIC HERROR xsvc_download_StartDsiDiiFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	HUINT16						usFilterId;
	SiRetrieveParam_t			stParam;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						hErr;

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
	stParam.hAction		 = pstContents->hAction;
	stParam.usTsUniqId	 = pstContents->usTsUniqId;
	stParam.ucDemuxId	 = (HUINT8)pstContents->ulDemuxId;

	stParam.usPid		 = pstContents->usPid;
	stParam.ucMode		 = eSiFilterMode_Continue;
	stParam.bTableFilter = FALSE;
	stParam.bUseExtId	 = FALSE;
	stParam.bCrc		 = TRUE;

	hErr = xsvc_download_DmxRetrieveDsiSection(&stParam, (void *)xsvc_download_CallbackDsiDii_Dvbssu, NULL, &usFilterId);
	if (hErr == ERR_OK)
	{
		pstDnInfo->usDsiFilterId = usFilterId;
	}
	else
	{
		pstDnInfo->usDsiFilterId = FILTER_ID_NULL;
	}

	return hErr;

}

STATIC HERROR xsvc_download_StopDsiDiiFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);

	if (pstDnInfo->usDsiFilterId != FILTER_ID_NULL)
	{
		xsvc_download_DmxStopOneSection (pstDnInfo->usDsiFilterId);
		pstDnInfo->usDsiFilterId = FILTER_ID_NULL;
	}

	return ERR_DOWN_RESULT_OK;

}

STATIC HERROR xsvc_download_StartDdbFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	HUINT16						usFilterId;
	SiRetrieveParam_t			stParam;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						hErr;

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	HxSTD_memset (&stParam, 0, sizeof(SiRetrieveParam_t));
	stParam.hAction		 = pstContents->hAction;
	stParam.usTsUniqId	 = pstContents->usTsUniqId;
	stParam.ucDemuxId	 = (HUINT8)pstContents->ulDemuxId;
	stParam.usPid		 = pstContents->usPid;
	stParam.ucMode		 = eSiFilterMode_Continue;
	stParam.bTableFilter = FALSE;
	stParam.bUseExtId	 = FALSE;
	stParam.bCrc		 = TRUE;
	stParam.ucTimeoutMode = eContinueTimeout_Repeat;

	hErr = xsvc_download_DmxRetrieveDdbSection (&stParam, (void *)xsvc_download_CallbackDdb_Dvbssu, NULL, &usFilterId);
	if (hErr == ERR_OK)
	{
		pstDnInfo->usDdbFilterId = usFilterId;
	}
	else
	{
		pstDnInfo->usDdbFilterId = FILTER_ID_NULL;
	}

	s_timeDdbTimeout = HLIB_STD_GetSystemTime();
	HxLOG_Debug ("-(%04x)(%04x)\n", pstDnInfo->usPid, pstDnInfo->usDdbFilterId);

	return hErr;

}

STATIC HERROR xsvc_download_StopDdbFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;

	if (pstContents == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);

	if (pstDnInfo->usDdbFilterId != FILTER_ID_NULL)
	{
		xsvc_download_DmxStopOneSection (pstDnInfo->usDdbFilterId);
		pstDnInfo->usDdbFilterId = FILTER_ID_NULL;
	}

	return ERR_DOWN_RESULT_OK;

}

STATIC HERROR xsvc_download_StopFiltering_Dvbssu (SvcDownload_DsmccContents_t *pstContents)
{
	xsvc_download_StopDsiDiiFiltering_Dvbssu (pstContents);
	xsvc_download_StopDdbFiltering_Dvbssu (pstContents);

	return ERR_DOWN_RESULT_OK;
}

#define _____SECTION_CALLBACK_____
STATIC void xsvc_download_CallbackDsiDii_Dvbssu (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16				usMessageId;
	HUINT32				ulResult, ulFilterId;
	HUINT8				*pucRaw;
	PalSef_RawData_t	*pstSiRawData;
	HUINT32				ulTsUniqId;

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;
	ulTsUniqId = ulfilterAndUniqId & 0x0000ffff;

	if (ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Critical("\n\n");
		svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);
		return;
	}

	pstSiRawData = (PalSef_RawData_t *)ulParam;
	if (pstSiRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pucRaw = pstSiRawData->buf[0];
	if (pucRaw == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	// Message ID : DsmccMessageHeader에서 2byte째, Section 에서 10byte째
	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DSI_MESSAGE_ID)
	{
		HxLOG_Print("RECV DSI - MESGID (0x%04x)\n", usMessageId);
		xsvc_download_ParseDsi_Dvbssu (pucRaw, hAction, ulFilterId, ulTsUniqId);
	}
	else if (usMessageId == DII_MESSAGE_ID)
	{
		HxLOG_Print("RECV DII - MESGID (0x%04x)\n", usMessageId);
		xsvc_download_ParseDii_Dvbssu (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Print("RECV SOMETHING - MESGID (0x%04x)\n", usMessageId);
		HxLOG_Critical("\n\n");
	}

	return;
}

STATIC void xsvc_download_CallbackDdb_Dvbssu (HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16			 		 usMessageId;
	HUINT32			 		 ulResult, ulFilterId;
	HUINT8					*pucRaw;
	PalSef_RawData_t		*pstSiRawData;
	HBOOL					 bDownloadFail = TRUE;

	ulResult = ulTableIdAndResult & 0x0000ffff;
	ulFilterId = (ulfilterAndUniqId & 0xffff0000) >> 16;

	if(ulResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Critical("\n\n");

		if (ulResult == (HUINT32)eSiFilterResult_Timeout)
		{
			if(HLIB_STD_GetSystemTime() < (s_timeDdbTimeout+DDB_DOWNLOAD_TIMEOUT))
			{
				bDownloadFail = FALSE;
			}
		}

		if(bDownloadFail == TRUE)
		{
			svc_download_SendSectionMessage (DOWNCB_DDB_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);
		}

		return;
	}

	s_timeDdbTimeout = HLIB_STD_GetSystemTime();

	pstSiRawData = (PalSef_RawData_t *)ulParam;
	if (pstSiRawData == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	pucRaw = pstSiRawData->buf[0];
	if (pucRaw == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

#ifdef DDB_DUMP
{
	HUINT32		i;

	VK_Print("\nDDB cb============================In==\n");
	for( i=0; i < (HINT32)get12bit (pucRaw + 1) + 3;i++)
	{
	if( i % 16 == 0 )
			VK_Print("\n" );
		VK_Print("%02X ", (unsigned char )pucRaw[i] );
	}
	VK_Print("\nDDB cb============================Out==\n");
}
#endif

	// Message ID : DsmccMessageHeader에서 2byte째, Section 에서 10byte째
	usMessageId = get16bit (pucRaw + SECTION_MESSAGEID_POS);

	if (usMessageId == DDB_MESSAGE_ID)
	{
		xsvc_download_ParseDdb_Dvbssu (pucRaw, hAction, ulFilterId);
	}
	else
	{
		HxLOG_Critical("\n\n");
	}

	return;
}

#define _____SECTION_PARSE_____

STATIC HERROR xsvc_download_ParseDdbHeader_Dvbssu (HUINT8 *pucRaw, HINT32 nSecLen, HUINT32 *pulDownloadId, HUINT32 *pulModuleId,
													HUINT32 *pulModuleVer, HUINT32 *pulBlockNo, HUINT32 *pulParsedLen)
{
	HUINT32		 ucAdaptLen;
	HUINT32		 ulParsedLen;

	ulParsedLen = 0;

	// DSM-CC Header
	*pulDownloadId = get32bit (pucRaw + 4);
	ucAdaptLen = pucRaw[9];
	pucRaw += (DDB_HEADER_LEN + ucAdaptLen);
	nSecLen -= (DDB_HEADER_LEN + ucAdaptLen);
	ulParsedLen += (DDB_HEADER_LEN + ucAdaptLen);

	if (nSecLen < 4)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	// Module ID : 2byte
	*pulModuleId = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;
	ulParsedLen += 2;

	// Module Version : 1byte
	*pulModuleVer = (HUINT32)*pucRaw;
	pucRaw ++;
	nSecLen --;
	ulParsedLen ++;

	// Reserve : 1byte
	pucRaw ++;
	nSecLen --;
	ulParsedLen ++;

	// Block Number :
	*pulBlockNo = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;
	ulParsedLen += 2;

	*pulParsedLen = ulParsedLen;
	return ERR_OK;
}

//13818-6 Table 7-3 DSM-CC Download Data Header Format
STATIC HERROR xsvc_download_ParseDsmccMessageHeader_Dvbssu (HUINT8 *pucRaw, HINT32 nSecLen, HUINT32 *pulTransactionId, HUINT32 *pulHeaderLen)
{
	HUINT8		 ucAdaptLen;
	HUINT32		 ulTransactionId;
	HUINT32		 ulHeaderLen;

	ulHeaderLen = 0;

	// protocolDiscriminator (1byte)
	pucRaw ++;
	nSecLen --;
	ulHeaderLen ++;

	// dsmccType (1byte)
	pucRaw ++;
	nSecLen --;
	ulHeaderLen ++;

	// message_id (2byte)
	pucRaw += 2;
	nSecLen -= 2;
	ulHeaderLen += 2;

	if (nSecLen < 6)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	// transaction_id (4byte)
	ulTransactionId = get32bit (pucRaw);
	pucRaw += 4;
	nSecLen -= 4;
	ulHeaderLen += 4;

	// reserved (1byte)
	pucRaw ++;
	nSecLen --;
	ulHeaderLen ++;

	// adaptation length (1byte)
	ucAdaptLen= *pucRaw;
	pucRaw ++;
	nSecLen --;
	ulHeaderLen ++;

	// message length (2byte)
	pucRaw += 2;
	nSecLen -= 2;
	ulHeaderLen += 2;

	// Adaptation field
	pucRaw += ucAdaptLen;
	ulHeaderLen += ucAdaptLen;
	nSecLen -= ucAdaptLen;

	if (nSecLen < 0)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	*pulTransactionId = ulTransactionId;
	*pulHeaderLen = ulHeaderLen;

	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR xsvc_download_ParseGroupInfo_Dvbssu (HUINT8 *pucRaw, SvcDownload_DnGrp_t *pstGroupItem, HINT32 nSecLen, HUINT32 *pulParsedLen, HBOOL *pbHwFound, HBOOL *pbSwFound)
{
	HINT32			 nCmptDesLen, nCmptDesCnt;
	HUINT32 		 ulCmptDesIdx;
	HUINT32			 ulParsedLen;
	HUINT32 		 ulSystemVer = 0;
	HINT32			 TmpLen = 0;

	ulParsedLen = 0;

	if (nSecLen < 10)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	// GroupId (4byte)
	pstGroupItem->ulGroupId = get32bit (pucRaw);
	pucRaw += 4;
	nSecLen -= 4;
	ulParsedLen += 4;

	HxLOG_Debug("[xsvc_download_ParseGroupInfo_Dvbssu] ulGroupId (0x%x) \n", pstGroupItem->ulGroupId);

	// GroupSize (4byte)
	pstGroupItem->ulGroupSize = get32bit (pucRaw);
	pucRaw += 4;
	nSecLen -= 4;
	ulParsedLen += 4;

	HxLOG_Debug("[xsvc_download_ParseGroupInfo_Dvbssu] ulGroupSize (%d) \n", pstGroupItem->ulGroupSize);

	// CompatibilityDescriptorLength (2byte)
	nCmptDesLen = (HINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;
	ulParsedLen += 2;

	HxLOG_Debug("[xsvc_download_ParseGroupInfo_Dvbssu] nCmptDesLen (%d) \n", nCmptDesLen);

	if (nSecLen < nCmptDesLen)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	// DescriptorCount (2byte)
	nCmptDesCnt = (HINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;
	ulParsedLen += 2;

	HxLOG_Debug("[xsvc_download_ParseGroupInfo_Dvbssu] nCmptDesCnt (%d) \n", nCmptDesCnt);

	for(ulCmptDesIdx=0 ; (HINT32)ulCmptDesIdx<nCmptDesCnt;ulCmptDesIdx++)
	{
		TmpLen = pucRaw[1];

		xsvc_download_ParseCompability_Dvbssu(pucRaw, NULL, pbHwFound, pbSwFound, &ulSystemVer);

		pucRaw += (TmpLen +2);
		nSecLen -= (TmpLen +2);
		ulParsedLen += (TmpLen +2);
	}

	if (nSecLen < 0)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	HxLOG_Debug("[xsvc_download_ParseGroupInfo_Dvbssu] ulParsedLen (%d) \n", ulParsedLen);

	*pulParsedLen = ulParsedLen;
	return ERR_DOWN_RESULT_OK;
}

STATIC HERROR xsvc_download_ParseModuleInfo_Dvbssu (HUINT8 *pucRaw, SvcDownload_DnMod_t *pstModuleItem, HINT32 nSecLen, HUINT32 *pulParsedLen)
{
	HUINT8			 ucModuleLen;
	HUINT32			 ulParsedLen;

	ulParsedLen = 0;

	if (nSecLen < 8)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	// Module ID (2byte)
	pstModuleItem->ulModuleId = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;
	ulParsedLen += 2;

	// Module Size (4byte)
	pstModuleItem->ulModuleSize = get32bit (pucRaw);
	pucRaw += 4;
	nSecLen -= 4;
	ulParsedLen += 4;

	// Module Version (1byte)
	pstModuleItem->ulModuleVer = (HUINT32)pucRaw[0];
	pucRaw ++;
	nSecLen --;
	ulParsedLen ++;

	// Module Info length (1byte)
	ucModuleLen = *pucRaw;
	pucRaw ++;
	nSecLen --;
	ulParsedLen ++;

	// Skip the module info : it is not used in this app:
	ulParsedLen += (HUINT32)ucModuleLen;

	*pulParsedLen = ulParsedLen;
	return ERR_DOWN_RESULT_OK;
}

//13818-6 Table7-12 DownloadserverInitiate message
STATIC HERROR xsvc_download_ParseCompability_Dvbssu (HUINT8 *pucDesRaw, HUINT8 **ppucNextDesRaw, HBOOL *pbHwFound, HBOOL *pbSwFound, HUINT32 *pulSystemVer)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSignalSystemId, ulSignalSystemVer;
	HUINT32		 ulOui, ulSystemId, ulSystemVer;
	HUINT8		*pucNextDesRaw;
	HERROR		 hErr;

	hErr  = PAL_SYS_GetSystemId (&ulSystemId);
	hErr |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSystemVer);
	if (hErr != ERR_OK)
	{
		return ERR_DOWN_RESULT_FAIL;
	}

	ucDesTag = pucDesRaw[0];
	ucDesLen = pucDesRaw[1];
	pucDesRaw += 2;
	pucNextDesRaw = pucDesRaw + ucDesLen;

	HxLOG_Print("[xsvc_download_ParseCompability_Dvbssu] ucDesTag(0x%x) ucDesLen(0x%x) \n", ucDesTag, ucDesLen);

	switch (ucDesTag)
	{
		case 0x01:				// System Hardware Descriptor
			ulOui = get32bit (pucDesRaw) & 0xffffff;					pucDesRaw += 4;
			ulSignalSystemId = get32bit (pucDesRaw);					pucDesRaw += 4;

			HxLOG_Print("[xsvc_download_ParseCompability_Dvbssu] Hardware Descriptor :: ulOui(0x%x) ulSignalSystemId(0x%x) \n",ulOui, ulSignalSystemId);

			if ( ((ulOui == eSWUP_OUI_HUMAX ) || (ulOui == eSWUP_OUI_DVB)) && (ulSignalSystemId == ulSystemId) )
			{
				if (ppucNextDesRaw != NULL)
				{
					*ppucNextDesRaw = pucNextDesRaw;
				}

				*pbHwFound = TRUE;
				return ERR_OK;
			}
			break;

		case 0x02:				// System Software Descriptor
			ulOui = get32bit (pucDesRaw) & 0xffffff;					pucDesRaw += 4;
			ulSignalSystemVer = get32bit (pucDesRaw);					pucDesRaw += 4;

			HxLOG_Print("[xsvc_download_ParseCompability_Dvbssu] System Software Descriptor :: ulOui(0x%x) ulSignalSystemVer(0x%x) \n",ulOui, ulSignalSystemVer);

			if (((ulOui == eSWUP_OUI_HUMAX ) || (ulOui == eSWUP_OUI_DVB)) && (ulSignalSystemVer > ulSystemVer))
			{
				if (ppucNextDesRaw != NULL)
				{
					*ppucNextDesRaw = pucNextDesRaw;
				}
				*pbSwFound = TRUE;
				*pulSystemVer = ulSignalSystemVer;
				return ERR_OK;
			}
			break;
	}

	if (ppucNextDesRaw != NULL)
	{
		*ppucNextDesRaw = pucNextDesRaw;
	}

	return ERR_OK;
}

STATIC void xsvc_download_ParseDsi_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId, HUINT32 ulTsUniqId)
{
	HINT32				 		nSecLen, nPrivateDataLen;
	HUINT32				 		ulTransactionId;
	HUINT32				 		ulParsedLen;
	HUINT32				 		ulGroupCnt, ulGroupNum;
	SvcDownload_DnGrp_t			*pstGroupItem	= NULL;
	SvcDownload_DsmccContents_t	*pstContents	= NULL;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo		= NULL;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t					*pstGroupList	= NULL;
#else
	POINTER_LIST_T				*pstGroupList	= NULL;
#endif
	HERROR					 	hErr;
	HBOOL 					 	bHwFound = FALSE, bSwFound = FALSE;

	pstGroupList = NULL;
	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	if (pstDnInfo->bDsiParsed == TRUE)
	{
		goto Error;
	}

	if (pstDnInfo->usDsiFilterId != (HUINT16)ulFilterId)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1);

	// Skip the Section Header
	pucRaw += SECTION_HEADER_LEN;
	nSecLen -= (SECTION_HEADER_LEN - 3);

	// DSM-CC Message Header
	hErr = xsvc_download_ParseDsmccMessageHeader_Dvbssu (pucRaw, nSecLen, &ulTransactionId, &ulParsedLen);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}
	HxLOG_Print("[xsvc_download_ParseDsi_Dvbssu]-------ulTransactionId = 0x%x, nSecLen = 0x%x, ulParsedLen = 0x%x, PID = 0x%x \n ", ulTransactionId, nSecLen, ulParsedLen, pstDnInfo->usPid);

	pucRaw += ulParsedLen;
	nSecLen -= ulParsedLen;

	// Server ID : we skip in here
	pucRaw += 20;
	nSecLen -= 20;

	// skip compatibilityDescriptor --> 0x0000;
	pucRaw++;
	ulParsedLen = (HUINT32)*pucRaw;
	pucRaw++;
	pucRaw += ulParsedLen;
	nSecLen -= (ulParsedLen + 2);

	if (nSecLen < 4)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// private_data_length (2byte)
	nPrivateDataLen = (HINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;

	if (nPrivateDataLen > nSecLen)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// GroupInfoIndacation
	ulGroupNum = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nPrivateDataLen -= 2;

	HxLOG_Print("[xsvc_download_ParseDsi_Dvbssu] >>>>> ulGroupNum(0x%x) \n", ulGroupNum);

	for (ulGroupCnt = 0; ulGroupCnt < ulGroupNum && nPrivateDataLen > 0; ulGroupCnt++)
	{
		bHwFound = FALSE;
		bSwFound = FALSE;

		hErr = SVC_DOWNLOAD_AllocGroupInfo (&pstGroupItem);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			goto Error;
		}

		hErr = xsvc_download_ParseGroupInfo_Dvbssu (pucRaw, pstGroupItem, nPrivateDataLen, &ulParsedLen, &bHwFound, &bSwFound);
		HxLOG_Print("[xsvc_download_ParseDsi_Dvbssu] >>>>> hErr(0x%x) ulGroupCnt (0x%x) bHwFound(%d) bSwFound(%d) \n", hErr, ulGroupCnt, bHwFound, bSwFound);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			SVC_DOWNLOAD_FreeGroupInfo(pstGroupItem);
			goto Error;
		}

		if(bHwFound == TRUE)
		{
			HxLOG_Print("[xsvc_download_ParseDsi_Dvbssu] >>>>> Found OTA ::::: GroupId(0x%x)  \n", pstGroupItem->ulGroupId);

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
			pstGroupList = HLIB_LIST_Append(pstGroupList, (void *)pstGroupItem);
#else
			pstGroupList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstGroupList, (void *)pstGroupItem);
#endif
			pstDnInfo->pstGroupInfo = pstGroupItem;
		}
		else
		{
			SVC_DOWNLOAD_FreeGroupInfo(pstGroupItem);
		}

		pucRaw += ulParsedLen;
		nPrivateDataLen -= ulParsedLen;

		// GroupInfoLength
		if (nPrivateDataLen < 2)
		{
			HxLOG_Critical("\n\n");
			goto Error;
		}

		ulParsedLen = (HUINT32)get16bit (pucRaw);
		pucRaw += 2;
		nPrivateDataLen -= 2;

		pucRaw += ulParsedLen;
		nPrivateDataLen -= ulParsedLen;

		// private data length
		if (nPrivateDataLen < 2)
		{
			HxLOG_Critical("\n\n");
			goto Error;
		}

		ulParsedLen = (HUINT32)get16bit (pucRaw);
		pucRaw += 2;
		nPrivateDataLen -= 2;

		pucRaw += ulParsedLen;
		nPrivateDataLen -= ulParsedLen;
	}

	// Finish :
	pstContents->pstGroupList = pstGroupList;
	pstGroupList = NULL;
	pstDnInfo->bDsiParsed = TRUE;

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	if (pstGroupList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstGroupList = HLIB_LIST_RemoveAllFunc (pstGroupList, SVC_DOWNLOAD_FreeGroupInfo);
#else
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstGroupList, (FREE_FUNC)SVC_DOWNLOAD_FreeGroupInfo);
#endif
	}

	return;
}


//13818-6 Talbe7-6 DownloadInfo Response and DownloadInfoIndication message.
STATIC void xsvc_download_ParseDii_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HINT32						nSecLen;
	HUINT32					 	ulGroupId;
	HUINT32						ulParsedLen;
	HUINT32						ulModuleCnt, ulModuleNum;
	HUINT32						ulBlockSize;
	SvcDownload_DnGrp_t			*pstGroupItem;
	SvcDownload_DnMod_t			*pstModuleItem;
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
	HxList_t					*pstModuleList;
#else
	POINTER_LIST_T				*pstModuleList;
#endif
	HERROR						 hErr;

	pstModuleList = NULL;
	pstContents = NULL;
	pstModuleItem = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	if (pstDnInfo->bDsiParsed == FALSE)
	{
		goto Error;
	}

	if (pstDnInfo->bDiiParsed == TRUE)
	{
		goto Error;
	}

	if (pstDnInfo->usDsiFilterId != (HUINT16)ulFilterId)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1);

	// Skip the Section Header
	pucRaw += SECTION_HEADER_LEN;
	nSecLen -= (SECTION_HEADER_LEN - 3);

	// DSM-CC Message Header
	hErr = xsvc_download_ParseDsmccMessageHeader_Dvbssu (pucRaw, nSecLen, &ulGroupId, &ulParsedLen);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	hErr = SVC_DOWNLOAD_GetGroupByGroupId (pstContents, ulGroupId, &pstGroupItem);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pucRaw += ulParsedLen;
	nSecLen -= ulParsedLen;

	if (nSecLen < 6)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// Download ID : 4byte
	pucRaw += 4;
	nSecLen -= 4;

	// Block Size : 2byte
	ulBlockSize = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;

	// WindowSize(1Byte), AckPeriod(1Byte), tCDownloadWindow(4Bytes), tCDownloadScenario(4Bytes)
	// is not used in OTA
	pucRaw += 10;
	nSecLen -= 10;

	if (nSecLen < 2)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// Compatibility Descriptor
	ulParsedLen = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;

	pucRaw += ulParsedLen;
	nSecLen -= ulParsedLen;

	if (nSecLen < 2)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// Number of Modules
	ulModuleNum = (HUINT32)get16bit (pucRaw);
	pucRaw += 2;
	nSecLen -= 2;

	for (ulModuleCnt = 0; ulModuleCnt < ulModuleNum && nSecLen > 0; ulModuleCnt++)
	{
		hErr = SVC_DOWNLOAD_AllocModuleInfo (&pstModuleItem);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			goto Error;
		}

		pstModuleItem->ulGroupId = pstGroupItem->ulGroupId;
		pstModuleItem->ulBlockSize = ulBlockSize;

#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstModuleList = HLIB_LIST_Append(pstModuleList, (void *)pstModuleItem);
#else
		pstModuleList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstModuleList, (void *)pstModuleItem);
#endif

		hErr = xsvc_download_ParseModuleInfo_Dvbssu (pucRaw, pstModuleItem, nSecLen, &ulParsedLen);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			goto Error;
		}

		pucRaw += ulParsedLen;
		nSecLen -= ulParsedLen;
	}

	// Finish :
	pstGroupItem->pstModuleList = pstModuleList;
	pstGroupItem->bGroupInfoReceived = TRUE;

	pstModuleList = NULL;

	if (SVC_DOWNLOAD_CheckAllGroupInfoReceived (pstContents) == ERR_OK)
	{
		// DII도 다 받았다.
		pstDnInfo->bDiiParsed = TRUE;
		pstDnInfo->pstModuleInfo = pstModuleItem;

		svc_download_SendSectionMessage (DOWNCB_DSI_TABLE, hAction, eSI_TABLE_RECEIVED, (HUINT32)pstDnInfo->usDsiFilterId);

		xsvc_download_StopDsiDiiFiltering_Dvbssu (pstContents);
	}

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	if (pstModuleList != NULL)
	{
#if defined(HXLIST_CHANGE_TO_LINKEDLST)
		pstModuleList = HLIB_LIST_RemoveAllFunc(pstModuleList, SVC_DOWNLOAD_FreeModuleInfo);
#else
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstModuleList, (FREE_FUNC)SVC_DOWNLOAD_FreeModuleInfo);
#endif
	}

	return;
}

#define ERASE_BLOCKSIZE 	(128*1024)
#define ERASE_NUMBLOCK		10			/* Loader will perforem CRC check; thus, delete 10 blocks from the beginning. */
/*	Do not neet to delelte RAM FILE */
STATIC HERROR xsvc_download_UpgradeInitStorage_Dvbssu(Handle_t hUpgrade, XsvcDownload_OtaMetaInfo_t *pstMetaInfo, HUINT32 ulBlockSize)
{
	HERROR	hErr = ERR_FAIL;
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo;

	if(pstMetaInfo == NULL)
	{
		HxLOG_Critical("pstMetaInfo == null\n");
		hErr = ERR_FAIL;
		goto Error;
	}

	if (pstMetaInfo->bStarted == TRUE)
	{
		hErr = ERR_FAIL;
		goto Error;
	}

	pstUpgradeInfo = NULL;

	hErr = OTL_UPGRADE_GetUserData(hUpgrade, (void **)&pstUpgradeInfo);
	if((hErr == ERR_OK)  && (pstUpgradeInfo != NULL))
	{
		pstUpgradeInfo->ulDataSize	= pstMetaInfo->stOTAMetaHeader.ulFileSize;
		pstUpgradeInfo->ulPeiceSize = ERASE_BLOCKSIZE;

		(void)OTL_UPGRADE_SetUserData(hUpgrade, (void *)pstUpgradeInfo);
	}
	else
	{
		HxLOG_Critical("Error : Check Null from OTL_UPGRADE_GetUserData().\n");
	}

Error:
	return hErr;
}

STATIC HERROR	xsvc_download_SetupMeta_Dvbssu(XsvcDownload_OtaMetaInfo_t *pstMetaInfo, SvcDownload_DnMod_t *pstModuleInfo)
{
	if (NULL == pstMetaInfo)
	{
		return ERR_FAIL;
	}

	if (NULL == pstModuleInfo)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("Different Block No (%d/%d)\n", pstMetaInfo->stOTAMetaHeader.ulFileNumTotal, pstModuleInfo->ulBlockNum);

	pstMetaInfo->stOTAMetaHeader.ulDownloadCompleted = FALSE;
	pstMetaInfo->stOTAMetaHeader.ulFileNumTotal = pstModuleInfo->ulBlockNum;
	pstMetaInfo->stOTAMetaHeader.ulFileSize = pstModuleInfo->ulBlockSize;
	pstMetaInfo->stOTAMetaHeader.ulReceivecTotal = pstModuleInfo->ulReceivedBlockNum;

	// receiving status
	if (pstMetaInfo->pucOTAMetaStatus != NULL)
	{
		OxSI_Free(pstMetaInfo->pucOTAMetaStatus);
	}

	pstMetaInfo->pucOTAMetaStatus = (HUINT8 *)OxSI_Malloc (sizeof(HUINT8) * pstMetaInfo->stOTAMetaHeader.ulFileNumTotal);
	if (pstMetaInfo->pucOTAMetaStatus == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstMetaInfo->pucOTAMetaStatus, 0x00, pstMetaInfo->stOTAMetaHeader.ulFileNumTotal);

	// block 개수가 변했으므로 처음부터 다시 받아야 한다.
	pstMetaInfo->bStarted = FALSE;

	return ERR_OK;
}

STATIC void xsvc_download_ParseDdb_Dvbssu (HUINT8 *pucRaw, Handle_t hAction, HUINT32 ulFilterId)
{
	HUINT32					 ulDownloadId, ulModuleId, ulModuleVer, ulBlockNo;
	HUINT32					 ulParsedLen, ulCopySize;
	HINT32					 nSecLen;
	SvcDownload_DsmccContents_t	*pstContents;
	SvcDownload_DnMod_t			*pstModuleInfo;
	XsvcDownload_DsmccDnInfo_t			*pstDnInfo;
	HERROR					 hErr;
	HUINT32					 tick;
	HBOOL					 bComplete;
	XsvcDownload_OtaMetaInfo_t			*pstMetaInfo = NULL;

	pstContents = NULL;

	ENTER_CRITICAL_DNLDCNTS;

	pstMetaInfo = xsvc_download_GetMetaInfo_Dvbssu();

	hErr = svc_download_GetContextData (hAction, (void **)(&pstContents));
	if (hErr != ERR_OK || pstContents == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pstModuleInfo = pstDnInfo->pstModuleInfo;
	if (pstModuleInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	if (pstDnInfo->bDdbAllReceived == TRUE)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	nSecLen = (HINT32)get12bit (pucRaw + 1);

	// Skip the Section Header
	pucRaw += SECTION_HEADER_LEN;
	nSecLen -= (SECTION_HEADER_LEN + 1);	// -3 (Offset 0~2 이후) + 4 (CRC32) = +1

	if (nSecLen < DDB_HEADER_LEN)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	hErr = xsvc_download_ParseDdbHeader_Dvbssu (pucRaw, nSecLen, &ulDownloadId, &ulModuleId, &ulModuleVer, &ulBlockNo, &ulParsedLen);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	pucRaw += ulParsedLen;
	nSecLen -= ulParsedLen;

	if (ulModuleId != pstModuleInfo->ulModuleId || ulBlockNo >= pstModuleInfo->ulBlockNum)
	{
		HxLOG_Critical("\n\n");
		goto Error;
	}

	// 이미 받은 경우에는 넘어간다.
	if (pstMetaInfo->pucOTAMetaStatus[ulBlockNo] != 0)
	{
		HxLOG_Debug(">>>>> Already Downloaded Block  : %d\n", ulBlockNo);
		HxLOG_Debug("[%s] %d; pass\n", xsvc_download_GetProgressString_Dvbssu(ulBlockNo), ulBlockNo);

		goto Error;
	}

	pstModuleInfo->ulReceivedBlockNum ++;
	if(0 == ulBlockNo)
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("ulReceivedBlockNum = %d (Blk:%d) [%02X %02X %02X %02X %02X %02X %02X %02X]\n"), pstModuleInfo->ulReceivedBlockNum, ulBlockNo
			, pucRaw[0], pucRaw[1], pucRaw[2], pucRaw[3]
			, pucRaw[4], pucRaw[5], pucRaw[6], pucRaw[7]
			);
	}

	// DDB Header를 뺀 데이터 그 자체들만을 받는다.
	ulCopySize = (pstModuleInfo->ulBlockSize > (HUINT32)nSecLen) ? (HUINT32)nSecLen : pstModuleInfo->ulBlockSize;
	pstContents->ulRcvBlocks = pstModuleInfo->ulReceivedBlockNum;

	// DDB block을 file로 저장.
	tick = VK_TIMER_GetSystemTick();

	// DDB block을 한번도 받은 적이 없으면 download 파일을 지우고,
	if (pstMetaInfo->bStarted != TRUE)
	{
		xsvc_download_UpgradeInitStorage_Dvbssu(pstContents->hUpgrade, pstMetaInfo, ulCopySize);
	}

	bComplete = FALSE;
	hErr = xsvc_download_WriteDdb1Block_Dvbssu(pstContents->hUpgrade, pucRaw, ulCopySize, ulBlockNo, pstModuleInfo->ulBlockSize, &bComplete);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[%s] %d; write fail\n", xsvc_download_GetProgressString_Dvbssu(ulBlockNo), ulBlockNo);

		svc_download_SendSectionMessage (DOWNCB_DDB_TABLE, hAction, eSI_TABLE_FAIL, ulFilterId);

		HxLOG_Critical("\n\n");
		goto Error;
	}

	pstMetaInfo->stOTAMetaHeader.ulReceivecTotal = pstModuleInfo->ulReceivedBlockNum;
	pstMetaInfo->pucOTAMetaStatus[ulBlockNo] = 1;
	pstMetaInfo->bStarted = TRUE;

	HxLOG_Debug("[%s] %d; write\n", xsvc_download_GetProgressString_Dvbssu(ulBlockNo), ulBlockNo);

	if (bComplete)
	{
		pstDnInfo->bDdbAllReceived = TRUE;

		// update ota meta info.
		pstMetaInfo->stOTAMetaHeader.ulDownloadCompleted = TRUE;

		xsvc_download_WriteOtaDbInfo_Dvbssu(&pstMetaInfo->stOTAMetaHeader);

		svc_download_SendSectionMessage (DOWNCB_DDB_TABLE, hAction, eSI_TABLE_RECEIVED, (HUINT32)pstDnInfo->usDdbFilterId);
	}

	if ((pstContents != NULL) && pstDnInfo && pstModuleInfo && pstModuleInfo->ulBlockNum)
	{
		pstMetaInfo->stOTAMetaHeader.ulProgress = (HFLOAT64)((HFLOAT64)pstModuleInfo->ulReceivedBlockNum / (HFLOAT64)pstModuleInfo->ulBlockNum) * (HFLOAT64)100;

		if (pstMetaInfo->stOTAMetaHeader.ulProgressBackup != pstMetaInfo->stOTAMetaHeader.ulProgress)
		{
			svc_download_UtilPostMsgToMgr(eSEVT_SWUP_DOWNLOAD_PROGRESS, hAction, pstContents->eActType, (HUINT32)pstDnInfo->usPid, pstMetaInfo->stOTAMetaHeader.ulProgress);
			pstMetaInfo->stOTAMetaHeader.ulProgressBackup = pstMetaInfo->stOTAMetaHeader.ulProgress;
		}
	}

	NOT_USED_PARAM(tick);

Error:
	LEAVE_CRITICAL_DNLDCNTS;

	return;
}




#define _____PROCESS_FUNCTIONS______

#if defined(CONFIG_DEBUG)
STATIC void xsvc_download_printdebug_Dvbssu(XsvcDownload_OtaMetaInfo_t *pstMetaInfo)
{
	HxLOG_Print("# OTA Completed : %s\n", (pstMetaInfo->stOTAMetaHeader.ulDownloadCompleted == 1 ? "TRUE" : "FALSE"));
	HxLOG_Print("# OTA split file total: %d\n", pstMetaInfo->stOTAMetaHeader.ulFileNumTotal);
	HxLOG_Print("# OTA split file size: %d\n", pstMetaInfo->stOTAMetaHeader.ulFileSize);
	HxLOG_Print("# Read total: %d\n", pstMetaInfo->stOTAMetaHeader.ulReceivecTotal);
}
#endif

char get_amount_char(int blocks, int max_blocks)
{
	char *amount_chars = ".,-=*";

	if (blocks == 0 || max_blocks == 0)
	{
		return ' ';
	}

	if (blocks >= max_blocks || max_blocks == 1)
	{
		return '#';
	}

	blocks--;
	max_blocks--;

	return amount_chars[blocks * strlen(amount_chars) / max_blocks];
}

#define MAX_PROGRESS_LEN		60	/* EXCLUDING color code & null terminator */
char *xsvc_download_GetProgressString_Dvbssu(HUINT32 ulBlockNo)
{
	XsvcDownload_OtaMetaInfo_t			*pstMetaInfo;
	int i, j, num_progress, blocks_counted, blocks_left, max_blocks, blocks_in_this_progress;
	STATIC char buf[MAX_PROGRESS_LEN + 20];	/* INCLUDING color code & null terminator */
	char *p = buf;

	pstMetaInfo = xsvc_download_GetMetaInfo_Dvbssu();

	num_progress = MAX_PROGRESS_LEN;
	if (pstMetaInfo->stOTAMetaHeader.ulFileNumTotal < (HUINT32)num_progress)
	{
		num_progress = pstMetaInfo->stOTAMetaHeader.ulFileNumTotal;
	}

	blocks_counted = 0;
	blocks_left = pstMetaInfo->stOTAMetaHeader.ulFileNumTotal;

	for (i = 0; i < num_progress; i++)
	{
		max_blocks = blocks_left / (num_progress - i);
		blocks_in_this_progress = 0;

		for (j = blocks_counted; j < blocks_counted + max_blocks; j++)
		{
			if (pstMetaInfo->pucOTAMetaStatus[j] == 1)
			{
				blocks_in_this_progress++;
			}
		}

		if ((HUINT32)blocks_counted <= ulBlockNo && ulBlockNo < (HUINT32)(blocks_counted + max_blocks))
		{
			p += snprintf(p, buf + sizeof(buf) - p, "\033[37;44m");
		}

		*p++ = get_amount_char(blocks_in_this_progress, max_blocks);

		if ((HUINT32)blocks_counted <= ulBlockNo && ulBlockNo < (HUINT32)(blocks_counted + max_blocks))
		{
			p += snprintf(p, buf + sizeof(buf) - p, "\033[0m");
		}

		blocks_counted += max_blocks;
		blocks_left -= max_blocks;
	}

	*p = '\0';

	return buf;
}

STATIC HERROR xsvc_download_StartDownload_Dvbssu (SvcDownload_DsmccContents_t *pstContents, SvcDownload_Msg_t *pstMsg)
{
	HBOOL						bReady;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						hErr;
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo = xsvc_download_GetUpgradeInfo_Dvbssu();
	SvcDownload_DnMod_t			*pstModuleInfo;

	bReady = FALSE;

	ENTER_CRITICAL_DNLDCNTS;

	pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);
	if (pstDnInfo != NULL)
	{
		hErr = SVC_DOWNLOAD_PrepareModuleInfoToDownload (pstDnInfo->pstModuleInfo);
		if (hErr == ERR_OK)
		{
			bReady = TRUE;
		}
	}

	if ((NULL == pstDnInfo) || (NULL == pstDnInfo->pstModuleInfo))
	{
		hErr = ERR_FAIL;
		goto hErr;
	}

	pstModuleInfo = pstDnInfo->pstModuleInfo;

	// do not check with BlockNum to reconize different stream ::
	// We already confirmed with signal-version before starting download in xsvc_download_CheckDownloadComplete_Dvbssu
	// to reserve downloaded DDB , we need to check wether this stream is same version or not
	hErr = xsvc_download_SetupMeta_Dvbssu(&pstUpgradeInfo->stMetaInfo, pstModuleInfo);
	if (ERR_OK != hErr)
	{
		hErr = ERR_FAIL;
		goto hErr;
	}

	// Write User Data
	pstUpgradeInfo->stMetaInfo.stOTAMetaHeader.ulSignalVersion = pstDnInfo->ulSignalVersion;
	OTL_UPGRADE_SetUserData(pstContents->hUpgrade, pstUpgradeInfo);

#if defined(CONFIG_DEBUG)
	xsvc_download_printdebug_Dvbssu(&pstUpgradeInfo->stMetaInfo);
#endif

	// 일단 DSI는 끄고 본다.
	xsvc_download_StopDsiDiiFiltering_Dvbssu (pstContents);

	if (bReady == TRUE)
	{
		// 총 Block 수를 일단 Received/Total에 Setting 해 놓는다.
		pstContents->ulRcvBlocks = 0;
		pstContents->ulTotalBlocks = pstModuleInfo->ulBlockNum;

		HxLOG_Print("Download start with (%d) bytes x (%d) blocks.\n", pstModuleInfo->ulBlockSize, pstModuleInfo->ulBlockNum);

		pstModuleInfo->ulReceivedBlockNum = pstUpgradeInfo->stMetaInfo.stOTAMetaHeader.ulReceivecTotal;

		xsvc_download_StartDdbFiltering_Dvbssu (pstContents);
	}
	else
	{
		// DDB download로 진행할 수 없다.
		// Fail Message를 보낸다.
		svc_download_UtilPostMsgToMgr (eSEVT_SWUP_DOWNLOAD_FAIL, pstContents->hAction, eSWUP_ERROR_WRITE_FILE, 0, 0);	// --> new : SW Update 는 분리
	}

	hErr = ERR_OK;

hErr:

	LEAVE_CRITICAL_DNLDCNTS;
	return hErr;
}

#define _______OTA_BUFFER_____________

STATIC HERROR xsvc_download_WriteDdb1Block_Dvbssu(Handle_t hUpgrade, HUINT8 *pucData, HUINT32 size, HUINT32 index, HUINT32 ulBlockSize, HBOOL *pbComplete)
{
	HINT32			nRet;

	HxLOG_Print("Write start !! index (%d), offset (0x%x) size (%d).\n", index, index * ulBlockSize, size);

	nRet = OTL_UPGRADE_Write(hUpgrade, pucData, index * ulBlockSize, size, pbComplete);
	if (nRet < 0)
	{
		return ERR_FAIL;
	}

	// cache 사용으로 인해 write validation 처리를 read -> compare 하는 것은 의미가 없음

	return ERR_OK;
}

STATIC void xsvc_download_WriteOtaDbInfo_Dvbssu(XsvcDownload_OtaMetaHeader_t *pstMetaHeader)
{
	HERROR					hErr;
	XsvcDownload_OtaDbInfo_t 	stDownloadInfo;

	if (pstMetaHeader)
	{
		stDownloadInfo.bDownloadCompleted = TRUE;
		stDownloadInfo.ulDownloadBlockNumTotal = pstMetaHeader->ulFileNumTotal;
		stDownloadInfo.ulDownloadBlockSize = pstMetaHeader->ulFileSize;
		stDownloadInfo.ulDownloadedVersion = pstMetaHeader->ulSignalVersion;
	}
	else
	{
		stDownloadInfo.bDownloadCompleted = FALSE;
		stDownloadInfo.ulDownloadBlockNumTotal = 0;
		stDownloadInfo.ulDownloadBlockSize = 0;
		stDownloadInfo.ulDownloadedVersion = 0x00;
	}

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO, (HUINT32)&stDownloadInfo, (HUINT32)sizeof(XsvcDownload_OtaDbInfo_t));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
	}
	DB_PARAM_Sync();

	// Sync Nand File after download & write completed
	HxLOG_Print("\nFlush Nand flash after download, write done! \n");
}


STATIC HBOOL xsvc_download_CheckOTAInfo_Dvbssu(HUINT32 ulSignalVersion)
{
	HERROR					hErr, hErr2;
	HBOOL					bRet = TRUE;
	XsvcDownload_OtaDbInfo_t 	stDownloadedInfo;
	XsvcDownload_OtaMetaInfo_t 		*pstMetaInfo;
	HUINT32		ulSystemId, ulSystemVer;

	pstMetaInfo = xsvc_download_GetMetaInfo_Dvbssu();

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO, (HUINT32 *)&stDownloadedInfo, sizeof(XsvcDownload_OtaDbInfo_t));

	hErr2 = PAL_SYS_GetSystemId (&ulSystemId);
	hErr2 |= PAL_SYS_GetSystemVersion (eVERSION_TYPE_APPL, &ulSystemVer);
	HLIB_CMD_Printf("hErr: 0x%x,hErr2: 0x%x, DB completed: %d, ver: 0x%x, signver : 0x%x, ulSystemVer:0x%x\n", hErr,hErr2, stDownloadedInfo.bDownloadCompleted, stDownloadedInfo.ulDownloadedVersion,ulSignalVersion, ulSystemVer);

	if (hErr != ERR_OK || hErr2 != ERR_OK)
	{
		pstMetaInfo->stOTAMetaHeader.ulSignalVersion = ulSignalVersion;
		bRet = FALSE;
	}
	else if (stDownloadedInfo.bDownloadCompleted != TRUE)
	{
		// 1. 다운로드 완료되지 않았음.
		pstMetaInfo->stOTAMetaHeader.ulSignalVersion = ulSignalVersion;
		bRet = FALSE;
	}
	else if (stDownloadedInfo.bDownloadCompleted == TRUE && ulSystemVer < ulSignalVersion)
	{
		// 2. 다운로드 완료되었지만 signal version이 다운로드하여 저장한 버전보다 더 높다.
		pstMetaInfo->stOTAMetaHeader.ulSignalVersion = ulSignalVersion;
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}

	return bRet;
}

#define _______Upgrade_____________


STATIC HERROR	 	xsvc_download_upgrade_GetDownloadInfoFromDb_Dvbssu(XsvcDownload_OtaMetaHeader_t *pstMetaHeader)
{
	HERROR						 hErr;
	XsvcDownload_OtaDbInfo_t 		 stDownloadedInfo;

	if (NULL == pstMetaHeader)
	{
		return ERR_FAIL;
	}

	// Read Data From DB
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO, (HUINT32 *)&stDownloadedInfo, sizeof(XsvcDownload_OtaDbInfo_t));
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem failed. eDB_PARAM_ITEM_OTA_DOWNLOADEDINFO \n ");
	}

	pstMetaHeader->ulFileNumTotal	= stDownloadedInfo.ulDownloadBlockNumTotal;
	pstMetaHeader->ulFileSize		= stDownloadedInfo.ulDownloadBlockSize;
	pstMetaHeader->ulSignalVersion	= stDownloadedInfo.ulDownloadedVersion;

	return ERR_OK;
}

STATIC HERROR		xsvc_download_UpgradeInit_Dvbssu(void)
{
	HERROR	hErr;

	HxSTD_memset(&s_stSsuUpgradeInfo, 0, sizeof(XsvcDownload_UpgradeInfo_t));

	s_stSsuUpgradeInfo.hTargetStorage = HANDLE_NULL;

	hErr = xsvc_download_upgrade_GetDownloadInfoFromDb_Dvbssu(&s_stSsuUpgradeInfo.stMetaInfo.stOTAMetaHeader);

	return hErr;
}

STATIC HERROR	 	xsvc_download_UpgradeGetUserData_Dvbssu(void **ppvUserData)
{
	if (NULL == ppvUserData)
	{
		return ERR_FAIL;
	}

	xsvc_download_upgrade_GetDownloadInfoFromDb_Dvbssu(&s_stSsuUpgradeInfo.stMetaInfo.stOTAMetaHeader);

	*ppvUserData = &s_stSsuUpgradeInfo;

	return ERR_OK;
}

STATIC HERROR		xsvc_download_UpgradeSetUserData_Dvbssu(void  *pvUserData)
{
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo;

	if (NULL == pvUserData)
	{
		return ERR_FAIL;
	}

	pstUpgradeInfo = (XsvcDownload_UpgradeInfo_t *)pvUserData;
	xsvc_download_WriteOtaDbInfo_Dvbssu(&pstUpgradeInfo->stMetaInfo.stOTAMetaHeader);
	if (&s_stSsuUpgradeInfo == pstUpgradeInfo)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(&s_stSsuUpgradeInfo, pstUpgradeInfo, sizeof(XsvcDownload_UpgradeInfo_t));

	return ERR_OK;
}

STATIC HBOOL		xsvc_download_UpgradeCheckComplete_Dvbssu(void *pvUserData)
{
	HUINT32				 i;
	XsvcDownload_UpgradeInfo_t		*pstUpgradeInfo;
	XsvcDownload_OtaMetaInfo_t		*pstMetaInfo;

	if (NULL == pvUserData)
	{
		return FALSE;
	}

	pstUpgradeInfo= (XsvcDownload_UpgradeInfo_t*)pvUserData;
	pstMetaInfo = &pstUpgradeInfo->stMetaInfo;

	if (pstMetaInfo->stOTAMetaHeader.ulFileNumTotal == 0)
	{
		return FALSE;
	}

	if (pstMetaInfo->stOTAMetaHeader.ulFileNumTotal == (pstMetaInfo->stOTAMetaHeader.ulReceivecTotal + 1))
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("completed!!! (ulReceivecTotal + 1 = %d)\n"), pstMetaInfo->stOTAMetaHeader.ulReceivecTotal+1);
		return TRUE;
	}

	for (i=0; i<pstMetaInfo->stOTAMetaHeader.ulFileNumTotal; i++)
	{
		if (pstMetaInfo->pucOTAMetaStatus[i] == 0)
		{
			return FALSE;
		}
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("completed!!! (ulFileSize = %d)\n"), pstMetaInfo->stOTAMetaHeader.ulFileSize);
	return TRUE;
}

STATIC HERROR		xsvc_download_UpgradeCreateHandle_Dvbssu(HUINT32 ulTag, void *pvUserData)
{
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo;

	if (NULL == pvUserData)
	{
		return ERR_FAIL;
	}

	pstUpgradeInfo = (XsvcDownload_UpgradeInfo_t*)pvUserData;
	pstUpgradeInfo->hTargetStorage = (Handle_t)0;

	return ERR_OK;
}

STATIC HERROR		xsvc_download_UpgradeDestoryHandle_Dvbssu(void *pvUserData)
{
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo;

	if (NULL == pvUserData)
	{
		return HANDLE_NULL;
	}

	pstUpgradeInfo = (XsvcDownload_UpgradeInfo_t*)pvUserData;
	pstUpgradeInfo->hTargetStorage = (Handle_t)HANDLE_NULL;

	return ERR_OK;
}

STATIC HINT32		xsvc_download_UpgradeRead_Dvbssu(void *pvUserData, HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize)
{
	HERROR						nRet;
	HUINT32						ulPieceIdx;
	XsvcDownload_UpgradeInfo_t	*pstUpgradeInfo;

	if (NULL == pvUserData)
	{
		return -1;
	}

	if (NULL == pucBuff)
	{
		return -1;
	}

	if (0 == ulSize)
	{
		return -1;
	}

	pstUpgradeInfo = (XsvcDownload_UpgradeInfo_t *)pvUserData;
	if (0 == pstUpgradeInfo->ulPeiceSize)
	{
		return -1;
	}

	if (ulSize > pstUpgradeInfo->ulPeiceSize)
	{
		return -1;
	}

	ulPieceIdx = ulOffset / pstUpgradeInfo->ulPeiceSize;

	nRet = PAL_SYS_Upgrade_ReadData(ulPieceIdx, pucBuff, ulSize);

	return nRet;
}

STATIC HINT32		xsvc_download_UpgradeWrite_Dvbssu(void *pvUserData, const HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize)
{
	HERROR			hErr;
	HINT32			nRet = -1;
	HxFILE_t		hFile = NULL;
	HUINT8			szFileName[256] = {0, };
	HUINT32			ulWriteSize = 0;

	HxSTD_MemSet(szFileName, 0, sizeof(szFileName));
	HxSTD_StrNCpy((char *)szFileName, DDB_SAVE_FILE_PATH, sizeof(szFileName)-1);

	if(szFileName == NULL)
	{
		goto END_FUNC;
	}

	hFile = HLIB_FILE_Open((const HUINT8 *)szFileName, (const HUINT8 *)HxFILE_MODE_BINARY_RW_OR_NULL_RETURL);
	if(hFile == NULL)
	{
		hFile = HLIB_FILE_Open((const HUINT8 *)szFileName, (const HUINT8 *)HxFILE_MODE_BINARY_RW_WITH_EMPTY_FILE);
		if (hFile == NULL)
		{
			goto END_FUNC;
		}
	}

	if(pucBuff == NULL)
	{
		goto END_FUNC;
	}

	if (0 == ulSize)
	{
		goto END_FUNC;
	}

	hErr = HLIB_FILE_Seek(hFile, ulOffset, eHxFILE_SEEK_SET, NULL);
	if (hErr != ERR_OK)
	{
		goto END_FUNC;
	}

	ulWriteSize = HLIB_FILE_Write(hFile, (void *)pucBuff, 1, ulSize);
	if (ulWriteSize == 0)
	{
		goto END_FUNC;
	}

	nRet = ERR_OK;

END_FUNC:
	if(hFile)
	{
		HLIB_FILE_Close(hFile);
	}

	return nRet;
}


STATIC Handle_t xsvc_download_CreateUpgradeHandle_Dvbssu(void)
{
	Handle_t					hUpgrade;
	OtlUpgrade_Info_t			stInfo;
	HUINT32 					ulTag;

	HxSTD_memset(&stInfo, 0, sizeof(OtlUpgrade_Info_t));

	// Set User Data
	stInfo.ulDataSize		= sizeof(XsvcDownload_UpgradeInfo_t);
	stInfo.pvUserData		= (void*)&s_stSsuUpgradeInfo;

	// Set Callback Functions
	stInfo.eTarget			= eUPGRADE_TARGET_File;
	stInfo.fnInit			= xsvc_download_UpgradeInit_Dvbssu;
	stInfo.fnGetUserData	= xsvc_download_UpgradeGetUserData_Dvbssu;
	stInfo.fnSetUserData	= xsvc_download_UpgradeSetUserData_Dvbssu;
	stInfo.fnCheckComplete	= xsvc_download_UpgradeCheckComplete_Dvbssu;
	stInfo.fnOpenTarget		= xsvc_download_UpgradeCreateHandle_Dvbssu;
	stInfo.fnCloseTarget	= xsvc_download_UpgradeDestoryHandle_Dvbssu;
	stInfo.fnRead			= xsvc_download_UpgradeRead_Dvbssu;
	stInfo.fnWrite			= xsvc_download_UpgradeWrite_Dvbssu;

	ulTag = (0xFFFF0000 & (eDOWN_DOWNLOAD_OTA_IMAGE << 16)) | (0x0000FFFF & eDOWN_DOWNLOAD_OTA_IMAGE);
	hUpgrade = OTL_UPGRADE_Open(ulTag, &stInfo);
	if (UPGRADE_HANDLE_INVALID == hUpgrade)
	{
		stInfo.pvUserData = NULL;
	}

	return hUpgrade;
}

STATIC void xsvc_download_Proc_Dvbssu (SvcDownload_Msg_t *pstMsg, void *pvData)
{
	SvcDownload_DsmccContents_t	*pstContents = NULL;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo = NULL;
	HERROR						hErr;

#if defined(CONFIG_DEBUG)

#endif

	if (pstMsg == NULL || pvData == NULL)
	{
		HxLOG_Critical("\n\n");
		return ;
	}

	pstContents = (SvcDownload_DsmccContents_t *)pvData;

	switch (pstMsg->ulMsgClass)
	{
		case CMD_DOWN_START :
			HxLOG_Print("### [xsvc_download_Proc_Dvbssu] CMD_DOWN_START ###\n");
			hErr = xsvc_download_SetDownloadContents_Dvbssu (pstContents, pstMsg);
			if (hErr == ERR_DOWN_RESULT_OK)
			{
				HxLOG_Print(" ### Start DSI/DII Filtering ... ###\n");
				xsvc_download_StartDsiDiiFiltering_Dvbssu(pstContents);
			}
			else
			{
				HxLOG_Print(" ### Ignore DSI Filtering ... ###\n");
			}
			break;

		case CMD_DOWN_STOP :
			HxLOG_Print(" ### [xsvc_download_Proc_Dvbssu] CMD_DOWN_STOP ###\n");
			xsvc_download_StopFiltering_Dvbssu (pstContents);
			break;

		case DOWNCB_DSI_TABLE:
			HxLOG_Print("### [xsvc_download_Proc_Dvbssu] DOWNCB_DSI_TABLE ###\n");
			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
			{
				xsvc_download_StartDownload_Dvbssu (pstContents, pstMsg);
			}
			else
			{
				xsvc_download_StopFiltering_Dvbssu (pstContents);
				svc_download_UtilPostMsgToMgr (eSEVT_SWUP_DOWNLOAD_FAIL, pstContents->hAction, eSWUP_ERROR_WRITE_FILE, 0, 0);	// --> new : SW Update 는 분리
			}
			break;

		case DOWNCB_DDB_TABLE:
			HxLOG_Print("### [xsvc_download_Proc_Dvbssu] DOWNCB_DDB_TABLE ###\n");

			xsvc_download_StopFiltering_Dvbssu (pstContents);
			xsvc_download_FreeDownloadInfo_Dvbssu (pstDnInfo);

			if (pstMsg->unArg.stSection.ulTableStatus == eSI_TABLE_RECEIVED)
			{
				HxLOG_Print(">>>>> All Block Received \n");
				svc_download_UtilPostMsgToMgr (eSEVT_SWUP_DOWNLOAD_SUCCESS, pstContents->hAction, pstContents->eActType, (HUINT32)pstContents->usPid, 0);
			}
			else
			{
				HxLOG_Print(">>>>> Block Reception Failed... \n");
				svc_download_UtilPostMsgToMgr (eSEVT_SWUP_DOWNLOAD_FAIL, pstContents->hAction, eSWUP_ERROR_WRITE_FILE, 0, 0);	// --> new : SW Update 는 분리
			}
			break;

		default :
			break;
	}
}

#define _____GLOBAL_FUNCTIONS____
HERROR xsvc_download_CheckDownloadComplete_Dvbssu(HUINT32 ulSignalVersion, HBOOL *pbComplete)
{
	if (NULL == pbComplete)
	{
		return ERR_FAIL;
	}

	*pbComplete = xsvc_download_CheckOTAInfo_Dvbssu(ulSignalVersion);

	return ERR_OK;
}

HERROR xsvc_download_ClearDownloadInfo_Dvbssu(void)
{
	xsvc_download_WriteOtaDbInfo_Dvbssu(NULL);

	return ERR_OK;
}

HERROR xsvc_download_InitAction_Dvbssu (void **ppvAction, void **ppvData, void *pvSiMsg)
{
	SvcDownload_DsmccContents_t	*pstContents;
	XsvcDownload_DsmccDnInfo_t	*pstDnInfo;
	HERROR						hErr;

	// for evading the warning message :
	(void)pvSiMsg;

	if (ppvAction == NULL || ppvData == NULL)
	{
		return ERR_DOWN_TARGET_NULL;
	}

	pstDnInfo = xsvc_download_AllocDownloadInfo_Dvbssu();
	if (pstDnInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	hErr = svc_download_AllocDsmccContents(&pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		xsvc_download_FreeDownloadInfo_Dvbssu (pstDnInfo);
		return ERR_DOWN_MEM_ALLOC_FAIL;
	}

	pstContents->hUpgrade		= xsvc_download_CreateUpgradeHandle_Dvbssu();
	pstContents->hAction		= HANDLE_NULL;
	pstContents->ulDemuxId		= RxRSCID_NULL;
	pstContents->pvDownloadInfo	= (void *)pstDnInfo;

	*ppvAction	= (void *)xsvc_download_Proc_Dvbssu;
	*ppvData	= (void *)pstContents;

	return ERR_DOWN_RESULT_OK;
}


HERROR xsvc_download_TermAction_Dvbssu (void **ppvAction, void **ppvData)
{
	SvcDownload_DsmccContents_t		*pstContents;
	XsvcDownload_DsmccDnInfo_t		*pstDnInfo;

	if (ppvAction == NULL || ppvData == NULL)
	{
    	return ERR_DOWN_TARGET_NULL;
	}

	*ppvAction = NULL;

	if (*ppvData != NULL)
	{
		ENTER_CRITICAL_DNLDCNTS;
		pstContents = (SvcDownload_DsmccContents_t *)(*ppvData);

		pstDnInfo = xsvc_download_GetDownloadInfo_Dvbssu (pstContents);

		OTL_UPGRADE_Close(pstContents->hUpgrade);

		xsvc_download_FreeDownloadInfo_Dvbssu (pstDnInfo);
		svc_download_FreeDsmccContents (pstContents);

	    	*ppvData = NULL;
		LEAVE_CRITICAL_DNLDCNTS;
	}

	return ERR_DOWN_RESULT_OK;
}

/*********************** End of File ******************************/

