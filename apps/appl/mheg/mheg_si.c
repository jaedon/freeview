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
#include <stdio.h>
#include <string.h>
#include <hlib.h>
#include <silib.h>
#include <dlib.h>
#include <apk.h>
#include <vkernel.h>

#include "mheg_sef.h"
#include "mheg_si.h"
#include "mheg_rawmgr.h"
#include "mheg_param.h"
#include "mheg_int.h"

#include "port_itk_main.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define GET_SECTION_LEN_FROM_MHEG_RAWDATA(data)	(((data[1] << 8) | data[2]) & 0x0FFF)
#define GET_TABLE_ID_FROM_MHEG_RAWDATA(data)			(data[0])
#define GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(data)	((data[3] << 8) | data[4])

#define MHEG_FILTER_LIMIT						(10)
#define MHEG_FILTER_ID_NULL						0xFFFFFFFF
#define MHEG_MAX_SECTION_NUM					1			// MHEG은 multi-Section을 지원하지 않는다. max - 256


#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
#define MHEG_SI_AUDIO_CHANNEL_NUM_NA				0x00
#define MHEG_SI_AUDIO_CHANNEL_NUM_STEREO			0x01
#define MHEG_SI_AUDIO_CHANNEL_NUM_MULTI			0x02
#define	MHEG_Util_AAC_ProfileLevelToChannelNum(a)		(((a>=0x50) && (a<=0x59)) ? MHEG_SI_AUDIO_CHANNEL_NUM_STEREO : (((a>=0x5A) && (a<=0x5B)) ? MHEG_SI_AUDIO_CHANNEL_NUM_MULTI : MHEG_SI_AUDIO_CHANNEL_NUM_NA))
#endif

#define	NUM_MAX_ACTION 1

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eMHEG_SI_ContextStatus_None = 0,
	eMHEG_SI_ContextStatus_Running,
	eMHEG_SI_ContextStatus_Waiting
} MHEG_SI_ContextStatus_t;

typedef struct tagMHEG_SI_PmtStatus
{
	HUINT32		ulPmtFilterId;
	HUINT16		usProgMapPid;
	HUINT8		ucStatus;
	HUINT8		ucMode;
} MHEG_SI_SrchPmtStatus_t, MHEG_SI_LivePmtStatus_t;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
typedef struct tagMHEG_SI_ISO639Lang
{
	AUDIO_Type_t	eAudType;
	HUINT8			szIso639LangCode[4];
} MHEG_SI_ISO639LANG_t;

/** @ AUDIO infomation in PMT */
typedef struct tagMHEG_SI_IP_Streaming_Audio
{
	HUINT16					usPid;						/** < Auido PID */
	DxAudioCodec_e			eAudCodec;					/** < Audio Codec */
	MHEG_SI_ISO639LANG_t	stIsoLangInfo[4];
	HUINT8					ucComponentTag;				/** < componet tag value */
	HUINT8					ucChannelNum;
} MHEG_SI_IP_STREAMING_AUDIO_t;

/** @ Subtitle infomation in PMT */
typedef struct tagMHEG_SI_IP_Streaming_Subtitle
{
	HUINT16		usPid;						/** < Subtitle PID */
	HUINT8		ucType;						/** < Teletext: 0x00~0x05, Subtitle: 0x10~0x13, 0x20~0x23 */
	HUINT16		usCompositionPageId;		/** < for DVB Subtitle */
	HUINT16		usAncillaryPageId;			/** < for DVB Subtitle */
	HUINT8		aucLangCode [4];
} MHEG_SI_IP_STREAMING_SUBTITLE_t;
#endif

typedef struct  tagMHEG_SI_Context
{
	// Action의 정보.
	HUINT32 				ulViewId;									/* Input Action Handle */
	HUINT32 				ulCurSvcUid;
	HUINT32					ulRequestSvcUid;
	HUINT32					ulPatFilterId;
	HUINT32					ulLivePatFilterId;
	HUINT32					ulLimitDmxNum;

	HUINT16 				usOrgNetId;
	HUINT16 				usTsId;
	HUINT16					usUniqueId;
	HUINT16					usPmtTotalNum;

	MHEG_SI_SrchPmtStatus_t		*pstPmtStatus;
	MHEG_SI_LivePmtStatus_t		stLivePmtStatus;
	MHEG_SI_ContextStatus_t	eStatus;
	HUINT8					 aucSectionStatus[MAX_SI_TABLE_TYPE];

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HUINT16							usAudioNum;								/** < number of AUDIO */
	MHEG_SI_IP_STREAMING_AUDIO_t	stAudioInfo [MAX_AUDIO_ES_NUM];			/** < AUDIO contents */

	HUINT16							usSubttlNum;							/** < number of Subtitle */
	MHEG_SI_IP_STREAMING_SUBTITLE_t	stSubttlInfo [MAX_SUBTTL_ES_NUM];	/** < Subtitle contents */
#endif
}MHEG_SI_Context_t;

/** @brief MHEG SI action result */
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG				s_ulMHEGSIQId = 0;
STATIC HULONG				s_ulMHEGSITaskId = 0xFFFFFFFF;
STATIC MHEG_SI_Context_t	s_stMHEGSiContext[NUM_MAX_ACTION];
STATIC HULONG				s_ulMhegPmtStatusSemId;
STATIC HUINT32				s_ulMHEGAction;
STATIC Handle_t				s_ulMHEGCurSvcUid = 0;
STATIC HUINT32				s_ulMHEGTryCount = 0;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
STATIC MHEG_SI_Context_t	s_stMHEGStreamingSiContext[NUM_MAX_ACTION];
STATIC HUINT32				s_ulMHEGStreamingAction;
STATIC HUINT32				s_ulMHEGStreamingTempUniqueId = 1001;
STATIC HUINT32				s_ulMHEGStreamingCurSvcUid = 0;
STATIC HUINT32				s_ulMHEGStreamingTryCount = 0;
STATIC HINT32				s_nMHEGStreramingDemuxId = 0;
STATIC HBOOL				s_bMHEGStreamingMode = FALSE;
#endif

#define	ENTER_CRITICAL_PMTSTATUS		VK_SEM_Get(s_ulMhegPmtStatusSemId)
#define	LEAVE_CRITICAL_PMTSTATUS		VK_SEM_Release(s_ulMhegPmtStatusSemId)

const unsigned long	s_Crc32Tbl[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
  */

#define	___1__LOCAL_FUNCTIONS____

static HERROR local_MHEG_SI_SendSectionMessage(MHEG_Command_t eMsgType, Handle_t hAction, HUINT32 ulTableStatus, HUINT32 ulFilterId,HUINT16 usCheckId);
static	void local_MHEG_ParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
static	void local_MHEG_FreePat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
static	void local_MHEG_ParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
static	void local_MHEG_FreePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
static	void local_MHEG_ParseLivePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId);
static	void local_MHEG_FreeLivePat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);
static	void local_MHEG_ParseLivePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId);
static	void local_MHEG_FreeLivePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2);


#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static	void local_MHEG_Streaming_ParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, Handle_t hAction);
static	void local_MHEG_Streaming_ParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId);

#endif

#define	_______1_1_MHEG_CONTEXT___

static unsigned long GetCrc32(void *inP, unsigned long n)
{
	register unsigned long	crc, *tbl;
	unsigned char		*p = (unsigned char*)inP;

	crc = -1;
	tbl = (unsigned long*)s_Crc32Tbl;
	while (n--)
	{
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *p++) & 0xff];
	}
	return crc;
} /* GetCrc32 */


static INLINE MHEG_SI_Context_t * local_MHEG_SI_GetContext (HUINT32 ulViewId)
{
	return &s_stMHEGSiContext[ulViewId];
}

#if 0
static INLINE void  local_MHEG_si_SetActionHandle(MHEG_SI_Context_t *pContext, Handle_t hAction)
{
	pContext->hAction	= hAction;
}
#endif

static INLINE void local_MHEG_si_SetContextStatus(MHEG_SI_Context_t *pContext, MHEG_SI_ContextStatus_t eStatus)
{
	pContext->eStatus 	= eStatus;
}

static INLINE MHEG_SI_ContextStatus_t local_MHEG_si_GetContextStatus(MHEG_SI_Context_t *pContext)
{
    return pContext->eStatus;
}

static INLINE void local_MHEG_si_GetTableIdAndResult(HUINT32 value, HUINT16* TableId, HUINT16 *result)
{
	*result = value & 0x0000ffff;
	*TableId = (value & 0xffff0000) >> 16;
}

static INLINE void local_MHEG_si_GetFilterAndTsUniqueId(HUINT32 value, HUINT16 *filterId, HUINT16 *tsUniqId)
{
	*tsUniqId = value & 0x0000ffff;
	*filterId = (value & 0xffff0000) >> 16;
}

static HERROR local_MHEG_si_InitContext (MHEG_SI_Context_t *pContext, HMSG *pstMsg)
{
	DxTransponder_t	*pstTsInfo = NULL;
	DxService_t 		*pstSvcInfo = NULL;
	HUINT32			ulViewId;
	HUINT16			usTsUniqueId;
	HINT32			tsuid;
	HUINT32			SvcUid;

	if(pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \n");
		return ERR_FAIL;
	}

	ulViewId		= pstMsg->ulParam;
	SvcUid		= pstMsg->unInfo.aulL[0];
	usTsUniqueId	= pstMsg->usParam;

	HxLOG_Debug("ulViewId(0x%x), SvcUid(0x%x), usTsUniqueId(0x%x) \r\n", ulViewId, SvcUid, usTsUniqueId);

	// Session Version Check.
	//nResult = MWC_RM_CheckIfLatestActionHandle(hAction);
	//if(nResult != ERR_OK)
	//{
	//	HxLOG_Error("Version\r\n");
	//	return ERR_FAIL;
	//}

	if(pContext->pstPmtStatus)
		HLIB_STD_MemFree(pContext->pstPmtStatus);

	HxSTD_memset(pContext,0x0,sizeof(MHEG_SI_Context_t));

	pContext->pstPmtStatus = NULL;
	pContext->ulCurSvcUid 	= SvcUid;
	pContext->ulViewId 	= ulViewId;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (s_bMHEGStreamingMode == TRUE)
	{
		pContext->usUniqueId = usTsUniqueId;
	}
	else
#endif
	{
		// 1. Get Svc Info
		pstSvcInfo = APK_META_SVC_GetService(SvcUid);
		if(pstSvcInfo == NULL)
		{
			HxLOG_Error("Service not found:\n");
			return ERR_FAIL;
		}
		tsuid = pstSvcInfo->tsuid;
		APK_META_SVC_Delete(pstSvcInfo);

		// 1. Get Ts Info
		pstTsInfo = APK_META_SVC_GetTransponder(tsuid);
		if(pstTsInfo == NULL)
		{
			HxLOG_Error("Transponder not found:\n");
			return ERR_FAIL;
		}

		pContext->usOrgNetId	= pstTsInfo->onid;
		pContext->usTsId		= pstTsInfo->tsid;
		pContext->usUniqueId	= pstTsInfo->uid;
		APK_META_SVC_Delete(pstTsInfo);
	}

	pContext->ulLimitDmxNum	=0;

	pContext->ulPatFilterId = MHEG_FILTER_ID_NULL;

	if(pContext->usUniqueId != usTsUniqueId)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	return ERR_OK;
}



#define	_______1_2_MHEG_FILTERING_FOR_TABLE___


HERROR local_MHEG_StartPatFilter(HUINT32 ulViewId)
{
	HUINT16 				usFilterId = 0;
	HUINT16 				usUniqueId = 0;
	HINT32 				unDmxId = -1;
	HERROR				hErr;
	SI_RETRIEVE_PARAM 	stFilterParam;
	MHEG_SI_Context_t 	*pContext = NULL;

	HxLOG_Debug("===== (+)\n");
	//printf("\033[30;43m[%s : %d] MHEG - START PAT Filter : %d  \033[0m\n", __FUNCTION__, __LINE__, ulViewId);

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL (ulViewId : %d) \n", ulViewId);
		return ERR_FAIL;
	}

	HxSTD_memset(&stFilterParam, 0x00, sizeof(SI_RETRIEVE_PARAM));

	unDmxId = PORT_ITK_GetDemuxId(ulViewId);
	usUniqueId = pContext->usUniqueId;

	hErr = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, unDmxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MHEG_SEF_SetDefaultRetrieveParam Error \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("ulViewId (%d), DemuxId(%d), UniqueId(0x%x) \r\n", ulViewId, unDmxId, usUniqueId);

	stFilterParam.ulViewId = ulViewId;
	stFilterParam.usTsUniqId = usUniqueId;
	stFilterParam.ucDemuxId = unDmxId;
	stFilterParam.usPid = PAT_PID;
	stFilterParam.ucMode = eSiFilterMode_Once;
	stFilterParam.bTableFilter = TRUE;
	stFilterParam.bUseExtId = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.bCrc = TRUE;

	hErr = MHEG_SEF_RetrievePatSection(&stFilterParam, local_MHEG_ParsePat, NULL/* local_MHEG_FreePat*/, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MWC_SIDMX_RetrievePatSection return Error!!!!(0x%X) - [hAction:0x%X][DmxId:%d][UniqueId:0x%X]\n",hErr, ulViewId, unDmxId, usUniqueId);
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		pContext->ulPatFilterId = MHEG_FILTER_ID_NULL;
		HxLOG_Error("usFilterId Error (usFilterId : 0x%x ) \n", usFilterId);
		return ERR_FAIL;
	}
	HxLOG_Debug("usFilterId (%d)\r\n", usFilterId);

	pContext->ulPatFilterId = (HUINT32)usFilterId;

	return ERR_OK;
}

HERROR local_MHEG_StopPatFilter(HUINT32 ulViewId)
{
	MHEG_SI_Context_t *pContext = NULL;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL\n");
		return ERR_FAIL;
	}

	if(pContext->ulPatFilterId == MHEG_FILTER_ID_NULL)
	{
		HxLOG_Debug("pContext->ulPatFilterId is Null (0x%x) \r\n", pContext->ulPatFilterId);
		return ERR_OK;
	}

	// Msg Send to SiThd
	MHEG_SEF_StopSecFilter(pContext->ulPatFilterId);

	pContext->ulPatFilterId = MHEG_FILTER_ID_NULL;

	return ERR_OK;
}

HERROR local_MHEG_StartPmtFilter(HUINT32 ulViewId, HUINT16 usTsUniqId, HUINT16 usMode, HUINT16 usPmtPid, HUINT16 usExtId,HUINT32 ulProgIdx )
{
	HUINT16 usFilterId = 0;
	HUINT16 usUniqueId = 0;
	HINT32 	unDmxId = -1;
	HERROR	err;

	SI_RETRIEVE_PARAM stFilterParam;
	MHEG_SI_Context_t *pContext = NULL;

	HxLOG_Debug(" \r\n");

	//printf("\033[30;43m[%s : %d] local_MHEG_StartPmtFilter [pid : 0x%x]  \033[0m\n", __FUNCTION__, __LINE__, usPmtPid);

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return ERR_FAIL;
	}

	//err = MWC_RM_GetResourceId(MWC_RM_GetActionId(hAction), eRmItem_DEMUX, &unDmxId);
	//if(err != ERR_OK)
	//{
	//	HxLOG_Error("\n MHEG SI Error");
	//}
	unDmxId = PORT_ITK_GetDemuxId(ulViewId);

	usUniqueId = usTsUniqId;

	//HxLOG_Debug("hAction(%d), DemuxId(%d), UniqueId(0x%x), usExtId(%d), usPmtPid(%d)\r\n", hAction, unDmxId, usUniqueId, usExtId, usPmtPid);

	err = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, unDmxId);
	if(err != ERR_OK)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	stFilterParam.ucMode = usMode;
	stFilterParam.bTableFilter = TRUE;
	stFilterParam.bUseExtId = TRUE;
	stFilterParam.usExtId = usExtId;
	stFilterParam.bCrc = TRUE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.usPid = usPmtPid;

	err = MHEG_SEF_RetrievePmtSection(&stFilterParam, local_MHEG_ParsePmt, NULL/*local_MHEG_FreePmt*/, &usFilterId);
	if(err != ERR_OK)
	{
		HxLOG_Error("MWC_SIDMX_RetrievePmtSection return Error!!!!(0x%X) - [ulViewId:0x%X][DmxId:%d][UniqueId:0x%X][SvcId:0x%X][PmtPid:0x%X]\n",err, ulViewId, unDmxId, usUniqueId, usExtId, usPmtPid);
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		if(pContext->pstPmtStatus && (pContext->usPmtTotalNum > ulProgIdx))
			pContext->pstPmtStatus[ulProgIdx].ulPmtFilterId = MHEG_FILTER_ID_NULL;

		HxLOG_Error("\n MHEG SI Error");
		return ERR_FAIL;
	}

	if(pContext->pstPmtStatus && (pContext->usPmtTotalNum > ulProgIdx))
		pContext->pstPmtStatus[ulProgIdx].ulPmtFilterId = (HUINT32)usFilterId;

	pContext->ulLimitDmxNum++;

	return ERR_OK;
}

HERROR local_MHEG_StopPmtFilter(HUINT32 ulViewId,HBOOL hAllStopFlag,HUINT32	ulFilterId)
{
	MHEG_SI_Context_t 	*pContext = NULL;
	HUINT32 				ulFilterCnt;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if ( pContext == NULL )
	{
		HxLOG_Error("pContext is NULL\n");
		return ERR_FAIL;
	}

	if(pContext->pstPmtStatus == NULL)
	{
		HxLOG_Error("pContext->pstPmtStatus is NULL\n");
		return ERR_FAIL;
	}

	for(ulFilterCnt=0; ulFilterCnt< pContext->usPmtTotalNum; ulFilterCnt++)
	{
		if(hAllStopFlag || ((hAllStopFlag == FALSE) && (pContext->pstPmtStatus[ulFilterCnt].ulPmtFilterId  == ulFilterId)))
		{
			if(pContext->pstPmtStatus[ulFilterCnt].ulPmtFilterId != MHEG_FILTER_ID_NULL)
			{
				HxLOG_Debug("STOP Search PMT filter...ulViewId : %d, ulFilterId : %d \r\n", ulViewId,pContext->pstPmtStatus[ulFilterCnt].ulPmtFilterId);

				MHEG_SEF_StopSecFilter(pContext->pstPmtStatus[ulFilterCnt].ulPmtFilterId );

				if(pContext->ulLimitDmxNum > 0)
					pContext->ulLimitDmxNum--;

				pContext->pstPmtStatus[ulFilterCnt].ulPmtFilterId  = MHEG_FILTER_ID_NULL;
			}
		}
	}

	return ERR_OK;
}

