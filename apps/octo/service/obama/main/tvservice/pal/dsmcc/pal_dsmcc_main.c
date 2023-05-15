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
#include <octo_common.h>

#include "util.h"
#include <pal_sef.h>
#include <pal_dsmcc.h>
#include "pal_dsmcc_lib.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC HERROR	pal_dsmcc_Init(void);
STATIC HERROR	pal_dsmcc_BuildDownloadData(HUINT16 usMessageId, HUINT8 *pucIsComplete, PalDsmcc_File_t **ppDsmccFile);
STATIC HERROR	pal_dsmcc_ParseDsmccForDsiNDii(HUINT8 *pucRawData, HUINT16 *pusMessageId);
STATIC HERROR	pal_dsmcc_ParseDsmccForDdb(HUINT8 *pucRawData, HUINT16 *pusMessageId);



#define	______GLOBAL_FUNC_START______

HERROR		PAL_DSMCC_Init(void)
{
	return pal_dsmcc_Init();
}

HERROR		PAL_DSMCC_FreeDsmcc(void)
{
	DSMCCLIB_FreeDsmcc();
	return ERR_OK;
}

HERROR		PAL_DSMCC_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime)
{
	return DSMCCLIB_GetDsmccSessionInfo(pulTransactionId, pulDsiCrc, pulRegisteredTime);
}


HERROR		PAL_DSMCC_BuildDownloadData(HUINT16 usMessageId, HUINT8 *pucIsComplete, PalDsmcc_File_t **ppDsmccFile)
{

	return pal_dsmcc_BuildDownloadData(usMessageId, pucIsComplete, ppDsmccFile);
}


HERROR		PAL_DSMCC_ParseDsmccForDsiNDii(HUINT8 *pucRawData, HUINT16 *pusMessageId)
{
	return pal_dsmcc_ParseDsmccForDsiNDii(pucRawData, pusMessageId);
}


HERROR		PAL_DSMCC_ParseDsmccForDdb(HUINT8 *pucRawData, HUINT16 *pusMessageId)
{
	return pal_dsmcc_ParseDsmccForDdb(pucRawData, pusMessageId);
}


HERROR		PAL_DSMCC_SetDsmccLayerType(HUINT32 ulLayerType)
{
	return DSMCCLIB_SetDsmccLayerType(ulLayerType);
}

HERROR		PAL_DSMCC_ParseDsmccSectionInfo(HUINT8 *pRawData, HUINT16 *psSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum)
{
	return DSMCCLIB_ParseDsmccSectionInfo(pRawData, psSectionLen, pulCrc, pucVerNum);
}

HERROR		PAL_DSMCC_ParseDsmccMessageHeader(	HUINT8 *pHeader, HINT32 *plHeaderLen, HUINT16 *pusMessageId, HUINT32 *pulTransactionId)
{
	return DSMCCLIB_ParseDsmccMessageHeader(pHeader, plHeaderLen, pusMessageId, pulTransactionId);
}


HERROR		PAL_DSMCC_GetDownloadSize(HUINT32 *pulSize)
{
	return DSMCCLIB_GetDownloadSize(pulSize);
}


#define	______LOCAL_FUNC_START______

