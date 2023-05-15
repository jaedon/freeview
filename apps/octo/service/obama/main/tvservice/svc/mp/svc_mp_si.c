/** **********************************************************************************************************
	@file 		mw_mp_si.c

	@date		2012/01/02
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include <octo_common.h>

#include "svc_mplayer.h"
#include "_svc_mp_manager.h"
#include "_svc_mp_utils.h"
//#include "mwc_util.h"
#include "pal_media.h"
#include "pal_media_sub.h"
#include "pal_scaler.h"
#include "pal_video.h"
#include "pal_audio.h"
#include "pal_output.h"
#include "pal_thumbnail.h"
#include "stb_params.h"
#include "pal_pipe.h"
#include "pal_sef.h"
#include "pal_demux.h"
//#include "si_desmgr.h"
#include "svc_mplayersi.h"
#include "util.h"

#include <string.h>
#include <pal_datastream.h>


/*---------------------------------------------------------------------------------
 * defines
 *---------------------------------------------------------------------------------*/



#define SVC_MP_ASSERT(x)		\
	if ( x == NULL )\
	{\
		HxLOG_Error("MW_MP: the param is NULL.\n");\
		return ERR_FAIL;\
	}\




/*---------------------------------------------------------------------------------
 * typedef
 *---------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------
  static variables
 *---------------------------------------------------------------------------------*/
static MW_MediaSiContext_t	s_stMediaSiContext;



/*---------------------------------------------------------------------------------
 * static functions
 *---------------------------------------------------------------------------------*/
static inline PAL_MEDIA_t *__pal(SvcMp_MediaPlayer_t *player)
{
	return (PAL_MEDIA_t *)player->phyMediaHandle;
}

static MW_MediaSiContext_t *local_Media_GetSiContext(void)
{
	return &s_stMediaSiContext;
}

static  HULONG local_Media_GetSem(void)
{
	MW_MediaSiContext_t		*pstSiContext = NULL;

	pstSiContext = local_Media_GetSiContext();
	return pstSiContext->ulSemId;
}

static void local_Media_ClearRawPool(void)
{
	MW_MediaSiContext_t *pstPlayContext = NULL;

	pstPlayContext = local_Media_GetSiContext();
	if (pstPlayContext == NULL)
	{
		HxLOG_Error("[local_Media_ClearRawPool]\n");
		return;
	}

	if (pstPlayContext->bInit == FALSE)
	{
		HxLOG_Error("s_stMediaSiContext is not initialized\n");
		return;
	}
	VK_SEM_Get(pstPlayContext->ulRawSem);
	MWC_RAW_ClearRawPool(pstPlayContext->pstRawDataPool);
	VK_SEM_Release(pstPlayContext->ulRawSem);
}