HERROR local_MHEG_StartLivePatFilter(HUINT32 ulViewId)
{
	HERROR				hErr;
	HUINT16 				usFilterId = 0, usUniqueId = 0;
	HUINT32 				ulDmxId = 0;
	SI_RETRIEVE_PARAM 	stFilterParam;
	MHEG_SI_Context_t 	*pContext = NULL;

	HxLOG_Debug("===== (+) \n");

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL\r\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	// CMD_MHEG_SI_START_LIVE_SECTION 호출할 때 초기화 하고 들어옴..
#if 0
	if (MHEG_FILTER_ID_NULL != pContext->ulLivePatFilterId)
	{
		MHEG_SEF_StopSecFilter (pContext->ulLivePatFilterId);
		pContext->ulLivePatFilterId = MHEG_FILTER_ID_NULL;
	}
#endif

	HxSTD_memset(&stFilterParam, 0x00, sizeof(SI_RETRIEVE_PARAM));

	ulDmxId = PORT_ITK_GetDemuxId(ulViewId);
	usUniqueId = pContext->usUniqueId;

	hErr = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, ulDmxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MHEG_SEF_SetDefaultRetrieveParam Error \n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("ulViewId (%d), DemuxId(%d), UniqueId(0x%x) \r\n", ulViewId, ulDmxId, usUniqueId);

	stFilterParam.ulViewId 		= ulViewId;
	stFilterParam.usTsUniqId 	= usUniqueId;
	stFilterParam.ucDemuxId 	= (HUINT8)ulDmxId;
	stFilterParam.usPid 			= PAT_PID;
	stFilterParam.ucMode 		= eSiFilterMode_Monitor;
	stFilterParam.bTableFilter 	= TRUE;
	stFilterParam.bUseExtId 		= FALSE;
	stFilterParam.bUseVerNum 	= FALSE;
	stFilterParam.bCrc 			= TRUE;

	hErr = MHEG_SEF_RetrievePatSection(&stFilterParam, local_MHEG_ParseLivePat, NULL/* local_MHEG_FreePat*/, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MWC_SIDMX_RetrievePatSection return Error!!!!(0x%X) \n",hErr);
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		pContext->ulLivePatFilterId = MHEG_FILTER_ID_NULL;

		HxLOG_Error("usFilterId Error (usFilterId : 0x%x) \n", usFilterId);
		return ERR_FAIL;
	}

	pContext->ulLivePatFilterId = (HUINT32)usFilterId;

	HxLOG_Debug("pContext->ulLivePatFilterId (%d)\r\n", pContext->ulLivePatFilterId);
	HxLOG_Debug("===== (-) \n");

	return ERR_OK;
}

HERROR local_MHEG_StopLivePatFilter(HUINT32 ulViewId)
{
	MHEG_SI_Context_t *pContext = NULL;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \n");
		return ERR_FAIL;
	}

	if(pContext->ulLivePatFilterId == MHEG_FILTER_ID_NULL)
	{
		HxLOG_Error("pContext->ulLivePatFilterId is Error (%d) \r\n", pContext->ulLivePatFilterId);
		return ERR_OK;
	}

	// Msg Send to SiThd
	MHEG_SEF_StopSecFilter(pContext->ulLivePatFilterId);

	pContext->ulLivePatFilterId = MHEG_FILTER_ID_NULL;

	return ERR_OK;
}



HERROR local_MHEG_StartLivePmtFilter(HUINT32 ulViewId, HUINT16 usTsUniqId, HUINT16 usMode, HUINT16 usPmtPid, HUINT16 usExtId)
{
	HUINT16	usFilterId = 0;
	HUINT32 	ulDmxId = 0;
	HERROR	hErr;
	SI_RETRIEVE_PARAM	stFilterParam;
	MHEG_SI_Context_t	*pContext = NULL;

	HxLOG_Debug("=====(+) \n");

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL\n");
		HxLOG_Debug("=====(-) \n");
		return ERR_FAIL;
	}

	if (MHEG_FILTER_ID_NULL != pContext->stLivePmtStatus.ulPmtFilterId)
	{
		MHEG_SEF_StopSecFilter (pContext->stLivePmtStatus.ulPmtFilterId);
		pContext->stLivePmtStatus.ulPmtFilterId = MHEG_FILTER_ID_NULL;
	}

	ulDmxId = PORT_ITK_GetDemuxId(ulViewId);
	pContext->stLivePmtStatus.usProgMapPid	= usPmtPid;
	pContext->stLivePmtStatus.ucMode		= usMode;

	HxLOG_Debug("ulViewId(%d), ulDmxId(%d), usTsUniqId(0x%x), usExtId(%d), usPmtPid(0x%x)\r\n", ulViewId, ulDmxId, usTsUniqId, usExtId, usPmtPid);

	hErr = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usTsUniqId, ulDmxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MHEG_SEF_SetDefaultRetrieveParam Error \n");
		HxLOG_Debug("=====(-) \n");
		return ERR_FAIL;
	}

	stFilterParam.ucMode		= usMode;
	stFilterParam.bTableFilter	= TRUE;
	stFilterParam.bUseExtId		= TRUE;
	stFilterParam.usExtId		= usExtId;
	stFilterParam.bCrc			= TRUE;
	stFilterParam.bUseVerNum	= FALSE;
	stFilterParam.usPid			= usPmtPid;

	hErr = MHEG_SEF_RetrievePmtSection(&stFilterParam, local_MHEG_ParseLivePmt, NULL/*local_MHEG_FreePmt*/, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MWC_SIDMX_RetrirebootevePmtSection return Error!!!!(0x%X) \n",hErr);
		HxLOG_Debug("=====(-) \n");
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		pContext->stLivePmtStatus.ulPmtFilterId = MHEG_FILTER_ID_NULL;
		pContext->stLivePmtStatus.ucStatus	= eSI_TABLE_TIMEOUT;
		return ERR_FAIL;
	}

	pContext->stLivePmtStatus.ulPmtFilterId = (HUINT32)usFilterId;
	pContext->stLivePmtStatus.ucStatus		= eSI_TABLE_WAITING;

	pContext->ulLimitDmxNum++;

	HxLOG_Debug("ulPmtFilterId(0x%x) pContext->ulLimitDmxNum(%d) \n", usFilterId, pContext->ulLimitDmxNum);
	HxLOG_Debug("=====(-) \n");

	return ERR_OK;
}

HERROR local_MHEG_StopLivePmtFilter(HUINT32 ulViewId)
{
	MHEG_SI_Context_t *pContext = NULL;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \n");
		return ERR_FAIL;
	}

	if(pContext->stLivePmtStatus.ulPmtFilterId == MHEG_FILTER_ID_NULL)
	{
		HxLOG_Error("pContext->stLivePmtStatus.ulPmtFilterId is MHEG_FILTER_ID_NULL \n");
		return ERR_FAIL;
	}

	MHEG_SEF_StopSecFilter(pContext->stLivePmtStatus.ulPmtFilterId);

	return ERR_OK;
}

HERROR local_MHEG_StopMonitorPmtFilter(HUINT32 ulViewId)
{
	MHEG_SI_Context_t *pContext = NULL;
	int	filterCnt;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(pContext->pstPmtStatus == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	for(filterCnt=0; filterCnt< pContext->usPmtTotalNum; filterCnt++)
	{
		if(pContext->pstPmtStatus[filterCnt].ucMode  == eSiFilterMode_Continue)
		{
			if(pContext->pstPmtStatus[filterCnt].ulPmtFilterId != MHEG_FILTER_ID_NULL)
			{
				HxLOG_Debug("STOP PMT filter...ulViewId : %d, ulFilterId : %d \r\n", ulViewId,pContext->pstPmtStatus[filterCnt].ulPmtFilterId);

				MHEG_SEF_StopSecFilter(pContext->pstPmtStatus[filterCnt].ulPmtFilterId );

				if(pContext->ulLimitDmxNum > 0)
					pContext->ulLimitDmxNum--;
				pContext->pstPmtStatus[filterCnt].ulPmtFilterId  = MHEG_FILTER_ID_NULL;
			}
		}
	}

	return ERR_OK;
}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static INLINE MHEG_SI_Context_t * local_MHEG_Streaming_SI_GetContext (HUINT32 ulViewId)
{
	return &s_stMHEGStreamingSiContext[ulViewId];
}

HERROR local_MHEG_Streaming_StartPatFilter(HUINT32 ulViewId)
{
	HERROR	hErr = 0;
	HUINT16	usFilterId = 0;
	HUINT16	usUniqueId = 0;
	HINT32 	nDmxId = -1;

	SI_RETRIEVE_PARAM stFilterParam;
	MHEG_SI_Context_t *pContext = NULL;

	HxLOG_Debug("START\r\n");

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return ERR_FAIL;
	}

	nDmxId = s_nMHEGStreramingDemuxId;
	usUniqueId = pContext->usUniqueId;

	HxLOG_Debug("ulViewId (%d), DemuxId(%d), UniqueId(0x%x) \r\n", ulViewId, nDmxId, usUniqueId);

	hErr = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, nDmxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	stFilterParam.usPid = PAT_PID;
	stFilterParam.ucMode = eSiFilterMode_Once;
	stFilterParam.bTableFilter = TRUE;
	stFilterParam.bUseExtId = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.bCrc = TRUE;

	hErr = MHEG_SEF_RetrievePatSection(&stFilterParam, local_MHEG_Streaming_ParsePat, NULL/* local_MHEG_FreePat*/, &usFilterId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("MWC_SIDMX_RetrievePatSection return Error!!!!(0x%X) - [hAction:0x%X][DmxId:%d][UniqueId:0x%X]\n",hErr, ulViewId, nDmxId, usUniqueId);
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		pContext->ulPatFilterId = MHEG_FILTER_ID_NULL;
		HxLOG_Error("\n MHEG SI Error");
		return ERR_FAIL;
	}
	HxLOG_Debug("usFilterId (%d)\r\n", usFilterId);

	pContext->ulPatFilterId = (HUINT32)usFilterId;

	return ERR_OK;
}

HERROR local_MHEG_Streaming_StopPatFilter(HUINT32 ulViewId)
{
	MHEG_SI_Context_t *pContext = NULL;

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(pContext->ulPatFilterId == MHEG_FILTER_ID_NULL)
	{
		HxLOG_Debug("FilterId(%d) \r\n", pContext->ulPatFilterId);
		return ERR_OK;
	}

	// Msg Send to SiThd
	MHEG_SEF_StopSecFilter(pContext->ulPatFilterId);

	pContext->ulPatFilterId = MHEG_FILTER_ID_NULL;

	return ERR_OK;
}

HERROR local_MHEG_Streaming_StartPmtFilter(HUINT32 ulViewId, HUINT16 usTsUniqId, HUINT16 usMode, HUINT16 usPmtPid, HUINT16 usExtId,HUINT32 ulProgIdx )
{
//	HUINT32 nResult = 0;
	HUINT16 usFilterId = 0;
	HUINT16 usUniqueId = 0;
	HINT32 	unDmxId = -1;
	HERROR	err;
	SI_RETRIEVE_PARAM stFilterParam;
	MHEG_SI_Context_t *pContext = NULL;

	HxLOG_Debug(" \r\n");

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return ERR_FAIL;
	}

	//err = MWC_RM_GetResourceId(MWC_RM_GetActionId(hAction), eRmItem_DEMUX, &unDmxId);
	//if(err != ERR_OK)
	//{
	//	HxLOG_Error("\n MHEG SI Error");
	//}

	usUniqueId = usTsUniqId;

	HxLOG_Debug("ulViewId(%d), DemuxId(%d), UniqueId(0x%x), usExtId(%d), usPmtPid(%d)\r\n", ulViewId, unDmxId, usUniqueId, usExtId, usPmtPid);

	err = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, unDmxId);
	if(err != ERR_OK)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	stFilterParam.ucMode = usMode;
	stFilterParam.bTableFilter = TRUE;
	stFilterParam.bUseExtId = TRUE;
	stFilterParam.usExtId = usExtId;
	stFilterParam.bCrc = TRUE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.usPid = usPmtPid;

	err = MHEG_SEF_RetrievePmtSection(&stFilterParam, local_MHEG_Streaming_ParsePmt, NULL/*local_MHEG_FreePmt*/, &usFilterId);
	if(err != ERR_OK)
	{

		HxLOG_Error("MWC_SIDMX_RetrievePmtSection return Error!!!!(0x%X) - [hAction:0x%X][DmxId:%d][UniqueId:0x%X][SvcId:0x%X][PmtPid:0x%X]\n",err, ulViewId, unDmxId, usUniqueId, usExtId, usPmtPid);
		return ERR_FAIL;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		if(pContext->pstPmtStatus && (pContext->usPmtTotalNum > ulProgIdx))
			pContext->pstPmtStatus[ulProgIdx].ulPmtFilterId = MHEG_FILTER_ID_NULL;
		HxLOG_Error("\n MHEG SI Error");
		return ERR_FAIL;
	}

	if(pContext->pstPmtStatus && (pContext->usPmtTotalNum > ulProgIdx))
		pContext->pstPmtStatus[ulProgIdx].ulPmtFilterId = (HUINT32)usFilterId;

	pContext->ulLimitDmxNum++;

	return ERR_OK;
}

HERROR local_MHEG_Streaming_StopPmtFilter(HUINT32 ulViewId, HBOOL hAllStopFlag, HUINT32	ulFilterId)
{
	MHEG_SI_Context_t *pContext = NULL;
	int	filterCnt;

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if(pContext->pstPmtStatus == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	for(filterCnt=0; filterCnt< pContext->usPmtTotalNum; filterCnt++)
	{
		if(hAllStopFlag || ((hAllStopFlag == FALSE) && (pContext->pstPmtStatus[filterCnt].ulPmtFilterId  == ulFilterId)))
		{
			if(pContext->pstPmtStatus[filterCnt].ulPmtFilterId != MHEG_FILTER_ID_NULL)
			{
				HxLOG_Debug("STOP PMT filter...ulViewId : %d, ulFilterId : %d \r\n", ulViewId,pContext->pstPmtStatus[filterCnt].ulPmtFilterId);

				MHEG_SEF_StopSecFilter(pContext->pstPmtStatus[filterCnt].ulPmtFilterId );

				if(pContext->ulLimitDmxNum > 0)
					pContext->ulLimitDmxNum--;
				pContext->pstPmtStatus[filterCnt].ulPmtFilterId  = MHEG_FILTER_ID_NULL;
			}
		}
	}

	return ERR_OK;
}

#endif

#define	_______1_3_TABLE_PARSING_FOR_MHEG___

static HERROR local_MHEG_SI_SendSectionMessage (MHEG_Command_t eMsgType, HUINT32 ulViewId, HUINT32 ulTableStatus, HUINT32 ulFilterId,HUINT16 usCheckId)
{
	HMSG		stMsg;

	HxSTD_memset(&stMsg, 0, sizeof(HMSG));

	stMsg.usMsgClass 		= (HUINT32)eMsgType;
	stMsg.ulParam			= ulViewId;
	stMsg.usParam			= usCheckId;
	stMsg.unInfo.aulL[0]	= ulFilterId;
	stMsg.unInfo.aulL[1]	= ulTableStatus;

	return (HERROR)VK_MSG_Send (s_ulMHEGSIQId, &stMsg, sizeof(HMSG));
}

HUINT8 *local_MHEG_MakeMultipleSecToOneSec(MhegSef_RawData_t *pData)
{
	HUINT32		i,ulCrc;
	HUINT8		*pSec;
	HUINT8		*pMakedSec = NULL;
	HUINT16		usTmpVale, usProgramCnt, usTmpSecLen, PrIdx, usProgNum, usPid;

	if (pData == NULL)
		return NULL;
	if (pData->num_sec <= 1)
		return NULL;

	// maximum section number is EIT's 4096...
	pMakedSec = HLIB_STD_MemAlloc(4096);
	if (pMakedSec == NULL)
		return NULL;
	HxSTD_memset(pMakedSec,0,4096);

	// Table Id
	pMakedSec[0] = 0;
	// Section Syntax indicator & Section length (In this stage, value is zero)
	usTmpVale = 0;
	usTmpVale = (1 << 15) | (0 << 14) ;
	pMakedSec[1] = ((usTmpVale >> 8) & 0xff);
	pMakedSec[2] = 0;
	// Ts Id
	pMakedSec[3] = pData->buf[0][3];
	pMakedSec[4] = pData->buf[0][4];
	// Version Number & Current Next Indicator
	pMakedSec[5] = pData->buf[0][5];
	// section number
	pMakedSec[6] = 0;
	// last section number;
	pMakedSec[7] = 0;

	// 각 program information은 4 byte.
	usProgramCnt = 0;
	PrIdx = 8;
	for (i=0; i < pData->num_sec; i++)
	{
		pSec = pData->buf[i];
		usTmpSecLen = HxMACRO_Get12Bit(&pSec[1]);
		pSec += 8;usTmpSecLen -= (5 + 4);
		HxLOG_Debug("\t[%02d] section's Program Info Length:%d\n",i,usTmpSecLen);
		usProgramCnt += (usTmpSecLen >> 2);
		while (usTmpSecLen)
		{
			usProgNum = HxMACRO_Get16Bit(pSec); pSec += 2;
			pMakedSec[PrIdx] = ((usProgNum >> 8) & 0xff);
			pMakedSec[PrIdx+1] = (usProgNum & 0xff);
			usPid = HxMACRO_Get13Bit(pSec); pSec += 2;
			pMakedSec[PrIdx+2] = ((usPid >> 8) & 0x1f);
			pMakedSec[PrIdx+3] = (usPid & 0xff);
			PrIdx += 4; usTmpSecLen -= 4;
		}
	}

	// calculate section length
	usTmpSecLen = 5 + 4 + (usProgramCnt << 2);
	pMakedSec[1] |= ((usTmpSecLen >> 8) & 0x0f);
	pMakedSec[2] = (usTmpSecLen & 0xff);
	ulCrc = HLIB_MATH_GetCrc32(pMakedSec,(usTmpSecLen + 3));
	pMakedSec[PrIdx] = ((ulCrc >> 24) & 0xff);
	pMakedSec[PrIdx+1] = ((ulCrc >> 16) & 0xff);
	pMakedSec[PrIdx+2] = ((ulCrc >> 8) & 0xff);
	pMakedSec[PrIdx+3] = ((ulCrc) & 0xff);

	return pMakedSec;
}

static	void local_MHEG_ParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HERROR hResult = 0;
	HUINT16 usFilterResult = 0, usTableId = 0, usFilterId = 0xFFFF, usTsUniqId = 0, usExtValue = 0;
	HINT32 i = 0, cnt = 0;
	DxService_t 			*pstSvcInfo = NULL;
	MhegSef_RawData_t 	*pSiRawData= NULL,stTmpRawData;
	HUINT8 				*pucTemp = NULL, *pucNewSec = NULL;
	HUINT16		usRawDataLen = 0, usExtTableId = 0;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData = NULL;
	HINT32	tsid;

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	if(usFilterId == 0xFFFF || usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("usFilterId Error Or usFilterResult Error (usFilteId = 0x%x, usFilterResult = %s) \n", usFilterId, (usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("pSiRawData is NULL \n");
		return;
	}

	HxLOG_Debug("ulViewId(0x%x) TableId(%d), FilterId(%d), UniqueId(0x%x) \r\n", ulViewId, usTableId, usFilterId, usTsUniqId);

	// for multiple pat section test. it is for basic test suit's si18 test stream.
	if (pSiRawData->num_sec > 1)
	{
		stTmpRawData.num_sec = 1;
		stTmpRawData.buf[0] = local_MHEG_MakeMultipleSecToOneSec(pSiRawData);
		pSiRawData = &stTmpRawData;
	}

	if (pSiRawData->num_sec > 0)
	{
		pucTemp = pSiRawData->buf[0];
		usExtValue = HxMACRO_Get16Bit (pucTemp + 3);

		/** 기존 내용을 지운다. */
		MHEG_RAW_FreeAllSection(usTsUniqId, eSI_TableType_PAT, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	}

	if(s_ulMHEGCurSvcUid != 0)
	{
		pstSvcInfo = APK_META_SVC_GetService(s_ulMHEGCurSvcUid);
		if(pstSvcInfo == NULL)
		{
			HxLOG_Error("Service not found:\n");
			return;
		}
		tsid = pstSvcInfo->tsid;
		APK_META_SVC_Delete(pstSvcInfo);

		if(tsid != usExtValue)
		{
			if(s_ulMHEGTryCount < 3)	// Let's prevent endless repetations
			{
				local_MHEG_StopPatFilter(ulViewId);
				local_MHEG_StartPatFilter(ulViewId);
				s_ulMHEGTryCount++;
			}
			return;
		}
	}

	HxLOG_Debug("pat section number (%d) \r\n", pSiRawData->num_sec);

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}

			HxLOG_Debug("[%d] usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", i, usTsUniqId, usExtTableId, usTableId);

			pstMHEGSiRawData->ulViewId			= ulViewId;
			pstMHEGSiRawData->uniqeId			= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn		= usExtTableId;
			pstMHEGSiRawData->tableId			= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData	 == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				HLIB_STD_MemFree(pstMHEGSiRawData);
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);
			pstMHEGSiRawData->pNext		= NULL;
			pstMHEGSiRawData->pid		= 0x0000;
			pstMHEGSiRawData->tableId		= 0x00;

			hResult = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if( hResult != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}
			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			hResult = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PAT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if(hResult != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			cnt++;
		}
	}

	if(i != cnt)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	local_MHEG_SI_SendSectionMessage (CMD_MHEG_SI_PAT, ulViewId, eSI_TABLE_RECEIVED, (HUINT32)usFilterId, usTsUniqId);

	if(hResult != ERR_OK)
	{
		HxLOG_Error("\n MHEG SI Error");
	}
}