STATIC HERROR			pal_dsmcc_Init(void)
{
	HERROR			hError = ERR_FAIL;


	hError = DSMCCLIB_InitResource();
	if(hError != ERR_OK)
	{
		HxLOG_Print("[pal_dsmcc_init] InitResource fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	hError = ERR_OK;

EXIT_DSMCCMAIN_FUNC :

	return hError;

}



STATIC HERROR		pal_dsmcc_BuildDownloadData(HUINT16 usMessageId, HUINT8 *pucIsComplete, PalDsmcc_File_t **ppDsmccFile)
{
	HERROR					hError = ERR_FAIL;
	HUINT8					ucIsDsmccComplete = 0;
	HUINT32					ulDsmccMode = 0;
	HUINT32					ulLayerType = 0;
	PalDsmcc_File_t				*pDsmccFile = NULL;

	if(pucIsComplete == NULL)
	{
		HxLOG_Print("[pal_dsmcc_BuildDownloadData] Param NULL!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}
	*pucIsComplete = 0;
	*ppDsmccFile = NULL;

	hError = DSMCCLIB_GetDsmccMode(&ulDsmccMode);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_BuildDownloadData] DSMCCLIB_GetDsmccMode fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	hError = DSMCCLIB_GetDsmccLayerType(&ulLayerType);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_BuildDownloadData] DSMCCLIB_GetDsmccLayerType fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	switch(usMessageId)
	{

		case DSI_MESSAGE_ID :
			hError = DSMCCLIB_SetDsmccMode(ePAL_DSMCC_DOWNLOAD_BUILD);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[pal_dsmcc_BuildDownloadData] local_dsmcclib_SetDsmccMode fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			break;

		case DII_MESSAGE_ID :
			if(ulLayerType == ePAL_DSMCC_ONE_LAYER)
			{
				hError = DSMCCLIB_SetDsmccMode(ePAL_DSMCC_DOWNLOAD_BUILD);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[pal_dsmcc_BuildDownloadData] DSMCCLIB_SetDsmccMode fail!!\n");
					goto EXIT_DSMCCMAIN_FUNC;
				}
			}
			else if(ulLayerType == ePAL_DSMCC_TWO_LAYER)
			{
				if(ulDsmccMode != ePAL_DSMCC_DOWNLOAD_BUILD)
				{
					HxLOG_Error("[pal_dsmcc_BuildDownloadData] ulDsmccMode != ePAL_DSMCC_DOWNLOAD_BUILD!!\n");
					goto EXIT_DSMCCMAIN_FUNC;
				}

#if				1
				hError = DSMCCLIB_BuildDii();
				if(hError != ERR_OK)
				{
					HxLOG_Error("[pal_dsmcc_BuildDownloadData] DSMCCLIB_BuildDii fail!!\n");
					goto EXIT_DSMCCMAIN_FUNC;
				}
#endif
			}
			else
			{
				HxLOG_Print("[pal_dsmcc_BuildDownloadData] ulLayerType(%x) fail!!\n", ulLayerType);
				goto EXIT_DSMCCMAIN_FUNC;
			}
			break;

		case DDB_MESSAGE_ID :
			if(ulDsmccMode != ePAL_DSMCC_DOWNLOAD_BUILD)
			{
				HxLOG_Error("[pal_dsmcc_BuildDownloadData] ulDsmccMode != ePAL_DSMCC_DOWNLOAD_BUILD!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			hError = DSMCCLIB_BuildDdb(ulLayerType, &ucIsDsmccComplete, &pDsmccFile);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[pal_dsmcc_BuildDownloadData] DSMCCLIB_BuildDii fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}
			*pucIsComplete = ucIsDsmccComplete;
			*ppDsmccFile = pDsmccFile;

			break;

		default :
			break;
	}

	hError = ERR_OK;

EXIT_DSMCCMAIN_FUNC :

	return hError;


}

// SECTION_BYTES_SKIP : table id ~ section_length(skip bytes)
#define palDsmcc_SECTION_DSMCCMSG_OFFSET			8
STATIC HERROR		pal_dsmcc_ParseDsmccForDsiNDii(HUINT8 *pucRawData, HUINT16 *pusMessageId)
{
	HUINT8				*pucNewSec = NULL;
	HINT16				sSecLen = 0;
	HINT16				sSectionLength = 0;
	HINT32				lHeaderLen = 0;
	HERROR				hError = ERR_FAIL;
	HUINT16				usMessageId = 0;
	HUINT32				ulTransactionId = 0;
	HUINT32				ulCurrentSessionTransactionId = 0;
	HUINT32				ulCrc = 0;
	HUINT8				ucVerNum = 0;
	HUINT32				ulRegisteredTime = 0;

	GROUP_INFO_t		*pGroupList = NULL;
	DIIMSG_t			*pDiiMsg = NULL;
	HUINT16				usNumOfGroup = 0;
	HUINT32 				ulDsiCrc  =0;
	HUINT8				ucDiiBuildComplete = FALSE;


	if(pucRawData == NULL || pusMessageId == NULL)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] GetMsgQueueId fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}
	*pusMessageId = 0;

	pucNewSec = pucRawData;
	sSecLen = (HINT16)get12bit((pucNewSec+1));

	hError = DSMCCLIB_ParseDsmccSectionInfo(pucNewSec, &sSectionLength, &ulCrc, &ucVerNum);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] ParseDsmccSectionInfo fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	hError = DSMCCLIB_ParseDsmccMessageHeader(	(pucNewSec+palDsmcc_SECTION_DSMCCMSG_OFFSET),
													&lHeaderLen,
													&usMessageId,
													&ulTransactionId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] ParseDsmccMessageHeader fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	// raw data info set
	pucNewSec = (pucNewSec + lHeaderLen + palDsmcc_SECTION_DSMCCMSG_OFFSET);
	sSecLen = (HINT16)(sSecLen - lHeaderLen -(palDsmcc_SECTION_DSMCCMSG_OFFSET - SECTION_BYTES_SKIP) - SECTION_BYTES_CRC);
	switch(usMessageId)
	{
		case DSI_MESSAGE_ID :
			HxLOG_Info("[pal_dsmcc_ParseDsmccForDsiNDii] DSI_MESSAGE_ID!!\n");

			hError = DSMCCLIB_GetDsmccSessionInfo(&ulCurrentSessionTransactionId, &ulDsiCrc, &ulRegisteredTime);
			if(hError != ERR_OK)
			{
				HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] GetRecentDsmccInfo fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			if(	ulCurrentSessionTransactionId == ulTransactionId
				&& ulDsiCrc == ulCrc)
			{
				HxLOG_Info("[pal_dsmcc_ParseDsmccForDsiNDii] Skip~ same dsi!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			// ulTransactionId : dsmcc session identifier
			// initialize or update
			if( ulCurrentSessionTransactionId != ulTransactionId)
			{
				// 1. free existed dsmcc data
				DSMCCLIB_FreeDsmcc();

				hError = DSMCCLIB_SetDsmccLayerType(ePAL_DSMCC_TWO_LAYER);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] DSMCCLIB_SetDsmccLayerType fail!!\n");
					goto EXIT_DSMCCMAIN_FUNC;
				}

				// 2. initialize new dsmcc data
				hError = DSMCCLIB_SetDsmccSessionInfo(ulTransactionId, ulCrc, 0);
				if(hError != ERR_OK)
				{
					HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] DSMCCLIB_SetDsmccSessionInfo fail!!\n");
					goto EXIT_DSMCCMAIN_FUNC;
				}
			}

			hError = DSMCCLIB_ParseDsi(pucNewSec, sSecLen, &usNumOfGroup, &pGroupList);
			if(hError != ERR_OK || pGroupList == NULL)
			{
				HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] ParseDsi fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			hError = DSMCCLIB_AddGroupInfoList(usNumOfGroup, pGroupList);
			if(hError != ERR_OK)
			{
				DSMCCLIB_FreeGroupInfo(pGroupList);
				pGroupList = NULL;

				HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] DSMCCLIB_AddDsmccGroupList fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}
			break;

		case DII_MESSAGE_ID :
			HxLOG_Info("[pal_dsmcc_ParseDsmccForDsiNDii] DII_MESSAGE_ID!!\n");

			hError = DSMCCLIB_CheckDiiBuild(&ucDiiBuildComplete);
			if(hError != ERR_OK || ucDiiBuildComplete == 1)
			{
				HxLOG_Error("[DSMCCLIB_CheckDiiBuild] DSMCCLIB_CheckDiiBuild fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			// ulTransactionId : group identifier
			hError = DSMCCLIB_ParseDii(pucNewSec, sSecLen, ulTransactionId, &pDiiMsg);
			if(hError != ERR_OK || pDiiMsg == NULL)
			{
				HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] ParseDii fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}

			hError = DSMCCLIB_AddDiiMsgList(pDiiMsg);
			if(hError != ERR_OK)
			{
				DSMCCLIB_FreeDiiMsg(pDiiMsg);
				pDiiMsg = NULL;

				HxLOG_Error("[pal_dsmcc_ParseDsmccForDsiNDii] DSMCCLIB_AddDiiMsgList fail!!\n");
				goto EXIT_DSMCCMAIN_FUNC;
			}
			break;
	}

	*pusMessageId = usMessageId;

	hError = ERR_OK;