static HUINT32 local_Media_AddRawSection(Handle_t hAction, HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec)
{
	HUINT32 	 ulRes = ERR_OK;
	MW_MediaSiContext_t *pstPlayContext = NULL;

	pstPlayContext = local_Media_GetSiContext();
	if (pstPlayContext == NULL)
	{
		HxLOG_Error("[local_Media_AddRawSection] hAction(0x%X)\n", hAction);
		return ERR_FAIL;
	}

	if (pstPlayContext->bInit == FALSE)
	{
		HxLOG_Error("s_stMediaSiContext is not initialized\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(pstPlayContext->ulRawSem);
	ulRes = MWC_RAW_AddRawSection(pstPlayContext->pstRawDataPool, usUniqueId, eSiTableType, pucRawSec);
	VK_SEM_Release(pstPlayContext->ulRawSem);

	return ulRes;
}

static void local_Media_FreeAllRawSection(Handle_t hAction, HUINT16 usUniqueId, SiTableType_e eSiTableType)
{
	MW_MediaSiContext_t *pstPlayContext = NULL;

	pstPlayContext = local_Media_GetSiContext();
	if (pstPlayContext == NULL)
	{
		HxLOG_Error("[local_Media_FreeAllRawSection] hAction(0x%X)\n", hAction);
		return;
	}

	if (pstPlayContext->bInit == FALSE)
	{
		HxLOG_Error("s_stMediaSiContext is not initialized\n");
		return;
	}

	VK_SEM_Get(pstPlayContext->ulRawSem);
	MWC_RAW_FreeAllSection(pstPlayContext->pstRawDataPool, usUniqueId, eSiTableType);
	VK_SEM_Release(pstPlayContext->ulRawSem);
}

static HUINT32 local_Media_FreeRawSection(Handle_t hAction, HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	HUINT32 	 ulRes = ERR_OK;
	MW_MediaSiContext_t *pstPlayContext = NULL;

	pstPlayContext = local_Media_GetSiContext();
	if (pstPlayContext == NULL)
	{
		HxLOG_Error("[local_Media_FreeRawSection] hAction(0x%X)\n", hAction);
		return ERR_FAIL;
	}

	if (pstPlayContext->bInit == FALSE)
	{
		HxLOG_Error("s_stMediaSiContext is not initialized\n");
		return ERR_FAIL;
	}

	VK_SEM_Get(pstPlayContext->ulRawSem);
	ulRes = MWC_RAW_FreeSection(pstPlayContext->pstRawDataPool, usUniqueId, eSiTableType, bUseExtId, (HUINT16)ulExtId);
	VK_SEM_Release(pstPlayContext->ulRawSem);

	return ulRes;
}

static HERROR		local_Media_StopSectionFilter(MW_MediaSiContext_t *pstMedisSIContext, SiTableType_e eSiType, MEDIA_RetrieveTableType_t eRetrieveType)
{
	HUINT32 ulRetriveTables = 0;

	if ((pstMedisSIContext == NULL) || (eSiType >= MAX_SI_TABLE_TYPE))
	{
		return ERR_FAIL;
	}

	ulRetriveTables = pstMedisSIContext->ulRetrieveTables;
	if (ulRetriveTables & eRetrieveType)
	{
		MWC_SIDMX_StopOneSection( pstMedisSIContext->aulFilterId[eSiType] );
		pstMedisSIContext->aulFilterId[eSiType] = MEDIA_FILTER_ID_NULL;
		pstMedisSIContext->ulRetrieveTables &= ~eRetrieveType;
	}

	return ERR_OK;
}

static inline void local_Media_GetTableIdAndResult(HUINT32 value, HUINT16* TableId, HUINT16 *result)
{
	*result = value & 0x0000ffff;
	*TableId = (value & 0xffff0000) >> 16;
}

static inline void local_Media_GetFilterAndTsUniqueId(HUINT32 value, HUINT16 *filterId, HUINT16 *tsUniqId)
{
	*tsUniqId = value & 0x0000ffff;
	*filterId = (value & 0xffff0000) >> 16;
}

static void     local_Media_ParsePAT(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		i, cnt;
	PalSef_RawData_t	*pSiRawData = NULL;
	HUINT16 	usTableId=0, usFilterId=0, usTsUniqId=0, usResult=0;
	HERROR		hErr = 0;

	local_Media_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	local_Media_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if (usResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n", (usResult == 2) ? "Time out" : "Error");
		SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_PAT_TABLE, usResult << 16 | usFilterId);
		return;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	/* 기존 내용을 지운다. */
	local_Media_FreeAllRawSection (hAction, usTsUniqId, eSI_TableType_PAT);
	for (i=0, cnt=0; (cnt < pSiRawData->num_sec) && (i < MEDIA_MAX_SECTION_NUM); i++)
	{
		if (pSiRawData->buf[i])
		{
			HUINT8		*pucTemp, *pucNewSec;
			HUINT16		usSecLen;

			pucTemp = pSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc((HUINT32)usSecLen + 3);
			if (pucNewSec == NULL)
			{
				HxLOG_Error("\n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pSiRawData->buf[i], (HUINT32)usSecLen + 3);

			hErr = local_Media_AddRawSection (hAction, usTsUniqId, eSI_TableType_PAT, pucNewSec);
			if (hErr != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}
			cnt++;
		}
	}

	SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_PAT_TABLE, usResult << 16 | usFilterId);

}

static void     local_Media_ParsePMT(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		i, usSecNum = 0;
	PalSef_RawData_t	*pSiRawData = NULL;
	HUINT16 		usTableId=0, usFilterId=0, usTsUniqId=0, usResult=0;
	HERROR		hErr;
	HUINT8		*pucRaw;
	HUINT16		usProgNum = 0;
	HUINT8		*pucTemp, *pucNewSec;
	HUINT16		usSecLen;

	local_Media_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	local_Media_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if (usResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n", (usResult == 2) ? "Time out" : "Error");
		SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_PMT_TABLE, usResult << 16 | usFilterId);
		return;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;

	/* 기존 내용을 지운다. */
	if (pSiRawData->num_sec > 0)
	{
		pucRaw = pSiRawData->buf[0];
		usProgNum = get16bit (pucRaw + 3);
		local_Media_FreeRawSection ( hAction, usTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)usProgNum);
	}

	for (i=0, usSecNum=0; (usSecNum < pSiRawData->num_sec) && (i < MEDIA_MAX_SECTION_NUM); i++)
	{
		if (pSiRawData->buf[i])
		{
			pucTemp = pSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc((HUINT32)usSecLen + 3);
			if (pucNewSec == NULL)
			{
				HxLOG_Error("\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pSiRawData->buf[i], (HUINT32)usSecLen + 3);

			hErr = local_Media_AddRawSection (hAction, usTsUniqId, eSI_TableType_PMT, pucNewSec);
			if (hErr != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}
			usSecNum++;
		}
	}

	SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_PMT_TABLE, usResult << 16 | usFilterId);
}

static void     local_Media_ParseSIT(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		i, cnt;
	PalSef_RawData_t	*pSiRawData = NULL;
	HUINT16 		usTableId=0, usFilterId=0, usTsUniqId=0, usResult=0;
	HERROR		hErr = 0;

	local_Media_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	local_Media_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if (usResult != (HUINT32)eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n", (usResult == 2) ? "Time out" : "Error");
		SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_SIT_TABLE, usResult << 16 | usFilterId);
		return;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	/* 기존 내용을 지운다. */
	local_Media_FreeAllRawSection (hAction, usTsUniqId, eSI_TableType_SIT);
	for (i=0, cnt=0; (cnt < pSiRawData->num_sec) && (i < MEDIA_MAX_SECTION_NUM); i++)
	{
		if (pSiRawData->buf[i])
		{
			HUINT8		*pucTemp, *pucNewSec;
			HUINT16		usSecLen;

			pucTemp = pSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc((HUINT32)usSecLen + 3);
			if (pucNewSec == NULL)
			{
				HxLOG_Error("\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pSiRawData->buf[i], (HUINT32)usSecLen + 3);

			hErr = local_Media_AddRawSection (hAction, usTsUniqId, eSI_TableType_SIT, pucNewSec);
			if (hErr != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}
			cnt++;
		}
	}

	SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_SIT_TABLE, usResult << 16 | usFilterId);
}

static void     local_Media_ParsePfEIT(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, Handle_t hAction)
{
	HUINT16		i, cnt;
	PalSef_RawData_t	*pSiRawData = NULL;
	HUINT16 		usTableId=0, usFilterId=0, usTsUniqId=0, usResult=0;
	HERROR		hErr = 0;

	local_Media_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usResult);
	local_Media_GetFilterAndTsUniqueId(ulFilterAndUniqId, &usFilterId, &usTsUniqId);

	if ( usResult != (HUINT32)eSiFilterResult_Complete )
	{
		HxLOG_Error("result = %s\n", (usResult == 2) ? "Time out" : "Error");
		SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_EIT_TABLE, usResult << 16 | usFilterId);
		return;
	}

	pSiRawData = (PalSef_RawData_t*)ulParam;
	/* 기존 내용을 지운다. */
	local_Media_FreeAllRawSection (hAction, usTsUniqId, eSI_TableType_EIT_ACTPF);
	for ( i=0, cnt=0; (cnt < pSiRawData->num_sec) && (i < MEDIA_MAX_SECTION_NUM); i++ )
	{
		if(pSiRawData->buf[i])
		{
			HUINT8		*pucTemp, *pucNewSec;
			HUINT16		usSecLen;

			pucTemp = pSiRawData->buf[i];
			usSecLen = (HUINT16)get12bit(pucTemp + 1);
			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc((HUINT32)usSecLen + 3);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pSiRawData->buf[i], (HUINT32)usSecLen + 3);

			hErr = local_Media_AddRawSection (hAction, usTsUniqId, eSI_TableType_EIT_ACTPF, pucNewSec);
			if(hErr != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}
			cnt++;
		}
	}

	SVC_MP_SendCommand(SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction)), SVC_MP_LOCAL_COMMAND_EIT_TABLE, usResult << 16 | usFilterId);
}


static HERROR local_Media_RequestPAT(MW_MediaSiContext_t *pstMedisSIContext)
{
	HERROR ulRet = ERR_OK;
	SiRetrieveParam_t stRetriveParam;
	HUINT16	usFilterId = 0;

	if (pstMedisSIContext == NULL)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[local_Media_RequestPAT]  hAction:0x%X\n", pstMedisSIContext->hAction);

	HxSTD_MemSet(&stRetriveParam, 0, sizeof(SiRetrieveParam_t));
	stRetriveParam.hAction = pstMedisSIContext->hAction;
	stRetriveParam.usTsUniqId = pstMedisSIContext->usTsUniqId;
	stRetriveParam.ucDemuxId = pstMedisSIContext->ulDemuxId;
	stRetriveParam.usPid = PAT_PID;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;
	stRetriveParam.bTableFilter = TRUE;
	stRetriveParam.bUseExtId = FALSE;
	stRetriveParam.bUseVerNum = FALSE;
	stRetriveParam.bCrc = TRUE;

	ulRet = MWC_SIDMX_RetrievePatSection(&stRetriveParam, local_Media_ParsePAT, NULL, &usFilterId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("[%s,%d] hAction:0x%X, ulRet:0x%X\n", pstMedisSIContext->hAction, ulRet);
		return ERR_FAIL;
	}
	pstMedisSIContext->ulRetrieveTables |= eMEDIA_RETRIEVE_PAT;
	pstMedisSIContext->aulFilterId[eSI_TableType_PAT] = usFilterId;

	return ERR_OK;
}