static	void local_MHEG_FreePat(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR				hResult = 0;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;

	HxLOG_Debug("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \r\n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	MHEG_RawMakeList_GetSemaphore();

	pstMHEGSiRawData = MHEG_GetSiRawData(ulTsUniqId,PAT_PID,ulTableId,ulExternalValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	if(pstMHEGSiRawData != NULL)
	{
		hResult = MHEG_RemoveSiTblRawData(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
		if(hResult != ERR_OK)
		{
			HxLOG_Error("MHEG_RemoveSiTblRawData Error \n");
		}
	}

	MHEG_RawMakeList_ReleaseSemaphore();

	MHEG_RAW_FreeAllSection(ulTsUniqId, eSI_TableType_PAT, eMHEG_SI_RAW_DATA_TYPE_LIVE);
}

// SiThd의Task Callback.
static	void local_MHEG_ParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HERROR hResult = 0;
	HUINT16 usFilterResult = 0, usTableId = 0, usFilterId = 0xFFFF, usTsUniqId = 0, usExtValue = 0, usPmtNum;
	HINT32 i = 0, cnt = 0;
	MHEG_SI_Context_t 	*pContext = NULL;
	MhegSef_RawData_t 	*pSiRawData =NULL;
	HUINT8 				*pucTemp = NULL, *pucNewSec = NULL;
	HUINT16				usRawDataLen = 0, usExtTableId = 0;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Debug("ulViewId(0x%x), TableId(%d), FilterId(%d), UniqueId(%x) \r\n", ulViewId, usTableId, usFilterId, usTsUniqId);

	if(usFilterId == 0xFFFF || usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("usFilterId Error Or usFilterResult Error (usFilteId = 0x%x, usFilterResult = %s) \n", usFilterId, (usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("pSiRawData is NULL \r\n");
		return;
	}

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \n");
		return;
	}

	if (pSiRawData->num_sec > 0)
	{
		pucTemp = pSiRawData->buf[0];
		usExtValue = HxMACRO_Get16Bit (pucTemp + 3);

		/** 기존 내용을 지운다. */
		hResult = MHEG_RAW_FreeSection (usTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)usExtValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
		if(hResult != ERR_OK)
		{
			HxLOG_Error("\n MHEG SI Error");
		}
	}

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MHEG_MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData MemAlloc Error \n");
				break;
			}

			HxSTD_memset(pstMHEGSiRawData, 0x00, sizeof(MHEG_SiRawDataList_t));

			HxLOG_Debug("[%d] usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", i, usTsUniqId, usExtTableId, usTableId);

			pstMHEGSiRawData->ulViewId		= ulViewId;
			pstMHEGSiRawData->uniqeId			= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn		= usExtTableId;
			pstMHEGSiRawData->tableId			= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData	 == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData->savedRawData MemAlloc Error \n");
				HLIB_STD_MemFree(pstMHEGSiRawData);
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);
			for(usPmtNum=0; usPmtNum < pContext->usPmtTotalNum; usPmtNum++)
			{
				if(pContext->pstPmtStatus[usPmtNum].ulPmtFilterId == usFilterId)
					pstMHEGSiRawData->pid	= pContext->pstPmtStatus[usPmtNum].usProgMapPid;
			}

			HxLOG_Debug("PMT APPEND uniqeId (0x%x), tableIdExtn (0x%x)\r\n", usTsUniqId, usExtTableId);

			hResult = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if( hResult != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("pucNewSec MemAlloc Error\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			hResult = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PMT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if(hResult != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			cnt++;
		}
	}

	if(i != cnt)
	{
		HxLOG_Error("\n MHEG SI Error");
	}
	HxLOG_Debug("SEND ulViewId(%d), usFilterId(%d), usExtValue(%d) \r\n", ulViewId, usFilterId, usExtValue);

	local_MHEG_SI_SendSectionMessage (CMD_MHEG_SI_PMT, ulViewId, eSI_TABLE_RECEIVED, (HUINT32)usFilterId, usExtValue);
}

// not used
static	void local_MHEG_FreePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR				hResult = 0;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;

	HxLOG_Debug("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \r\n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	MHEG_RawMakeList_GetSemaphore();

	pstMHEGSiRawData = MHEG_GetSiRawData(ulTsUniqId,MHEG_DEMUX_VALUE_DONT_CARE,ulTableId,ulExternalValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	if(pstMHEGSiRawData != NULL)
	{
		hResult = MHEG_RemoveSiTblRawData(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
		if(hResult != ERR_OK)
		{
			HxLOG_Error("MHEG_RemoveSiTblRawData Error \n");
		}
	}

	MHEG_RawMakeList_ReleaseSemaphore();

	MHEG_RAW_FreeSection (ulTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)ulExternalValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
}

static	void local_MHEG_ParseLivePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HERROR				hError;
	MHEG_SI_Context_t 	*pContext = local_MHEG_SI_GetContext(ulViewId);
	MhegSef_RawData_t 	*pSiRawData,stTmpRawData;
	HUINT32 				 ulProgIdx, i, cnt;
	HUINT16				usPmtTotalNum, usTsUniqId, usFilterId, usFilterResult, usTableId;
	PatProgInfo_t			stProgInfo;
	DxService_t 			*pstSvcInfo = NULL;
	HINT32				svcId;
	HUINT8 				*pucTemp = NULL, *pucNewSec = NULL;
	HUINT16 				usRawDataLen, usExtTableId;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;

	HxLOG_Debug("===== (+) \n");

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("usFilterId Error \n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n",(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("pSiRawData is NULL \n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	if(pSiRawData->num_sec < 1)
	{
		HxLOG_Error("section invalid (pSiRawData->num_sec : %d) \n", pSiRawData->num_sec);
		HxLOG_Debug("===== (-) \n");
		return;
	}


	pstSvcInfo = APK_META_SVC_GetService(pContext->ulRequestSvcUid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Error("pstSvcInfo is NULL ( pContext->ulRequestSvcUid : 0x%x) \n", pContext->ulRequestSvcUid);
		HxLOG_Debug("===== (-) \n");
		return;
	}
	svcId = pstSvcInfo->svcid;
	APK_META_SVC_Delete(pstSvcInfo);

	HxLOG_Debug("ulViewId(0x%x) TableId(%d), FilterId(%d), TsUniqId(0x%x) svcId(0x%x) \n", ulViewId, usTableId, usFilterId, usTsUniqId, svcId);

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData MemAlloc Error\n");
				break;
			}

			HxSTD_MemSet(pstMHEGSiRawData, 0x00, sizeof(MHEG_SiRawDataList_t));

			HxLOG_Debug("[%d] usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", i, usTsUniqId, usExtTableId, usTableId);

			pstMHEGSiRawData->ulViewId			= ulViewId;
			pstMHEGSiRawData->uniqeId			= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn		= usExtTableId;
			pstMHEGSiRawData->tableId			= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData	 == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData->savedRawData MemAlloc Error\n");
				HLIB_STD_MemFree(pstMHEGSiRawData);
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);

			hError = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if( hError != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("pucNewSec MemAlloc Error \n");
				break;
			}

			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			hError = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PAT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if(hError != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			cnt++;
		}
	}

	// for multiple pat section test. it is for basic test suit's si18 test stream.
	if (pSiRawData->num_sec > 1)
	{
		stTmpRawData.num_sec = 1;
		stTmpRawData.buf[0] = local_MHEG_MakeMultipleSecToOneSec(pSiRawData);
		pSiRawData = &stTmpRawData;
	}

	HxLOG_Debug("pat section number (%d) \r\n", pSiRawData->num_sec);

	if(pSiRawData->buf[0] != NULL)
	{
		pucTemp = pSiRawData->buf[0];

		hError = MHEG_SI_CountPatProgramFromRaw (pucTemp, (HUINT32*)&usPmtTotalNum);
		if(hError != ERR_OK)
		{
			HxLOG_Error("MHEG_SI_CountPatProgramFromRaw Error \n");
			HxLOG_Debug("===== (-) \n");
			return;
		}

		for (ulProgIdx = 0; ulProgIdx < usPmtTotalNum; ulProgIdx++)
		{
			HxSTD_MemSet(&stProgInfo, 0x00, sizeof(PatProgInfo_t));			;

			hError = MHEG_SI_GetPatProgramInfoFromRaw (pucTemp, ulProgIdx, &stProgInfo);
			if(hError != ERR_OK)
				continue;

			if(stProgInfo.usProgNum == svcId)
			{
				local_MHEG_StartLivePmtFilter(ulViewId, usTsUniqId, eSiFilterMode_Monitor, stProgInfo.usProgMapPid, svcId);
				HxLOG_Debug("local_MHEG_StartLivePmtFilter Call by eSiFilterMode_Monitor (usProgMapPid = 0x%x)  \n", stProgInfo.usProgMapPid);
				HxLOG_Debug("===== (-) \n");
				return;
			}
		}
	}

	HxLOG_Debug("===== (-) \n");
}


static	void local_MHEG_ParseLivePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HERROR 				hError = 0;
	HUINT16 				usFilterResult = 0, usExtTableId = 0;
	HUINT16 				usTableId = 0, usFilterId = 0xFFFF, usTsUniqId = 0;
	HINT32 				i = 0, cnt = 0;
	HUINT8 				*pucTemp = NULL;
	HUINT16				usRawDataLen = 0 ;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;
	MHEG_SI_Context_t 	*pContext = NULL;
	MhegSef_RawData_t 	*pSiRawData = (MhegSef_RawData_t*)NULL;
	HBOOL				bMhegAvailable = FALSE;
	HUINT8				*pucNewSec;

	HxLOG_Debug("===== (+) \n");

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("usFilterId  Error \n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n",(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("pSiRawData is NULL \r\n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \r\n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	HxLOG_Debug("ulViewId(0x%x), TableId(%d), FilterId(%d), TsUniqueId(%x) TotalSecNum(%d) \n", ulViewId, usTableId, usFilterId, usTsUniqId, pSiRawData->num_sec);

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MHEG_MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData MemAlloc Error \n");
				HxLOG_Debug("===== (-) \n");
				break;
			}

			HxSTD_MemSet(pstMHEGSiRawData, 0x00, sizeof(MHEG_SiRawDataList_t));

			HxLOG_Debug("SecIndex(%d) TsUniqueId(0x%x), usTableId(0x%x) , ExtTableId(0x%x)\n", i, usTsUniqId, usTableId , usExtTableId);

			pstMHEGSiRawData->ulViewId			= ulViewId;
			pstMHEGSiRawData->uniqeId				= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn			= usExtTableId;
			pstMHEGSiRawData->tableId				= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData == NULL )
			{
				HxLOG_Error("pstMHEGSiRawData->savedRawData MemAlloc Error \n");
				HLIB_STD_MemFree(pstMHEGSiRawData);
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);

			if(pContext->stLivePmtStatus.ulPmtFilterId == usFilterId)
			{
				pstMHEGSiRawData->pid	= pContext->stLivePmtStatus.usProgMapPid;
			}

			HxLOG_Debug("PMT APPEND : TsUniqueId (0x%x), ExtTableId (0x%x) Pid(0x%x) \r\n", usTsUniqId, usExtTableId, pstMHEGSiRawData->pid);

			hError = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if( hError != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("pucNewSec MemAlloc Error\n");
				break;
			}
			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			hError = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PMT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if(hError != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			// Check Mheg available
			{
				HUINT16 usSvcId;
				HUINT32	ulCntStream, ulIndex, ulDesIndex, ulDesCnt;
				//HUINT8	*pucElemDes;
				HERROR	ulErr;
				SIxDataBroadcastIdDes_t *pDBIdes = NULL;

				usSvcId = HxMACRO_Get16Bit(pucTemp + 3);

				MHEG_SI_CountPmtElementFromRaw(pucTemp, &ulCntStream);
				if(ulCntStream > 0)
				{
					HxLOG_Debug("Live (%d)  th PMT Section  %p [svcid : %x]!!  \n", i,pucTemp, usSvcId);
					HxLOG_Debug("Num of PMT Element : %d!!  \n", ulCntStream);

					for(ulIndex = 0; ulIndex < ulCntStream; ulIndex++)
					{
						ulErr = MHEG_SI_CountPmtElementDescriptorFromRaw(pucTemp, ulIndex, eSIxDESCTAG_DATA_BROADCAST_ID, &ulDesCnt);
						if(ulErr == ERR_OK && ulDesCnt > 0)
						{
							HxLOG_Debug("eSIxDESCTAG_DATA_BROADCAST_ID descriptor Count: %d!!  \n", ulDesCnt);

							for(ulDesIndex = 0; ulDesIndex < ulDesCnt; ulDesIndex++)
							{
								ulErr = MHEG_SI_GetPmtElementDescriptorFromRaw(pucTemp, ulIndex, eSIxDESCTAG_DATA_BROADCAST_ID, 0, (void**)&pDBIdes);
								if ((ulErr == ERR_OK) && (pDBIdes != NULL))
								{
									if(pDBIdes->usDataBroadcastId == UK_DTT_MHEG_DATA_BROADCAST_ID && pDBIdes->pucIdSelector)
									{
										HUINT8 ucIdx;
										HUINT16 usAppTypeCode;
										HUINT8 ucAppSpecificDataLen;

										for(ucIdx = 0; ucIdx < pDBIdes->usSelectorLen; ucIdx++)
										{
											usAppTypeCode = (pDBIdes->pucIdSelector[ucIdx] << 8) | pDBIdes->pucIdSelector[ucIdx+1];

											HxLOG_Debug("usAppTypeCode (0x%04x) \n", usAppTypeCode);

											if ((usAppTypeCode == UK_DTT_MHEG_APPLICATION_CODE1) || (usAppTypeCode == UK_DTT_MHEG_APPLICATION_CODE2))
											{
												HxLOG_Debug("Ok Data Broadcast ID : SvcId (0x%x) \n", usSvcId);
												bMhegAvailable = TRUE;
												break;
											}
											ucIdx += 3;
											ucAppSpecificDataLen = pDBIdes->pucIdSelector[ucIdx];

											ucIdx++;
											ucIdx += ucAppSpecificDataLen;
										}
									}
									SILIB_DESC_Delete((SIxDescriptor_t *)pDBIdes);
									pDBIdes = NULL;
								}
							}
						}
					}
				}
			}
			break;
		}
	}

	PORT_ITK_PostMsg(eMHEG_SI_PMTCHANGED, ulViewId, bMhegAvailable, 0, 0, 0);

	HxLOG_Debug("===== (-) \n");
}

// not used
static	void local_MHEG_FreeLivePmt(HUINT32 ulTsUniqId, HUINT32 ulTableId, HUINT32 ulExternalValue, HUINT32 ulParam2)
{
	HERROR 				hResult = 0;
	MHEG_SiRawDataList_t	*pstMHEGSiRawData = NULL;

	HxLOG_Debug("UniqueId (0x%x), TableId (0x%x), ExtVal (0x%x), Param2 (0x%x) \r\n", ulTsUniqId, ulTableId, ulExternalValue, ulParam2);

	MHEG_RawMakeList_GetSemaphore();

	pstMHEGSiRawData = MHEG_GetSiRawData(ulTsUniqId,MHEG_DEMUX_VALUE_DONT_CARE,ulTableId,ulExternalValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	if(pstMHEGSiRawData != NULL)
	{
		hResult = MHEG_RemoveSiTblRawData(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
		if(hResult != ERR_OK)
		{
			HxLOG_Error("MHEG_RemoveSiTblRawData Error \n");
		}
	}

	MHEG_RawMakeList_ReleaseSemaphore();

	MHEG_RAW_FreeSection (ulTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)ulExternalValue, eMHEG_SI_RAW_DATA_TYPE_LIVE);
}


static HERROR local_MHEG_InitPmtStatus (HUINT32 ulPmtProgNum, MHEG_SI_SrchPmtStatus_t *pstPmtStatus)
{
	HUINT32		 ulIndex;

	if (pstPmtStatus == NULL)
	{
		HxLOG_Error("pstPmtStatus is Null !!!\n");
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < ulPmtProgNum; ulIndex++)
	{
		pstPmtStatus[ulIndex].usProgMapPid 	= 0xFFFF;
		pstPmtStatus[ulIndex].ucStatus 		= eSI_TABLE_SKIPPED;
		pstPmtStatus[ulIndex].ulPmtFilterId	= MHEG_FILTER_ID_NULL;
		pstPmtStatus[ulIndex].ucMode		= eSiFilterMode_Once;
	}

	return ERR_OK;
}

static HERROR local_MHEG_RetrievePmtsByPat (MHEG_SI_Context_t *pContext)
{
	HUINT32 			 ulProgIdx;
	HUINT16				usPmtTotalNum;
	PatProgInfo_t		 stProgInfo;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus;
	HERROR				 hErr;

	HxLOG_Debug("===== (+) \n");

	if (pContext == NULL)
	{
		HxLOG_Error("pContext is Null !!!\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	pstPmtStatus = pContext->pstPmtStatus;
	if (pstPmtStatus == NULL)
	{
		HxLOG_Error("pstPmtStatus is Null !!\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	usPmtTotalNum = pContext->usPmtTotalNum;

	HxLOG_Debug("pContext->usPmtTotalNum : %d\n", usPmtTotalNum);

	for (ulProgIdx = 0; ulProgIdx < usPmtTotalNum; ulProgIdx++)
	{
		HxLOG_Debug("pstPmtStatus[%d].ucStatus (0x%d) \r\n",ulProgIdx, pstPmtStatus[ulProgIdx].ucStatus);
		if (pstPmtStatus[ulProgIdx].ucStatus == eSI_TABLE_FAIL)
		{
			local_MHEG_StopPmtFilter(pContext->ulViewId, FALSE, pstPmtStatus[ulProgIdx].ulPmtFilterId);
			pstPmtStatus[ulProgIdx].ucStatus = eSI_TABLE_SKIPPED;
		}

		if (pstPmtStatus[ulProgIdx].ucStatus == eSI_TABLE_SKIPPED)
		{
			hErr = MHEG_SI_GetPatProgramInfo (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_LIVE), pContext->usUniqueId, 0, ulProgIdx, &stProgInfo);
			if (hErr != ERR_OK)
			{
				pstPmtStatus[ulProgIdx].ucStatus = eSI_TABLE_SKIPPED;
				continue;
			}

			if(pContext->ulLimitDmxNum < MHEG_FILTER_LIMIT)
			{
				HxLOG_Debug("PMT (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x), usProgNum (0x%x), ulProgIdx (%d)\r\n",ulProgIdx, pContext->ulViewId, pContext->usUniqueId, stProgInfo.usProgMapPid, stProgInfo.usProgNum, ulProgIdx);
				hErr = local_MHEG_StartPmtFilter(pContext->ulViewId, pContext->usUniqueId, eSiFilterMode_Once, stProgInfo.usProgMapPid, stProgInfo.usProgNum, ulProgIdx);
				pstPmtStatus[ulProgIdx].ucMode = eSiFilterMode_Once;
			}
			else
			{
				HxLOG_Debug("DMX LIMITATION -> NEXT Time (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x), usProgNum (0x%x), ulProgIdx (%d)\r\n",ulProgIdx, pContext->ulViewId, pContext->usUniqueId,stProgInfo.usProgMapPid, stProgInfo.usProgNum,ulProgIdx);
				hErr = ERR_FAIL;
			}

			if (hErr == ERR_OK )
			{
				pstPmtStatus[ulProgIdx].usProgMapPid = stProgInfo.usProgMapPid;
				pstPmtStatus[ulProgIdx].ucStatus  = eSI_TABLE_WAITING;
			}
			else
			{
				pstPmtStatus[ulProgIdx].usProgMapPid = stProgInfo.usProgMapPid;
				pstPmtStatus[ulProgIdx].ucStatus  = eSI_TABLE_TIMEOUT;
			}
			HxLOG_Debug("pstPmtStatus[%d].ucStatus : %d\r\n",ulProgIdx,pstPmtStatus[ulProgIdx].ucStatus);
		}
	}

	HxLOG_Debug("===== (-) \n");
	return ERR_OK;
}

HERROR local_MHEG_ProcessPat_Base (HUINT32 ulViewId,HUINT16	usTsUniqId)
{
	HERROR				hError = ERR_OK;
	HUINT32				ulPatSecNum = 0, ulPmtProgNum = 0 ,ulTmpNum = 0;
	HUINT32				ulIndex = 0;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus = NULL;
	MHEG_SI_Context_t 	*pContext = NULL;


	HxLOG_Debug("===== (+) \n");

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is NULL \r\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	// PAT table received.
	pContext->aucSectionStatus[eSI_TableType_PAT] = eSI_TABLE_RECEIVED;

	MHEG_Raw_GetSemaphore();

	hError = MHEG_SI_CountPatSection (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_LIVE), usTsUniqId, &ulPatSecNum);
	if(hError != ERR_OK || ulPatSecNum == 0)
	{
		HxLOG_Error("MHEG_SI_CountPatSection Error (hError = 0x%x, ulPatSecNum = %d) \n", hError, ulPatSecNum);
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("usTsUniqId:0x%X , ulPatSecNum:%d\n\r",usTsUniqId,ulPatSecNum);

	for (ulIndex = 0; ulIndex < ulPatSecNum; ulIndex++)
	{
		hError = MHEG_SI_CountPatProgram (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_LIVE),(HUINT32)usTsUniqId, ulIndex, &ulTmpNum);
		if (hError ==  ERR_OK)
		{
			ulPmtProgNum += ulTmpNum;
		}
	}

	HxLOG_Debug("ulPmtProgNum (0x%d) \r\n", ulPmtProgNum);

	if (ulPmtProgNum > 0)
	{
		if(pContext->pstPmtStatus)
			HLIB_STD_MemFree(pContext->pstPmtStatus);

		pContext->pstPmtStatus = NULL;
		pContext->usPmtTotalNum = 0;
		pstPmtStatus = (MHEG_SI_SrchPmtStatus_t *)HLIB_STD_MemAlloc (sizeof(MHEG_SI_SrchPmtStatus_t) * ulPmtProgNum);
		if (pstPmtStatus != NULL)
		{
			local_MHEG_InitPmtStatus (ulPmtProgNum, pstPmtStatus);

			pContext->usPmtTotalNum		= (HUINT16)ulPmtProgNum;
			pContext->pstPmtStatus		= pstPmtStatus;

			// Now, try to retrieve all the related PMTs.
			local_MHEG_RetrievePmtsByPat (pContext);
		}
	}

	MHEG_Raw_ReleaseSemaphore();

	if (pContext->usPmtTotalNum > 0)
	{
		pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_WAITING;
		hError = ERR_OK;
	}
	else
	{
		// No PMT to be received : Stopl.
		pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_SKIPPED;
		hError = ERR_FAIL;
	}

	HxLOG_Debug("===== (-) hError(%d) \n", hError);

	return hError;
}

HERROR local_MHEG_ProcessPmt_Base (HUINT32 ulViewId, HUINT32	ulFilterId, eFilterTableStatus_t	eStatus)
{
	HUINT32				 ulIndex;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus;
	HERROR				 hErr;
	MHEG_SI_Context_t 	*pContext = NULL;
	PatProgInfo_t		 stProgInfo;

	HxLOG_Debug("===== (+) \n");

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("pContext is Null \n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	pstPmtStatus = pContext->pstPmtStatus;
	if (pstPmtStatus == NULL)
	{
		HxLOG_Error("pstPmtStatus is Null \n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("ulViewId (0x%x), ulFilterId (0x%x), eStatus (0x%x)\r\n", ulViewId, ulFilterId, eStatus);

	for(ulIndex=0; ulIndex < pContext->usPmtTotalNum; ulIndex++)
	{
		if(pContext->pstPmtStatus[ulIndex].ulPmtFilterId == ulFilterId)
		{
			HxLOG_Debug("Found FilterId ( %d th FilterId (0x%x : 0x%x) \n", ulIndex, pContext->pstPmtStatus[ulIndex].ulPmtFilterId, ulFilterId);
			break;
		}
	}

	if(ulIndex >= pContext->usPmtTotalNum)
	{
		HxLOG_Error(" Matched FilterId Not Found \n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	if(eStatus == eSI_TABLE_RECEIVED)
	{
		pstPmtStatus[ulIndex].ucStatus = eStatus;
		if(pstPmtStatus[ulIndex].ucMode	!= eSiFilterMode_Continue)
		{
			local_MHEG_StopPmtFilter(ulViewId, FALSE, ulFilterId);
		}
	}

	if (pContext->aucSectionStatus[eSI_TableType_PMT] == eSI_TABLE_WAITING)
	{
		//Now, check whether all the PMT is received, or not.
		for (ulIndex = 0; ulIndex < (HUINT32)pContext->usPmtTotalNum; ulIndex++)
		{
			if (pstPmtStatus[ulIndex].ucStatus != eSI_TABLE_RECEIVED && pstPmtStatus[ulIndex].ucStatus != eSI_TABLE_SKIPPED)
			{
				break;
			}
		}

		if (ulIndex == (HUINT32)pContext->usPmtTotalNum)
		{
			// All PMT is received or timeout:
			pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_RECEIVED;
			HxLOG_Debug("ulViewId (0x%x), ulFilterId (0x%x), eStatus (0x%x)\r\n",ulViewId, ulFilterId, eStatus);
			HxLOG_Debug("===== (-) \n");

			return ERR_OK;
		}
		else if(pContext->ulLimitDmxNum < MHEG_FILTER_LIMIT)
		{
			for (ulIndex = 0; ulIndex < (HUINT32)pContext->usPmtTotalNum; ulIndex++)
			{
				if (pstPmtStatus[ulIndex].ucStatus == eSI_TABLE_TIMEOUT)
				{
					hErr = MHEG_SI_GetPatProgramInfo (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_LIVE), pContext->usUniqueId, 0,  ulIndex, &stProgInfo);
					if (hErr != ERR_OK)
					{
						pstPmtStatus[ulIndex].ucStatus = eSI_TABLE_SKIPPED;
						continue;
					}

					hErr = local_MHEG_StartPmtFilter(pContext->ulViewId, pContext->usUniqueId, eSiFilterMode_Once,stProgInfo.usProgMapPid, stProgInfo.usProgNum,ulIndex);
					pstPmtStatus[ulIndex].ucMode			= eSiFilterMode_Once;

					if ((hErr == ERR_OK))
					{
						pstPmtStatus[ulIndex].ucStatus  = eSI_TABLE_WAITING;
						pstPmtStatus[ulIndex].usProgMapPid = stProgInfo.usProgMapPid;
						HxLOG_Debug("[TIMEOUT-->WAITING] usProgMapPid (0x%d) \r\n", pstPmtStatus[ulIndex].usProgMapPid);
						HxLOG_Debug("PMT (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x)\r\n",ulIndex, pContext->ulViewId, pContext->usUniqueId, pstPmtStatus[ulIndex].usProgMapPid);
					}
					else
					{
						HxLOG_Error("PMT (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x)\r\n",ulIndex, pContext->ulViewId, pContext->usUniqueId, pstPmtStatus[ulIndex].usProgMapPid);
					}
				}
				if(pContext->ulLimitDmxNum >= MHEG_FILTER_LIMIT)
					break;
			}

		}
	}

	HxLOG_Debug("===== (-) \n");
	//HxLOG_Debug("--- ulViewId (0x%x), ulFilterId (0x%x), eStatus (0x%x)\r\n", ulViewId, ulFilterId, eStatus);

	return ERR_OK;
}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static	void local_MHEG_Streaming_ParsePat(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HUINT16 usFilterResult = 0;
	HUINT16 usTableId = 0;
	HUINT16 usFilterId;
	HUINT16 usTsUniqId = 0;
	HUINT16 usExtValue = 0;
	HINT32 i = 0, cnt = 0;
	HUINT32 nResult = 0;
	DxService_t *pstSvcInfo;
	MhegSef_RawData_t *pSiRawData,stTmpRawData;
	HUINT8 *pucTemp = NULL, *pucNewSec = NULL;
	HUINT16		usRawDataLen = 0, usExtTableId = 0;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData = NULL;
	HINT32	tsid;

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);

	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Debug(" ulViewId(0x%x) TableId(%d), FilterId(%d), UniqueId(0x%x) \r\n", ulViewId, usTableId, usFilterId, usTsUniqId);

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("\n MHEG SI Error");
		return;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n",(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("Input param is invalid \r\n");
		return;
	}

	// for multiple pat section test. it is for basic test suit's si18 test stream.
	if (pSiRawData->num_sec > 1)
	{
		stTmpRawData.num_sec = 1;
		stTmpRawData.buf[0] = local_MHEG_MakeMultipleSecToOneSec(pSiRawData);
		pSiRawData = &stTmpRawData;
	}

	if (pSiRawData->num_sec > 0)
	{
		pucTemp = pSiRawData->buf[0];
		usExtValue = HxMACRO_Get16Bit(pucTemp + 3);

		/** 기존 내용을 지운다. */
		MHEG_RAW_FreeAllSection(usTsUniqId, eSI_TableType_PAT, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
	}

	if(s_ulMHEGStreamingCurSvcUid != 0 && s_bMHEGStreamingMode == FALSE)
	{
		//DB_SVC_GetServiceInfo(s_ulMHEGStreamingCurSvc, &stSvcInfo);
		pstSvcInfo = APK_META_SVC_GetService(s_ulMHEGStreamingCurSvcUid);
		if(pstSvcInfo == NULL)
		{
			HxLOG_Error("Service not found:\n");
			return;
		}
		tsid = pstSvcInfo->tsid;
		APK_META_SVC_Delete(pstSvcInfo);

		if(tsid != usExtValue)
		{
			if(s_ulMHEGStreamingTryCount < 3)	// Let's prevent endless repetations
			{
				local_MHEG_Streaming_StopPatFilter(ulViewId);
				local_MHEG_Streaming_StartPatFilter(ulViewId);
				s_ulMHEGStreamingTryCount++;
			}
			return;
		}
	}

	HxLOG_Debug("pmt section number (%d) \r\n", pSiRawData->num_sec);

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}

			HxLOG_Debug("[%d] usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", i, usTsUniqId, usExtTableId, usTableId);

			pstMHEGSiRawData->ulViewId			= ulViewId;
			pstMHEGSiRawData->uniqeId			= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn		= usExtTableId;
			pstMHEGSiRawData->tableId			= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData	 == NULL )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData);
				HxLOG_Error("\n MHEG SI Error");
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);
			pstMHEGSiRawData->pNext			= NULL;
			pstMHEGSiRawData->pid			= 0x0000;
			pstMHEGSiRawData->tableId		= 0x00;

			nResult = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
			if( nResult != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}
			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			nResult = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PAT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
			if(nResult != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			cnt++;
		}
	}

	if(i != cnt)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

	local_MHEG_SI_SendSectionMessage (CMD_MHEG_STREAMING_SI_PAT, ulViewId, eSI_TABLE_RECEIVED, (HUINT32)usFilterId, usTsUniqId);

	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n MHEG SI Error");
	}

}

static	void local_MHEG_Streaming_ParsePmt(HUINT32 ulTableIdAndResult, HUINT32 ulfilterAndUniqId, HUINT32 ulParam, HUINT32 ulViewId)
{
	HUINT16 usFilterResult = 0;
	HUINT16 usTableId = 0;
	HUINT16 usFilterId;
	HUINT16 usTsUniqId = 0;
	HUINT16 usExtValue = 0;
	HINT32 i = 0, cnt = 0, pmtcnt;
	HUINT32 nResult = 0;
	MHEG_SI_Context_t 	*pContext = NULL;
	MhegSef_RawData_t *pSiRawData = (MhegSef_RawData_t*)NULL;
	HUINT8 *pucTemp = NULL, *pucNewSec = NULL;
	HUINT16		usRawDataLen = 0, usExtTableId = 0;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData = NULL;

	// Get TableId, FilterId, UniqueId, Filter Result
	local_MHEG_si_GetTableIdAndResult(ulTableIdAndResult, &usTableId, &usFilterResult);
	local_MHEG_si_GetFilterAndTsUniqueId(ulfilterAndUniqId, &usFilterId, &usTsUniqId);

	HxLOG_Debug("hAction(0x%x), TableId(%d), FilterId(%d), UniqueId(%x) \r\n", ulViewId, usTableId, usFilterId, usTsUniqId);

	if(usFilterId == 0xFFFF)
	{
		HxLOG_Error("\n MHEG SI Error");
		return;
	}

	if(usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("result = %s\n",(usFilterResult == eSiFilterResult_Timeout) ? "Time out" : "Error");
		return;
	}

	pSiRawData = (MhegSef_RawData_t*)ulParam;
	if(pSiRawData == NULL)
	{
		HxLOG_Error("Input param is invalid \r\n");
		return;
	}

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if ( pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return;
	}

#if 1
	if (pSiRawData->num_sec > 0)
	{
		pucTemp = pSiRawData->buf[0];
		usExtValue = HxMACRO_Get16Bit(pucTemp + 3);

		/** 기존 내용을 지운다. */
		nResult = MHEG_RAW_FreeSection (usTsUniqId, eSI_TableType_PMT, TRUE, (HUINT32)usExtValue, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("\n MHEG SI Error");
		}
	}
#endif

	for ( i=0, cnt=0; cnt < pSiRawData->num_sec && i < MHEG_MAX_SECTION_NUM; i++)
	{
		if(pSiRawData->buf[i] != NULL)
		{
			pucTemp = pSiRawData->buf[i];

			/* get table_id, table id extension and raw data length from section raw Data */
			usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucTemp) + 3;
			usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucTemp);

			/* make SIRawDataList format */
			pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
			if ( pstMHEGSiRawData == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}

			HxLOG_Debug("[%d] usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", i, usTsUniqId, usExtTableId, usTableId);

			pstMHEGSiRawData->ulViewId			= ulViewId;
			pstMHEGSiRawData->uniqeId			= usTsUniqId;
			pstMHEGSiRawData->tableIdExtn		= usExtTableId;
			pstMHEGSiRawData->tableId			= (HUINT8)usTableId;
			pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
			pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pstMHEGSiRawData->savedRawData	 == NULL )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData);
				HxLOG_Error("\n MHEG SI Error");
				break;
			}

			HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucTemp, usRawDataLen);
			pstMHEGSiRawData->pNext				= NULL;

			pstMHEGSiRawData->pid = 0;
			for(pmtcnt=0; pmtcnt < pContext->usPmtTotalNum; pmtcnt++)
				if(pContext->pstPmtStatus[pmtcnt].ulPmtFilterId == usFilterId)
					pstMHEGSiRawData->pid	= pContext->pstPmtStatus[pmtcnt].usProgMapPid;

			HxLOG_Debug("PMT APPEND uniqeId (0x%x), tableIdExtn (0x%x)\r\n", usTsUniqId, usExtTableId);

			nResult = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
			if( nResult != ERR_OK )
			{
				HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
				HLIB_STD_MemFree(pstMHEGSiRawData);
			}

			pucNewSec = (HUINT8 *)HLIB_STD_MemAlloc(usRawDataLen);
			if ( pucNewSec == NULL )
			{
				HxLOG_Error("\n MHEG SI Error");
				break;
			}
			HxSTD_memcpy(pucNewSec, pucTemp, (usRawDataLen));

			nResult = MHEG_RAW_AddRawSection (usTsUniqId, eSI_TableType_PMT, pucNewSec, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
			if(nResult != ERR_OK)
			{
				HLIB_STD_MemFree(pucNewSec);
			}

			cnt++;
		}
	}

	if(i != cnt)
	{
		HxLOG_Error("\n MHEG SI Error");
	}
	HxLOG_Debug("SEND ulViewId(%d), usFilterId(%d), usExtValue(%d) \r\n", ulViewId, usFilterId, usExtValue);

	local_MHEG_SI_SendSectionMessage (CMD_MHEG_STREAMING_SI_PMT, ulViewId, eSI_TABLE_RECEIVED, (HUINT32)usFilterId, usExtValue);
}