EXIT_DSMCCMAIN_FUNC :

	return hError;
}



STATIC HERROR		pal_dsmcc_ParseDsmccForDdb(HUINT8 *pucRawData, HUINT16 *pusMessageId)
{
	HUINT8				*pucNewSec = NULL;
	HINT16				sSecLen = 0;
	HINT16				sSectionLength = 0;
	HINT32				lHeaderLen = 0;
	HERROR				hError = ERR_FAIL;
	HUINT16				usMessageId = 0;
	HUINT32				ulDownloadId = 0;
	HUINT32				ulCrc = 0;
	HUINT8				ucVerNum = 0;
	DDBMSG_t			*pNewDdbMsg = NULL;


	if(pucRawData == NULL || pusMessageId == NULL)
	{
		HxLOG_Print("[pal_dsmcc_ParseDsmccForDdb] GetMsgQueueId fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}
	*pusMessageId = 0;

	pucNewSec = pucRawData;
	sSecLen = (HUINT16)get12bit((pucNewSec+1));

	hError = DSMCCLIB_ParseDsmccSectionInfo(pucNewSec, &sSectionLength, &ulCrc, &ucVerNum);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDdb] DSMCCLIB_ParseDsmccSectionInfo fail!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	hError = DSMCCLIB_ParseDsmccDownloadDataHeader(	(pucNewSec+palDsmcc_SECTION_DSMCCMSG_OFFSET),
														&lHeaderLen,
														&usMessageId,
														&ulDownloadId);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDdb] DSMCCLIB_ParseDsmccDownloadDataHeader null!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	// raw data info set
	pucNewSec = (pucNewSec + lHeaderLen + palDsmcc_SECTION_DSMCCMSG_OFFSET);
	sSecLen = (HINT16)(sSecLen - lHeaderLen -(palDsmcc_SECTION_DSMCCMSG_OFFSET - SECTION_BYTES_SKIP) - SECTION_BYTES_CRC);

	hError = DSMCCLIB_ParseDdb(pucNewSec, sSecLen, ulDownloadId, &pNewDdbMsg);
	if(hError != ERR_OK || pNewDdbMsg == NULL)
	{
		HxLOG_Error("[pal_dsmcc_ParseDsmccForDdb] DSMCCLIB_ParseDdb null!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	hError = DSMCCLIB_AddDdbMsgList(pNewDdbMsg);
	if(hError != ERR_OK)
	{
		DSMCCLIB_FreeDdbMsg(pNewDdbMsg);
		pNewDdbMsg = NULL;

		HxLOG_Info("[pal_dsmcc_ParseDsmccForDdb] DSMCCLIB_AddDdbMsgList null!!\n");
		goto EXIT_DSMCCMAIN_FUNC;
	}

	*pusMessageId = usMessageId;

	hError = ERR_OK;

EXIT_DSMCCMAIN_FUNC :

	return hError;
}




/*********************** End of File ******************************/