static HERROR local_Media_RequestPMT(MW_MediaSiContext_t *pstMedisSIContext)
{
	HERROR ulRet = ERR_OK;
	SiRetrieveParam_t stRetriveParam;
	HUINT16 usFilterId = 0;

	if (pstMedisSIContext == NULL)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[local_Media_RequestPMT]  hAction:0x%X\n", pstMedisSIContext->hAction);

	HxSTD_MemSet(&stRetriveParam, 0, sizeof(SiRetrieveParam_t));
	stRetriveParam.hAction = pstMedisSIContext->hAction;
	stRetriveParam.usTsUniqId = pstMedisSIContext->usTsUniqId;
	stRetriveParam.ucDemuxId = pstMedisSIContext->ulDemuxId;
	stRetriveParam.usPid = pstMedisSIContext->usPmtPid;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;
	stRetriveParam.bTableFilter = TRUE;
	stRetriveParam.bUseExtId = TRUE;
	stRetriveParam.usExtId = pstMedisSIContext->usPmtSvcId; /* Program Number */
	stRetriveParam.bUseVerNum = FALSE;
	stRetriveParam.bCrc = TRUE;

	ulRet = MWC_SIDMX_RetrievePmtSection(&stRetriveParam, local_Media_ParsePMT, NULL, &usFilterId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("[%s,%d] hAction:0x%X, ulRet:0x%X\n", pstMedisSIContext->hAction, ulRet);
		return ERR_FAIL;
	}
	pstMedisSIContext->ulRetrieveTables |= eMEDIA_RETRIEVE_PMT;
	pstMedisSIContext->aulFilterId[eSI_TableType_PMT] = usFilterId;

	return ERR_OK;
}


static HERROR local_Media_RequestSIT(MW_MediaSiContext_t *pstMedisSIContext)
{
	HERROR ulRet = ERR_OK;
	SiRetrieveParam_t stRetriveParam;
	HUINT16	usFilterId = 0;

	if (pstMedisSIContext == NULL)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("[local_Media_RequestSIT]  hAction:0x%X\n", pstMedisSIContext->hAction);

	HxSTD_MemSet(&stRetriveParam, 0, sizeof(SiRetrieveParam_t));
	stRetriveParam.hAction = pstMedisSIContext->hAction;
	stRetriveParam.usTsUniqId = pstMedisSIContext->usTsUniqId;
	stRetriveParam.ucDemuxId = pstMedisSIContext->ulDemuxId;
	stRetriveParam.usPid = SIT_PID;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;
	stRetriveParam.ucHead = ACT_HEAD;
	stRetriveParam.bTableFilter = TRUE;
	stRetriveParam.bUseExtId = FALSE;
	stRetriveParam.bUseVerNum = FALSE;
	stRetriveParam.bCrc = TRUE;

	ulRet = MWC_SIDMX_RetrieveSitSection(&stRetriveParam, local_Media_ParseSIT, NULL, &usFilterId);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("[%s,%d] hAction:0x%X, ulRet:0x%X\n", pstMedisSIContext->hAction, ulRet);
		return ERR_FAIL;
	}

	pstMedisSIContext->ulRetrieveTables |= eMEDIA_RETRIEVE_SIT;
	pstMedisSIContext->aulFilterId[eSI_TableType_SIT] = usFilterId;

	return ERR_OK;
}

static HERROR local_Media_RequestPfEit(MW_MediaSiContext_t *pstMedisSIContext)
{
	HERROR ulRet = ERR_OK;
	SiRetrieveParam_t stRetriveParam;
	HUINT16	usFilterId = 0;

	if( pstMedisSIContext == NULL )
		return ERR_FAIL;

	HxLOG_Error("[local_Media_RequestPfEit]  hAction:0x%X PmtSvcId:0x%X\n", pstMedisSIContext->hAction, pstMedisSIContext->usPmtSvcId);

	HxSTD_MemSet( &stRetriveParam, 0, sizeof(SiRetrieveParam_t) );

	stRetriveParam.hAction = pstMedisSIContext->hAction;
	stRetriveParam.usTsUniqId = pstMedisSIContext->usTsUniqId;
	stRetriveParam.ucDemuxId = (HUINT8)pstMedisSIContext->ulDemuxId;
	stRetriveParam.usPid = EIT_PID;
	stRetriveParam.ucMode = eSiFilterMode_Monitor;
	stRetriveParam.ucHead = ACT_HEAD;
	stRetriveParam.bTableFilter = TRUE;
	stRetriveParam.bUseExtId = TRUE;
	stRetriveParam.usExtId = pstMedisSIContext->usPmtSvcId;
	stRetriveParam.bUseVerNum = FALSE;
	stRetriveParam.bCrc = TRUE;

	ulRet = MWC_SIDMX_RetrieveEitPfSection( &stRetriveParam, local_Media_ParsePfEIT, NULL, &usFilterId );
	if( ulRet != ERR_OK )
	{
		HxLOG_Error("[%s,%d] hAction:0x%X, ulRet:0x%X\n", pstMedisSIContext->hAction, ulRet);
		return ERR_FAIL;
	}
	pstMedisSIContext->ulRetrieveTables |= eMEDIA_RETRIEVE_EIT_ACT_PF;
	pstMedisSIContext->aulFilterId[eSI_TableType_EIT_ACTPF] = (HUINT32)usFilterId;

	return ERR_OK;
}