static HERROR local_MHEG_Streaming_RetrievePmtsByPat (MHEG_SI_Context_t *pContext)
{
	HUINT32 			 ulProgIdx;
	HUINT16				usPmtTotalNum;
	PatProgInfo_t		 stProgInfo;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus;
	HERROR				 hErr;

	HxLOG_Debug(" +++ (pContext:%p)\r\n", pContext);

	if (pContext == NULL)
	{
		HxLOG_Debug("Error Leave!!\r\n");
		return ERR_FAIL;
	}

	pstPmtStatus = pContext->pstPmtStatus;
	usPmtTotalNum = pContext->usPmtTotalNum;
	if (pstPmtStatus == NULL)
	{
		HxLOG_Debug("Error Leave!!\r\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("pstTable->usPmtTotalNum : %d\n", usPmtTotalNum);
	for (ulProgIdx = 0; ulProgIdx < usPmtTotalNum; ulProgIdx++)
	{
		HxLOG_Debug("pstPmtStatus[%d].ucStatus (0x%d) \r\n",ulProgIdx, pstPmtStatus[ulProgIdx].ucStatus);
		if (pstPmtStatus[ulProgIdx].ucStatus == eSI_TABLE_FAIL)
		{
			local_MHEG_Streaming_StopPmtFilter(pContext->ulViewId, FALSE, pstPmtStatus[ulProgIdx].ulPmtFilterId);
			pstPmtStatus[ulProgIdx].ucStatus = eSI_TABLE_SKIPPED;
		}

		if (pstPmtStatus[ulProgIdx].ucStatus == eSI_TABLE_SKIPPED)
		{
			hErr = MHEG_SI_GetPatProgramInfo (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_STREAMING), pContext->usUniqueId, 0, ulProgIdx, &stProgInfo);
			if (hErr != ERR_OK)
			{
				pstPmtStatus[ulProgIdx].ucStatus = eSI_TABLE_SKIPPED;
				continue;
			}

			if(pContext->ulLimitDmxNum < MHEG_FILTER_LIMIT)
			{
				HxLOG_Debug("PMT (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x), usProgNum (0x%x), ulProgIdx (%d)\r\n",ulProgIdx, pContext->ulViewId, pContext->usUniqueId, stProgInfo.usProgMapPid, stProgInfo.usProgNum, ulProgIdx);
				hErr = local_MHEG_Streaming_StartPmtFilter(pContext->ulViewId, pContext->usUniqueId, eSiFilterMode_Once, stProgInfo.usProgMapPid, stProgInfo.usProgNum, ulProgIdx);
				pstPmtStatus[ulProgIdx].ucMode = eSiFilterMode_Once;
			}
			else
			{
				HxLOG_Debug("DMX LIMITATION -> NEXT Time (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x), usProgNum (0x%x), ulProgIdx (%d)\r\n",ulProgIdx, pContext->ulViewId, pContext->usUniqueId,stProgInfo.usProgMapPid, stProgInfo.usProgNum,ulProgIdx);
				hErr = ERR_FAIL;
			}

			if (hErr == ERR_OK )
			{
				pstPmtStatus[ulProgIdx].usProgMapPid = stProgInfo.usProgMapPid;
				pstPmtStatus[ulProgIdx].ucStatus  = eSI_TABLE_WAITING;
			}
			else
			{
				pstPmtStatus[ulProgIdx].usProgMapPid = stProgInfo.usProgMapPid;
				pstPmtStatus[ulProgIdx].ucStatus  = eSI_TABLE_TIMEOUT;
			}
			HxLOG_Debug("pstPmtStatus[%d].ucStatus : %d\r\n",ulProgIdx,pstPmtStatus[ulProgIdx].ucStatus);

		}
	}

	HxLOG_Debug("--- \r\n");
	return ERR_OK;
}

HERROR local_MHEG_Streaming_ProcessPat_Base (HUINT32 ulViewId, HUINT16	usTsUniqId)
{
	HUINT32				 ulPatSecNum, ulPmtProgNum,ulTmpNum,i;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus;
	HERROR				 hErr;
	MHEG_SI_Context_t 	*pContext = NULL;

	HxLOG_Debug("+++ \r\n");

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return ERR_FAIL;
	}

	// PAT table received.
	pContext->aucSectionStatus[eSI_TableType_PAT] = eSI_TABLE_RECEIVED;

	MHEG_Raw_GetSemaphore();

	hErr = MHEG_SI_CountPatSection (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_STREAMING), usTsUniqId, &ulPatSecNum);
	HxLOG_Debug("\tusTsUniqId:0x%X , ulPatSecNum:%d\n\r",usTsUniqId,ulPatSecNum);
	ulPmtProgNum=0;ulTmpNum = 0;
	for (i = 0; i < ulPatSecNum; i++)
	{
		hErr = MHEG_SI_CountPatProgram (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_STREAMING),(HUINT32)usTsUniqId,i, &ulTmpNum);
		if (ERR_OK == hErr)
		{
			ulPmtProgNum += ulTmpNum;
		}
	}
	HxLOG_Debug("ulPmtProgNum (0x%d) \r\n", ulPmtProgNum);

	if (ulPmtProgNum > 0)
	{
		if(pContext->pstPmtStatus)
			HLIB_STD_MemFree(pContext->pstPmtStatus);
		pContext->pstPmtStatus = NULL;
		pContext->usPmtTotalNum = 0;
		pstPmtStatus = (MHEG_SI_SrchPmtStatus_t *)HLIB_STD_MemAlloc (sizeof(MHEG_SI_SrchPmtStatus_t) * ulPmtProgNum);
		if (pstPmtStatus != NULL)
		{
			local_MHEG_InitPmtStatus (ulPmtProgNum, pstPmtStatus);

			pContext->usPmtTotalNum		= (HUINT16)ulPmtProgNum;
			pContext->pstPmtStatus		= pstPmtStatus;

			// Now, try to retrieve all the related PMTs.
			local_MHEG_Streaming_RetrievePmtsByPat (pContext);
		}
	}

	MHEG_Raw_ReleaseSemaphore();

	if (pContext->usPmtTotalNum > 0)
	{
		pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_WAITING;
		hErr = ERR_OK;
	}
	else
	{
		// No PMT to be received : Stopl.
		pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_SKIPPED;
		hErr = ERR_FAIL;
	}

	HxLOG_Debug("--- (Err:0x%X) \r\n", hErr);

	return hErr;
}

HERROR local_MHEG_Streaming_ProcessPmt_Base (HUINT32 ulViewId, HUINT32	ulFilterId, eFilterTableStatus_t	eStatus)
{
	HUINT32				 ulIndex;
	MHEG_SI_SrchPmtStatus_t	*pstPmtStatus;
	HERROR				 hErr;
	MHEG_SI_Context_t 	*pContext = NULL;
	PatProgInfo_t		 stProgInfo;

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("+++ ulViewId (0x%x), ulFilterId (0x%x), eStatus (0x%x)\r\n", ulViewId, ulFilterId, eStatus);

	pstPmtStatus = pContext->pstPmtStatus;
	if (pstPmtStatus == NULL)				{ return ERR_FAIL; }

	for(ulIndex=0; ulIndex < pContext->usPmtTotalNum; ulIndex++)
	{
		if(pContext->pstPmtStatus[ulIndex].ulPmtFilterId == ulFilterId)
			break;
	}

	if(ulIndex >= pContext->usPmtTotalNum)
	{
		return ERR_FAIL;
	}

	if(eStatus == eSI_TABLE_RECEIVED)
	{
		pstPmtStatus[ulIndex].ucStatus = eStatus;
		if(pstPmtStatus[ulIndex].ucMode	!= eSiFilterMode_Continue)
			local_MHEG_Streaming_StopPmtFilter(ulViewId, FALSE, ulFilterId);
	}

	if (pContext->aucSectionStatus[eSI_TableType_PMT] == eSI_TABLE_WAITING)
	{
		//Now, check whether all the PMT is received, or not.
		for (ulIndex = 0; ulIndex < (HUINT32)pContext->usPmtTotalNum; ulIndex++)
		{
			if (pstPmtStatus[ulIndex].ucStatus != eSI_TABLE_RECEIVED && pstPmtStatus[ulIndex].ucStatus != eSI_TABLE_SKIPPED)
			{
				break;
			}
		}


		if (ulIndex == (HUINT32)pContext->usPmtTotalNum)
		{
			// All PMT is received or timeout:
			pContext->aucSectionStatus[eSI_TableType_PMT] = eSI_TABLE_RECEIVED;
			HxLOG_Debug("--- ulViewId (0x%x), ulFilterId (0x%x), eStatus (0x%x)\r\n",ulViewId, ulFilterId, eStatus);

			return ERR_OK;
		}
		else if(pContext->ulLimitDmxNum < MHEG_FILTER_LIMIT)
		{
			for (ulIndex = 0; ulIndex < (HUINT32)pContext->usPmtTotalNum; ulIndex++)
			{
				if (pstPmtStatus[ulIndex].ucStatus == eSI_TABLE_TIMEOUT)
				{
					hErr = MHEG_SI_GetPatProgramInfo (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_STREAMING), pContext->usUniqueId, 0,  ulIndex, &stProgInfo);
					if (hErr != ERR_OK)
					{
						pstPmtStatus[ulIndex].ucStatus = eSI_TABLE_SKIPPED;
						continue;
					}

					hErr = local_MHEG_Streaming_StartPmtFilter(pContext->ulViewId, pContext->usUniqueId, eSiFilterMode_Once,stProgInfo.usProgMapPid, stProgInfo.usProgNum,ulIndex);
					pstPmtStatus[ulIndex].ucMode			= eSiFilterMode_Once;

					if ((hErr == ERR_OK))
					{
						pstPmtStatus[ulIndex].ucStatus  = eSI_TABLE_WAITING;
						pstPmtStatus[ulIndex].usProgMapPid = stProgInfo.usProgMapPid;
						HxLOG_Debug("[TIMEOUT-->WAITING] usProgMapPid (0x%d) \r\n", pstPmtStatus[ulIndex].usProgMapPid);
						HxLOG_Debug("PMT (%d)th hAction (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x)\r\n",ulIndex, pContext->ulViewId, pContext->usUniqueId, pstPmtStatus[ulIndex].usProgMapPid);
					}
					else
					{
						HxLOG_Error("PMT (%d)th ulViewId (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x)\r\n",ulIndex, pContext->ulViewId, pContext->usUniqueId, pstPmtStatus[ulIndex].usProgMapPid);
					}
				}
				if(pContext->ulLimitDmxNum >= MHEG_FILTER_LIMIT)
					break;
			}

		}
	}

	HxLOG_Debug("--- ulViewId (0x%x), ulFilterId (0x%x), e2902Status (0x%x)\r\n", ulViewId, ulFilterId, eStatus);

	return ERR_OK;
}
#endif

#define	_______1_4_MHEG_SEARCHING_TABLE___
static	HERROR local_MHEG_StartService(HUINT32 ulViewId)
{
	HERROR hResult = 0;

	HxLOG_Debug(" START\r\n");

	// Pat Filter Start
	hResult = local_MHEG_StartPatFilter(ulViewId);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("local_MHEG_StartPatFilter Error \n");
	}


	return ERR_OK;
}