static HERROR locla_Media_UpdateMultiTrackInfoByPmt(Handle_t hAction, MW_MEDIA_PmtInfo_t *pstPmtInfo)
{
	HERROR eRet = ERR_OK;
	SvcMp_MediaPlayer_t *pPlayer = NULL;
	PAL_MEDIA_MultiTrack_t stMultiTrack;
	HUINT32 i = 0, curAudioNum = 0;

	if (pstPmtInfo == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pPlayer = SVC_MP_GetActivePlayer(PAL_PIPE_GetActionId(hAction));
	curAudioNum = SVC_MP_GetAudioTrackCount(pPlayer);

	/* 현재 Audio Track 수와 PMT의 Audio 수가 다를 경우에만 Update */
	if (pstPmtInfo->usAudioNum == curAudioNum)
	{
		HxLOG_Print("[%s:%d] It has same audio number\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}

	HxLOG_Warning("Current aubio number=%d, PMT audio=%d\n", curAudioNum, pstPmtInfo->usAudioNum);
	/* PMT Audio정보로 Track정보를 Update한다. */
	HxSTD_MemSet(&stMultiTrack, 0, sizeof(PAL_MEDIA_MultiTrack_t));
	stMultiTrack.type = PAL_MEDIA_TRACK_TYPE_AUTIO;
	stMultiTrack.track_count = pstPmtInfo->usAudioNum;
	for (i=0; i<stMultiTrack.track_count; i++)
	{
		stMultiTrack.track_info.audio[i].audio_pes_id = pstPmtInfo->stAudioInfo[i].usPid;
		stMultiTrack.track_info.audio[i].audio_stream_id = pstPmtInfo->stAudioInfo[i].usPid;
		stMultiTrack.track_info.audio[i].audio_codec = pstPmtInfo->stAudioInfo[i].eAudCodec;
	}
	eRet = PAL_MEDIA_UpdateMultiTrack(__pal(pPlayer), &stMultiTrack);
	if (eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SetPlaySpeed: Cannot change the play speed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


/*---------------------------------------------------------------------------------
 * public functions
 *---------------------------------------------------------------------------------*/

HERROR SVC_MP_SI_Init(void)
{
	HUINT8 aucSemName1[32];
	HINT32 nError = 0;
	MW_MediaSiContext_t *pstSiContext = NULL;

	pstSiContext = local_Media_GetSiContext();
	/* DLNA에서 SI데이터를 관리할 Context를 초기화 */
	HxSTD_memset(pstSiContext, 0x00, sizeof(MW_MediaSiContext_t));
	pstSiContext->ulDemuxId = MEDIA_RxRSCID_NULL;
	pstSiContext->usPmtPid = MEDIA_FILTER_ID_NULL;
	pstSiContext->usPmtSvcId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usNetworkId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usTsId = MEDIA_FILTER_ID_NULL;
	pstSiContext->bPmtValid = FALSE;
	pstSiContext->bEventValid = FALSE;
	nError = VK_SEM_Create (&pstSiContext->ulSemId, "MediaPb", VK_SUSPENDTYPE_PRIORITY);
	if (nError != ERR_OK)
	{
		HxLOG_Error(" [SVC_MP_SI_Init] VK_SEM_Create fail, ulRet:0x%X\n", nError);
		return ERR_FAIL;
	}

	snprintf(aucSemName1, sizeof(aucSemName1),"MdRaw%d", 1);
	nError = VK_SEM_Create(&pstSiContext->ulRawSem, aucSemName1, VK_SUSPENDTYPE_PRIORITY);
	if (nError != ERR_OK)
	{
		HxLOG_Error(" [SVC_MP_SI_Init] VK_SEM_Create fail, ulRet:0x%X\n", nError);
		return ERR_FAIL;
	}

	pstSiContext->pstRawDataPool = MWC_RAW_AllocRawPool(16, aucSemName1);
	if (pstSiContext->pstRawDataPool == NULL)
	{
		HxLOG_Error(" [__mediaplayer_init_si] pstRawDataPool is NULL, %s\n", aucSemName1);
		return ERR_FAIL;
	}
	pstSiContext->bInit = TRUE;

	return ERR_OK;
}

HERROR SVC_MP_SI_Setup(SvcMp_MediaPlayer_t *player)
{
	HERROR				hError = ERR_OK;
	HINT32				nDemuxId = 0;
	MW_MediaSiContext_t	*pstSiContext = NULL;

	PAL_MEDIA_t			*pstPalMedia = (PAL_MEDIA_t *)player->phyMediaHandle;

	SVC_MP_ASSERT(player);

	VK_SEM_Get(local_Media_GetSem());
	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext == NULL)
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* Action ID로 부터 Demux resource를 얻음. */
	hError = PAL_PIPE_GetResourceId(player->hActionId, eRxRSCTYPE_DEMUX, &nDemuxId);
	if (hError != ERR_OK)
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	local_Media_ClearRawPool();
	pstSiContext->hAction = PAL_PIPE_GetActionHandle(player->hActionId);
	pstSiContext->ulDemuxId = nDemuxId;
	pstSiContext->ulRetrieveTables = 0;
	pstSiContext->ulReceivedTables = 0;
	pstSiContext->usTsUniqId = 0;
	pstSiContext->usPmtPid = MEDIA_FILTER_ID_NULL;
	pstSiContext->usPmtSvcId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usNetworkId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usTsId = MEDIA_FILTER_ID_NULL;

	pstSiContext->bPmtValid = FALSE;
	HxSTD_MemSet(&pstSiContext->stPmtInfo, 0x00, sizeof(MW_MEDIA_PmtInfo_t));
	pstSiContext->bEventValid = FALSE;
	SVC_MP_SI_FreeEventInfo(&pstSiContext->stEvtInfo);
	HxSTD_MemSet(&pstSiContext->stEvtInfo, 0x00, sizeof(MW_MEDIA_EventInfo_t));

	PAL_PIPE_SetOutputResource (player->hActionId, ePAL_PIPE_OUTPUT_DECORDER, 0);
	PAL_PIPE_SetDemux (player->hActionId, pstSiContext->ulDemuxId);
	PAL_PIPE_SetInputResource (player->hActionId, ePAL_PIPE_INPUT_MEDIA_PLAYER, 0);

	if (pstPalMedia != NULL)
	{
		pstPalMedia->nDemuxId = nDemuxId;
	}

	VK_SEM_Release(local_Media_GetSem());
	return ERR_OK;
}


HERROR SVC_MP_SI_Release(void)
{
	MW_MediaSiContext_t	*pstSiContext = NULL;

	VK_SEM_Get(local_Media_GetSem());
	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext == NULL)
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstSiContext->ulDemuxId != MEDIA_RxRSCID_NULL) && (pstSiContext->ulRetrieveTables != 0))
	{
		PAL_SEF_StopAllSecFilterSync(pstSiContext->ulDemuxId);
		(void)PAL_PIPE_ResetPipe (PAL_PIPE_GetActionId (pstSiContext->hAction));
	}

	pstSiContext->hAction = 0;
	pstSiContext->usTsUniqId = 0;
	pstSiContext->ulRetrieveTables = 0;
	pstSiContext->ulReceivedTables = 0;
	pstSiContext->ulDemuxId = MEDIA_RxRSCID_NULL;
	pstSiContext->usPmtPid = MEDIA_FILTER_ID_NULL;
	pstSiContext->usPmtSvcId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usNetworkId = MEDIA_FILTER_ID_NULL;
	pstSiContext->usTsId = MEDIA_FILTER_ID_NULL;

	VK_SEM_Release(local_Media_GetSem());
	return ERR_OK;
}

HERROR SVC_MP_SI_StartSI(SvcMp_MediaPlayer_t *player)
{
	SVC_MP_ASSERT(player);

	/* start si for DLNA playback */
	SVC_MP_SendCommand(player, SVC_MP_LOCAL_COMMAND_SI, 0);

	HxLOG_Print("MW_MP: StartSI: .\n");

	return ERR_OK;
}


HERROR SVC_MP_SI_CmdSi(Handle_t hAction, HUINT32 ulRetrieveType)
{
	HUINT32 	nRet = ERR_OK;
	MW_MediaSiContext_t	*pstSiContext = NULL;

	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstSiContext->bInit == FALSE)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstSiContext->hAction == hAction)
	{
		/* PAT */
		if ((ulRetrieveType & eMEDIA_RETRIEVE_PAT) == eMEDIA_RETRIEVE_PAT)
		{
			nRet = local_Media_RequestPAT(pstSiContext);
			if (nRet != ERR_OK)
			{
				HxLOG_Error("local_Media_RequestPAT, nRet:0x%X\n", nRet);
			}
		}

		/* PMT */
		if ((ulRetrieveType & eMEDIA_RETRIEVE_PMT) == eMEDIA_RETRIEVE_PMT)
		{
			nRet = local_Media_RequestPMT(pstSiContext);
			if( nRet != ERR_OK )
			{
				HxLOG_Error("local_PbBase_RequestPMT, nRet:0x%X\n", nRet);
			}
		}

		/* SIT */
		if ((ulRetrieveType & eMEDIA_RETRIEVE_SIT) == eMEDIA_RETRIEVE_SIT)
		{
			nRet = local_Media_RequestSIT(pstSiContext);
			if( nRet != ERR_OK )
			{
				HxLOG_Error("local_PbBase_RequestSIT, nRet:0x%X\n", nRet);
			}
		}

		/* EIT p/f*/
		if ((ulRetrieveType & eMEDIA_RETRIEVE_EIT_ACT_PF) == eMEDIA_RETRIEVE_EIT_ACT_PF)
		{
			nRet = local_Media_RequestPfEit(pstSiContext);
			if( nRet != ERR_OK )
			{
				HxLOG_Error("local_Media_RequestPfEit, nRet:0x%X\n", nRet);
			}
		}
	}
	else
	{
		HxLOG_Error("[%s] Error:s_stMediaSiContext.hAction=%d hAction=%d\n", __FUNCTION__, pstSiContext->hAction, hAction);
	}

	return nRet;
}