static HERROR local_MHEG_StopService(HUINT32 ulViewId, HUINT16 usTsUniqId, HUINT32 ulCurSvcUid)
{
	HERROR hResult = 0;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData;
	MHEG_SI_Context_t 	*pContext = NULL;
	DxService_t 		*pstSvcInfo;
	HINT32				tsuid;
	HINT32				pmtpid;

	HxLOG_Debug("STOP \n");

	// PAT Filter Stop.
	hResult = local_MHEG_StopPatFilter(ulViewId);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("Pat filter stop\r\n");
	}

	hResult = local_MHEG_StopLivePatFilter(ulViewId);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("Live Pat filter stop\r\n");
	}

	// PMT Filter Stop.
	hResult = local_MHEG_StopPmtFilter(ulViewId, TRUE, 0);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("Pmt filter stop\r\n");
	}

	hResult = local_MHEG_StopLivePmtFilter(ulViewId);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("Live Pmt filter stop\r\n");
	}

	// 한 TP에 대한 PAT와 PMT만 저장한다.
	MHEG_RAW_FreeAllSection (usTsUniqId, eSI_TableType_PAT, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	MHEG_RAW_FreeAllSection (usTsUniqId, eSI_TableType_PMT, eMHEG_SI_RAW_DATA_TYPE_LIVE);

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if(pContext == NULL)
	{
		HxLOG_Error("There is something Wrong\r\n");
		return ERR_FAIL;
	}

	MHEG_RawMakeList_GetSemaphore();

	pstMHEGSiRawData = MHEG_GetSiRawData(usTsUniqId,PAT_PID,PAT_TABLE_ID,MHEG_DEMUX_VALUE_DONT_CARE, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	if(pstMHEGSiRawData != NULL)
	{
		if(pstMHEGSiRawData->uniqeId !=  pContext->usUniqueId)
		{
			hResult = MHEG_RemoveSiTblRawData(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
		}
	}

	MHEG_RawMakeList_ReleaseSemaphore();

	if(ulCurSvcUid)
	{
		pstSvcInfo = APK_META_SVC_GetService(ulCurSvcUid);
		if(pstSvcInfo == NULL)
		{
			HxLOG_Error("Service not found:\n");
			return ERR_FAIL;
		}
		tsuid = pstSvcInfo->tsuid;
		pmtpid = pstSvcInfo->pmtPid;
		APK_META_SVC_Delete(pstSvcInfo);

		MHEG_RemoveSiTblPMTDataExceptCurSvc(tsuid,pmtpid, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	}

	return ERR_OK;
}

static  HERROR	local_MHEG_AddCurTableFromBaseSI(HUINT32 ulViewId, HUINT8 *pucRawData, HUINT32 SvcUid, HUINT8 usTableId)
{
	HUINT16					usTsUniqId;
	HUINT8					ucTableId;
	HUINT16					usRawDataLen = 0, usExtTableId = 0;
	DxService_t				*pstSvcInfo;
	HERROR					ulErr = 0;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData = NULL;
	HINT32					pmtpid;

	HxLOG_Debug(" ulViewId(0x%x), SvcUid(0x%x)\n", ulViewId, SvcUid);

	if(pucRawData == NULL)
	{
		HxLOG_Error(" fail to retrieve Raw Data : ulViewId(%d), SvcUid(0x%x) \n", ulViewId, SvcUid);
		return ERR_FAIL;
	}

	/* get Unique Id from Service handle */
	//DB_SVC_GetUniqueIdFromSvcHandle(hSvc, &usTsUniqId);
	/* get service information for default service */
	//if(DB_SVC_GetServiceInfo(hSvc, &stSvcInfo) != ERR_OK)
	//{
	//	HxLOG_Error("\n MHEG SI Error");
	//	return ERR_FAIL;
	//}
	pstSvcInfo = APK_META_SVC_GetService(SvcUid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Error("Service not found:\n");
		return ERR_FAIL;
	}
	pmtpid = pstSvcInfo->pmtPid;
	usTsUniqId = pstSvcInfo->tsuid;
	APK_META_SVC_Delete(pstSvcInfo);


	/* get table_id, table id extension and raw data length from section raw Data */
	ucTableId 	= GET_TABLE_ID_FROM_MHEG_RAWDATA(pucRawData);
	usRawDataLen	= GET_SECTION_LEN_FROM_MHEG_RAWDATA(pucRawData) + 3;
	usExtTableId	= GET_EXT_TABLE_ID_FROM_MHEG_RAWDATA(pucRawData);

	/* make SIRawDataList format */
	pstMHEGSiRawData = (MHEG_SiRawDataList_t*)HLIB_STD_MemAlloc(sizeof(MHEG_SiRawDataList_t));
	if ( pstMHEGSiRawData == NULL )
	{
		HxLOG_Error("\n MHEG SI Error");
		return ERR_FAIL;
	}

	HxLOG_Debug("usTsUniqId(0x%x), usExtTableId(0x%x), usTableId(0x%x)\n", usTsUniqId, usExtTableId, usTableId);

	pstMHEGSiRawData->ulViewId			= ulViewId;
	pstMHEGSiRawData->uniqeId			= usTsUniqId;
	pstMHEGSiRawData->tableIdExtn		= usExtTableId;
	pstMHEGSiRawData->tableId			= usTableId;
	pstMHEGSiRawData->savedRawDataSize	= usRawDataLen;
	pstMHEGSiRawData->savedRawData		= (HUINT8*)HLIB_STD_MemAlloc(usRawDataLen);
	if ( pstMHEGSiRawData->savedRawData	 == NULL )
	{
		HLIB_STD_MemFree(pstMHEGSiRawData);
		HxLOG_Error("\n MHEG SI Error");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstMHEGSiRawData->savedRawData, pucRawData, usRawDataLen);
	pstMHEGSiRawData->pNext				= NULL;

	if(usTableId == PMT_TABLE_ID)
	        pstMHEGSiRawData->pid			= pmtpid;
	else
		pstMHEGSiRawData->pid			= 0;

	ulErr = MHEG_AppendSiTblRawDataToList(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	if( ulErr != ERR_OK )
	{
		HLIB_STD_MemFree(pstMHEGSiRawData->savedRawData);
		HLIB_STD_MemFree(pstMHEGSiRawData);
	}

	UNUSED(ucTableId);

	return ERR_OK;

}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static	HERROR local_MHEG_Streaming_StartService(HUINT32 ulViewId)
{
	HUINT32 nResult = 0;

	HxLOG_Debug("+++ \n");

	// Pat Filter Start
	nResult = local_MHEG_Streaming_StartPatFilter(ulViewId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("[ERROR] Streaming Pat filter star \n");
	}

	HxLOG_Debug("--- \n");

	return ERR_OK;
}

static HERROR local_MHEG_Streaming_StopService(HUINT32 ulViewId, HUINT16 usTsUniqId, HUINT32 ulCurSvcUid)
{
	HUINT32 nResult = 0;
	//SVC_TsInfo_t tsInfo_next;
	MHEG_SiRawDataList_t*	pstMHEGSiRawData;
	MHEG_SI_Context_t 	*pContext = NULL;
	DxService_t			*pstSvcInfo;
	HINT32				tsuid;
	HINT32				pmtpid;

	HxLOG_Debug(" START\r\n");

	// PAT Filter Stop.
	nResult = local_MHEG_Streaming_StopPatFilter(ulViewId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Pat filter stop\r\n");
	}

	// PMT Filter Stop.

	nResult = local_MHEG_Streaming_StopPmtFilter(ulViewId, TRUE, 0);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Pmt filter stop\r\n");
	}

	// 한 TP에 대한 PAT와 PMT만 저장한다.

	MHEG_RAW_FreeAllSection (usTsUniqId, eSI_TableType_PAT, eMHEG_SI_RAW_DATA_TYPE_LIVE);
	MHEG_RAW_FreeAllSection (usTsUniqId, eSI_TableType_PMT, eMHEG_SI_RAW_DATA_TYPE_LIVE);

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if(pContext == NULL)
	{
		HxLOG_Error("There is something Wrong\r\n");
		return ERR_FAIL;
	}

	MHEG_RawMakeList_GetSemaphore();

	pstMHEGSiRawData = MHEG_GetSiRawData(usTsUniqId,PAT_PID,PAT_TABLE_ID,MHEG_DEMUX_VALUE_DONT_CARE, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
	if(pstMHEGSiRawData != NULL)
	{
		if(pstMHEGSiRawData->uniqeId !=  pContext->usUniqueId)
		{
			nResult = MHEG_RemoveSiTblRawData(pstMHEGSiRawData, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
		}
	}
	MHEG_RawMakeList_ReleaseSemaphore();

	if(ulCurSvcUid)
	{
		//DB_SVC_GetServiceInfo(uCurSvc,&stSvcInfo);
		//DB_SVC_GetTsInfo(stSvcInfo.usTsIdx, &tsInfo);
		pstSvcInfo = APK_META_SVC_GetService(ulCurSvcUid);
		if(pstSvcInfo == NULL)
		{
			HxLOG_Error("Service not found:\n");
			return ERR_FAIL;
		}
		tsuid = pstSvcInfo->tsuid;
		pmtpid = pstSvcInfo->pmtPid;
		APK_META_SVC_Delete(pstSvcInfo);

	    MHEG_RemoveSiTblPMTDataExceptCurSvc(tsuid, pmtpid, eMHEG_SI_RAW_DATA_TYPE_STREAMING);
	}

	return ERR_OK;
}

#if 0 //
static HERROR local_MHEG_Streaming_SI_GetComponentTag( MwcRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT8 *ucComponentTag)
{
	HUINT32				ulRet;
	StmIdentDes_t		*streamIdDes = NULL;

	HxLOG_Debug("+++ \n"));

	if( (pstPool == NULL) || (ucComponentTag == NULL) )
	{
		return ERR_FAIL;
	}

	ulRet = MHEG_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulIdx, nSubIdx, TAG_STREAM_IDENTIFIER, 0, (void **)&streamIdDes);
	if ( ulRet != ERR_OK )
	{
		HxLOG_Error("ERROR : Get Descriptor -> TAG_STREAM_IDENTIFIER \n");
		return ERR_FAIL;
	}

	*ucComponentTag = streamIdDes->ucCompTag;
	SILIB_DESC_Delete (streamIdDes);

	HxLOG_Debug("--- \n"));

	return ERR_OK;
}
#endif

static HERROR local_MHEG_Streaming_SI_GetAudioLanguageCode(MhegRawPool *pstPool, HUINT16 usTsUniqId,
														HUINT32 ulIdx,
														HUINT32 nSubIdx,
														HUINT32 nMaxLangNum,
														MhegIso639LangInfo_t *pstIsoLangInfo)
{
	HUINT32				ulRet;
	HUINT32				ulLangDesCnt = 0, ulTargetIdx;
	HINT32				i;
	SIxIso639LangDes_t		*pLangDes = NULL;
	SIxLangInfo_t			*pLangInfo = NULL;
	MhegIso639LangInfo_t 	*pstCurLangInfo;

	HxLOG_Debug("+++ \n");

	if (NULL == pstIsoLangInfo)
		return ERR_FAIL;

	HxSTD_memset(pstIsoLangInfo,0,sizeof(MhegIso639LangInfo_t)*nMaxLangNum);

	/*
	 * Dual mono로 2개의 Language가 2개의 descriptor로 각각 들어 올수도 있고
	 * 1개의 descriptor에 내용이 2개가 들어오는 경우도 있다.
	 */
	ulRet = MHEG_SI_CountPmtElementDescriptor(pstPool, usTsUniqId, ulIdx, nSubIdx, eSIxDESCTAG_ISO_639_LANGUAGE, &ulLangDesCnt);
	if ( ulRet != ERR_OK )
	{
		HxLOG_Debug("--- \n");
		return ERR_FAIL;
	}

	ulTargetIdx=0;

	if (ulLangDesCnt >= nMaxLangNum)
	{
		HxLOG_Error("Audio Language Descriptor Num is over than max: %d]\n", ulLangDesCnt);
		ulLangDesCnt = nMaxLangNum - 1;
	}

	for (i=0; i < ulLangDesCnt; i++)
	{
		ulRet = MHEG_SI_GetPmtElementDescriptor(pstPool, usTsUniqId, ulIdx, nSubIdx, eSIxDESCTAG_ISO_639_LANGUAGE, i, (void **)&pLangDes);
		if ( ulRet == ERR_OK )
		{
			pLangInfo = pLangDes->pstInfo;
			while(NULL != pLangInfo)
			{
				pstCurLangInfo = &pstIsoLangInfo[ulTargetIdx];
				pstCurLangInfo->eAudType = pLangInfo->ucAudioType;
				HxSTD_memcpy(pstCurLangInfo->szIso639LangCode, pLangInfo->szIso639LangCode, 4);
				HxLOG_Error("[%02d][AudioType:0x%X][Audio Language code : %s]\n", ulTargetIdx, pstCurLangInfo->eAudType, (char *)pstCurLangInfo->szIso639LangCode);
				ulTargetIdx++;
				pLangInfo = pLangInfo->pstNext;
			}

			SILIB_DESC_Delete ((SIxDescriptor_t *)pLangDes);
		}
	}

	HxLOG_Debug("--- \n");

	return ERR_OK;
}

static DxAudioCodec_e local_MHEG_Streaming_SI_GetaudCodeFromAACType(HUINT8 ucAACType)
{
	switch (ucAACType)
	{
		case 0x01 :
		case 0x03 :
		case 0x05 :
		case 0x40 :
		case 0x41 :
		case 0x42 :
		case 0x43 :
		case 0x44 :
		case 0x45 :
		case 0x46 :
		case 0x47 :
		case 0x48 :
		case 0x49 :
		case 0x4A :
			return eDxAUDIO_CODEC_AAC_PLUS; //eAUDIO_Codec_HEAAC

		default :
			return eDxAUDIO_CODEC_AAC;//eAUDIO_Codec_AAC
	}
}

static HERROR local_MHEG_Streaming_SI_ParseDescriptor(HUINT32 ulViewId)
{
	HERROR		hErr = ERR_OK;
	HINT32		nDemuxId;
	HINT32		i, j, ulPmtIndex;
	HUINT32		ulPmtSecCnt = 0, ulDesCnt = 0, ulEsElemCnt = 0;
	HUINT32		audioCnt = 0, subttlCnt = 0;
	HUINT16		usTsUniqId;

	MHEG_SI_Context_t	*pContext = NULL;
	MhegRawPool			*pstPool = NULL;
	PmtElementInfo_t	stPmtElemInfo;
	SIxAACDes_t			*pAACDes = NULL;
	SIxSubtitleDes_t	*pSubTitleDes = NULL;
	SIxSubtitleInfo_t	*pSubTitleInfo = NULL;

	HxLOG_Debug("+++ \n");

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if(pContext == NULL)
	{
		HxLOG_Error("ERROR : Context is NULL\r\n");
		return ERR_FAIL;
	}

	usTsUniqId = pContext->usUniqueId;
	nDemuxId = s_nMHEGStreramingDemuxId;

	pstPool = MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_STREAMING);
	if( pstPool == NULL )
	{
		HxLOG_Error("ERROR : SI Pool is NULL\n");
		return ERR_FAIL;
	}

	hErr = MHEG_SI_CountPmtSection (pstPool, pContext->usUniqueId, &ulPmtSecCnt);
	if(hErr != ERR_OK || ulPmtSecCnt == 0)
	{
		HxLOG_Error("ERROR : MHEG_SI_CountPmtSection fail!!\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("Pmt Section Count (%d)\n", ulPmtSecCnt);

	for (ulPmtIndex = 0; ulPmtIndex < ulPmtSecCnt; ulPmtIndex++)
	{
		hErr = MHEG_SI_CountPmtElement( pstPool, usTsUniqId, ulPmtIndex, &ulEsElemCnt);
		if ( (hErr != ERR_OK) || (ulEsElemCnt == 0) )
		{
			HxLOG_Error("MWC_SI_CountPmtElement, hErr:0x%X, usEsElem:%d\n", hErr, ulEsElemCnt);
			return ERR_FAIL;
		}

		HxLOG_Debug("Element Count (%d)\n", ulEsElemCnt);

		for (i = 0; i < ulEsElemCnt; i++)
		{
			hErr = MHEG_SI_GetPmtElementInfo(pstPool, usTsUniqId, ulPmtIndex, i, &stPmtElemInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("MHEG_SI_GetPmtElementInfo, hErr(0x%x), usEsElem(%d)\n", hErr, i);
				continue;
			}

			HxLOG_Debug("Element StreamType(0x%x)\n", stPmtElemInfo.ucStreamType);

			switch (stPmtElemInfo.ucStreamType)
			{
			/* AUDIO */
			case ISO_AUDIO_1 :
			case ISO_AUDIO_2 :
			case ISO_AUDIO_AAC :
			case ISO_AUDIO_4 :
				if ( audioCnt >= MAX_AUDIO_ES_NUM ) break;

	 			pContext->stAudioInfo[audioCnt].usPid = stPmtElemInfo.usElemPid;
				MHEG_SI_FindAudioCodecFromPSI (stPmtElemInfo.ucStreamType, &pContext->stAudioInfo[audioCnt].eAudCodec);
				//local_MHEG_Streaming_SI_GetComponentTag(pstPool, usTsUniqId, ulPmtIndex, i, &pContext->stAudioInfo[audioCnt].ucComponentTag);
				local_MHEG_Streaming_SI_GetAudioLanguageCode(pstPool, usTsUniqId, ulPmtIndex, i, 4, (MhegIso639LangInfo_t *)pContext->stAudioInfo[audioCnt].stIsoLangInfo);

				/* AAC존재 여부를 확인한다. */
				hErr = MHEG_SI_GetPmtElementDescriptor (pstPool, usTsUniqId, ulPmtIndex, i, eSIxDESCTAG_AAC, 0, (void **)&pAACDes);
				if ( (hErr == ERR_OK) && (pAACDes != NULL) )
				{
					/*
					 *	APS Test stream에서 flag = 0, ucAACType = 0으로 들어 오는데 stream은 HE-AAC 라고 한다.
					 *	flag가 없는 경우 그냥 ucStreamType을 따르자. 일단 아래와 같이 처리하고 추후에 더 많은
					 *	stream을 봐서 수정하길 바람.
					 */
					if ( pAACDes->bAACTypeFlag == TRUE )
					{
						pContext->stAudioInfo[audioCnt].eAudCodec		= local_MHEG_Streaming_SI_GetaudCodeFromAACType(pAACDes->ucAACType);
						pContext->stAudioInfo[audioCnt].ucChannelNum	= MHEG_Util_AAC_ProfileLevelToChannelNum(pAACDes->ucAACType);
					}

					SILIB_DESC_Delete ((SIxDescriptor_t *)pAACDes);
				}
				audioCnt++;
				break;

			case ISO_PRIVDATA_1 :
			case ISO_PRIVDATA_2 :
				/* Subtitle Descriptor 존재 여부를 확인한다. */
				hErr = MHEG_SI_CountPmtElementDescriptor(pstPool, usTsUniqId, ulPmtIndex, i, eSIxDESCTAG_SUBTITLING, &ulDesCnt);
				if ( (hErr == ERR_OK) && (ulDesCnt != 0) )
				{
					for ( j = 0; j < ulDesCnt; j++ )
					{
						hErr = MHEG_SI_GetPmtElementDescriptor( pstPool, usTsUniqId, ulPmtIndex, i, eSIxDESCTAG_SUBTITLING, j, (void **)&pSubTitleDes);
						if ( (hErr == ERR_OK) && (pSubTitleDes != NULL) )
						{
							pSubTitleInfo = pSubTitleDes->pstInfo;
							while ( pSubTitleInfo )
							{
								if ( subttlCnt >= MAX_SUBTTL_ES_NUM ) break;

								switch (pSubTitleInfo->ucSubtitleType)
								{
								    /* Normal */
								    case 0x10 :
								    case 0x11 :
								    case 0x12 :
								    case 0x13 :
								    case 0x14 :	/* Freesat HD stream에서 여기로 들어 옴. */
								    /* For the hard of hearing */
								    case 0x20 :
								    case 0x21 :
								    case 0x22 :
								    case 0x23 :
									case 0x24 :
										pContext->stSubttlInfo[subttlCnt].usPid = stPmtElemInfo.usElemPid;
										pContext->stSubttlInfo[subttlCnt].ucType = pSubTitleInfo->ucSubtitleType;
										pContext->stSubttlInfo[subttlCnt].usCompositionPageId = pSubTitleInfo->usCompositionPageId;
										pContext->stSubttlInfo[subttlCnt].usAncillaryPageId = pSubTitleInfo->usAncillaryPageId;
										HxSTD_memcpy(pContext->stSubttlInfo[subttlCnt].aucLangCode, pSubTitleInfo->szIso639LangCode, 4);
										subttlCnt++;
								    	break;

								    default :
								    	break;
								}

								pSubTitleInfo = pSubTitleInfo->pstNext;
							}
							SILIB_DESC_Delete ((SIxDescriptor_t *)pSubTitleDes);

							pSubTitleInfo = NULL;
							pSubTitleDes = NULL;
						}
					}
				}
				break;
			}
		}
	}

	pContext->usAudioNum  = (HUINT16)audioCnt;
	pContext->usSubttlNum = (HUINT16)subttlCnt;

	HxLOG_Debug("--- PMT info : nAudio(%d), nSubtitle(%d)\n", pContext->usAudioNum, pContext->usSubttlNum);

	if (audioCnt > 0 || subttlCnt > 0)
	{
		PORT_ITK_PostMsg(eMHEG_SI_IP_STREAMING_PMT, ulViewId/*handle*/, audioCnt, subttlCnt, 0, 0);
	}

	return ERR_OK;
}
#endif

//local semi parser function
static INLINE void local_si_GetFirstProgElemInfoPos(HUINT8 *pucSection, HUINT8 **ppFirstElement, HINT32 nProgDesLen)
{
	*ppFirstElement = pucSection + 12 + nProgDesLen;
}

static INLINE void local_si_GetNextProgElemInfoPos(HUINT8 *pucElement, HUINT8 **ppNextElement, HUINT32 ulElemDesLen)
{
	*ppNextElement = pucElement + 5 + ulElemDesLen;
}

static INLINE void local_si_GetFirstProgElemDesPos(HUINT8 *pucElement, HUINT8 **ppFirstElemDes)
{
	*ppFirstElemDes = pucElement + 5;
}

static INLINE void local_si_GetNextProgElemDesPos(HUINT8 *pucElemDes, HUINT8 **ppNextElemDes, HUINT32 ulElemDesLen)
{
	*ppNextElemDes = pucElemDes + 2 + ulElemDesLen;
}

static INLINE HBOOL local_si_IsSameDesTag (HUINT32 ulSecDesTag, HUINT32 ulArgDesTag)
{
  	return ((ulArgDesTag == TAG_ALL_DESCRIPTORS) || ((ulArgDesTag & 0xff) == (ulSecDesTag & 0xff))) ? TRUE : FALSE;
}

static INLINE void local_si_GetFirstProgramInfoPos (HUINT8 *pucSection, HUINT8 **ppFirstProg)
{
	*ppFirstProg = pucSection + 8;
}

static INLINE void local_si_GetNextProgramInfoPos (HUINT8 *pucProg, HUINT8 **ppNextProg)
{
	*ppNextProg = pucProg + 4;
}


static HUINT8 *local_si_FindPmtSectionTable (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT16			 usRawIndex;
	HUINT32			 ulPmtSecCnt;
	HUINT8			*pucRaw;
	MhegRawSecTable	*pstTable;

	if (pstPool == NULL)		{ return NULL; }


	usAllUniqueId = DxMAX_TS_NUM; //DB_SVC_GetAllUniqueId();
	pstTable = MHEG_RAW_GetRawTable (pstPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return NULL; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		if (ulPmtSecCnt >= ulIdx)
		{
			usRawIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = MHEG_RAW_GetRawSection (pstPool, (HUINT32)usRawIndex);
			if (pucRaw != NULL)
			{
				return pucRaw;
			}
		}
		else
		{
			ulPmtSecCnt++;
		}
	}

	return NULL;
}

static HUINT8 *local_si_FindPatSectinTable (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usSecIndex;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	HUINT8			*pucRaw;
	MhegRawSecTable	*pstTable;

	if (pstPool == NULL)			return NULL;

	usAllUniqueId = DxMAX_TS_NUM;//DB_SVC_GetAllUniqueId();
	pstTable = MHEG_RAW_GetRawTable (pstPool, eSI_TableType_PAT);

	ulSecCnt = 0;

	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			if (ulSecCnt == ulIdx)
			{
				usSecIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
				pucRaw = MHEG_RAW_GetRawSection (pstPool, (HUINT32)usSecIndex);
				if (pucRaw != NULL)
				{
					return pucRaw;
				}
			}
			else
			{
				ulSecCnt++;
			}
		}
	}


	// elsewhere, return NULL;
	return NULL;
}


#define	___2__GLOBAL_FUNCTIONS____

void MHEG_SI_ProcessTask(void *pParam)
{
	HUINT32				ulRet = 0;
	HMSG				stMsg;
	MHEG_SI_Context_t 	*pContext = NULL;
	HUINT32				ulViewId;
	HUINT32				SvcUid;
	HUINT16				usTsUniqId;
	HUINT8				*pucRawData;
	HUINT8				ucTableId;
	HUINT32				usPmtCnt;
	PatProgInfo_t		stProgInfo;
	HBOOL				bRequstMonitoringPMTFlag;

	while(1)
	{
		ulRet = VK_MSG_Receive(s_ulMHEGSIQId, &stMsg, sizeof(HMSG));
		if (ulRet != VK_OK)
		{
			HxLOG_Debug("fail to create player task. err = 0x%x\n", ulRet);
			continue ;
		}

		ulViewId = stMsg.ulParam;

		switch(stMsg.usMsgClass)
		{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case CMD_MHEG_STREAMING_SI_START:
		case CMD_MHEG_STREAMING_SI_STOP:
		case CMD_MHEG_STREAMING_SI_PAT:
		case CMD_MHEG_STREAMING_SI_PMT:
			pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
			break;
#endif
		default:
			pContext = local_MHEG_SI_GetContext(ulViewId);
			break;
		}

		if (pContext == NULL)
		{
			HxLOG_Error("pContext\r\n");
			continue;
		}

		switch ( stMsg.usMsgClass )
		{
		case CMD_MHEG_SI_START_SEARCH :
			usTsUniqId = stMsg.usParam;
			SvcUid = stMsg.unInfo.aulL[0];

			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_START_SEARCH] ===== usTsUniqId(0x%x) SvcUid(0x%x)"C_CODE_RESET"\n", usTsUniqId, SvcUid);

			ENTER_CRITICAL_PMTSTATUS;
			s_ulMHEGAction = ulViewId;
			s_ulMHEGCurSvcUid = SvcUid;
			s_ulMHEGTryCount = 0;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			s_bMHEGStreamingMode = FALSE;
#endif

			//if(MWC_RM_CheckIfLatestActionHandle(ulViewId) == ERR_OK)
			{
#if 0
			        // If it is not opened, then it shall be initiated at first.
			        if ( local_MHEG_si_GetContextStatus(pContext) == eMHEG_SI_ContextStatus_None )
			        {
				        local_MHEG_StopService(hAction,usTsUniqId);

				        local_MHEG_si_InitContext(pContext, &stMsg);
				        //local_MHEG_si_SetActionHandle(pContext, hAction);
				        local_MHEG_si_SetContextStatus(pContext, eMHEG_SI_ContextStatus_Waiting);
			        }
			        HxLOG_Debug("local_MHEG_si_GetContextStatus(pContext) : %d\n",local_MHEG_si_GetContextStatus(pContext));

			        if ( local_MHEG_si_GetContextStatus(pContext) == eMHEG_SI_ContextStatus_Waiting )
			        {
				        local_MHEG_StartService(hAction);
			        }
			        else if ( local_MHEG_si_GetContextStatus(pContext) == eMHEG_SI_ContextStatus_Running )
			        {
				        HxLOG_Debug("pContext->usUniqueId : %d, usTsUniqId: %d\n",pContext->usUniqueId,usTsUniqId);
				        if(pContext->usUniqueId != usTsUniqId)
				        {
					       local_MHEG_StopService(hAction,usTsUniqId);

					       local_MHEG_si_InitContext(pContext, &stMsg);
					       //local_MHEG_si_SetActionHandle(pContext, hAction);
					       local_MHEG_si_SetContextStatus(pContext, eMHEG_SI_ContextStatus_Running);
					       local_MHEG_StartService(hAction);

				        }
			        }
			        // State Change.
			        local_MHEG_si_SetContextStatus(pContext, eMHEG_SI_ContextStatus_Running);
#else

				local_MHEG_StopService(pContext->ulViewId, pContext->usUniqueId, SvcUid);
				local_MHEG_si_InitContext(pContext, &stMsg);
				local_MHEG_StartService(ulViewId);
#endif
			}

			LEAVE_CRITICAL_PMTSTATUS;

			break;

		case CMD_MHEG_SI_STOP_SEARCH :
			usTsUniqId = stMsg.usParam;
			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_STOP_SEARCH] ===== usTsUniqId(0x%x) "C_CODE_RESET"\n", usTsUniqId);

			ENTER_CRITICAL_PMTSTATUS;
			local_MHEG_StopPatFilter(ulViewId);
			local_MHEG_StopPmtFilter(ulViewId, TRUE, 0);
			LEAVE_CRITICAL_PMTSTATUS;
			break;

		case CMD_MHEG_SI_PAT:
			usTsUniqId = stMsg.usParam;
			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_PAT] ===== usTsUniqId(0x%x) "C_CODE_RESET"\n", usTsUniqId);

			ENTER_CRITICAL_PMTSTATUS;

			//if((MWC_RM_CheckIfLatestActionHandle(ulViewId) == ERR_OK))
			{
			       local_MHEG_ProcessPat_Base (ulViewId, usTsUniqId);
			       local_MHEG_StopPatFilter(ulViewId);
			}
			LEAVE_CRITICAL_PMTSTATUS;
			break;

		case CMD_MHEG_SI_PMT :
			usTsUniqId = stMsg.usParam;
			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_PMT] ===== usTsUniqId(0x%x) "C_CODE_RESET"\n", usTsUniqId);

			//if((MWC_RM_CheckIfLatestActionHandle(ulViewId) == ERR_OK))
			{
				ENTER_CRITICAL_PMTSTATUS;
				ulRet = local_MHEG_ProcessPmt_Base (ulViewId,stMsg.unInfo.aulL[0], stMsg.unInfo.aulL[1]);

				HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_PMT] ===== usTsUniqId(0x%x) "C_CODE_RESET"\n", usTsUniqId);

				if((ulRet == ERR_OK) && (pContext->aucSectionStatus[eSI_TableType_PMT] == eSI_TABLE_RECEIVED))
				{
					HUINT16		ulIndex;

					for(ulIndex=0; ulIndex < pContext->usPmtTotalNum; ulIndex++)
					{
						if(pContext->pstPmtStatus[ulIndex].ulPmtFilterId == stMsg.unInfo.aulL[0])
							break;
					}

					if(pContext->pstPmtStatus[ulIndex].ucMode != eSiFilterMode_Continue)
						local_MHEG_StopPmtFilter(ulViewId, FALSE, pContext->pstPmtStatus[ulIndex].ulPmtFilterId);

				}
				LEAVE_CRITICAL_PMTSTATUS;
			}
			//else
			//{
			//	local_MHEG_StopPmtFilter(ulViewId, TRUE, 0);
			//}
			break;

		case CMD_MHEG_SI_REQUEST_MONITOR_PMT:
			bRequstMonitoringPMTFlag = stMsg.unInfo.aulL[0] ;
			pContext->ulViewId =  stMsg.ulParam;

			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_REQUEST_MONITOR_PMT] ===== bRequstMonitoringPMTFlag(%d)  ViewId(%d)"C_CODE_RESET"\n", bRequstMonitoringPMTFlag, pContext->ulViewId );

			if(bRequstMonitoringPMTFlag == TRUE)
			{
				if(pContext->ulLimitDmxNum < MHEG_FILTER_LIMIT)
				{
					ENTER_CRITICAL_PMTSTATUS;

					if(pContext->usPmtTotalNum)
					{
					        local_MHEG_StopMonitorPmtFilter(pContext->ulViewId );
						for(usPmtCnt=0; usPmtCnt < pContext->usPmtTotalNum; usPmtCnt++)
						{
							if(pContext->pstPmtStatus[usPmtCnt].usProgMapPid == stMsg.usParam && (pContext->pstPmtStatus[usPmtCnt].ucStatus  == eSI_TABLE_RECEIVED))
							{
								ulRet = MHEG_SI_GetPatProgramInfo (MHEG_Raw_GetRawPool(eMHEG_SI_RAW_DATA_TYPE_LIVE), pContext->usUniqueId, 0,  usPmtCnt, &stProgInfo);
								if (ulRet == ERR_OK)
								{
									ulRet = local_MHEG_StartPmtFilter(pContext->ulViewId, pContext->usUniqueId, eSiFilterMode_Continue,stProgInfo.usProgMapPid, stProgInfo.usProgNum,usPmtCnt);
									pContext->pstPmtStatus[usPmtCnt].ucMode		= eSiFilterMode_Continue;

									if ((ulRet == ERR_OK))
									{
										pContext->pstPmtStatus[usPmtCnt].ucStatus  = eSI_TABLE_WAITING;
										HxLOG_Debug("[PMT MONITOR REREQUEST] usProgMapPid (0x%d) \r\n", pContext->pstPmtStatus[usPmtCnt].usProgMapPid);
										HxLOG_Debug("PMT (%d)th hAction (0x%x), usUniqueId (0x%x), usProgMapPid (0x%x)\r\n",usPmtCnt, pContext->ulViewId, pContext->usUniqueId, pContext->pstPmtStatus[usPmtCnt].usProgMapPid);
									}

								}
							}
						}
				        }
				        LEAVE_CRITICAL_PMTSTATUS;
			        }
			        else
				{
					ulRet = ERR_FAIL;
					HxLOG_Debug("[PMT MONITOR REREQUEST] ulRet (0x%d) \r\n", ulRet);
				}
			}
			break;

		case CMD_MHEG_CURRENT_TABLE_ADD :

			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_CURRENT_TABLE_ADD] ===== TableType(%d) "C_CODE_RESET"\n", stMsg.usParam );

			pucRawData = (HUINT8 *)stMsg.unInfo.aulL[0];
			if(pucRawData == NULL)
			{
				HxLOG_Warning("pucRawData is Null \n");
				break;
			}

			if(stMsg.usParam == eSI_TableType_PMT)
			{
				ucTableId = PMT_TABLE_ID;
			}
			else if(stMsg.usParam == eSI_TableType_PAT)
			{
				ucTableId = PAT_TABLE_ID;
			}
			else
			{
				HxLOG_Warning("UnKnoew Table Type(%d) \n", stMsg.usParam);
				break;
			}

			local_MHEG_AddCurTableFromBaseSI(ulViewId, pucRawData, stMsg.unInfo.aulL[1], ucTableId);
			if(pucRawData != NULL)
				HLIB_STD_MemFree(pucRawData);
			break;

		case CMD_MHEG_SI_START_LIVE_SECTION :

			HxLOG_Debug(C_CODE_F_RED_B_BLACK"[CMD_MHEG_SI_START_LIVE_SECTION] ===== ulLivePatFilterId(0x%x) ulPmtFilterId(0x%x) ulRequestSvcUid(0x%x)"C_CODE_RESET"\n",
				pContext->ulLivePatFilterId,  pContext->stLivePmtStatus.ulPmtFilterId,stMsg.unInfo.aulL[0]);

			if (MHEG_FILTER_ID_NULL != pContext->ulLivePatFilterId)
			{
				MHEG_SEF_StopSecFilter (pContext->ulLivePatFilterId);
				pContext->ulLivePatFilterId = MHEG_FILTER_ID_NULL;
			}

			if (MHEG_FILTER_ID_NULL != pContext->stLivePmtStatus.ulPmtFilterId)
			{
				MHEG_SEF_StopSecFilter (pContext->stLivePmtStatus.ulPmtFilterId);
				pContext->stLivePmtStatus.ulPmtFilterId = MHEG_FILTER_ID_NULL;
			}

			pContext->ulRequestSvcUid = stMsg.unInfo.aulL[0];
			ulRet = local_MHEG_StartLivePatFilter(ulViewId);
			if(ulRet != ERR_OK)
			{
				HxLOG_Error("local_MHEG_StartLivePatFilter Fail !!\n");
			}
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case CMD_MHEG_STREAMING_SI_START:
			{
				HINT32 nDemuxId;

				HxLOG_Debug("[CMD_MHEG_STREAMING_SI_START]\n");
				usTsUniqId = stMsg.usParam;
				SvcUid = stMsg.unInfo.aulL[0];
				nDemuxId = (HINT32)stMsg.unInfo.aulL[1];

				ENTER_CRITICAL_PMTSTATUS;
				s_ulMHEGStreamingAction = ulViewId;
				s_ulMHEGStreamingCurSvcUid = SvcUid;
				s_ulMHEGStreamingTryCount = 0;
				s_nMHEGStreramingDemuxId = nDemuxId;
				s_bMHEGStreamingMode = TRUE;

				//if(MWC_RM_CheckIfLatestActionHandle(hAction) == ERR_OK)
				{

					local_MHEG_Streaming_StopService(pContext->ulViewId, pContext->usUniqueId, SvcUid);
					local_MHEG_si_InitContext(pContext, &stMsg);
					local_MHEG_Streaming_StartService(ulViewId);
				}

				LEAVE_CRITICAL_PMTSTATUS;
			}
			break;

		case CMD_MHEG_STREAMING_SI_PAT:
			usTsUniqId = stMsg.usParam;
			ENTER_CRITICAL_PMTSTATUS;

			//if((MWC_RM_CheckIfLatestActionHandle(hAction) == ERR_OK))
			{
			       local_MHEG_Streaming_ProcessPat_Base (ulViewId, usTsUniqId);
			       local_MHEG_Streaming_StopPatFilter(ulViewId);
			}
			LEAVE_CRITICAL_PMTSTATUS;
			break;

		case CMD_MHEG_STREAMING_SI_PMT :
			usTsUniqId = stMsg.usParam;

			//if((MWC_RM_CheckIfLatestActionHandle(hAction) == ERR_OK))
			{
				ENTER_CRITICAL_PMTSTATUS;
				ulRet = local_MHEG_Streaming_ProcessPmt_Base (ulViewId, stMsg.unInfo.aulL[0], stMsg.unInfo.aulL[1]);
				if((ulRet==ERR_OK) && (pContext->aucSectionStatus[eSI_TableType_PMT] == eSI_TABLE_RECEIVED))
				{
					HUINT16		ulIndex;

					for(ulIndex=0; ulIndex < pContext->usPmtTotalNum; ulIndex++)
					{
						if(pContext->pstPmtStatus[ulIndex].ulPmtFilterId == stMsg.unInfo.aulL[0])
							break;
					}

					if(pContext->pstPmtStatus[ulIndex].ucMode != eSiFilterMode_Continue)
						local_MHEG_Streaming_StopPmtFilter(ulViewId, FALSE, pContext->pstPmtStatus[ulIndex].ulPmtFilterId);

					if (s_bMHEGStreamingMode == TRUE)
					{
						local_MHEG_Streaming_SI_ParseDescriptor(ulViewId);
					}
				}
				LEAVE_CRITICAL_PMTSTATUS;
			}
			//else
			//{
			//	local_MHEG_Streaming_StopPmtFilter(ulViewId, TRUE, 0);
			//}
			break;

		case CMD_MHEG_STREAMING_SI_STOP :
			HxLOG_Debug("[CMD_MHEG_STREAMING_SI_STOP]\n");
			usTsUniqId = stMsg.usParam;
			ENTER_CRITICAL_PMTSTATUS;
			local_MHEG_Streaming_StopPatFilter(ulViewId);
			local_MHEG_Streaming_StopPmtFilter(ulViewId, TRUE, 0);
			LEAVE_CRITICAL_PMTSTATUS;
			break;
#endif
		default :
			break;
		}
	}
}


HULONG		MHEG_SI_GetQid(void)
{
	return	s_ulMHEGSIQId;
}

void MHEG_SI_Init(void)
{
	HUINT32		ulError = 0;
	HUINT8		nAct;

	MHEG_Raw_InitModule();

	HxSTD_memset(s_stMHEGSiContext, 0, NUM_MAX_ACTION*(sizeof(MHEG_SI_Context_t)));

	ulError = VK_SEM_Create(&s_ulMhegPmtStatusSemId, "PmtStatusMgr", VK_SUSPENDTYPE_PRIORITY);
	if(ulError != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		return ;
	}

	/* initialize contexts */
	for ( nAct = 0; nAct < NUM_MAX_ACTION; nAct++ )
	{
		s_stMHEGSiContext[nAct].ulViewId		= 0;

		s_stMHEGSiContext[nAct].stLivePmtStatus.ulPmtFilterId	= MHEG_FILTER_ID_NULL;
		s_stMHEGSiContext[nAct].stLivePmtStatus.usProgMapPid	= 0xFFFF;
		s_stMHEGSiContext[nAct].stLivePmtStatus.ucStatus		= eSI_TABLE_SKIPPED;
		s_stMHEGSiContext[nAct].stLivePmtStatus.ucMode			= eSiFilterMode_Monitor;
	}

	/* create message queue */
	if((ulError = (HUINT32)VK_MSG_Create(256/*MW_MHEG_MSGQ_SIZE*/, sizeof(HMSG), "MHEG_SI_ProcessQ", &s_ulMHEGSIQId, VK_SUSPENDTYPE_FIFO)) != ERR_OK)
	{
		HxLOG_Error(" Can not create MHEG_Proc message Queue\n");
		return;
	}

	/* create task for main porting layer */
	if((ulError = (HUINT32)VK_TASK_Create(MHEG_SI_ProcessTask, MHEG_RCV_TASK_PRIORITY, MHEG_RCV_TASK_STACK_SIZE, "tMwItkRcv", NULL, &s_ulMHEGSITaskId, 0)) != ERR_OK)
	{
		HxLOG_Error(" Can not create new MHEG_Proc Task...\n");
		return;
	}

	/* start MHEG Task */
	if((ulError = VK_TASK_Start(s_ulMHEGSITaskId)) != ERR_OK)
	{
		HxLOG_Error(" Can not start MHEG_Proc  task !!\n");
		return;
	}


}
void MHEG_SI_Task_Stop(HUINT32 ulViewId, HUINT16 usTsUniqId)
{
	if(s_ulMHEGSITaskId != 0xFFFFFFFF)
	{
		local_MHEG_StopService(ulViewId, usTsUniqId,0);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		local_MHEG_Streaming_StopService(ulViewId, usTsUniqId,0);
#endif
		VK_TASK_Stop(s_ulMHEGSITaskId);
	}
}

void MHEG_SI_Task_Start(void)
{
	if(s_ulMHEGSITaskId != 0xFFFFFFFF)
	{
		VK_TASK_Start(s_ulMHEGSITaskId);
	}
}
void MHEG_SI_Task_Destroy(void)
{
	VK_TASK_Stop(s_ulMHEGSITaskId);
}

HERROR MHEG_SI_SendSectionMessage (MHEG_Command_t eMsgType, HUINT32 ulViewId, HUINT32 ulTableStatus, HUINT32 ulFilterId,HUINT16 usCheckId)
{
	return local_MHEG_SI_SendSectionMessage(eMsgType, ulViewId, ulTableStatus, ulFilterId, usCheckId);
}


HERROR MHEG_SI_GetPmtProgramInfoFromRaw (HUINT8 *pucRaw, PmtProgramInfo_t *pstInfo)
{
	HUINT16		 nSecLen;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	HxSTD_memset(pstInfo, 0, sizeof(PmtProgramInfo_t));
	nSecLen = HxMACRO_Get12Bit(pucRaw + 1);

	if (nSecLen < 5)
	{
		HxLOG_Error("nSecLen < 5");
		return ERR_FAIL;
	}

	pstInfo->usProgNum	 = HxMACRO_Get16Bit (pucRaw + 3);
	pstInfo->ucVer		 = (pucRaw[5] & 0x3e) >> 1;
	pstInfo->bCurNextInd = pucRaw[5] & 0x01;
	pstInfo->usPCRPid	 = HxMACRO_Get13Bit (pucRaw + 8);
	pstInfo->ulCrc32	 = HxMACRO_Get32Bit (pucRaw + nSecLen - 1);			// +3 (After the length) - 4 (CRC) = -1

	HxLOG_Debug("ProgramNum = 0x%x\n", pstInfo->usProgNum);
	HxLOG_Debug("Version num = 0x%x\n", pstInfo->ucVer);
	HxLOG_Debug("Current next indicator = 0x%x\n", pstInfo->bCurNextInd);
	HxLOG_Debug("PCR Pid = 0x%x\n", pstInfo->usPCRPid);
	HxLOG_Debug("Crc 32 = 0x%X\n", pstInfo->ulCrc32);

	return ERR_OK;
}

HERROR MHEG_SI_GetPatProgramInfoFromRaw (HUINT8 *pucRaw,
														HUINT32 ulInfoIdx,
														PatProgInfo_t *pstInfo)
{
	HUINT8		*pucProgSection;
	HUINT32		 ulPatProgCnt;
	HINT32		 nSecLen;


	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pstInfo == NULL)				{ return ERR_FAIL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	ulPatProgCnt = 0;
	local_si_GetFirstProgramInfoPos (pucRaw, &pucProgSection);

	while (nSecLen > 0)
	{
		if (nSecLen < 4)
		{
			HxLOG_Error("nSecLen < 4");
			return ERR_FAIL;
		}

		if (ulInfoIdx == ulPatProgCnt && HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			HxSTD_memset(pstInfo, 0, sizeof(PatProgInfo_t));

			pstInfo->usProgNum = HxMACRO_Get16Bit(pucProgSection);
			pstInfo->usProgMapPid = HxMACRO_Get13Bit(pucProgSection + 2);

			HxLOG_Debug("Program num = 0x%x, Program Map Pid = 0x%x\n", pstInfo->usProgNum,pstInfo->usProgMapPid);

			return ERR_OK;
		}

		//  if program number == 0, that indicate NIT PID.
		if ( HxMACRO_Get16Bit(pucProgSection) != 0 )
		{
			ulPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		local_si_GetNextProgramInfoPos(pucProgSection, &pucProgSection);
	}

	return ERR_FAIL;
}


HERROR MHEG_SI_CountPmtElementFromRaw (HUINT8 *pucRaw, HUINT32 *pulElemCnt)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pulElemCnt == NULL)			{ return ERR_FAIL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 10);

	nSecLen -= nProgInfoLen;
	local_si_GetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Error("nSecLen < 5");
			return ERR_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);
		ulElemCnt++;
		local_si_GetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)usEsInfoLen);
	}

	*pulElemCnt = ulElemCnt;
	//HxLOG_Debug("Pmt Element count = %d\n",ulElemCnt);

	return ERR_OK;
}