HERROR SVC_MP_SI_ReceivedPAT(Handle_t hAction, HUINT32 param1, HUINT32 param2)
{
	HUINT32 ulRet = ERR_OK;
	MW_MediaSiContext_t *pstSiContext = NULL;
	HUINT16 usResult = 0, usFilterId=0;
	PatSectionInfo_t stPatSectionInfo;
	HUINT16 usPmtPid = 0;

	pstSiContext = local_Media_GetSiContext();

	usResult = (HUINT16)param1;
	usFilterId = (HUINT16)param2;

	if (pstSiContext->hAction != hAction)
	{
		HxLOG_Critical("s_stMediaSiContext.hAction:0x%X hAction:0x%X\n", pstSiContext->hAction, hAction);
		return ERR_FAIL;
	}

	if ((pstSiContext->ulRetrieveTables & eMEDIA_RETRIEVE_PAT) != eMEDIA_RETRIEVE_PAT)
	{
		HxLOG_Error("hAction:0x%X PAT Monitoring is not running.\n", hAction );
		return ERR_FAIL;
	}

	if (usResult != eSiFilterResult_Complete)
	{
		HxLOG_Warning(", Timeout\n");
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_PAT_TIMEOUT, hAction, 0, 0, 0);
		return ERR_OK;
	}

	VK_SEM_Get(pstSiContext->ulRawSem);
	ulRet = MWC_SI_GetPatSection(pstSiContext->pstRawDataPool, pstSiContext->usTsUniqId, 0/* ulIdx */, &stPatSectionInfo);
	pstSiContext->usTsId = stPatSectionInfo.usTsId;
	ulRet = MWC_SI_GetFirstPmtPid(pstSiContext->pstRawDataPool, pstSiContext->usTsUniqId, &pstSiContext->usPmtSvcId, &usPmtPid);
	pstSiContext->ulReceivedTables |= eMEDIA_RETRIEVE_PAT;
	VK_SEM_Release(pstSiContext->ulRawSem);
	if ((ulRet != ERR_OK) || (usPmtPid == PID_NULL))
	{
		local_Media_StopSectionFilter(pstSiContext, eSI_TableType_PMT, eMEDIA_RETRIEVE_PMT);
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_PMT_PIDREMOVED, hAction, 0, 0, 0);
		return ERR_FAIL;
	}

	// No change or updated previously.
	if (pstSiContext->usPmtPid == usPmtPid)
	{
		return ERR_OK;
	}

	if (pstSiContext->usPmtPid != PID_NULL)
	{
		// PMT changed: Stop the PMT
		pstSiContext->usPmtPid = usPmtPid;
		ulRet = local_Media_StopSectionFilter(pstSiContext, eSI_TableType_PMT, eMEDIA_RETRIEVE_PMT);
		if( ulRet != ERR_OK )
		{
			HxLOG_Error("local_Media_StopSectionFilter hAction:0x%X, ulRet:0x%X\n",hAction, ulRet);
		}

		// Inform that the PMT PID changed:
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_PMT_PIDCHANGED, hAction, 0, 0, 0);
	}

	// Request PMT
	pstSiContext->usPmtPid = usPmtPid;
	ulRet = local_Media_RequestPMT(pstSiContext);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("local_Media_RequestPMT, hAction:0x%X\n", hAction);
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR SVC_MP_SI_ReceivedPMT(Handle_t hAction, HUINT32 param1, HUINT32 param2)
{
	HERROR hErr = ERR_OK;
	MW_MediaSiContext_t *pstSiContext = NULL;
	HUINT16 usResult = 0, usFilterId = 0;

	usResult = (HUINT16)param1;
	usFilterId = (HUINT16)param2;

	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext->hAction != hAction)
	{
		HxLOG_Critical("s_stMediaSiContext.hAction:0x%X hAction:0x%X\n", pstSiContext->hAction, hAction);
		return ERR_FAIL;
	}

	if ((pstSiContext->ulRetrieveTables & eMEDIA_RETRIEVE_PMT) != eMEDIA_RETRIEVE_PMT)
	{
		HxLOG_Error("hAction:0x%X PMT Monitoring is not running.\n", hAction );
		return ERR_FAIL;
	}

	if (usResult != eSiFilterResult_Complete)
	{
		HxLOG_Warning(", Timeout\n");
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_PMT_TIMEOUT, hAction, 0, 0, 0);
		return ERR_OK;
	}

	VK_SEM_Get(pstSiContext->ulRawSem);
	pstSiContext->ulReceivedTables |= eMEDIA_RETRIEVE_PMT;
	VK_SEM_Release(pstSiContext->ulRawSem);

	if ((hErr == ERR_OK) && (pstSiContext->bPmtValid != FALSE))
	{
		/* PMT정보로 Media DI의 Track Info를 Update */
		locla_Media_UpdateMultiTrackInfoByPmt(pstSiContext->hAction, &pstSiContext->stPmtInfo);

		/* PMT변경을 알린다. */
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_PMT_CHANGED, hAction, 0, 0, 0);

		/* PMT Update시 PVR_SI_InitPmtInfo()에 의해 SIT에서 Update한 내용이 초기화 됨으로
		   SIT를 이미 받은 다음에는 PMT Update시 무조건 기존 SIT 정보를 다시 Update한다. */
		if (((pstSiContext->ulReceivedTables & eMEDIA_RETRIEVE_SIT) == eMEDIA_RETRIEVE_SIT) && (pstSiContext->bEventValid == TRUE))
		{
			HxLOG_Warning("Forced SIT Info update.\n");
			media_player_utils_PostMsgToMgr(SVC_MP_EVENT_SIT_CHANGED, hAction, 0, 0, 0);
		}
	}
	else
	{
		HxLOG_Error("PMT is not valid\n");
	}

	return ERR_OK;
}