HERROR MHEG_SI_GetPmtElementInfoFromRaw (HUINT8 *pucRaw,
															HUINT32 ulElemIdx,
															PmtElementInfo_t *pstInfo)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	local_si_GetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Error("nSecLen < 5");
			return ERR_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit (pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			local_si_GetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)usEsInfoLen);
			continue;
		}

		if (nSecLen < 0)
		{
			HxLOG_Error("nSecLen < 0");
			return ERR_FAIL;
		}

		HxSTD_memset(pstInfo, 0, sizeof(PmtElementInfo_t));
		pstInfo->ucStreamType = HxMACRO_Get8Bit (pucElement);
		pstInfo->usElemPid = HxMACRO_Get13Bit (pucElement + 1);

		HxLOG_Debug("StreamType = 0x%x, ElemPid = 0x%x\n", pstInfo->ucStreamType, pstInfo->usElemPid);

		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR MHEG_SI_GetPmtElementDescriptorFromRaw (HUINT8 *pucRaw,
																	HUINT32 ulElemIdx,
																	HUINT32 ulTag,
																	HUINT32 ulDesIdx,
																	void **ppvDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;
	SIxDescriptor_t*	pucDes = NULL;
	HERROR		 hErr;
	SIxDescriptorHeaderParam_t	 stDesParam;
	SIxSpec_e					 eSpec = eSIxSPEC_DVB;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (ppvDes == NULL)				{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 10);

	nSecLen -= nProgInfoLen;
	local_si_GetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit (pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Error("nSecLen < 0");
			return ERR_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			local_si_GetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		local_si_GetFirstProgElemDesPos (pucElement, &pucElemDes);
		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Error("nEsInfoLen < 2");
				return ERR_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit (pucElemDes);
			ucDesLen = HxMACRO_Get8Bit (pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Error("nEsInfoLen < 0");
				return ERR_FAIL;
			}

			if (local_si_IsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
			{
				local_si_GetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			if (ulDesCnt != ulDesIdx)		// Different Index
			{
				ulDesCnt++;
				local_si_GetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			hErr = SILIB_DESC_New(ucDesTag, eSpec, &stDesParam, pucElemDes, &pucDes);
			if (hErr != ERR_OK)
			{
				//local_si_GetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				//continue;
				return ERR_FAIL;
			}

			*ppvDes = (void *)pucDes;
			return ERR_OK;
		}

		break;
	}

	return ERR_FAIL;
}

HERROR MHEG_SI_CountPmtElementDescriptorFromRaw (HUINT8 *pucRaw,
																	HUINT32 ulElemIdx,
																	HUINT32 ulTag,
																	HUINT32 *pulDesCnt)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pulDesCnt == NULL)			{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	local_si_GetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);

	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Error("nSecLen < 0");
			return ERR_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			local_si_GetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		local_si_GetFirstProgElemDesPos (pucElement, &pucElemDes);

		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Error("nEsInfoLen < 2");
				return ERR_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Error("nEsInfoLen < 0");
				return ERR_FAIL;
			}

			if (local_si_IsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)			// Same TAG
			{
				ulDesCnt++;
			}

			local_si_GetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
		}

		*pulDesCnt = ulDesCnt;
		//HxLOG_Debug("Pmt Element descriptor count = %d\n", ulDesCnt);

		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR MHEG_SI_CountPatSection (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	HUINT16			 usCount;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	MhegRawSecTable	*pstTable;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pulCnt == NULL)			{ return ERR_FAIL; }

	usAllUniqueId = DxMAX_TS_NUM;//DB_SVC_GetAllUniqueId();
	pstTable = MHEG_RAW_GetRawTable (pstPool, eSI_TableType_PAT);

	ulSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			ulSecCnt++;
		}
	}

	*pulCnt = ulSecCnt;
	HxLOG_Debug("Pat section count = %d\n", ulSecCnt);

	return ERR_OK;
}


HERROR MHEG_SI_CountPatProgramFromRaw (HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	HUINT8		*pucProgSection;
	HINT32		 nSecLen, nPatProgCnt;


	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pulCnt == NULL)					{ return ERR_FAIL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_FAIL; }

	*pulCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit (pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	nPatProgCnt = 0;
	local_si_GetFirstProgramInfoPos (pucRaw, &pucProgSection);
	while (nSecLen > 0)
	{
		//  if program number == 0, that indicate NIT PID.
		if (HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			nPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		local_si_GetNextProgramInfoPos (pucProgSection, &pucProgSection);
	}

	*pulCnt = nPatProgCnt;
	HxLOG_Debug("Pat Program count = %d\n",nPatProgCnt);

	return ERR_OK;
}


HERROR MHEG_SI_CountPatProgram (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCnt)
{
	HUINT8		*pucRaw;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pulCnt == NULL)			{ return ERR_FAIL; }

	pucRaw = local_si_FindPatSectinTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_CountPatProgramFromRaw (pucRaw, pulCnt);
}


HERROR MHEG_SI_CountPmtSection (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 *pulSecCnt)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT32			 ulPmtSecCnt;
	MhegRawSecTable	*pstTable;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pulSecCnt == NULL)		{ return ERR_FAIL; }

	usAllUniqueId = DxMAX_TS_NUM;	//DB_SVC_GetAllUniqueId();
	pstTable = MHEG_RAW_GetRawTable (pstPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return ERR_FAIL; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		ulPmtSecCnt++;
	}

	*pulSecCnt = ulPmtSecCnt;
	HxLOG_Debug("Pmt section count = %d\n", ulPmtSecCnt);

	return ERR_OK;
}

HERROR MHEG_SI_CountPmtElement (MhegRawPool *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt)
{
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pnCnt == NULL)				{ return ERR_FAIL; }

	pucRaw = local_si_FindPmtSectionTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_CountPmtElementFromRaw (pucRaw, pnCnt);
}

HERROR MHEG_SI_CountPmtElementDescriptor (MhegRawPool *pstPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 ulTag,
															HUINT32 *pnDesCnt)
{
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pnDesCnt == NULL)			{ return ERR_FAIL; }

	pucRaw = local_si_FindPmtSectionTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_CountPmtElementDescriptorFromRaw (pucRaw, nSubIdx, ulTag, pnDesCnt);
}


HERROR MHEG_SI_GetPmtElementDescriptor (MhegRawPool *pstPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															void **ppvDes)
{
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (ppvDes == NULL)				{ return ERR_FAIL; }

	pucRaw = local_si_FindPmtSectionTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_GetPmtElementDescriptorFromRaw (pucRaw, nSubIdx, ulTag, nDesIdx, ppvDes);
}

HERROR MHEG_SI_GetPmtElementInfo (MhegRawPool *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													PmtElementInfo_t *pstInfo)
{
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	pucRaw = local_si_FindPmtSectionTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_GetPmtElementInfoFromRaw (pucRaw, nSubIdx, pstInfo);
}

HERROR MHEG_SI_GetPatProgramInfo (MhegRawPool *pstPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 ulSubIdx,
												PatProgInfo_t *pstInfo)
{
	HUINT8			*pucRaw;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pstInfo == NULL)		{ return ERR_FAIL; }

	pucRaw = local_si_FindPatSectinTable (pstPool, usTsUniqId, ulIdx);
	return MHEG_SI_GetPatProgramInfoFromRaw (pucRaw, ulSubIdx, pstInfo);
}

HERROR MHEG_SI_GetRawSection (MhegRawPool *pstPool, SiTableType_t eTableType, HUINT16 usTsUniqId, HUINT32 ulSecNum, HBOOL bUseExtId, HUINT16 usExtId, HUINT8 **ppucRaw)
{
	HUINT16			 usCount, usSecIndex;
	HUINT16			 usAllUniqueId, usSiExtId;
	MhegRawSecTable	*pstTable = NULL;
	HUINT8			*pucRaw = NULL , *pucDstRaw = NULL;
	HUINT32			ulSecCnt, ulSecLen;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (ppucRaw == NULL)			{ return ERR_FAIL; }

	ulSecCnt = 0;

	usAllUniqueId = DxMAX_TS_NUM;
	pstTable = MHEG_RAW_GetRawTable (pstPool, eTableType);

	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			usSecIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = MHEG_RAW_GetRawSection (pstPool, (HUINT32)usSecIndex);
			if(pucRaw == NULL)			{continue;}

			usSiExtId	= HxMACRO_Get16Bit (pucRaw + 3);
			if (bUseExtId == TRUE && usExtId != usSiExtId)
			{
				continue;
			}

			if (ulSecCnt >= ulSecNum)
			{
				ulSecLen = (HUINT32)HxMACRO_Get12Bit (pucRaw + 1) + 3;
				pucDstRaw = (HUINT8 *)SI_MEM_Alloc (ulSecLen);
				if (pucDstRaw != NULL)
				{
					VK_memcpy (pucDstRaw, pucRaw, ulSecLen);
					*ppucRaw = pucDstRaw;
				}

				return (pucDstRaw != NULL) ? ERR_OK : ERR_FAIL;
			}
			else
			{
				ulSecCnt ++;
			}
		}
	}

	return ERR_FAIL;
}


//duplicate function - svc_si_LibConvertSiStreamTypeToVideoCodec
HERROR MHEG_SI_FindVideoCodecFromPSI (HUINT32 ulStreamType, DxVideoCodec_e *peCodec)
{
	if (NULL == peCodec)			{ return ERR_FAIL; }

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{
	/* VIDEO */
	case ISO_VIDEO_1:
	case ISO_VIDEO_2:
		*peCodec = eDxVIDEO_CODEC_MPEG2;
		break;

	case ISO_VIDEO_4:
		*peCodec = eDxVIDEO_CODEC_H264;
		break;

	default:
		HxLOG_Critical("\nUnknown StreamType : %d\n", ulStreamType);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR MHEG_SI_FindAudioCodecFromPSI(HUINT32 ulStreamType, DxAudioCodec_e *ucCodec)
{
	if ( ucCodec == NULL )
	{
		return ERR_FAIL;
	}

	/* SI에서 결정 하기로 합의 됨. */
	switch (ulStreamType)
	{

	/* AUDIO */
	case ISO_AUDIO_1 :
		*ucCodec = eDxAUDIO_CODEC_MPEG;	/* ISO/IEC 11172 Audio */
		break;

	case ISO_AUDIO_2 :
		*ucCodec = eDxAUDIO_CODEC_MPEG;	/* ISO/IEC 13818-3 Audio */
		break;

	case ISO_AUDIO_AAC :
		*ucCodec = eDxAUDIO_CODEC_AAC;
		break;

	case ISO_AUDIO_4 :
		*ucCodec = eDxAUDIO_CODEC_AAC_PLUS;
		break;

	default :
		HxLOG_Critical("\nUnknown StreamType : %d\n", ulStreamType);
		break;
	}

	return ERR_OK;
}


#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
HERROR MHEG_Streaming_SI_GetSubtitleInfo(HUINT32 ulViewId, HxLANG_Id_e eLangIdx, HUINT16 *pPid, HUINT16 *pCompositionPageId, HUINT16 *pAncillaryPageId)
{
//	HERROR				hErr;
	HINT32 				i;
	HUINT8 				*pszStr1 = NULL;
	HUINT8				szStr2[4] = {0,};
	MHEG_SI_Context_t	*pContext = NULL;

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\n");
		return ERR_FAIL;
	}

	pszStr1 = (HUINT8*)HLIB_LANG_IdTo639(eLangIdx);
	if(pszStr1 == NULL)
	{
		/* set default(first) value */
		*pPid = pContext->stSubttlInfo[0].usPid;
		*pCompositionPageId = pContext->stSubttlInfo[0].usCompositionPageId;
		*pAncillaryPageId = pContext->stSubttlInfo[0].usAncillaryPageId;

		HxLOG_Error("MWC_UTIL_Get639CodeByLangIdx fail, so set first subtitle -> PID (0x%x), CompositionPageId (%d), AncillaryPageId (%d)\n",
						*pPid, *pCompositionPageId, *pAncillaryPageId);
		return ERR_OK;
	}

	for (i = 0; i < pContext->usSubttlNum; i++)
	{
		// MWC_UTIL_DvbStrcpy DVB string 으로 처리해야 하나 ? 일단 Skip
		HxSTD_StrCpy(szStr2, pContext->stSubttlInfo[i].aucLangCode);
		HLIB_STD_StrLower(szStr2);
		if (HxSTD_StrCmp(pszStr1, szStr2) == 0)
		{
			*pPid = pContext->stSubttlInfo[i].usPid;
			*pCompositionPageId = pContext->stSubttlInfo[i].usCompositionPageId;
			*pAncillaryPageId = pContext->stSubttlInfo[i].usAncillaryPageId;

			HxLOG_Error("set (%d) subtitle -> PID (0x%x), CompositionPageId (%d), AncillaryPageId (%d)\n",
						i, *pPid, *pCompositionPageId, *pAncillaryPageId);

			break;
		}
	}

	if (i == pContext->usSubttlNum)
	{
		/* set default(first) value */
		*pPid = pContext->stSubttlInfo[0].usPid;
		*pCompositionPageId = pContext->stSubttlInfo[0].usCompositionPageId;
		*pAncillaryPageId = pContext->stSubttlInfo[0].usAncillaryPageId;

		HxLOG_Error("set first subtitle -> PID (0x%x), CompositionPageId (%d), AncillaryPageId (%d)\n",
						*pPid, *pCompositionPageId, *pAncillaryPageId);

	}

	HLIB_STD_MemFree(pszStr1);

	return ERR_OK;
}

HERROR MHEG_Streaming_SI_GetAudioInfo(HUINT32 ulViewId, HxLANG_Id_e eLangIdx, HUINT32 ulPid, HUINT16 *usAudiaIdx)
{
	//HERROR				hErr;
	HINT32 				i;
	HUINT8 				*pszStr1 = NULL;
	HUINT8 				szStr2[4] = {0,};
	MHEG_SI_Context_t	*pContext = NULL;

	pContext = local_MHEG_Streaming_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\n");
		return ERR_FAIL;
	}

	pszStr1 = (HUINT8*)HLIB_LANG_IdTo639(eLangIdx);
	if(pszStr1 == NULL)
	{
		/* set default(first) value */
		*usAudiaIdx = 0;
		HxLOG_Error("MWC_UTIL_Get639CodeByLangIdx fail, so set first audio\n");
		return ERR_OK;
	}

	for (i = 0; i < pContext->usAudioNum; i++)
	{
		// TODO: Audio가 1개인 경우만 고려됨. 추후에 multi 지원?
		// MWC_UTIL_DvbStrcpy DVB string 으로 처리해야 하나 ? 일단 Skip
		// prevent 수정(#213792)
		HLIB_STD_StrNCpySafe(szStr2, pContext->stAudioInfo[i].stIsoLangInfo[0].szIso639LangCode, 4);
		HLIB_STD_StrLower(szStr2);
		if (HxSTD_StrCmp(pszStr1, szStr2) == 0)
		{
			if (pContext->stAudioInfo[i].usPid == ulPid)
			{
				*usAudiaIdx = i;
				HxLOG_Error("skip Audio -> Index (%d)\n", *usAudiaIdx);
				HLIB_STD_MemFree(pszStr1);
				return ERR_FAIL;
			}
			*usAudiaIdx = i;
			HxLOG_Error("set Audio -> Index (%d)\n", *usAudiaIdx);
			break;
		}
	}

	if (i == pContext->usAudioNum)
	{
		/* set default(first) value */
		*usAudiaIdx = 0;
		HxLOG_Error("MWC_UTIL_Get639CodeByLangIdx fail, so set first audio\n");
		HLIB_STD_MemFree(pszStr1);
		return ERR_OK;
	}

	HLIB_STD_MemFree(pszStr1);
	return ERR_OK;
}
#endif

#ifdef CONFIG_DEBUG
void MHEG_SI_DEBUG_SetupMediaPatFilter(void)
{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HUINT32 nResult = 0;
	HUINT16 usFilterId = 0;
	HUINT16 usUniqueId;
	HINT32 unDmxId = 5;
	SI_RETRIEVE_PARAM stFilterParam;

	HUINT32	ulViewId;

	HxLOG_Debug("================ Setup MHEG Media Pat ===================\n");

	usUniqueId = s_ulMHEGStreamingTempUniqueId;
	ulViewId = s_ulMHEGStreamingAction;

	nResult = MHEG_SEF_SetDefaultRetrieveParam(&stFilterParam, ulViewId, usUniqueId, unDmxId);

	stFilterParam.usPid = PAT_PID;
	stFilterParam.ucMode = eSiFilterMode_Once;
	stFilterParam.bTableFilter = TRUE;
	stFilterParam.bUseExtId = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.bCrc = TRUE;

	nResult = MHEG_SEF_RetrievePatSection(&stFilterParam, local_MHEG_ParsePat, NULL, &usFilterId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("MHEG_SEF_RetrievePatSection return Error!!!!(0x%X) - [hAction:0x%X][DmxId:%d][UniqueId:0x%X]\n",nResult, ulViewId, unDmxId, usUniqueId);
		return;
	}

	if(usFilterId == (HUINT16)0 || usFilterId == (HUINT16)0xFFFF)
	{
		HxLOG_Error("\n MHEG SI Error");
		return;
	}
#endif
}

void MHEG_SI_DEBUG_PrintMHEGIPStreamingStatus(void)
{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HINT32	i;

	MHEG_SI_Context_t *pContext = NULL;
	HUINT32 ulViewId = s_ulMHEGAction;

	pContext = local_MHEG_SI_GetContext(ulViewId);
	if (pContext == NULL)
	{
		HxLOG_Error("handle is invalid\r\n");
		return;
	}

	// subtitle
	HxLOG_Debug("=========== Print IP Streaming Info =========== \n");
	HxLOG_Debug("=========== Subtitle List =========== \n");

	for (i = 0; i < pContext->usSubttlNum; i++)
	{
		HxLOG_Debug("[%d]=========================\n", i);
		HxLOG_Debug("PID	(0x%x)\n", pContext->stSubttlInfo[i].usPid);
		HxLOG_Debug("Type	(0x%x)\n", pContext->stSubttlInfo[i].ucType);
		HxLOG_Debug("CompositionPageId	(0x%x)\n", pContext->stSubttlInfo[i].usCompositionPageId);
		HxLOG_Debug("AncillaryPageId	(0x%x)\n", pContext->stSubttlInfo[i].usAncillaryPageId);
		HxLOG_Debug("LangCode	(%s)\n", pContext->stSubttlInfo[i].aucLangCode);
	}

	HxLOG_Debug("=========== Audio List =========== \n");

	for (i = 0; i < pContext->usAudioNum; i++)
	{
		HxLOG_Debug("[%d]=========================\n", i);
		HxLOG_Debug("PID	(0x%x)\n", pContext->stAudioInfo[i].usPid);
		HxLOG_Debug("AudCodec	(0x%x)\n", pContext->stAudioInfo[i].eAudCodec);
		HxLOG_Debug("AudType	(0x%x)\n", pContext->stAudioInfo[i].stIsoLangInfo[i].eAudType);
		HxLOG_Debug("Iso639LangCode	(%s)\n", pContext->stAudioInfo[i].stIsoLangInfo[i].szIso639LangCode);
		HxLOG_Debug("ComponentTag	(0x%x)\n", pContext->stAudioInfo[i].ucComponentTag);
		HxLOG_Debug("ChannelNum	(0x%x)\n", pContext->stAudioInfo[i].ucChannelNum);
	}
#endif
}
#endif

/*********************** End of File ******************************/