HERROR SVC_MP_SI_ReceivedSIT(Handle_t hAction, HUINT32 param1, HUINT32 param2)
{
	HERROR hErr = ERR_OK;
	MW_MediaSiContext_t *pstSiContext = NULL;
	HUINT16 usResult = 0, usFilterId = 0;

	usResult = (HUINT16)param1;
	usFilterId = (HUINT16)param2;

	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext->hAction != hAction)
	{
		HxLOG_Critical("s_stMediaSiContext.hAction:0x%X hAction:0x%X\n", pstSiContext->hAction, hAction);
		return ERR_FAIL;
	}

	if ((pstSiContext->ulRetrieveTables & eMEDIA_RETRIEVE_SIT) != eMEDIA_RETRIEVE_SIT)
	{
		HxLOG_Error("hAction:0x%X SIT Monitoring is not running.\n", hAction );
		return ERR_FAIL;
	}

	if (usResult != eSiFilterResult_Complete)
	{
		HxLOG_Warning(", Timeout\n");
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_SIT_TIMEOUT, hAction, 0, 0, 0);
		return ERR_OK;
	}

	/* Parsing PMT */
	VK_SEM_Get(pstSiContext->ulRawSem);
	pstSiContext->ulReceivedTables |= eMEDIA_RETRIEVE_SIT;
	VK_SEM_Release(pstSiContext->ulRawSem);

	if ((hErr == ERR_OK) && (pstSiContext->bEventValid == TRUE))
	{
		/* Inform to APP */
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_SIT_CHANGED, hAction, 0, 0, 0);
	}
	else
	{
		/* Transcode된 Sony DMS Content의 경우 SIT는 존재하나 실제 Event정보는 존재 하지 않는다. */
		if (((pstSiContext->ulReceivedTables & eMEDIA_RETRIEVE_PMT) == eMEDIA_RETRIEVE_PMT) && (pstSiContext->bPmtValid == TRUE))
		{
			HxLOG_Warning("SIT is not valid. but it needs to inform CopyProtection info to app.\n");
			media_player_utils_PostMsgToMgr(SVC_MP_EVENT_SIT_CHANGED, hAction, 0, 0, 0);
		}
		else
		{
			HxLOG_Error("SIT is not valid\n");
		}
	}

	return ERR_OK;
}

HERROR SVC_MP_SI_ReceivedPfEIT(Handle_t	hAction, HUINT32 param1, HUINT32 param2)
{
	MW_MediaSiContext_t *pstSiContext = NULL;
	HUINT16 usResult = 0, usFilterId = 0;

	usResult = (HUINT16)param1;
	usFilterId = (HUINT16)param2;

	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext->hAction != hAction)
	{
		HxLOG_Critical("s_stMediaSiContext.hAction:0x%X hAction:0x%X\n", pstSiContext->hAction, hAction);
		return ERR_FAIL;
	}

	if ((pstSiContext->ulRetrieveTables & eMEDIA_RETRIEVE_EIT_ACT_PF) != eMEDIA_RETRIEVE_EIT_ACT_PF)
	{
		HxLOG_Error("hAction:0x%X EIT Monitoring is not running.\n", hAction );
		return ERR_FAIL;
	}

	if (usResult != eSiFilterResult_Complete)
	{
		media_player_utils_PostMsgToMgr(SVC_MP_EVENT_EIT_TIMEOUT, hAction, 0, 0, 0);
		return ERR_OK;
	}

	/* Parsing PMT */
	VK_SEM_Get(pstSiContext->ulRawSem);

	pstSiContext->ulReceivedTables |= eMEDIA_RETRIEVE_EIT_ACT_PF;

	VK_SEM_Release(pstSiContext->ulRawSem);

	/* Inform to APP */
	media_player_utils_PostMsgToMgr(SVC_MP_EVENT_EIT_CHANGED, hAction, 0, 0, 0);

	return ERR_OK;
}

HERROR SVC_MP_SI_GetTripleId(Handle_t hAction, DbSvc_TripleId_t *pstTripleId)
{
	MW_MediaSiContext_t		*pstSiContext = NULL;

	VK_SEM_Get(local_Media_GetSem());

	pstSiContext = local_Media_GetSiContext();
	if (pstSiContext == NULL)
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstTripleId->usOnId = pstSiContext->usNetworkId;
	pstTripleId->usTsId = pstSiContext->usTsId;
	pstTripleId->usSvcId = pstSiContext->usPmtSvcId;

	VK_SEM_Release(local_Media_GetSem());
	return ERR_OK;
}

HERROR SVC_MP_SI_GetDrmInfo(Handle_t hAction, DxCopyrightControl_t *pstDrmInfo)
{
	MW_MediaSiContext_t		*pstSiContext = NULL;

	VK_SEM_Get(local_Media_GetSem());

	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pstDrmInfo == NULL))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstDrmInfo, &pstSiContext->stPmtInfo.stDrmInfo, sizeof(DxCopyrightControl_t));

	VK_SEM_Release(local_Media_GetSem());
	return ERR_OK;
}

HERROR SVC_MP_SI_GetVideoList(Handle_t hAction, MW_Media_VIDEO_InfoList_t *pVideoInfo)
{
	MW_MediaSiContext_t		*pstSiContext = NULL;
	MW_MEDIA_PmtInfo_t 		*pstPmtInfo = NULL;
	HUINT32					i;

	if (pVideoInfo == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(local_Media_GetSem());

	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pstSiContext->bPmtValid == FALSE))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstPmtInfo = &pstSiContext->stPmtInfo;
	HxSTD_memset(pVideoInfo, 0x00, sizeof(MW_Media_VIDEO_InfoList_t));

	pVideoInfo->nVideo = pstPmtInfo->usVideoNum;
	if (pVideoInfo->nVideo >= MW_MEDIA_MAX_VIDEO_ES_NUM)
	{
		pVideoInfo->nVideo = MW_MEDIA_MAX_VIDEO_ES_NUM;
	}

	for (i = 0; i < pstPmtInfo->usVideoNum; i++)
	{
		pVideoInfo->stVideoInfo[i].usPid = pstPmtInfo->stVideoInfo[i].usPid;
		pVideoInfo->stVideoInfo[i].ucComponentTag = pstPmtInfo->stVideoInfo[i].ucComponentTag;
		pVideoInfo->stVideoInfo[i].ucStreamType = pstPmtInfo->stVideoInfo[i].ucStreamType;
		pVideoInfo->stVideoInfo[i].ucComponentType = pstPmtInfo->stVideoInfo[i].ucComponentType;
		HxSTD_MemCopy(pVideoInfo->stVideoInfo[i].aucStreamName, pstPmtInfo->stVideoInfo[i].aucStreamName, MW_MEDIA_MAX_ES_STRING_LEN);
	}

	VK_SEM_Release(local_Media_GetSem());
	return ERR_OK;
}

HERROR SVC_MP_SI_GetAudioList(Handle_t hAction, MW_Media_AUDIO_InfoList_t *pAudInfo)
{
	MW_MediaSiContext_t *pstSiContext = NULL;
	MW_MEDIA_PmtInfo_t *pstPmtInfo = NULL;
	HUINT32 i;
	HUINT32 len;

	if (pAudInfo == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(local_Media_GetSem());

	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pstSiContext->bPmtValid == FALSE))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstPmtInfo = &pstSiContext->stPmtInfo;
	HxSTD_MemSet(pAudInfo, 0x00, sizeof(MW_Media_AUDIO_InfoList_t));

	pAudInfo->nAudio = pstPmtInfo->usAudioNum;
	if (pAudInfo->nAudio >= MW_MEDIA_MAX_AUDIO_ES_NUM)
	{
		pAudInfo->nAudio = MW_MEDIA_MAX_AUDIO_ES_NUM;
	}

	for (i = 0; i < pAudInfo->nAudio; i++)
	{
		/* Get PID */
		pAudInfo->stAudioInfo[i].usPid = pstPmtInfo->stAudioInfo[i].usPid;
		pAudInfo->stAudioInfo[i].ucComponentTag = pstPmtInfo->stAudioInfo[i].ucComponentTag;
		pAudInfo->stAudioInfo[i].ucStreamType = pstPmtInfo->stAudioInfo[i].ucStreamType;
		pAudInfo->stAudioInfo[i].ucComponentType = pstPmtInfo->stAudioInfo[i].ucComponentType;
		pAudInfo->stAudioInfo[i].ucStreamContent = pstPmtInfo->stAudioInfo[i].ucStreamContent;

		/* Copy Language Code */
		HxSTD_MemCopy(pAudInfo->stAudioInfo[i].stIsoLangInfo, pstPmtInfo->stAudioInfo[i].stIsoLangInfo, sizeof(MW_MEDIA_Iso639LangInfo_t)*MW_MEDIA_MAX_AUD_NUM);

		/* Copy Stream name */
		len = MWC_UTIL_DvbStrlen(pstPmtInfo->stAudioInfo[i].aucStreamName);
		if (len >= MW_MEDIA_MAX_LEN_AUD_NAME)
		{
			len = MW_MEDIA_MAX_LEN_AUD_NAME - 1;
		}
		HxSTD_MemCopy(pAudInfo->stAudioInfo[i].aucStreamName, pstPmtInfo->stAudioInfo[i].aucStreamName, len);
		pAudInfo->stAudioInfo[i].aucStreamName[len] = '\0';

		/* 삭제가 필요하다. => APKS_MEDIADEC_AUDIO_t에서 필요하다. 삭제 하지 말 것 */
		pAudInfo->stAudioInfo[i].eCodec = pstPmtInfo->stAudioInfo[i].eAudCodec;
		pAudInfo->stAudioInfo[i].stIsoLangInfo[0].eAudType = pstPmtInfo->stAudioInfo[i].stIsoLangInfo[0].eAudType;
		if ((pstPmtInfo->stAudioInfo[i].eAudCodec == eDxAUDIO_CODEC_DOLBY_AC3) || (pstPmtInfo->stAudioInfo[i].eAudCodec == eDxAUDIO_CODEC_DOLBY_AC3P))
		{
			pAudInfo->stAudioInfo[i].bIsDolby = TRUE;
		}
		else
		{
			pAudInfo->stAudioInfo[i].bIsDolby = FALSE;
		}
	}

	VK_SEM_Release(local_Media_GetSem());

	HxLOG_Print(" [SVC_MP_SI_GetAudioList] hAction:0x%X, nAudio:%d\n", hAction, pAudInfo->nAudio);
	for (i=0; i<pAudInfo->nAudio; i++)
	{
		HxLOG_Print("	%d) eCodec:%d, usPid:0x%X, lan1=%s lan2=%s\n",
				i, pAudInfo->stAudioInfo[i].eCodec, pAudInfo->stAudioInfo[i].usPid,
				pAudInfo->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode, pAudInfo->stAudioInfo[i].stIsoLangInfo[1].szIso639LangCode);
	}

	return ERR_OK;
}

HERROR SVC_MP_SI_GetCaptionSuperList(Handle_t hAction, MW_Media_DATA_InfoList_t *pCaptionSuperInfo)
{
	MW_MediaSiContext_t	*pstSiContext = NULL;
	MW_MEDIA_PmtInfo_t	*pstPmtInfo = NULL;
	HUINT32				i = 0;

	if (pCaptionSuperInfo == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(local_Media_GetSem());

	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pstSiContext->bPmtValid == FALSE))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pstPmtInfo = &pstSiContext->stPmtInfo;
	HxSTD_MemSet(pCaptionSuperInfo, 0x00, sizeof(MW_Media_DATA_InfoList_t));

	pCaptionSuperInfo->nData = pstPmtInfo->usCaptionNum;
	if (pCaptionSuperInfo->nData >= MW_MEDIA_MAX_CAPTION_ES_NUM)
	{
		pCaptionSuperInfo->nData = MW_MEDIA_MAX_CAPTION_ES_NUM;
	}

	for (i=0; i<pCaptionSuperInfo->nData; i++)
	{
		pCaptionSuperInfo->stDataInfo[i].usPid = pstPmtInfo->stCaptionInfo[i].usPid;
		pCaptionSuperInfo->stDataInfo[i].ucComponentTag = pstPmtInfo->stCaptionInfo[i].ucComponentTag;
		pCaptionSuperInfo->stDataInfo[i].unISO639LanguageCode = pstPmtInfo->stCaptionInfo[i].unISO639LanguageCode;
	}

	VK_SEM_Release(local_Media_GetSem());

	return ERR_OK;
}

void	SVC_MP_SI_FreeExtEvt(MW_MEDIA_ExtEvt_t *pstExtEvt)
{
	HUINT32 i;
	MW_MEDIA_ExtEvt_t *pstOrgEvt= NULL;

	if( pstExtEvt == NULL ) return;

	while( pstExtEvt != NULL )
	{
		if( pstExtEvt->pszChar != NULL )
		{
			HLIB_STD_MemFree( pstExtEvt->pszChar );
			pstExtEvt->pszChar = NULL;
		}

		for( i=0; i<pstExtEvt->ucItemNum; i++ )
		{
			if( pstExtEvt->astItem[i].pszDescription != NULL )
			{
				HLIB_STD_MemFree( pstExtEvt->astItem[i].pszDescription );
				pstExtEvt->astItem[i].pszDescription = NULL;
			}

			if( pstExtEvt->astItem[i].pszItem != NULL )
			{
				HLIB_STD_MemFree( pstExtEvt->astItem[i].pszItem );
				pstExtEvt->astItem[i].pszItem = NULL;
			}
		}

		pstOrgEvt = pstExtEvt;
		pstExtEvt = pstExtEvt->pNext;
		if( pstOrgEvt != NULL )
		{
			HLIB_STD_MemFree( pstOrgEvt );
		}
	}
}

static HERROR local_Media_CopyToExtEvt(MW_MEDIA_ExtEvt_t *pstSrc, MW_MEDIA_ExtEvt_t *pstTarget)
{
	HUINT32 i;
	MW_MEDIA_ExtEvt_t *pstExtEvt= NULL, *pstDesEvt = NULL;
	HUINT32 ulLength = 0;

	if( (pstSrc == NULL) || (pstTarget == NULL ) )
		return ERR_FAIL;

	pstExtEvt = pstSrc;
	pstDesEvt = pstTarget;

	while( pstExtEvt != NULL )
	{
		pstDesEvt->ucDesNum = pstExtEvt->ucDesNum;
		pstDesEvt->ucLastDesNum = pstExtEvt->ucLastDesNum;
		HxSTD_MemCopy( pstDesEvt->szIso639LangCode, pstExtEvt->szIso639LangCode, 4 );
		pstDesEvt->ucItemNum = pstExtEvt->ucItemNum;

		if( pstExtEvt->pszChar != NULL )
		{
			ulLength = MWC_UTIL_DvbStrlen( pstExtEvt->pszChar );
			pstDesEvt->pszChar = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength+1));
			if( pstDesEvt->pszChar != NULL )
			{
				HxSTD_MemCopy( pstDesEvt->pszChar, pstExtEvt->pszChar, ulLength);
				pstDesEvt->pszChar[ulLength] = '\0';
			}
		}

		for( i=0; i<pstExtEvt->ucItemNum; i++ )
		{
			if( pstExtEvt->astItem[i].pszDescription != NULL )
			{
				ulLength = MWC_UTIL_DvbStrlen( pstExtEvt->astItem[i].pszDescription );
				pstDesEvt->astItem[i].pszDescription = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength+1));
				if( pstDesEvt->astItem[i].pszDescription != NULL )
				{
					HxSTD_MemCopy( pstDesEvt->astItem[i].pszDescription, pstExtEvt->astItem[i].pszDescription, ulLength );
					pstDesEvt->astItem[i].pszDescription[ulLength] = '\0';
				}
			}

			if( pstExtEvt->astItem[i].pszItem != NULL )
			{
				ulLength = MWC_UTIL_DvbStrlen( pstExtEvt->astItem[i].pszItem );
				pstDesEvt->astItem[i].pszItem = (HUINT8 *) HLIB_STD_MemCalloc( sizeof(HUINT8) * (ulLength+1));
				if( pstDesEvt->astItem[i].pszItem != NULL )
				{
					HxSTD_MemCopy( pstDesEvt->astItem[i].pszItem, pstExtEvt->astItem[i].pszItem, ulLength );
					pstDesEvt->astItem[i].pszItem[ulLength] = '\0';
				}
			}
		}

		pstExtEvt = pstExtEvt->pNext;
		if( pstExtEvt != NULL )
		{
			pstDesEvt->pNext = (MW_MEDIA_ExtEvt_t *)HLIB_STD_MemCalloc( sizeof(MW_MEDIA_ExtEvt_t) );
			if( pstDesEvt->pNext == NULL )
			{

				HxLOG_Error("HLIB_STD_MemCalloc Error,  pstDesEvt->pNext\n");
				break;
			}

			pstDesEvt = pstDesEvt->pNext;
		}
		else
		{
			pstDesEvt->pNext = NULL;
		}
	}

	return ERR_OK;
}

HERROR SVC_MP_SI_GetEventInfo(Handle_t hAction, MW_MEDIA_EventInfo_t *pstEvtInfo)
{
	HERROR					err = ERR_OK;
	MW_MediaSiContext_t		*pstSiContext = NULL;
	MW_MEDIA_EventInfo_t 	*pstTmpEvtInfo = NULL;
	MW_MEDIA_ExtEvt_t		*pstExtEvt = NULL;

	if (pstEvtInfo == NULL)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(local_Media_GetSem());
	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pstSiContext->bEventValid == FALSE))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstTmpEvtInfo = &pstSiContext->stEvtInfo;
	/* Copy Event Info */
	if (pstTmpEvtInfo->pstExtEvt != NULL)
	{
		pstExtEvt = (MW_MEDIA_ExtEvt_t *)HLIB_STD_MemAlloc(sizeof(MW_MEDIA_ExtEvt_t));
		if (pstExtEvt == NULL)
		{

			HxLOG_Error("HLIB_STD_MemCalloc Error\n");

			VK_SEM_Release(local_Media_GetSem());
			return ERR_FAIL;
		}

		HxSTD_MemSet(pstExtEvt, 0, sizeof(MW_MEDIA_ExtEvt_t));
		local_Media_CopyToExtEvt(pstTmpEvtInfo->pstExtEvt, pstExtEvt);
	}
	HxSTD_MemCopy(pstEvtInfo, pstTmpEvtInfo, sizeof(MW_MEDIA_EventInfo_t));
	pstEvtInfo->pstExtEvt = pstExtEvt;

	VK_SEM_Release(local_Media_GetSem());

	return err;
}

HERROR SVC_MP_SI_FreeEventInfo(MW_MEDIA_EventInfo_t *pstEvtInfo)
{
	MW_MEDIA_ExtEvt_t *pstExtEvt = NULL;

	if (pstEvtInfo == NULL)
	{
		return ERR_FAIL;
	}

	pstExtEvt = pstEvtInfo->pstExtEvt;
	SVC_MP_SI_FreeExtEvt(pstExtEvt);

	return ERR_OK;
}

/* SIT에 의해서 Stereo Type이 결정되는데 SIT가 늦게 도착하는 경우에는
   Dualmono stream에도 불구하고 Audio Change를 할 수 없는 이슈를 해결
   하기 위해 DI Audio에서 파악된 stereo type으로 SI 정보를 변경 한다. */
HERROR SVC_MP_SI_UpdateAudioStereoInfoByStereoType(Handle_t	hAction, DxStereoFormat_e eStereoType, HBOOL *pbChangeFlag)
{
	MW_MediaSiContext_t	*pstSiContext = NULL;
	MW_MEDIA_PmtInfo_t 	*pstPmtInfo = NULL;

	VK_SEM_Get(local_Media_GetSem());
	pstSiContext = local_Media_GetSiContext();
	if ((pstSiContext == NULL) || (pbChangeFlag == NULL) || (pstSiContext->hAction != hAction))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	*pbChangeFlag = FALSE;
	pstPmtInfo = &pstSiContext->stPmtInfo;
	if ((pstSiContext->hAction != hAction) || (pstSiContext->bPmtValid == FALSE))
	{
		VK_SEM_Release(local_Media_GetSem());
		HxLOG_Print("[%s:%d] bPmtValid=%d [%#x,%#x]\n",__FUNCTION__, __LINE__, pstSiContext->bPmtValid, pstSiContext->hAction, hAction);
		return ERR_FAIL;
	}

	switch (eStereoType)
	{
		case eDxSTEREO_FORMAT_MONO:
		{
			/*
			Stream_content 	Component_type  Description
			0x02   			0x01         	Audio, 1/0 mode (single mono)
			*/
			if ((pstPmtInfo->stAudioInfo[0].ucStreamContent != 0x02) || (pstPmtInfo->stAudioInfo[0].ucComponentType != 0x01))
			{
				HxLOG_Warning("[%s:%d] This is mono stream. but SI information has [%#x,%#x]. \n",
						__FUNCTION__, __LINE__, pstPmtInfo->stAudioInfo[0].ucStreamContent, pstPmtInfo->stAudioInfo[0].ucComponentType);

				*pbChangeFlag = TRUE;
				/* Set Mono Audio Info */
				pstPmtInfo->stAudioInfo[0].ucStreamContent = 0x02;
				pstPmtInfo->stAudioInfo[0].ucComponentType = 0x01;
			}
		}
			break;

		case eDxSTEREO_FORMAT_DUALMONO:
		{
			/*
			Stream_content 	Component_type  Description
			0x02   			0x02         	Audio, 1/0+1/0 mode (dual mono)
			*/
			if ((pstPmtInfo->stAudioInfo[0].ucStreamContent != 0x02) || (pstPmtInfo->stAudioInfo[0].ucComponentType != 0x02))
			{
				HxLOG_Warning("[%s:%d] This is dualmono stream. but SI information has [%#x,%#x]. \n",
						__FUNCTION__, __LINE__, pstPmtInfo->stAudioInfo[0].ucStreamContent, pstPmtInfo->stAudioInfo[0].ucComponentType);

				*pbChangeFlag = TRUE;
				/* Set Dual Mono Audio Info */
				pstPmtInfo->stAudioInfo[0].ucStreamContent = 0x02;
				pstPmtInfo->stAudioInfo[0].ucComponentType = 0x02;
			}
		}
			break;

		default:
			break;
	}

	VK_SEM_Release(local_Media_GetSem());

	return ERR_OK;
}

