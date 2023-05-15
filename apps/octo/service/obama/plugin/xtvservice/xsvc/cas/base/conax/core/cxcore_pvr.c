/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     cxcore_pvr.c
	@brief    cxcore_pvr.c (CONAX PVR)

	Description: pvr °ü¸® ±â´É \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_common.h>
#include <_svc_cas_sub_dev_api.h>
#include <conax.h>
#include <svc_cas.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#include <hlib.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_rec.h>
#include <rlib.h>

#include <cxmgr_action.h>
#include "../local_include/cxcore_pvr.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_ui.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_main.h"
#include <pal_cipher.h>
#include <pal_sys.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/
#define USE_PVR_TASK
#define USE_PVR_SEMAPHORE

#define CX_PVR_TASK_MSGQ_SIZE			64
#define CX_PVR_TASK_PRIORITY 			(VK_TASK_PRIORITY_MW_REF + 5)
#define CX_PVR_TASK_STACK_SIZE			SIZE_32K

#define CONAX_PVR_META_FILE_EXT			".cxm"

#define CONAX_PVR_HASH_SIZE				SHA_1_DIGEST_SIZE
#define CONAX_PVR_BLOCK_SIZE			16

#define CONAX_PVR_CA_PIN_KEY_SIZE		PAL_PVR_CRYPTO_KEY_LEN
#define CONAX_PVR_CA_PIN_SIZE			5

#define CONAX_PVR_10GB_SIZE				(10 * 1024)
#define CONAX_SESSION_META_NAME_LEN		(MD5_DIGEST_SIZE * 2)
#define CONAX_META_VERSION				1
#define CONAX_PVR_META_TIME_MARGIN		1
#define CONAX_META_ENCRYPT
#define CONAX_REC_KEY_ENCRYPT

#if defined(CONFIG_DEBUG)
//#define DEBUG_HMAC
//#define DEBUG_SEMA
#endif

#define CALC_VALID_META_TIMECODE(ulMetaTime, ulTime)		(((ulTime -  CONAX_PVR_META_TIME_MARGIN) <= ulMetaTime) && (ulMetaTime < (ulTime + CONAX_PVR_META_TIME_MARGIN)))


/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/
typedef enum
{
	eCxPvrMsg_RecOpen,
	eCxPvrMsg_RecTimecode,
	eCxPvrMsg_RecClose,
	eCxPvrMsg_PbOpen,
	eCxPvrMsg_PbTimecode,
	eCxPvrMsg_PbClose,
	eCxPvrMsg_ChangeTsrToDelayedRec,
	eCxPvrMsg_CalcMacAndWriteMeta,
	eCxPvrMsg_Remove,
	eCxPvrMsg_ScInsert,
	eCxPvrMsg_Max
}CxPvrMsgType_t;

typedef enum
{
	eConaxPvrState_Close,
	eConaxPvrState_Open,
	eConaxPvrState_Running,
}ConaxPvrtState_e;

typedef enum
{
	eConaxRecType_None,
	eConaxRecType_TSR,
	eConaxRecType_Delayed,
	eConaxRecType_Record,
}ConaxRecType_t;

typedef enum
{
	eConaxFileType_Session = 0x01,
	eConaxFileType_Meta = 0x02,
}ConaxRecFileType_t;

typedef enum
{
	eMetaType_FinterPrint,
	eMetaType_URI,
	eMetaType_MAX
}ConaxMetaType_e;

typedef struct
{
	HUINT32	ulTime;			// 0xffffff = immediately
	HUINT16	usDuration;		// ´ÜÀ§ centi-second (1/100 ÃÊ)
	HUINT16	usPosX;
	HUINT16	usPosY;
	HUINT16	usHeight;
	HUINT8	szText[CX_MAX_FINGERPRINT + 1];
} ConaxMetaFingerprint_t;

typedef struct
{
	HUINT8	ucApsCopyCtrl;
	HUINT8	ucEmiCopyCtrl;
	HUINT8	ucImageConstraint;
	HUINT8	ucRedistributionCtrl;
	HUINT8	ucRetentionLimit;
	HUINT8	ucTrickPlayCtrl;
	HUINT8	ucMaturityRate;
	HUINT8	ucDisableAnalOutput;
} ConaxMetaUri_t;

typedef struct
{
	HUINT32 ulTime;
	ConaxMetaType_e eMetaType;

	union
	{
		ConaxMetaFingerprint_t stFingerPrint;
		DxCopyrightControl_ConaxDrmInfo_t stUri;
	}u;
}ConaxMetaData_t;

typedef struct _ConaxMeta_t_
{
	ConaxMetaData_t d;
	struct _ConaxMeta_t_ *pstPrev;
	struct _ConaxMeta_t_ *pstNext;
}ConaxMeta_t;

typedef struct
{
	HUINT32		ulApplyTime;
	HUINT8		aucRecKey[CONAX_PVR_KEY_SIZE];
	ConaxMeta_t	*pstMetaList;
	HUINT8		aucMac[CONAX_PVR_HASH_SIZE];
}ConaxMetaFile_t;

#define __________CONAX_PVR_SESSION__________

typedef struct _ConaxSession_t_
{
	HUINT32			ulStartTime;
	HUINT32			ulEndTime;
	HCHAR			szMetaName[CONAX_SESSION_META_NAME_LEN + 1];
	HUINT8			aucRecKey[CONAX_PVR_KEY_SIZE];
	ConaxMetaFile_t	stMetaFile;

	struct _ConaxSession_t_ *pstPrev;
	struct _ConaxSession_t_ *pstNext;
}ConaxSession_t;

typedef struct
{
	HBOOL					bUsed;
	Handle_t				hAction;
	Handle_t				hSvc;
	HUINT32					ulContentId;
	ConaxRecType_t			eRecType;
	ConaxPvrtState_e		eState;
	HUINT32					ulTimecode;
	HUINT32					ulFileSize;
	HCHAR					szContentName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	ConaxSession_t			*pstCurrentSession;
	ConaxMeta_t				*pstCurrentMeta;
	DxCopyrightControl_ConaxDrmInfo_t stApplyUri;
	ConaxSession_t			*pstSessionList;
	CxMaturityRatingValue_t	stMaturityRate;
	HULONG					ulMaturityRateTime;
} ConaxContext_t;

/*
  Record Contents - SESSION #0   - META DATA #0-0
                  | (0~10GB)     - META DATA #0-1
                  |              - ...
                  |              - META DATA #0-M
                  |
                  - SESSION #1   - META DATA #1-0
                  | (10~20GB)    - META DATA #1-1
                  |              - ...
                  |              - META DATA #1-M
                  |
                  | ...          ...
                  |
                  - SESSION #N   - META DATA #N-0
                    (N*10~       - META DATA #N-1
                     (N+1)*10GB) - ...
                                 - META DATA #N-M
*/

#if 0	// TODO: delete
#define MAX_MEDIA_CONTEXT_NUM				(1)
#define MAX_MEDIA_DLNACONTENT_USERNUM		(255)

#define CONAX_PVR_SESSION_SLOT_NUM			100	// 1TB ±âÁØÀ¸·Î 100°³ Á¤µµ¸¸...
#define CONAX_PVR_FINGERPRINT_SLOT_NUM		720	// 2½Ã°£ ±âÁØÀ¸·Î ECMÀÌ 10ÃÊ¸¶´Ù µé¾î¿Â´Ù°í °¡Á¤½Ã...

// pn_dsjeong : Specifications and Guidelines - Chipset Pairing STB Integration Kit 6.0
// 5.6.1 Storage consideration¿¡ µû¶ó, 1 ½Ã°£ ´ç ÃÖ¼Ò 20°³ÀÇ URI Slot ÇÊ¿ä.
// ¿©À¯ ÀÖ°Ô ½½·Ô ¿¹¾àÇÏÀÚ -> Instant Recording ±âÁØ durationÀÌ 24½Ã°£(CXHD-5150C model ±âÁØ)ÀÌ¹Ç·Î 24 x 20 ÇØ¼­ ÃÑ 480°³·Î ¼³Á¤.
// °Ô´Ù°¡ 480°³´Â 16 bytes align¿¡µµ ÀûÇÕÇÑ °ª!
#define CONAX_PVR_URI_SLOT_NUM				480
#define CONAX_PVR_URI_START_TIME			0

typedef struct
{
	HUINT32	ulDisplayTime;
	HUINT8	ucDisplayed;
	HBOOL	bFPFromProtectedData;
	ConaxMetaFingerprint_t *pstFp;
} ConaxFingerprint_t;

typedef struct
{
	/**
	  *	16 byte align !!(PAL_CIPHER_BLOCK_SIZE)
	**/
	HUINT8				ucMaturityRateValue;
	ConaxRecSession_t	stSession[CONAX_PVR_SESSION_SLOT_NUM];
	ConaxFingerprint_t	stFingerprint[CONAX_PVR_FINGERPRINT_SLOT_NUM];
	ConaxUri_t			stUri[CONAX_PVR_URI_SLOT_NUM];
	// pn_dsjeong  : ConaxMetaInfo_t ±¸Á¶Ã¼´Â 16 bytes(PAL_CIPHER_BLOCK_SIZE) alignÀ¸·Î ¹Ýµå½Ã ¸ÂÃç¾ß ÇÔ.
	// 16 bytes align ¸ÂÁö ¾ÊÀ¸¸é playback setup´Ü°è Áß meta fileÀ» È®ÀÎÇÏ´Â °úÁ¤¿¡¼­ "Hmac result mismatch" ÀÇ ¿øÀÎÀÌ µÊ.
	// ´Ù¸¥ ¸ðµ¨¿¡ ¿µÇâ ÁÖÁö ¾Ê±â À§ÇØ µû·Î ¼³Á¤.
	HUINT8				aucReserved[8];
	HUINT8				aucSHA1[CONAX_PVR_HASH_SIZE];		// HASH°ªÀ¸·Î Á¦ÀÏ ¸¶Áö¸·À¸·Î Ç×»ó À§Ä¡...
} ConaxMetaInfo_t;

typedef struct
{
	Handle_t		hAction;
	HUINT8			bUsed;
	HUINT32			ulTimecode;
	HUINT32			ulSameTimecode;
	HBOOL			bIncreaseTimecode;
	HINT8			ulSessionPos;
	HUINT8			szMetaName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	HUINT8			szContentName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	ConaxMetaInfo_t	stMetaInfo;
} ConaxContext_t;
#endif

/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/
#ifdef USE_PVR_TASK
STATIC HULONG			s_ulCxPvrTaskId;
STATIC HULONG			s_ulCxPvrTaskMsgQId;
#endif

// chase pbÁß¿¡ readÇÑ meta file¿¡ corruption ¹ß»ýÇÏ¿© ÀÓ½Ã Ãß°¡
#ifdef USE_PVR_SEMAPHORE
STATIC HULONG			s_ulCxPvrRecSemId;
STATIC HULONG			s_ulCxPvrPbSemId;
#endif

STATIC HUINT8			s_aucRootKey[CONAX_PVR_KEY_SIZE];

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
STATIC HUINT8			s_aucCaPin[CONAX_PVR_CA_PIN_SIZE];
#endif

STATIC ConaxContext_t	s_stRecContext[PAL_MAX_REC_INST_NUM];
STATIC ConaxContext_t	s_stPbContext[PAL_MAX_PB_NUM];

#if 0
#if defined(CONFIG_MW_MM_DLNA) && defined(CONFIG_MW_MM_DLNA_DMS)
STATIC ConaxContext_t	s_stCxDlnaSession[MAX_MEDIA_DLNACONTENT_USERNUM];
#endif
#endif

#if defined(DEBUG_SEMA)
HUINT8					ucRecCnt		= 100;
HUINT8					ucPbCnt			= 100;

void DBG_REC_SEMA_GET(HULONG id, HUINT8* pszStr, HUINT32 ulLine)
{
	ucRecCnt++;

	if ((ucRecCnt < 100) || (ucRecCnt > 101))
	{
		VK_Print(" [mmm] get REC sema by %s, %d cnt(%d)\n", pszStr, ulLine, ucRecCnt);
	}
	CX_SEM_Get(id);
}

void DBG_REC_SEMA_REL(HULONG id, HUINT8* pszStr, HUINT32 ulLine)
{
	ucRecCnt--;

	if ((ucRecCnt < 100) || (ucRecCnt > 101))
	{
		VK_Print(" [mmm] rel REC sema by %s, %d cnt(%d)\n", pszStr, ulLine, ucRecCnt);
	}
	CX_SEM_Release(id);
}

void DBG_PB_SEMA_GET(HULONG id, HUINT8* pszStr, HUINT32 ulLine)
{
	ucPbCnt++;

	if ((ucPbCnt < 100) || (ucPbCnt > 101))
	{
		VK_Print(" [mmm] get PB sema by %s, %d cnt(%d)\n", pszStr, ulLine, ucPbCnt);
	}
	CX_SEM_Get(id);
}

void DBG_PB_SEMA_REL(HULONG id, HUINT8* pszStr, HUINT32 ulLine)
{
	ucPbCnt--;

	if ((ucPbCnt < 100) || (ucPbCnt > 101))
	{
		VK_Print(" [mmm] rel PB sema by %s, %d cnt(%d)\n", pszStr, ulLine, ucPbCnt);
	}
	CX_SEM_Release(id);
}

	#define CXREC_SEM_Get(a)		DBG_REC_SEMA_GET(a, __FUNCTION__, __LINE__)
	#define CXREC_SEM_Release(a)	DBG_REC_SEMA_REL(a, __FUNCTION__, __LINE__)

	#define CXPB_SEM_Get(a)			DBG_PB_SEMA_GET(a, __FUNCTION__, __LINE__)
	#define CXPB_SEM_Release(a)		DBG_PB_SEMA_REL(a, __FUNCTION__, __LINE__)
#else
	#define CXREC_SEM_Get(a)		CX_SEM_Get(a)
	#define CXREC_SEM_Release(a)	CX_SEM_Release(a)

	#define CXPB_SEM_Get(a)			CX_SEM_Get(a)
	#define CXPB_SEM_Release(a)		CX_SEM_Release(a)
#endif


#ifdef USE_PVR_SEMAPHORE
	#define CXREC_GET_SEMAPHORE			do {CXREC_SEM_Get(s_ulCxPvrRecSemId);}while(0);
	#define CXREC_RELEASE_SEMAPHORE		do {CXREC_SEM_Release(s_ulCxPvrRecSemId);}while(0);
	#define CXPB_GET_SEMAPHORE			do {CXPB_SEM_Get(s_ulCxPvrPbSemId);}while(0);
	#define CXPB_RELEASE_SEMAPHORE		do {CXPB_SEM_Release(s_ulCxPvrPbSemId);}while(0);
#else
	#define CXREC_GET_SEMAPHORE			do {;}while(0);
	#define CXREC_RELEASE_SEMAPHORE		do {;}while(0);
	#define CXPB_GET_SEMAPHORE			do {;}while(0);
	#define CXPB_RELEASE_SEMAPHORE		do {;}while(0);
#endif


/*******************************************************************/
/********************      Local Functions  	********************/
/*******************************************************************/
#if defined(DEBUG_HMAC)
void _cxcore_pvr_DBG_PrintHMAC(HUINT8* pszComment, HUINT8* pucSha1)
{
	HUINT32	i;

	VK_Print("\n");
	VK_Print("[%s ++ ] =======================================\n", pszComment);
	VK_Print("key : ");
	for (i = 0; i < CONAX_PVR_HASH_SIZE; i++)
	{
		VK_Print("%02X ", pucSha1[i]);
	}
	VK_Print("\n");
	VK_Print("[%s -- ] =======================================\n", pszComment);
	VK_Print("\n");

	return;
}

void _cxcore_pvr_DBG_Print_AllMetaFile(HUINT8* pszComment, ConaxMetaInfo_t* pstMetaInfo)
{
	HUINT32	i, j;
	HBOOL	bPrint	= FALSE;

	VK_Print("\n");
	VK_Print("[%s ++ ] =======================================\n", pszComment);
	VK_Print("stSession: \n");
	for (i = 0; i < CONAX_PVR_SESSION_SLOT_NUM; i++)
	{
		bPrint = FALSE;
		//if (pstMetaInfo->stSession[i].ulApplySessionKeyTime == 0)
		{
			for (j = 0; j < CONAX_PVR_KEY_SIZE; j++)
			{
				if (pstMetaInfo->stSession[i].aucSessionKey[j] != 0)
				{
					bPrint = TRUE;
				}
			}
		}

		if (bPrint != FALSE)
		{
			VK_Print(" idx(%d) ssKeyTime(0x%X) key: ", i, pstMetaInfo->stSession[i].ulApplySessionKeyTime);
			for (j = 0; j < CONAX_PVR_KEY_SIZE; j++)
			{
				VK_Print("%02X ", pstMetaInfo->stSession[i].aucSessionKey[j]);
			}
			VK_Print("\n");
		}
	}

	VK_Print("stFingerprint: \n");
	for (i = 0; i < CONAX_PVR_FINGERPRINT_SLOT_NUM; i++)
	{
		if ((pstMetaInfo->stFingerprint[i].ucHasFingerprint == 0)			//
			&& (pstMetaInfo->stFingerprint[i].ulDisplayTime == 0)			//
			&& (pstMetaInfo->stFingerprint[i].ulTime == 0)					//
			&& (pstMetaInfo->stFingerprint[i].usDuration == 0)				//
			&& (pstMetaInfo->stFingerprint[i].usPosX == 0)					//
			&& (pstMetaInfo->stFingerprint[i].usPosY == 0)					//
			&& (pstMetaInfo->stFingerprint[i].usHeight == 0)				//
			&& (pstMetaInfo->stFingerprint[i].ucDisplayed == 0)				//
			&& (pstMetaInfo->stFingerprint[i].bFPFromProtectedData == 0))	//
		{
			// skip print~
			continue;
		}

		VK_Print(" idx(%d) hasFP(%X) time(%X, %X, %X) pos(%X, %X, %X) displayed(%X) Protected(%X) [%s] \n ",		//
					i,																								//
					pstMetaInfo->stFingerprint[i].ucHasFingerprint,													//
					pstMetaInfo->stFingerprint[i].ulDisplayTime, pstMetaInfo->stFingerprint[i].ulTime, pstMetaInfo->stFingerprint[i].usDuration,	//
					pstMetaInfo->stFingerprint[i].usPosX, pstMetaInfo->stFingerprint[i].usPosY, pstMetaInfo->stFingerprint[i].usHeight,				//
					pstMetaInfo->stFingerprint[i].ucDisplayed,														//
					pstMetaInfo->stFingerprint[i].bFPFromProtectedData,												//
					pstMetaInfo->stFingerprint[i].szText															//
		);
	}

	VK_Print("stUri: \n");
	for (i = 0; i < CONAX_PVR_URI_SLOT_NUM; i++)
	{
		if ((pstMetaInfo->stUri[i].ucUriValid == 0)								//
			&& (pstMetaInfo->stUri[i].ulTime == 0)								//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eApsCopyCtrl == 0)			//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eEmiCopyCtrl == 0)			//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eICT == 0)					//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eRedistributionCtrl == 0)	//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eRetentionLimit == 0)		//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eTrickPlayCtrl == 0)		//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eMR == 0)					//
			&& (pstMetaInfo->stUri[i].stConaxDrmInfo.eDisableAnalOutput == 0))	//
		{
			// skip print~
			continue;
		}

		VK_Print(" idx(%d) valid(%d) time(0x%X) drm(%d, %d, %d, %d, %d, %d, %d, %d) \n ",	//
					i,															//
					pstMetaInfo->stUri[i].ucUriValid,							//
					pstMetaInfo->stUri[i].ulTime,								//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eApsCopyCtrl,			//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eEmiCopyCtrl,			//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eICT,					//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eRedistributionCtrl,	//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eRetentionLimit,		//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eTrickPlayCtrl,		//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eMR,					//
					pstMetaInfo->stUri[i].stConaxDrmInfo.eDisableAnalOutput		//
		);
	}

	VK_Print("aucReserved: ");
	for (i = 0; i < 30; i++)
	{
		VK_Print("%02X ", pstMetaInfo->aucReserved[i]);
	}
	VK_Print("\n");

	VK_Print("Sha: ");
	for (i = 0; i < CONAX_PVR_HASH_SIZE; i++)
	{
		VK_Print("%02X ", pstMetaInfo->aucSHA1[i]);
	}
	VK_Print("\n");

	VK_Print("[%s -- ] =======================================\n", pszComment);
	VK_Print("\n");

	return;
}
#endif

#define __________CONAX_PVR_LOCAL_FUNCTION__________

STATIC HERROR _cxcore_pvr_AesCipherProcess(HUINT8 *pucKey, HUINT32 ulKeySize, HUINT8 *pucSrc, HUINT8 *pucDst, HUINT32 ulSize, HBOOL bEncrypt)
{
	SvcCas_Crypt_t stCrypt;

	stCrypt.etCryptMode = eCAS_CRYPT_AES;
	stCrypt.etCryptKey = pucKey ? eCAS_CRYPT_USER_KEY : eCAS_CRYPT_DEFAULT_SYSTEM_KEY;
	stCrypt.pKey = pucKey;
	stCrypt.ulKeySize = ulKeySize;
	stCrypt.pSrcBuffer = pucSrc;
	stCrypt.pDstBuffer = pucDst;
	stCrypt.ulBuffSize = ulSize;

	if(bEncrypt)
		return svc_cas_DevPortSysEncryptData(&stCrypt);
	else
		return svc_cas_DevPortSysDecryptData(&stCrypt);
}

STATIC HERROR _cxcore_pvr_CheckRootKey(void)
{
	ENTER_FUNCTION;

	if(CXAPI_PVR_ReadRootKey() == ERR_OK)
		return ERR_OK;

	if(CXAPI_PVR_GenerateRootKey() != ERR_OK)
		return ERR_FAIL;

	return CXAPI_PVR_ReadRootKey();
}

STATIC HERROR _cxcore_pvr_CheckCaPin(void)
{
	ENTER_FUNCTION;

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
	return CXAPI_PVR_ReadCaPin();
#endif

	return ERR_OK;
}

STATIC HUINT8* _cxcore_pvr_GetRootKey(void)
{
	return s_aucRootKey;
}

STATIC HERROR _cxcore_pvr_GetRandomNumber(HUINT8* pucRandomNumber, HUINT32 ulSize)
{
	int i, fd;
	HINT32	nRandomNumber = 0;

	if (pucRandomNumber == NULL)
	{
		HxLOG_Error("pucRandomNumber NULL\n");
		return ERR_FAIL;
	}

	/*
		From programming point of view,
		reading data from /dev/random will block while read on
		/dev/urandom will return immediately.
	*/
	fd = open("/dev/urandom", O_RDONLY);
	if(fd < 0)
	{
		HxLOG_Error("device opne error\n");
		return ERR_FAIL;
	}

	for (i = 0 ; i < ulSize ; i++)
	{
		if(read(fd, &nRandomNumber, sizeof(HINT32)) != sizeof(HINT32))
		{
			HxLOG_Error("read() error\n");
			nRandomNumber = 0;
		}

		pucRandomNumber[i] = nRandomNumber % 256;
	}

	close(fd);

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_NotifyUpdateKey(ConaxContext_t *pstContext, HUINT32 ulTimecode, HBOOL *pbUpdateKey)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		return ERR_FAIL;
	}

	HxLOG_Warning("UPDATE KEY... Action (%x), Timecode (%d)\n", pstContext->hAction, ulTimecode);

#ifdef USE_PVR_TASK
	if(svc_cas_MgrSubUpdateCasEtcEvent(pstContext->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CX, eSEVT_CAS_UPDATE_KEYINFO, 0, eDxCAS_GROUPID_CX, 0) != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() error\n");
		return ERR_FAIL;
	}
#else
	if(pbUpdateKey != NULL)
	{
		*pbUpdateKey = TRUE;
	}
#endif
	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_RequestPlaybackStop(ConaxContext_t *pstContext, HBOOL bDelete)
{
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		return ERR_FAIL;
	}

	if(svc_cas_MgrSubUpdateCasEtcEvent(pstContext->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CX, eSEVT_CAS_CONAX_PLAYBACK_ERROR, 0, eDxCAS_GROUPID_CX, (HUINT32)bDelete) != ERR_OK)
	{
		HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void _cxcore_pvr_ContextCleanup(ConaxContext_t *pstContext)
{
	ConaxSession_t	*pstSession, *pstSessionFree;
	ConaxMeta_t		*pstMeta, *pstMetaFree;

	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
		pstMeta = pstSession->stMetaFile.pstMetaList;
		while(pstMeta)
		{
			pstMetaFree = pstMeta;
			pstMeta = pstMeta->pstNext;
			CX_MEM_Free(pstMetaFree);
		}

		pstSessionFree = pstSession;
		pstSession = pstSession->pstNext;
		CX_MEM_Free(pstSessionFree);
	}

	pstContext->pstSessionList = NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetRecContextByContentId(HUINT32 ulContentId)
{
	int i;

	for (i = 0 ; i < PAL_MAX_REC_INST_NUM ; i++)
	{
		if (s_stRecContext[i].bUsed == TRUE)
		{
			if(s_stRecContext[i].ulContentId == ulContentId)
			{
				return &s_stRecContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetPbContextByContentId(HUINT32 ulContentId)
{
	int i;

	for (i = 0 ; i < PAL_MAX_PB_NUM ; i++)
	{
		if (s_stPbContext[i].bUsed == TRUE)
		{
			if(s_stPbContext[i].ulContentId == ulContentId)
			{
				return &s_stPbContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_AllocRecContext(void)
{
	int i;

	for (i = 0 ; i < PAL_MAX_REC_INST_NUM ; i++)
	{
		if (s_stRecContext[i].bUsed != TRUE)
		{
			return &s_stRecContext[i];
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetRecContext(Handle_t hAction)
{
	int i;

	for (i = 0 ; i < PAL_MAX_REC_INST_NUM ; i++)
	{
		if (s_stRecContext[i].bUsed == TRUE)
		{
			if(svc_cas_SubSvcRmGetActionId(s_stRecContext[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction))
			{
				return &s_stRecContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetRecContextByActionId(HUINT32 ulActionId)
{
	int i;

	for (i = 0 ; i < PAL_MAX_REC_INST_NUM ; i++)
	{
		if (s_stRecContext[i].bUsed == TRUE)
		{
			if(svc_cas_SubSvcRmGetActionId(s_stRecContext[i].hAction) == ulActionId)
			{
				return &s_stRecContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxSession_t* _cxcore_pvr_GetRecCurrentSession(ConaxContext_t *pstContext)
{
	if(pstContext == NULL)
	{
		return NULL;
	}

	return pstContext->pstCurrentSession;
}

STATIC HERROR _cxcore_pvr_ReleaseRecContext(Handle_t hAction)
{
	ConaxContext_t	*pstContext;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;
	if(_cxcore_pvr_GetPbContextByContentId(pstContext->ulContentId) != NULL)
	{
		HxLOG_Print("TSB or Chase PB...\n");
		goto _RTN;
	}

	_cxcore_pvr_ContextCleanup(pstContext);

_RTN:
	CX_memset(pstContext, 0, sizeof(ConaxContext_t));
	CX_memset(&pstContext->stApplyUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}

STATIC ConaxContext_t* _cxcore_pvr_AllocPbContext(void)
{
	int i;

	for (i = 0 ; i < PAL_MAX_PB_NUM ; i++)
	{
		if (s_stPbContext[i].bUsed != TRUE)
		{
			return &s_stPbContext[i];
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetPbContext(Handle_t hAction)
{
	int i;

	for (i = 0 ; i < PAL_MAX_PB_NUM ; i++)
	{
		if (s_stPbContext[i].bUsed == TRUE)
		{
			if(svc_cas_SubSvcRmGetActionId(s_stPbContext[i].hAction) == svc_cas_SubSvcRmGetActionId(hAction))
			{
				return &s_stPbContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxContext_t* _cxcore_pvr_GetPbContextByActionId(HUINT32 ulActionId)
{
	int i;

	for (i = 0 ; i < PAL_MAX_PB_NUM ; i++)
	{
		if (s_stPbContext[i].bUsed == TRUE)
		{
			if(svc_cas_SubSvcRmGetActionId(s_stPbContext[i].hAction) == ulActionId)
			{
				return &s_stPbContext[i];
			}
		}
	}

	return NULL;
}

STATIC ConaxSession_t *_cxcore_pvr_GetPbCurrentSession(ConaxContext_t *pstContext)
{
	if(pstContext == NULL)
	{
		return NULL;
	}

	return pstContext->pstCurrentSession;
}

STATIC HERROR _cxcore_pvr_ReleasePbContext(Handle_t hAction)
{
	ConaxContext_t	*pstContext;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		return ERR_FAIL;
	}

	CXREC_GET_SEMAPHORE;
	if(_cxcore_pvr_GetRecContextByContentId(pstContext->ulContentId) != NULL)
	{
		HxLOG_Print("TSB or Chase PB...\n");
		goto _RTN;
	}

	_cxcore_pvr_ContextCleanup(pstContext);

_RTN:
	CX_memset(pstContext, 0, sizeof(ConaxContext_t));
	CX_memset(&pstContext->stApplyUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

STATIC void _cxcore_pvr_DisplayMaturityRate(ConaxContext_t *pstContext)
{
	Handle_t hSession, hStbHandle;
	HUINT8 ucProgram, ucSmartcard;
	HULONG ulUnixTime;
	HBOOL bDisplay = FALSE;

	hStbHandle = CXACT_GetStbHandleFromActionHandle(pstContext->hAction);
	hSession = CXSESSION_GetHandleByStbHandle(hStbHandle);
	ucProgram = CXSC_ConvertUriMrToScMr(pstContext->stApplyUri.eMR);
	ucSmartcard = CXSC_GetMaturityRatingLevel(CX_SC_SLOT_0);

	if(VK_CLOCK_GetTime(&ulUnixTime) != VK_OK)
	{
		HxLOG_Error("VK_CLOCK_GetTime() error\n");
	}

	if(ulUnixTime - pstContext->ulMaturityRateTime >= 5)
	{
		bDisplay = TRUE;
	}

	if((pstContext->stMaturityRate.ucProgram != ucProgram) || (pstContext->stMaturityRate.ucSmartcard != ucSmartcard))
	{
		pstContext->stMaturityRate.ucProgram = ucProgram;
		pstContext->stMaturityRate.ucSmartcard = ucSmartcard;
		bDisplay = TRUE;

		if(pstContext->eState != eConaxPvrState_Open)
		{
			CXSESSION_SetCaPinPass(hSession, FALSE);
		}
	}

	if(bDisplay != TRUE)
	{
		return;
	}

	if(CXUI_DisplayMaturityRate(hSession, &pstContext->stMaturityRate, FALSE) != ERR_OK)
	{
		HxLOG_Error("CXUI_DisplayMaturityRate() error\n");
		return;
	}

	pstContext->ulMaturityRateTime = ulUnixTime;
}

STATIC void _cxcore_pvr_CheckOpenTimecode(ConaxContext_t *pstContext, HUINT32 ulTimecode)
{
	ConaxSession_t			*pstSession;
	ConaxMeta_t				*pstMeta = NULL, *pstMetaNext = NULL;

	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstSession == NULL)
	{
		HxLOG_Print("pstSession NULL\n");
		return;
	}

	pstMeta = pstSession->stMetaFile.pstMetaList;
	while(pstMeta)
	{
		if(pstMeta->d.eMetaType == eMetaType_URI)
		{
			pstMetaNext = pstMeta->pstNext;
			while(pstMetaNext)
			{
				if(pstMetaNext->d.eMetaType == eMetaType_URI)
					break;

				pstMetaNext = pstMetaNext->pstNext;
			}

			if(pstMetaNext == NULL)
				break;

			if((pstMeta->d.ulTime < ulTimecode) && (ulTimecode <= pstMetaNext->d.ulTime))
				break;
		}

		pstMeta = pstMeta->pstNext;
	}

	if(pstMeta == NULL)
	{
		HxLOG_Print("pstMeta NULL\n");
		return;
	}

	if (pstMeta->d.eMetaType != eMetaType_URI)
	{
		HxLOG_Error("pstMeta is not for URI\n");
		return;
	}

	if(CXACT_UpdateDrmInfo(pstContext->hAction, (void*)&pstMeta->d.u.stUri) != ERR_OK)
	{
		HxLOG_Error("CXACT_UpdateDrmInfo() error\n");
	}

	CX_memcpy(&pstContext->stApplyUri, &pstMeta->d.u.stUri, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	pstContext->pstCurrentMeta = pstMeta;
}

STATIC void _cxcore_pvr_CheckMetaData(ConaxContext_t *pstContext, HUINT32 ulTimecode)
{
	ConaxSession_t			*pstSession;
	ConaxMeta_t				*pstMeta;
	ConaxMeta_t				*pstMetaNext;
	CxFingerprint_t			stFingerprint;
	Handle_t				hSession;

	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstSession == NULL)
	{
		HxLOG_Print("pstSession NULL\n");
		return;
	}
#if 0
	if(pstSession->pstCurrentMeta == NULL)
	{
		HxLOG_Print("pstSession->pstCurrentMeta NULL\n");
		return;
	}

	pstMeta = pstSession->pstCurrentMeta;
#endif

	pstMeta = pstSession->stMetaFile.pstMetaList;
	while (pstMeta)
	{
		if (pstMeta->d.eMetaType != eMetaType_MAX)
		{
			if (pstMeta->d.ulTime <= ulTimecode)
			{
				pstMetaNext = pstMeta->pstNext;
				if (pstMetaNext != NULL)
				{
					if (pstMetaNext->d.ulTime > ulTimecode)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		pstMeta = pstMeta->pstNext;
	}

	if(pstMeta == NULL || pstMeta == pstContext->pstCurrentMeta)
	{
		HxLOG_Print("pstMeta (%p), pstSession->pstCurrentMeta (%p)\n", pstMeta, pstContext->pstCurrentMeta);
		return;
	}

	pstContext->pstCurrentMeta = pstMeta;

	switch(pstMeta->d.eMetaType)
	{
		case eMetaType_FinterPrint:
			CX_memset(&stFingerprint, 0, sizeof(CxFingerprint_t));

			stFingerprint.ulTime = pstMeta->d.u.stFingerPrint.ulTime;
			stFingerprint.usDuration = pstMeta->d.u.stFingerPrint.usDuration;
			stFingerprint.usPosX = pstMeta->d.u.stFingerPrint.usPosX;
			stFingerprint.usPosY = pstMeta->d.u.stFingerPrint.usPosY;
			stFingerprint.usHeight = pstMeta->d.u.stFingerPrint.usHeight;
			stFingerprint.ulTime = pstMeta->d.u.stFingerPrint.ulTime;
			CX_memcpy(stFingerprint.szText, pstMeta->d.u.stFingerPrint.szText, CX_MAX_FINGERPRINT);

			hSession = CXSESSION_GetHandleByStbHandle(CXACT_GetStbHandleFromActionHandle(pstContext->hAction));
			if(CXUI_FingerprintReceived(&stFingerprint, hSession) != ERR_OK)
			{
				HxLOG_Error("CXUI_FingerprintReceived() error\n");
			}
			break;

		case eMetaType_URI:
			if(CXACT_UpdateDrmInfo(pstContext->hAction, (void*)&pstMeta->d.u.stUri) != ERR_OK)
			{
				HxLOG_Error("CXACT_UpdateDrmInfo() error\n");
			}

			CX_memcpy(&pstContext->stApplyUri, &pstMeta->d.u.stUri, sizeof(DxCopyrightControl_ConaxDrmInfo_t));
			break;

		default:
			break;
	}
}

STATIC void _cxcore_pvr_CheckSessionKey(ConaxContext_t *pstContext, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	ConaxSession_t	*pstSession;

	if(pstContext->ulTimecode < ulTimecode)	// forword
	{
		pstSession = pstContext->pstCurrentSession;

		while(pstSession)
		{
			if((pstSession->ulStartTime <= ulTimecode) && (ulTimecode < pstSession->ulEndTime))
				break;

			pstSession = pstSession->pstNext;
		}

		if(pstSession == NULL || pstSession == pstContext->pstCurrentSession)
		{
			return;
		}
	}
	else	// rewind
	{
		pstSession = pstContext->pstCurrentSession;

		while(pstSession)
		{
			if((pstSession->ulStartTime <= ulTimecode) && (ulTimecode < pstSession->ulEndTime))
				break;

			pstSession = pstSession->pstPrev;
		}

		if(pstSession == NULL || pstSession == pstContext->pstCurrentSession)
		{
			return;
		}
	}

	pstContext->pstCurrentSession = pstSession;

	_cxcore_pvr_NotifyUpdateKey(pstContext, ulTimecode, pbUpdateKey);

	return;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
// ÀúÀå½Ã¿£ uri emi value¸¦ º¯°æÇØ¾ß ÇÑ´Ù
// URI spec 4.1.5 emi_copy_control ÂüÁ¶
STATIC eDxCopyrightControl_ConaxEmiCopyCtrl_e _cxcore_pvr_ConvertEmiValueForRecording(eDxCopyrightControl_ConaxEmiCopyCtrl_e eOriginalEmi)
{
	eDxCopyrightControl_ConaxEmiCopyCtrl_e	eConvertedEmi;

	if ((eOriginalEmi < eDxCopyrightControl_ConaxEmiCopyCtrl_Freely) && (eOriginalEmi > eDxCopyrightControl_ConaxEmiCopyCtrl_Never))
	{
		HxLOG_Error("Invalid parameter !!!\n");
		return eOriginalEmi;
	}

	switch (eOriginalEmi)
	{
	case eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore:
		// pn_dsjeong : Specifications and Guidelines - Chipset Pairing STB Integration Kit 6.0
		// 5.3.2 ¸¦ º¸¸é ECMÀÇ BRIDGE_CONTROL APDU¿¡¼­ Copy No More·Î µé¾î¿À´Â ÀÏÀÌ ¾øÀ½.
		eConvertedEmi = eOriginalEmi;
		break;

	case eDxCopyrightControl_ConaxEmiCopyCtrl_Once:
		eConvertedEmi = eDxCopyrightControl_ConaxEmiCopyCtrl_NoMore;
		break;

	case eDxCopyrightControl_ConaxEmiCopyCtrl_Freely:
		/* fall through */
	case eDxCopyrightControl_ConaxEmiCopyCtrl_Never:
		// ÀÌ °æ¿ì¿¡´Â ÀúÀåÇÏ¸é ¾È µÇ´Âµ¥.. ½Ã³ª¸®¿À ¹ÌÁ¤..
	default:
		eConvertedEmi = eOriginalEmi;
		break;
	}

	HxLOG_Print("EMI Converted !! (%d) -> (%d) \n", eOriginalEmi, eConvertedEmi);

	return eConvertedEmi;
}
#endif

STATIC HERROR _cxcore_pvr_GetStrictMaturityRateValue(Handle_t hAction, Handle_t hSvc, HUINT8* pucMrLv)
{
#define GET_SVCHANDLE_FROM_STBHANDLE(x)		(x & 0x00ffffff)

	Handle_t	hSession, hEcm;
	HERROR		hError;
	HUINT8		ucMaxMr			= 0, ucMrLv	= 0;
	HUINT32		ulCount			= 0, i ;
	Handle_t	ahSession[CX_MAX_NUM_OF_SESSION];
	Handle_t	hStbHandle_Tmp = HANDLE_NULL;

	if (pucMrLv == NULL)
	{
		VK_Print("pucMrLv NULL!!\n");
		return ERR_FAIL;
	}

	CX_memset(ahSession, 0xff, sizeof(ahSession));
	ulCount = CXSESSION_GetAllSessionByScSlot(CX_SC_NONE, ahSession);
	if (ulCount == 0)
	{
		HxLOG_Print("CXSESSION_GetAllSessionByScSlot() error\n");
		return ERR_FAIL;
	}

	// ÀüÃ¼ sessionÀ» µÚÁ®¼­ µ¿ÀÏÇÑ service handleÀ» °¡Áø session¿¡¼­µµ mr value¸¦ °¡Á®¿Â´Ù
	for (i = 0; i < ulCount; i++)
	{
		hSession = ahSession[i];
		if (hSession == HANDLE_NULL)
		{
			continue;
		}

		hStbHandle_Tmp = CXSESSION_GetStbHandle(hSession);
		if (hStbHandle_Tmp == HANDLE_NULL)
		{
			continue;
		}

		if (GET_SVCHANDLE_FROM_STBHANDLE(hStbHandle_Tmp) == hSvc)
		{
			// same service
			hEcm = CXECM_GetHandleBySession(hSession);
			if (hEcm == HANDLE_NULL)
			{
				continue;
			}

			ucMrLv = 0;
			hError = CXECM_GetAccessStatus(hEcm, &ucMrLv);
			if (hError != ERR_OK)
			{
				HxLOG_Print("CXECM_GetAccessStatus() error\n");
				continue;
			}

			// º¸Åë mr service¿¡¼­ live sessionÀº mrÀÌ ÀÖ´Âµ¥ recording sessionÀº mrÀÌ ¾ÆÁ÷ ¾ø´Ù (0)
			// ÀÌ¿¡ live sessionÀÇ mrµµ »ç¿ëÇÒ ¼ö ÀÖµµ·Ï max mrÀ» Ã£´Â´Ù
			if (ucMaxMr < ucMrLv)
			{
				ucMaxMr = ucMrLv;
			}
		}
	}

	*pucMrLv = ucMaxMr;
	HxLOG_Print("Get MR(%d) \n", ucMaxMr);

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_GetMetaFileSize(HUINT8* pszFileName, HUINT32 *pulSize)
{
	HxFILE_t hFile;

	hFile = HLIB_FILE_Open(pszFileName, "r");
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", pszFileName);
		return ERR_FAIL;
	}

	if(HLIB_FILE_Size32(hFile, pulSize) != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Size32().\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_ReadMetaFile(HUINT8* pszFileName, HUINT8* pucData, HUINT32 ulSize)
{
	PAL_FS_UINT32	ulReadSize;
	PAL_FS_INT64	nPosOffset;
	HxFILE_t		hFile;
	HERROR			hError;

	hFile = HLIB_FILE_Open(pszFileName, "r");
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", pszFileName);
		return ERR_FAIL;
	}

	hError = HLIB_FILE_Seek(hFile, 0, eHxFILE_SEEK_SET, &nPosOffset);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hError);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	ulReadSize = HLIB_FILE_Read(hFile, pucData, 1, ulSize);
	if (ulReadSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Read.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_WriteMetaFile(HUINT8* pszFileName, HUINT8* pucData, HUINT32 ulSize)
{
	PAL_FS_INT64	nPosOffset;
	PAL_FS_UINT32	ulWrittenSize;
	HxFILE_t		hFile;
	HERROR			hError;

	hFile = HLIB_FILE_Open(pszFileName, "w");
	if (hFile == NULL)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Open(%s).\n", pszFileName);
		return ERR_FAIL;
	}

	hError = HLIB_FILE_Seek(hFile, 0, eHxFILE_SEEK_SET, &nPosOffset);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hError);
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	ulWrittenSize = HLIB_FILE_Write(hFile, pucData, 1, ulSize);
	if (ulWrittenSize <= 0)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Write.\n");
		HLIB_FILE_Close(hFile);
		return ERR_FAIL;
	}

	HLIB_FILE_Close(hFile);

	return ERR_OK;
}

STATIC HUINT32 _cxcore_pvr_GetSessionCountFromMemory(ConaxContext_t *pstContext)
{
	ConaxSession_t *pstSession;
	HUINT32 ulCount = 0;

	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
		pstSession = pstSession->pstNext;
		ulCount++;
	}

	return ulCount;
}

STATIC HUINT32 _cxcore_pvr_GetMetaCountFromMemory(ConaxSession_t *pstSession)
{
	ConaxMeta_t	*pstMeta;
	HUINT32 ulCount = 0;

	pstMeta = pstSession->stMetaFile.pstMetaList;
	while(pstMeta)
	{
		pstMeta = pstMeta->pstNext;
		ulCount++;
	}

	return ulCount;
}

STATIC HUINT32 _cxcore_pvr_GetMetaCountFromMetaSize(HUINT32 ulMetaSize)
{
	HUINT32 ulCount;

	ulMetaSize -= 4;
	ulMetaSize -= 4;
	ulMetaSize -= CONAX_PVR_KEY_SIZE;
	ulMetaSize -= CONAX_PVR_HASH_SIZE;

	ulCount = ulMetaSize / sizeof(ConaxMetaData_t);

	return ulCount;
}

STATIC HERROR _cxcore_pvr_CalcMacAndWriteMetaFile(Handle_t hAction, ConaxRecFileType_t eFileType)
{
	ConaxContext_t		*pstContext;
	ConaxSession_t		*pstSession;
	ConaxMeta_t			*pstMeta;
	HUINT8				*pucMeta = NULL;
	HUINT32				ulMetaSize, ulPos, ulAlignSize;
	HERROR				hError = ERR_FAIL;
	HCHAR 				szFullMetaName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	SvcMeta_Record_t	stRecInfo;

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		goto _EXIT;
	}

	if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		goto _EXIT;
	}

	if(eFileType & eConaxFileType_Session)
	{
		ulMetaSize = 4 + (_cxcore_pvr_GetSessionCountFromMemory(pstContext) * CONAX_SESSION_META_NAME_LEN) + CONAX_PVR_HASH_SIZE;

		pucMeta = CX_MEM_Alloc(ulMetaSize);
		if(pucMeta == NULL)
		{
			HxLOG_Error("CX_MEM_Alloc() error\n");
			goto _EXIT;
		}

		ulPos = 0;

		HxMACRO_Put32Bit(pucMeta + ulPos, CONAX_META_VERSION);
		ulPos += 4;

		pstSession = pstContext->pstSessionList;
		while(pstSession)
		{
			CX_memcpy(pucMeta + ulPos, pstSession->szMetaName, CONAX_SESSION_META_NAME_LEN);
			ulPos += CONAX_SESSION_META_NAME_LEN;

			pstSession = pstSession->pstNext;
		}

		if(svc_cas_DevPortChiperHmacSha1(_cxcore_pvr_GetRootKey(), CONAX_PVR_KEY_SIZE, pucMeta, ulPos, pucMeta + ulPos) != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevPortChiperHmacSha1() error\n");
			goto _EXIT;
		}

		ulPos += CONAX_PVR_HASH_SIZE;

		if(ulMetaSize != ulPos)
		{
			HxLOG_Error("check META Size (%d), (%d)\n", ulMetaSize, ulPos);
			goto _EXIT;
		}

#if defined(CONAX_META_ENCRYPT)
		// BLOCK aglign °è»ê ÈÄ ÇØ´ç ±æÀÌ¸¸Å­¸¸ ENCRYPT
		ulAlignSize = (ulMetaSize / CONAX_PVR_BLOCK_SIZE) * CONAX_PVR_BLOCK_SIZE;

		if(_cxcore_pvr_AesCipherProcess(NULL, 0, pucMeta, pucMeta, ulAlignSize, TRUE) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
			goto _EXIT;
		}
#endif

		CX_memset(szFullMetaName, 0, PAL_PVR_MAX_FILE_NAME_LENGTH);
		HxSTD_snprintf(szFullMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", stRecInfo.aucPath, stRecInfo.szConaxMetaName);

		if(HLIB_FILE_WriteFile(szFullMetaName, pucMeta, ulMetaSize) != ERR_OK)
		{
			HxLOG_Error("HLIB_FILE_WriteFile() error\n");
			goto _EXIT;
		}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(szFullMetaName, UID_obama, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", szFullMetaName);
		}

		if (HLIB_EXTRA_Chmod(szFullMetaName, 0660) != ERR_OK)
		{
			HxLOG_Error("	Fail to chmod path = %s \n", szFullMetaName);
		}
#endif
		CX_MEM_Free(pucMeta);
		pucMeta = NULL;
	}

	if(eFileType & eConaxFileType_Meta)
	{
		pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
		if(pstSession == NULL)
		{
			HxLOG_Error("pstCurrentSession NULL\n");
			goto _EXIT;
		}

		ulMetaSize = 4 + 4 + CONAX_PVR_KEY_SIZE + (_cxcore_pvr_GetMetaCountFromMemory(pstSession) * sizeof(ConaxMetaData_t)) + CONAX_PVR_HASH_SIZE;

		pucMeta = CX_MEM_Alloc(ulMetaSize);
		if(pucMeta == NULL)
		{
			HxLOG_Error("CX_MEM_Alloc() error\n");
			goto _EXIT;
		}

		ulPos = 0;

		HxMACRO_Put32Bit(pucMeta + ulPos, CONAX_META_VERSION);
		ulPos += 4;

		HxMACRO_Put32Bit(pucMeta + ulPos, pstSession->stMetaFile.ulApplyTime);
		ulPos += 4;

		CX_memcpy(pucMeta + ulPos, pstSession->stMetaFile.aucRecKey, CONAX_PVR_KEY_SIZE);
		ulPos += CONAX_PVR_KEY_SIZE;

		pstMeta = pstSession->stMetaFile.pstMetaList;
		while(pstMeta)
		{
			CX_memcpy(pucMeta + ulPos, &pstMeta->d, sizeof(ConaxMetaData_t));
			ulPos += sizeof(ConaxMetaData_t);

			pstMeta = pstMeta->pstNext;
		}

		// META µ¥ÀÌÅÍ HMAC °è»ê
		if(svc_cas_DevPortChiperHmacSha1(pstSession->aucRecKey, CONAX_PVR_KEY_SIZE, pucMeta, ulPos, pstSession->stMetaFile.aucMac) != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevPortChiperHmacSha1() error\n");
			goto _EXIT;
		}

		CX_memcpy(pucMeta + ulPos, pstSession->stMetaFile.aucMac, CONAX_PVR_HASH_SIZE);
		ulPos += CONAX_PVR_HASH_SIZE;

		if(ulMetaSize != ulPos)
		{
			HxLOG_Error("check META Size (%d), (%d)\n", ulMetaSize, ulPos);
			goto _EXIT;
		}

#if defined(CONAX_META_ENCRYPT)
		// BLOCK aglign °è»ê ÈÄ ÇØ´ç ±æÀÌ¸¸Å­¸¸ ENCRYPT
		ulAlignSize = (ulMetaSize / CONAX_PVR_BLOCK_SIZE) * CONAX_PVR_BLOCK_SIZE;

		if(_cxcore_pvr_AesCipherProcess(NULL, 0, pucMeta, pucMeta, ulAlignSize, TRUE) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
			goto _EXIT;
		}
#endif

		// Encrypted META ÀúÀå
		HxSTD_snprintf(szFullMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", stRecInfo.aucPath, pstSession->szMetaName);
		if(_cxcore_pvr_WriteMetaFile(szFullMetaName, pucMeta, ulMetaSize) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_WriteMetaFile(META) error\n");
			goto _EXIT;
		}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
		if (HLIB_EXTRA_ChownWithParam(szFullMetaName, UID_obama, GID_pvr) != ERR_OK)
		{
			HxLOG_Error("Fail to chown path = %s \n", szFullMetaName);
		}

		if (HLIB_EXTRA_Chmod(szFullMetaName, 0660) != ERR_OK)
		{
			HxLOG_Error("	Fail to chmod path = %s \n", szFullMetaName);
		}
#endif
		CX_MEM_Free(pucMeta);
		pucMeta = NULL;
	}

	hError = ERR_OK;

_EXIT:
	if(pucMeta != NULL)
		CX_MEM_Free(pucMeta);

	CXREC_RELEASE_SEMAPHORE;

	return hError;
}

STATIC HERROR _cxcore_pvr_UpdateMetaFile(ConaxContext_t* pstContext, ConaxRecFileType_t eFileType)
{
#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};
#endif
	HERROR hError;

#ifdef USE_PVR_TASK
	aulMsg[0] = eCxPvrMsg_CalcMacAndWriteMeta;
	aulMsg[1] = pstContext->hAction;
	aulMsg[2] = eFileType;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	// µé¾î¿Ã ¶§ SEMAPHORE¸¦ °É°í µé¾î¿Â´Ù.
	CXREC_RELEASE_SEMAPHORE;
	hError = _cxcore_pvr_CalcMacAndWriteMetaFile(pstContext->hAction, eFileType);
	CXREC_GET_SEMAPHORE;
#endif

	return hError;
}

STATIC HERROR _cxcore_pvr_AddSessionToContext(ConaxContext_t* pstContext)
{
	ConaxSession_t	*pstSession, *pstNewSession;
	unsigned long	ulUnixTime;
	HUINT8			aucMd5Digest[MD5_DIGEST_SIZE];
	int i;

	pstNewSession = CX_MEM_Calloc(sizeof(ConaxSession_t));
	if(pstNewSession == NULL)
	{
		HxLOG_Error("CX_MEM_Calloc() error\n");
		return ERR_FAIL;
	}

	if(pstContext->pstSessionList == NULL)
	{
		pstContext->pstSessionList = pstNewSession;

		pstNewSession->pstPrev = NULL;
		pstNewSession->pstNext = NULL;
	}
	else
	{
		pstSession = pstContext->pstSessionList;

		while(pstSession->pstNext)
			pstSession = pstSession->pstNext;

		pstSession->pstNext = pstNewSession;

		pstNewSession->pstPrev = pstSession;
		pstNewSession->pstNext = NULL;
	}

	pstContext->pstCurrentSession = pstNewSession;

	(void)VK_CLOCK_GetTime(&ulUnixTime);
	ulUnixTime += pstContext->hAction;
	PAL_CIPHER_MD5((HUINT8 *)&ulUnixTime, sizeof(ulUnixTime), aucMd5Digest);

	for(i = 0 ; i < MD5_DIGEST_SIZE ; i++)
	{
		HxSTD_sprintf(pstNewSession->szMetaName + i*2, "%02x", aucMd5Digest[i]);
	}

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_AddMetaToSession(ConaxContext_t* pstContext, ConaxMeta_t *pstMeta)
{
	ConaxSession_t	*pstSession;
	ConaxMeta_t		*pstNewMeta, *pstTemp;

	if(pstMeta == NULL)
	{
		HxLOG_Error("pstMeta NULL\n");
		return ERR_FAIL;
	}

	pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
	if(pstContext  == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		return ERR_FAIL;
	}

	pstNewMeta = CX_MEM_Alloc(sizeof(ConaxMeta_t));
	if(pstNewMeta == NULL)
	{
		HxLOG_Error("CX_MEM_Alloc() error\n");
		return ERR_FAIL;
	}

	CX_memcpy(pstNewMeta, pstMeta, sizeof(ConaxMeta_t));

	if(pstSession->stMetaFile.pstMetaList == NULL)
	{
		pstSession->stMetaFile.pstMetaList = pstNewMeta;

		pstNewMeta->pstPrev = NULL;
		pstNewMeta->pstNext = NULL;
	}
	else
	{
		pstTemp = pstSession->stMetaFile.pstMetaList;

		while(pstTemp->pstNext)
			pstTemp = pstTemp->pstNext;

		pstTemp->pstNext = pstNewMeta;

		pstNewMeta->pstPrev = pstTemp;
		pstNewMeta->pstNext = NULL;
	}

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_DecryptAndCheckMacContentFile(HCHAR *pszFile, HUINT8 **pucPlain, HUINT32 *pulSize)
{
	HxFILE_t 	hFile;
	HUINT8		*pucData = NULL, aucMac[CONAX_PVR_HASH_SIZE];
	HUINT32		ulSize, ulAlignSize;

	hFile = HLIB_FILE_Open(pszFile, "r");
	if(hFile == NULL)
	{
		HxLOG_Error("HLIB_FILE_Open(%s) error\n", pszFile);
		goto _EXIT;
	}

	if(HLIB_FILE_Size32(hFile, &ulSize) != ERR_OK)
	{
		HxLOG_Error("HLIB_FILE_Size32() error\n");
		goto _EXIT;
	}

	pucData = CX_MEM_Alloc(ulSize);
	if(pucData == NULL)
	{
		HxLOG_Error("CX_MEM_Alloc() error\n");
		goto _EXIT;
	}

	if(HLIB_FILE_Read(hFile, pucData, 1, ulSize) != ulSize)
	{
		HxLOG_Error("HLIB_FILE_Read() error\n");
		goto _EXIT;
	}

#if defined(CONAX_META_ENCRYPT)
	// BLOCK aglign °è»ê ÈÄ ÇØ´ç ±æÀÌ¸¸Å­¸¸ DECRYPT
	ulAlignSize = (ulSize / CONAX_PVR_BLOCK_SIZE) * CONAX_PVR_BLOCK_SIZE;

	if(_cxcore_pvr_AesCipherProcess(NULL, 0, pucData, pucData, ulAlignSize, FALSE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		goto _EXIT;
	}
#endif

	if (svc_cas_DevPortChiperHmacSha1(_cxcore_pvr_GetRootKey(), CONAX_PVR_KEY_SIZE, pucData, ulSize - CONAX_PVR_HASH_SIZE, aucMac) != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortChiperHmacSha1() fail\n");
		goto _EXIT;
	}

	// sha1 compare...
	if (CX_memcmp(pucData + (ulSize - CONAX_PVR_HASH_SIZE), aucMac, CONAX_PVR_HASH_SIZE) != 0)
	{
		HxLOG_Error("[01;33 MetaFile SHA1 mismatch !!! [00m\n");
		goto _EXIT;
	}

	*pucPlain = pucData;
	*pulSize = ulSize;

	HLIB_FILE_Close(hFile);

	return ERR_OK;

_EXIT:

	if(hFile != NULL)	HLIB_FILE_Close(hFile);
	if(pucData != NULL)	CX_MEM_Free(pucData);

	return ERR_FAIL;
}

STATIC HERROR _cxcore_pvr_GetSessionFromContentFile(ConaxContext_t* pstContext, SvcMeta_Record_t *pstRecInfo)
{
	ConaxSession_t *pstNewSession, *pstCurrentSession = NULL;
	HCHAR		szMetaName[CONAX_SESSION_META_NAME_LEN + 1] = {0,}, szFullMetaName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	HUINT32		ulMetaSize, ulPos;
	HUINT8		*pucMeta = NULL;
	HERROR		hError = ERR_FAIL;

	HxSTD_snprintf(szFullMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", pstRecInfo->aucPath, pstRecInfo->szConaxMetaName);

	if(_cxcore_pvr_DecryptAndCheckMacContentFile(szFullMetaName, &pucMeta, &ulMetaSize) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_DecryptAndCheckMacContentFile() error\n");
		return ERR_FAIL;
	}

	ulPos = 0;

	if(HxMACRO_Get32Bit(pucMeta + ulPos) != CONAX_META_VERSION)
	{
		HxLOG_Error("META version mismatch\n");
		goto _EXIT;
	}

	ulPos += 4;

	while(ulMetaSize > (ulPos + CONAX_PVR_HASH_SIZE))
	{
		CX_memcpy(szMetaName, pucMeta + ulPos, CONAX_SESSION_META_NAME_LEN);
		ulPos += CONAX_SESSION_META_NAME_LEN;

		HxSTD_snprintf(szFullMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", pstRecInfo->aucPath, szMetaName);

		if(HLIB_FILE_Exist(szFullMetaName) != TRUE)
		{
			HxLOG_Error("HLIB_FILE_Exist(%s) error\n", szFullMetaName);
			goto _EXIT;
		}

		pstNewSession = CX_MEM_Calloc(sizeof(ConaxSession_t));
		if(pstNewSession == NULL)
		{
			HxLOG_Error("CX_MEM_Calloc() error\n");
			goto _EXIT;
		}

		CX_memcpy(pstNewSession->szMetaName, szMetaName, CONAX_SESSION_META_NAME_LEN);

		pstNewSession->pstPrev = pstCurrentSession;
		pstNewSession->pstNext = NULL;

		if(pstContext->pstSessionList == NULL)
			pstContext->pstSessionList = pstNewSession;
		else
			pstCurrentSession->pstNext = pstNewSession;

		pstCurrentSession = pstNewSession;
	}

	if(ulMetaSize != (ulPos + CONAX_PVR_HASH_SIZE))
	{
		HxLOG_Error("check META Size (%d), (%d)\n", ulMetaSize, ulPos);
		goto _EXIT;
	}

	hError = ERR_OK;

_EXIT:
	CX_MEM_Free(pucMeta);
	return hError;
}

STATIC HERROR _cxcore_pvr_GetMetaFromSessionFile(ConaxContext_t *pstContext, SvcMeta_Record_t *pstRecInfo)
{
	ConaxSession_t	*pstSession;
	ConaxMeta_t		*pstMeta = NULL, *pstNewMeta;
	HUINT32 ulMetaSize, ulPos, ulAlignSize;
	HUINT8	*pucMeta = NULL, aucMac[CONAX_PVR_HASH_SIZE];
	HCHAR	szFullPath[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	HERROR	hError = ERR_FAIL;

	pstSession = pstContext->pstSessionList;

	while(pstSession)
	{
		HxSTD_snprintf(szFullPath, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", pstRecInfo->aucPath, pstSession->szMetaName);

		if(_cxcore_pvr_GetMetaFileSize(szFullPath, &ulMetaSize) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_GetMetaFileSize() error\n");
			goto _EXIT;
		}

		pucMeta = CX_MEM_Alloc(ulMetaSize);
		if(pucMeta == NULL)
		{
			HxLOG_Error("CX_MEM_Alloc() error\n");
			goto _EXIT;
		}

		hError = _cxcore_pvr_ReadMetaFile(szFullPath, pucMeta, ulMetaSize);
		if (hError != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_ReadMetaFile() error\n");
			goto _EXIT;
		}

#if defined(CONAX_META_ENCRYPT)
		// BLOCK aglign °è»ê ÈÄ ÇØ´ç ±æÀÌ¸¸Å­¸¸ DECRYPT
		ulAlignSize = (ulMetaSize / CONAX_PVR_BLOCK_SIZE) * CONAX_PVR_BLOCK_SIZE;

		if(_cxcore_pvr_AesCipherProcess(NULL, 0, pucMeta, pucMeta, ulAlignSize, FALSE) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
			goto _EXIT;
		}
#endif

#if defined(CONAX_REC_KEY_ENCRYPT)
		if(_cxcore_pvr_AesCipherProcess(_cxcore_pvr_GetRootKey(), CONAX_PVR_KEY_SIZE, pucMeta + 8, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE, FALSE) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_AesCipherProcess() fail\n");
			goto _EXIT;
		}
#else
		CX_memcpy(pstSession->aucRecKey, pucMeta + 8, CONAX_PVR_KEY_SIZE);
#endif
		if (svc_cas_DevPortChiperHmacSha1(pstSession->aucRecKey, CONAX_PVR_KEY_SIZE, pucMeta, ulMetaSize - CONAX_PVR_HASH_SIZE, aucMac) != ERR_OK)
		{
			HxLOG_Error("svc_cas_DevPortChiperHmacSha1() fail\n");
			goto _EXIT;
		}

		// sha1 compare...
		if (CX_memcmp(pucMeta + (ulMetaSize - CONAX_PVR_HASH_SIZE), aucMac, CONAX_PVR_HASH_SIZE) != 0)
		{
			HxLOG_Error("[01;33 MetaFile (%s) SHA1 mismatch !!! [00m\n", szFullPath);
			goto _EXIT;
		}

		ulPos = 0;

		if(HxMACRO_Get32Bit(pucMeta + ulPos) != CONAX_META_VERSION)
		{
			HxLOG_Error("META version mismatch\n");
			goto _EXIT;
		}

		ulPos += 4;

		pstSession->stMetaFile.ulApplyTime = HxMACRO_Get32Bit(pucMeta + ulPos);
		ulPos += 4;

		CX_memcpy(pstSession->stMetaFile.aucRecKey, pucMeta + ulPos, CONAX_PVR_KEY_SIZE);
		ulPos += CONAX_PVR_KEY_SIZE;

		while(ulMetaSize > (ulPos + CONAX_PVR_HASH_SIZE))
		{
			pstNewMeta = CX_MEM_Alloc(sizeof(ConaxMeta_t));
			if(pstNewMeta == NULL)
			{
				HxLOG_Error("CX_MEM_Alloc() error\n");
				goto _EXIT;
			}

			CX_memcpy(&pstNewMeta->d, pucMeta + ulPos, sizeof(ConaxMetaData_t));
			ulPos += sizeof(ConaxMetaData_t);

			if(pstMeta == NULL)
				pstSession->stMetaFile.pstMetaList = pstNewMeta;
			else
				pstMeta->pstNext = pstNewMeta;

			pstNewMeta->pstPrev = pstMeta;
			pstNewMeta->pstNext = NULL;

			pstMeta = pstNewMeta;
		}

		CX_memcpy(pstSession->stMetaFile.aucMac, pucMeta + ulPos, CONAX_PVR_HASH_SIZE);
		ulPos += CONAX_PVR_HASH_SIZE;

		if(ulMetaSize != ulPos)
		{
			HxLOG_Error("check META Size (%d), (%d)\n", ulMetaSize, ulPos);
			goto _EXIT;
		}

		CX_MEM_Free(pucMeta);

		pstSession = pstSession->pstNext;
	}

	hError = ERR_OK;
	pucMeta = NULL;

_EXIT:
	if(pucMeta != NULL)
		CX_MEM_Free(pucMeta);

	return hError;
}

STATIC HERROR _cxcore_pvr_CalcSessionValidTime(ConaxContext_t* pstContext)
{
	ConaxSession_t	*pstSession;

	pstSession = pstContext->pstSessionList;

	while(pstSession)
	{
		if(pstSession->pstPrev == NULL)
			pstSession->ulStartTime = pstSession->stMetaFile.ulApplyTime;
		else
			pstSession->ulStartTime = pstSession->pstPrev->ulEndTime;

		if(pstSession->pstNext == NULL)
			pstSession->ulEndTime = 0xFFFFFFFF;	// ¸¶Áö¸· Timecode¸¦ ¾Ë ¼ö°¡ ¾ø³×...
		else
			pstSession->ulEndTime = pstSession->pstNext->stMetaFile.ulApplyTime;

		pstSession = pstSession->pstNext;
	}

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_PbDeleteFile(SvcMeta_Record_t* pstRecInfo, ConaxContext_t* pstContext)
{
	HUINT8	szRecFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	HUINT8	szMetaFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	HUINT8	szCxMetaFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	HUINT8	szThumbnailFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	HUINT8	szMediaInfoFileName[PAL_PVR_MAX_FILE_NAME_LENGTH];
	HERROR	hError = ERR_OK;

	CX_memset(szRecFileName, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);
	CX_memset(szMetaFileName, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);
	CX_memset(szCxMetaFileName, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);
	CX_memset(szThumbnailFileName, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);
	CX_memset(szMediaInfoFileName, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);

	snprintf(szRecFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", pstRecInfo->aucPath, pstRecInfo->aucFileName, EXT_TS_FILE);
	snprintf(szMetaFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", pstRecInfo->aucPath, pstRecInfo->aucFileName, EXT_META_FILE);
	snprintf(szCxMetaFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", pstRecInfo->aucPath, pstRecInfo->aucFileName, CONAX_PVR_META_FILE_EXT);
	snprintf(szThumbnailFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", pstRecInfo->aucPath, pstRecInfo->aucFileName, EXT_THUMBNAIL_FILE);
	snprintf(szMediaInfoFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", pstRecInfo->aucPath, pstRecInfo->aucFileName, EXT_MEDIA_INFO_FILE);

	hError = HLIB_FILE_Delete(szRecFileName);
	if (hError != ERR_OK)
	{
		HxLOG_Error("[%s:%d] PAL_FS_Delete() failed! file(%s) \n", __func__, __LINE__, szRecFileName);
		return hError;
	}

	if (svc_cas_DevFsIsFileExist((char*)szMetaFileName) == TRUE)
	{
		if (svc_cas_DevFsDelete(0, (char*)szMetaFileName) != ERR_OK)
		{
			HxLOG_Error("[%s:%d] PAL_FS_Delete() failed! file(%s) \n", __func__, __LINE__, szMetaFileName);
		}
	}

	if (svc_cas_DevFsIsFileExist((char*)szCxMetaFileName) == TRUE)
	{
		if (svc_cas_DevFsDelete(0, (char*)szCxMetaFileName) != ERR_OK)
		{
			HxLOG_Error("[%s:%d] PAL_FS_Delete() failed! file(%s) \n", __func__, __LINE__, szCxMetaFileName);
		}
	}

	if (svc_cas_DevFsIsFileExist((char*)szThumbnailFileName) == TRUE)
	{
		if (svc_cas_DevFsDelete(0, (char*)szThumbnailFileName) != ERR_OK)
		{
			HxLOG_Error("[%s:%d] PAL_FS_Delete() failed! file(%s) \n", __func__, __LINE__, szThumbnailFileName);
		}
	}

	if (svc_cas_DevFsIsFileExist((char*)szMediaInfoFileName) == TRUE)
	{
		if (svc_cas_DevFsDelete(0, (char*)szMediaInfoFileName) != ERR_OK)
		{
			HxLOG_Error("[%s:%d] PAL_FS_Delete() failed! file(%s) \n", __func__, __LINE__, szMediaInfoFileName);
		}
	}

	return hError;
}

STATIC HUINT32 _cxcore_pvr_GetRecordFileSize(ConaxContext_t *pstContext)
{
	SvcCas_DevFsFileState_t	stFileState;
	HCHAR	szFileName[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];
	HUINT32	ulIndex = 0;
	HUINT64 ullFileSize = 0;
	HERROR	ulRet;

	switch(pstContext->eRecType)
	{
		case eConaxRecType_TSR:
			do
			{
				snprintf(szFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s.%d", pstContext->szContentName, ulIndex);

				if(HLIB_FILE_Exist(szFileName) != TRUE)
					break;

				ulRet = svc_cas_DevFsGetFileInfo(szFileName, PAL_FS_INFO_NAMETYPE, &stFileState);
				if (ulRet != ERR_OK)
				{
					HxLOG_Error("svc_cas_DevFsGetFileInfo() error, ulRet:0x%X\n", ulRet);
					break;
				}

				ullFileSize += stFileState.ulFileSize;
				ulIndex++;
			}while(1);
			break;

		case eConaxRecType_Delayed:
			do
			{
				snprintf(szFileName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s/chunk.%d", pstContext->szContentName, ulIndex);

				if(HLIB_FILE_Exist(szFileName) != TRUE)
					break;

				ulRet = svc_cas_DevFsGetFileInfo(szFileName, PAL_FS_INFO_NAMETYPE, &stFileState);
				if (ulRet != ERR_OK)
				{
					HxLOG_Error("svc_cas_DevFsGetFileInfo() error, ulRet:0x%X\n", ulRet);
					break;
				}

				ullFileSize += stFileState.ulFileSize;
				ulIndex++;
			}while(1);
			break;

		case eConaxRecType_Record:
			ulRet = svc_cas_DevFsGetFileInfo(pstContext->szContentName, PAL_FS_INFO_NAMETYPE, &stFileState);
			if (ulRet != ERR_OK)
			{
				HxLOG_Error("svc_cas_DevFsGetFileInfo() error, ulRet:0x%X\n", ulRet);
				CXREC_RELEASE_SEMAPHORE;
				return ulRet;
			}

			ullFileSize = stFileState.ulFileSize;
			break;

		default:
			break;
	}

	/*
		ÆÄÀÏ Å©±â¸¦ MB ´ÜÀ§·Î return ÇÑ´Ù.
		if¿¡¼­ long long typeÀÌ ºñ±³°¡ Àß µÇÁö ¾Ê´Â´Ù...(°°Àº long long ³¢¸®ÀÇ ºñ±³ÀÓ¿¡µµ ºÒ±¸ÇÏ°í...)
	*/
	return (HUINT32)(ullFileSize >> 20);
}

STATIC HERROR _cxcore_pvr_RecOpen(Handle_t hAction, HUINT32 ulContentId, Handle_t hSvc, SvcCas_ActionType_e eActionType)
{
	SvcMeta_Record_t	stRecInfo;
	ConaxContext_t		*pstContext = NULL;

	HxLOG_Print("hAction (%x), ulContentId (%d)\n", hAction, ulContentId);

	if(SVC_META_GetRecordInfo(ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		return ERR_FAIL;
	}

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContextByActionId(svc_cas_SubSvcRmGetActionId(hAction));
	if(pstContext != NULL)
	{
		HxLOG_Warning("RecAction (%d) already exist\n", svc_cas_SubSvcRmGetActionId(hAction));
		_cxcore_pvr_ReleaseRecContext(pstContext->hAction);
	}

	pstContext = _cxcore_pvr_AllocRecContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->bUsed = TRUE;
	pstContext->hAction = hAction;
	pstContext->hSvc = hSvc;
	pstContext->ulContentId = ulContentId;
	pstContext->eRecType = eActionType == eCAS_ActionType_Tsr_Rec ? eConaxRecType_TSR : eConaxRecType_Record;
	pstContext->ulTimecode = 0;

	HxSTD_snprintf(pstContext->szContentName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_TS_FILE);

#if 0
	if(_cxcore_pvr_AddSessionToContext(pstContext) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AddSessionToContext() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

#if 0
	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Session) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}
#endif

	pstContext->pstCurrentSession = pstContext->pstSessionList;

	// maturity rate Á¤º¸ ÀúÀå...
	_cxcore_pvr_GetStrictMaturityRateValue(hAction, hSvc, &stMeta.d.u.stUri.eMR);

	// ³ªÁß¿¡ default uri°¡ ¹ß»ýÇÒ ÅÙµ¥ ±¸Áö ÇØ¾ßÇÏ³ª...?
#if defined(CONFIG_MW_CAS_CONAX_URI)
	CX_memset(&stMeta, 0, sizeof(ConaxMeta_t));

	stMeta.d.ulTime = pstContext->ulTimecode;
	stMeta.d.eMetaType = eMetaType_URI;

	if(CXSC_GetDefaultUri(&stMeta.d.u.stUri) != ERR_OK)
	{
		HxLOG_Error("CXSC_GetDefaultUri() fail !!!\n");
	}

	stMeta.d.u.stUri.eEmiCopyCtrl = _cxcore_pvr_ConvertEmiValueForRecording(stMeta.d.u.stUri.eEmiCopyCtrl);

	if(_cxcore_pvr_AddMetaToSession(pstContext, &stMeta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AddMetaToSession() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Meta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}
#endif
#endif

	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Session) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	HxSTD_snprintf(stRecInfo.szConaxMetaName, FS_MAX_FILE_NAME_LEN, "%s%s", stRecInfo.aucFileName, CONAX_PVR_META_FILE_EXT);
	if(SVC_META_SetRecordInfo(ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->eState = eConaxPvrState_Open;
	_cxcore_pvr_NotifyUpdateKey(pstContext, 0, NULL);

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_RecChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	ConaxContext_t			*pstContext;
	HUINT32					ulFileSize = 0;

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->ulTimecode = ulTimecode;

	switch(pstContext->eState)
	{
		case eConaxPvrState_Open:
			pstContext->eState = eConaxPvrState_Running;
			break;

		case eConaxPvrState_Running:
			ulFileSize = _cxcore_pvr_GetRecordFileSize(pstContext);

			if((ulFileSize - pstContext->ulFileSize) >= CONAX_PVR_10GB_SIZE)
			{
				pstContext->ulFileSize = ulFileSize;
				_cxcore_pvr_NotifyUpdateKey(pstContext, ulTimecode, pbUpdateKey);
			}
			break;

		default:
			CXREC_RELEASE_SEMAPHORE;
			return ERR_FAIL;
	}

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_RecClose(Handle_t hAction)
{
	HxLOG_Print("hAction (%x)\n", hAction);

	CXREC_GET_SEMAPHORE;

	if(_cxcore_pvr_ReleaseRecContext(hAction) != ERR_OK)
	{
		CXREC_RELEASE_SEMAPHORE;
		HxLOG_Error("_cxcore_pvr_ReleaseRecContext() error\n");
		return ERR_FAIL;
	}

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
STATIC HERROR _cxcore_pvr_RecChangeTsrToDelayedRec(Handle_t hAction, HUINT32 ulContentId, HCHAR *pucOldPath)
{
	ConaxContext_t		*pstContext;
	SvcMeta_Record_t	stRecInfo;
	HERROR				ulRet;
	ConaxSession_t		*pstSession;
	HCHAR				aucOldFile[PAL_PVR_MAX_FILE_NAME_LENGTH + 1], aucNewFile[PAL_PVR_MAX_FILE_NAME_LENGTH + 1];

	HxLOG_Print("hAction (%x), ulContentId (%d)\n", hAction, ulContentId);

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	ulRet = SVC_META_GetRecordInfo(ulContentId, &stRecInfo);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() fail!!!\n");
		CXREC_RELEASE_SEMAPHORE;
		return ulRet;
	}

	// SESSION ÆÄÀÏµéÀ» ¿Å±â°í...
	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
		HxSTD_snprintf(aucOldFile, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", pucOldPath, pstSession->szMetaName);
		HxSTD_snprintf(aucNewFile, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", stRecInfo.aucPath, pstSession->szMetaName);

		HxLOG_Warning("Move File [%s] -> [%s]\n", aucOldFile, aucNewFile);

		ulRet = HLIB_FILE_Move(aucOldFile, aucNewFile);
		if (ulRet != ERR_OK)
		{
			HxLOG_Error("HLIB_FILE_Move() fail!!!\n");
			CXREC_RELEASE_SEMAPHORE;
			return ERR_FAIL;
		}

		pstSession = pstSession->pstNext;
	}

	// CONAX META Entry ÆÄÀÏµµ ¿Å±â°í...
	HxSTD_snprintf(aucOldFile, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", pucOldPath, stRecInfo.szConaxMetaName);
	HxSTD_snprintf(stRecInfo.szConaxMetaName, FS_MAX_FILE_NAME_LEN, "%s%s", stRecInfo.aucFileName, CONAX_PVR_META_FILE_EXT);
	HxSTD_snprintf(aucNewFile, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s", stRecInfo.aucPath, stRecInfo.szConaxMetaName);

	HxLOG_Warning("Move File [%s] -> [%s]\n", aucOldFile, aucNewFile);

	ulRet = HLIB_FILE_Move(aucOldFile, aucNewFile);
	if (ulRet != ERR_OK)
	{
		HxLOG_Error("HLIB_FILE_Move() fail!!!\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	// º¯°æµÈ CONAX Meta Root ÆÄÀÏ¿¡ ´ëÇÑ Á¤º¸µµ HUMAX ¸ÞÅ¸ÆÄÀÏ¿¡ ¹Ý¿µÇÏ°í...
	if(SVC_META_SetRecordInfo(ulContentId, &stRecInfo) != ERR_OK)
	{
		HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	HxSTD_snprintf(pstContext->szContentName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s%s%s", stRecInfo.aucPath, stRecInfo.aucFileName, EXT_TS_FILE);
	pstContext->eRecType = eConaxRecType_Delayed;

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}
#endif

STATIC HERROR _cxcore_pvr_PbOpen(Handle_t hAction, HUINT32 ulContentId, Handle_t hSvc)
{
	SvcMeta_Record_t		stRecInfo;
	ConaxContext_t			*pstContext, *pstRecContext;
	ConaxSession_t			*pstSession = NULL;
	ConaxMeta_t				*pstMeta = NULL;
	HBOOL					bDelete = FALSE;
	HERROR					hErr = ERR_FAIL;

	HxLOG_Print("hAction (%x), ulContentId (%d)\n", hAction, ulContentId);

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContextByActionId(svc_cas_SubSvcRmGetActionId(hAction));
	if(pstContext != NULL)
	{
		HxLOG_Warning("PbAction (%d) already exist\n", svc_cas_SubSvcRmGetActionId(hAction));
		_cxcore_pvr_ReleasePbContext(pstContext->hAction);
	}

	pstContext = _cxcore_pvr_AllocPbContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->bUsed = TRUE;
	pstContext->hAction = hAction;
	pstContext->hSvc = hSvc;
	pstContext->ulContentId = ulContentId;
	pstContext->ulTimecode = 0;

	CXREC_GET_SEMAPHORE;
	pstRecContext = _cxcore_pvr_GetRecContextByContentId(ulContentId);
	if(pstRecContext != NULL)
	{
		pstContext->pstSessionList = pstRecContext->pstSessionList;
	}
	CXREC_RELEASE_SEMAPHORE;

	if(pstContext->pstSessionList == NULL)
	{
		if(SVC_META_GetRecordInfo(ulContentId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("SVC_META_GetRecordInfo() error\n");
			goto _ERROR;
		}

		if(_cxcore_pvr_GetSessionFromContentFile(pstContext, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_GetSessionFromContentFile() error\n");
			bDelete = TRUE;
			goto _ERROR;
		}

		if(_cxcore_pvr_GetMetaFromSessionFile(pstContext, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_GetMetaFromSessionFile() error\n");
			bDelete = TRUE;
			goto _ERROR;
		}
	}

	if(pstContext->pstSessionList == NULL)
	{
		HxLOG_Warning("pstContext->pstSessionList NULL, Maybe FTA service.\n");
		goto _RTN;
	}

#if 0	// TODO: delete
	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
#if defined(CONAX_REC_KEY_ENCRYPT)
		if(_cxcore_pvr_AesCipherProcess(_cxcore_pvr_GetRootKey(), CONAX_PVR_KEY_SIZE, pstSession->stMetaFile.aucRecKey, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE, FALSE) != ERR_OK)
		{
			HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
			CXPB_RELEASE_SEMAPHORE;
			return ERR_FAIL;
		}
#else
		CX_memcpy(pstSession->aucRecKey, pstSession->stMetaFile.aucRecKey, CONAX_PVR_KEY_SIZE);
#endif

		pstSession = pstSession->pstNext;
	}
#endif

	/*
	 *	STB Security Requirements Specification - Chipset Pairing STB Integration Kit 6.0
	 *	#18.4-F : ÄÜÅÙÃ÷¿Í ¿¬°áµÈ URI data°¡ Á¸ÀçÇÏÁö ¾ÊÀ¸¸é ÄÜÅÙÃ÷¸¦ »èÁ¦ÇÑ´Ù.
	 */
	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
		pstMeta = pstSession->stMetaFile.pstMetaList;

		while(pstMeta)
		{
			if (pstMeta->d.eMetaType == eMetaType_URI)
				break;

			pstMeta = pstMeta->pstNext;
		}

		pstSession = pstSession->pstNext;
	}

	if(pstMeta == NULL)
	{
		HxLOG_Warning(HxANSI_COLOR_YELLOW("The contents shall be deleted, because no associated URI data are found.")"\n");
		bDelete = TRUE;
		goto _ERROR;
	}

#if 0
	/*
	 *	³ìÈ­ ½ÃÀÛÇÒ ¶§, ECM filtering ½ÃÀÛµÇ±â ÀüÀÌ¶ó ¹Ì¸® 0¹øÂ° ½½·Ô¿¡ default URI °ªÀ» default °ªÀ¸·Î ÀúÀåÇÏ°í ½ÃÀÛÇÑ´Ù.
	 *	¸¸¾à ECM filtering ½ÃÀÛ°ú ÇÔ²², URI °ªÀÌ Á¸ÀçÇÑ´Ù¸é 0¹øÂ° ½½·ÔÀ» skipÇÏ°í, 1¹øÂ° ½½·Ô URIºÎÅÍ À¯È¿ÇÑ °ÍÀ¸·Î ÀÌ¸¦ Àû¿ëÇÑ´Ù.
	 */
	 -> default URI Àû¿ë µÈ ÈÄ ¹Ù·Î ECMÀ¸·Î Àü´ÞµÈ URI°¡ ¿Ã °ÍÀÌ´Ï ³öµÎÀÚ...
	if ((pstContext->stMetaInfo.stUri[1].ucUriValid == TRUE) && (pstContext->stMetaInfo.stUri[1].ulTime <= 5))
	{
		pstContext->stMetaInfo.stUri[0].ulTime = 0;

		for (i = 0; i < (CONAX_PVR_URI_SLOT_NUM - 1); i++)
		{
			if (pstContext->stMetaInfo.stUri[i + 1].ucUriValid == FALSE)
			{
				pstContext->stMetaInfo.stUri[i].ucUriValid = FALSE;
				break;
			}
			CX_memcpy(&pstContext->stMetaInfo.stUri[i], &pstContext->stMetaInfo.stUri[i + 1], sizeof(ConaxUri_t));
		}
	}
#endif

	pstContext->pstCurrentSession = pstContext->pstSessionList;
//	pstContext->pstCurrentMeta = pstContext->pstCurrentSession->stMetaFile.pstMetaList;

	_cxcore_pvr_CalcSessionValidTime(pstContext);

_RTN:
	pstContext->eState = eConaxPvrState_Open;
	_cxcore_pvr_NotifyUpdateKey(pstContext, 0, NULL);
	hErr = ERR_OK;

_ERROR:
	if(hErr != ERR_OK)
	{
		_cxcore_pvr_RequestPlaybackStop(pstContext, bDelete);
	}

	CXPB_RELEASE_SEMAPHORE;
	return hErr;
}

STATIC HERROR _cxcore_pvr_PbChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	ConaxContext_t	*pstContext;
	HUINT32 ulPlayTime;

	CXPB_GET_SEMAPHORE;
	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(SVC_PB_GetPlayTime(hAction, &ulPlayTime) != ERR_OK)
	{
		HxLOG_Error("SVC_PB_GetPlayTime() error\n");
	}

	switch(pstContext->eState)
	{
		case eConaxPvrState_Open:
			_cxcore_pvr_CheckOpenTimecode(pstContext, ulPlayTime);
			_cxcore_pvr_DisplayMaturityRate(pstContext);
			pstContext->eState = eConaxPvrState_Running;
			break;

		case eConaxPvrState_Running:
			/*
				1. playback Áß¿¡ 1ÃÊ ³»¿¡ °°Àº timecode°¡ ¿©·¯°³ µé¾î¿Â´Ù... -> OCTO 2.0 ¿À¸é¼­ EVENT ¹ß»ý ÁÖ±â¸¦ 1ÃÊ·Î ÇÏ°í ÀÖÀ½.
				2. pause Áß...(È£ÃâÇÏ´Â °÷¿¡¼­ Á¶°ÇÀ¸·Î Ã³¸®ÇßÀ½... pause»óÅÂÀÏ ¶§ È£Ãâ ¾ÈµÇµµ·Ï...)
				3. X¹è¼Ó play½Ã¿¡ session key º¯°æ Á÷ÀüÀÇ timecode°¡ °è¼Ó µé¾î¿À³×...
			*/
			if(pstContext->ulTimecode != ulPlayTime)
			{
				_cxcore_pvr_CheckSessionKey(pstContext, ulPlayTime, pbUpdateKey);
				_cxcore_pvr_CheckMetaData(pstContext, ulPlayTime);
			}

			_cxcore_pvr_DisplayMaturityRate(pstContext);
			pstContext->ulTimecode = ulPlayTime;
			break;

		default:
			CXPB_RELEASE_SEMAPHORE;
			return ERR_FAIL;
	}

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_PbClose(Handle_t hAction)
{
	HxLOG_Print("hAction (%x)\n", hAction);

	CXPB_GET_SEMAPHORE;

	if(_cxcore_pvr_ReleasePbContext(hAction) != ERR_OK)
	{
		CXPB_RELEASE_SEMAPHORE;
		HxLOG_Error("_cxcore_pvr_ReleasePbContext() error\n");
		return ERR_FAIL;
	}

	CXPB_RELEASE_SEMAPHORE;

	return ERR_OK;
}

STATIC HERROR _cxcore_pvr_Remove(HCHAR *pszPath, HCHAR *pszName)
{
	HCHAR szFullPath[PAL_PVR_MAX_FILE_NAME_LENGTH + 1] = {0,};
	HCHAR szFileName[CONAX_SESSION_META_NAME_LEN + 1] = {0,};
	HUINT8	*pucMeta;
	HUINT32	ulMetaSize, ulPos;
	HERROR hErr = ERR_FAIL;

	if((pszPath == NULL) || (pszName == NULL))
	{
		HxLOG_Error("pszPath (%p), pszName (%p)\n", pszPath, pszName);
		return ERR_FAIL;
	}

	HxLOG_Print("pszPath (%s), pszName (%s)\n", pszPath, pszName);

	HxSTD_snprintf(szFullPath, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s/%s", pszPath, pszName);

	if(_cxcore_pvr_DecryptAndCheckMacContentFile(szFullPath, &pucMeta, &ulMetaSize) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_DecryptAndCheckMacContentFile() error\n");
		return ERR_FAIL;
	}

	ulPos = 0;

	if(HxMACRO_Get32Bit(pucMeta + ulPos) != CONAX_META_VERSION)
	{
		HxLOG_Error("META version mismatch\n");
		goto _ERROR;
	}

	ulPos += 4;

	while(ulMetaSize > (ulPos + CONAX_PVR_HASH_SIZE))
	{
		CX_memcpy(szFileName, pucMeta + ulPos, CONAX_SESSION_META_NAME_LEN);
		ulPos += CONAX_SESSION_META_NAME_LEN;

		HxSTD_snprintf(szFullPath, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s/%s", pszPath, szFileName);

		if(HLIB_FILE_Delete(szFullPath) != ERR_OK)
			HxLOG_Error("HLIB_FILE_Delete(%s) error\n", szFullPath);
	}

	HxSTD_snprintf(szFullPath, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s/%s", pszPath, pszName);

	if(HLIB_FILE_Delete(szFullPath) != ERR_OK)
		HxLOG_Error("HLIB_FILE_Delete(%s) error\n", szFullPath);

	hErr = ERR_OK;

_ERROR:
	CX_MEM_Free(pucMeta);

	return hErr;
}

STATIC HERROR _cxcore_pvr_ScInsert(Handle_t hSession)
{
	ConaxContext_t *pstContext;
	Handle_t hStbHandle, hAction;

	hStbHandle = CXSESSION_GetStbHandle(hSession);
	hAction = CXACT_GetActionHandle(hStbHandle);

	if(hStbHandle == HANDLE_NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Error("hStbHandle (0x%x), hAction (0x%x) error\n", hStbHandle, hAction);
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->stMaturityRate.ucSmartcard = CX_MATURITY_RATE_OFF;
	_cxcore_pvr_DisplayMaturityRate(pstContext);

	CXPB_RELEASE_SEMAPHORE;

	return ERR_OK;
}

#ifdef USE_PVR_TASK
STATIC void _cxcore_pvr_Task(void *pvArg)
{
	HUINT32 aulMsg[5] = {0,};
	HERROR hError;

	_cxcore_pvr_CheckRootKey();
	_cxcore_pvr_CheckCaPin();

	while(1)
	{
		if(CX_MSG_Receive(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg)) != ERR_OK)
			continue;

		switch(aulMsg[0])
		{
			case eCxPvrMsg_RecOpen:
				hError = _cxcore_pvr_RecOpen(aulMsg[1], aulMsg[2], aulMsg[3], aulMsg[4]);
				break;

			case eCxPvrMsg_RecTimecode:
				hError = _cxcore_pvr_RecChangeTimecode(aulMsg[1], aulMsg[2], NULL);
				break;

			case eCxPvrMsg_RecClose:
				hError = _cxcore_pvr_RecClose(aulMsg[1]);
				break;

			case eCxPvrMsg_PbOpen:
				hError = _cxcore_pvr_PbOpen(aulMsg[1], aulMsg[2], aulMsg[3]);
				break;

			case eCxPvrMsg_PbTimecode:
				hError = _cxcore_pvr_PbChangeTimecode(aulMsg[1], aulMsg[2], NULL);
				break;

			case eCxPvrMsg_PbClose:
				hError = _cxcore_pvr_PbClose(aulMsg[1]);
				break;

#if defined(CONFIG_MW_MM_DELAYED_REC)
			case eCxPvrMsg_ChangeTsrToDelayedRec:
				hError = _cxcore_pvr_RecChangeTsrToDelayedRec(aulMsg[1], aulMsg[2], (HCHAR *)aulMsg[3]);
				CX_MEM_Free((HCHAR *)aulMsg[3]);
				break;
#endif

			case eCxPvrMsg_CalcMacAndWriteMeta:
				hError = _cxcore_pvr_CalcMacAndWriteMetaFile(aulMsg[1], aulMsg[2]);
				break;

			case eCxPvrMsg_Remove:
				hError = _cxcore_pvr_Remove((HCHAR *)aulMsg[1], (HCHAR *)aulMsg[2]);
				if(aulMsg[1])	CX_MEM_Free((HCHAR *)aulMsg[1]);
				if(aulMsg[2])	CX_MEM_Free((HCHAR *)aulMsg[2]);
				break;

			case eCxPvrMsg_ScInsert:
				hError = _cxcore_pvr_ScInsert(aulMsg[1]);
				break;

			default:
				hError = ERR_FAIL;
				break;
		}

		if(hError != ERR_OK)
		{
			HxLOG_Error("CONAX PVR Msg error (%d)\n", aulMsg[0]);
		}
	}
}
#endif

/*******************************************************************/
/********************      Glocal Functions 	 *******************/
/*******************************************************************/

#define __________CONAX_PVR_GLOBAL_FUNCTION__________
HERROR CXPVR_InitCore(void)
{
	HINT32 i;

#ifdef USE_PVR_TASK
	if(CX_TASK_Create(_cxcore_pvr_Task, CX_PVR_TASK_PRIORITY, CX_PVR_TASK_STACK_SIZE, "CxCorePvrTask", NULL, &s_ulCxPvrTaskId, FALSE) != ERR_OK)
	{
		HxLOG_Error("Failure in PVR TASK\n");
		return ERR_FAIL;
	}

	// create message queue in advance -> HUINT32 * 5...
	if(CX_MSG_Create(CX_PVR_TASK_MSGQ_SIZE, 20, "CxCorePvrTaskQ", &s_ulCxPvrTaskMsgQId) != ERR_OK)
	{
		HxLOG_Error("fail to create msg queue\n");
		return ERR_FAIL;
	}

	CX_TASK_Start(s_ulCxPvrTaskId);
#endif

#ifdef USE_PVR_SEMAPHORE
	if(CX_SEM_Create("s_ulCxPvrRecSemId", &s_ulCxPvrRecSemId) != ERR_OK)
	{
		HxLOG_Error("Failure in s_ulCxPvrRecSemId Semaphore Creation \r\n");
		return ERR_FAIL;
	}

	if(CX_SEM_Create("s_ulCxPvrPbSemId", &s_ulCxPvrPbSemId) != ERR_OK)
	{
		HxLOG_Error("Failure in s_ulCxPvrPbSemId Semaphore Creation \r\n");
		return ERR_FAIL;
	}
#endif

	CX_memcpy(s_aucCaPin, "1234", CONAX_PVR_CA_PIN_SIZE);

	CX_memset(&s_stRecContext, 0, sizeof(ConaxContext_t) * PAL_MAX_REC_INST_NUM);
	CX_memset(&s_stPbContext, 0, sizeof(ConaxContext_t) * PAL_MAX_PB_NUM);

	for(i = 0 ; i < PAL_MAX_REC_INST_NUM ; i++)
		CX_memset(&s_stRecContext[i].stApplyUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	for(i = 0 ; i < PAL_MAX_PB_NUM ; i++)
		CX_memset(&s_stPbContext[i].stApplyUri, 0xff, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

#if 0
#if defined(CONFIG_MW_MM_DLNA) && defined(CONFIG_MW_MM_DLNA_DMS)
	CX_memset(&s_stCxDlnaSession, 0, sizeof(ConaxContext_t) * MAX_MEDIA_DLNACONTENT_USERNUM);
#endif
#endif

	return ERR_OK;
}

HERROR CXAPI_PVR_ReadRootKey(void)
{
	HUINT8	aucEncKey[CONAX_PVR_KEY_SIZE] = {0,};
	HUINT8	aucPvrKey[CONAX_PVR_KEY_SIZE] = {0,};
	HUINT8	aucHASHRef[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucHASH[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucKeyWithHash[CONAX_PVR_KEY_SIZE + CONAX_PVR_HASH_SIZE] = {0,};

	ENTER_FUNCTION;

	if(CXNVM_ReadPvrKeyWithHash(aucKeyWithHash, CONAX_PVR_KEY_SIZE + CONAX_PVR_HASH_SIZE) != ERR_OK)
	{
		HxLOG_Error("CXNVM_ReadPvrKeyWithHash() error\n");
		return ERR_FAIL;
	}

	CX_memcpy(aucEncKey, aucKeyWithHash, CONAX_PVR_KEY_SIZE);
	CX_memcpy(aucHASHRef, aucKeyWithHash + CONAX_PVR_KEY_SIZE, CONAX_PVR_HASH_SIZE);

	if(_cxcore_pvr_AesCipherProcess(NULL, 0, aucEncKey, aucPvrKey, CONAX_PVR_KEY_SIZE, FALSE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		return ERR_FAIL;
	}

	svc_cas_DevPortChiperSha1(aucPvrKey, CONAX_PVR_KEY_SIZE, aucHASH);

	if (CX_memcmp(aucHASH, aucHASHRef, CONAX_PVR_HASH_SIZE))
	{
		HxLOG_Error("invalid SHA-1...\n");
		return ERR_FAIL;
	}

	CX_memcpy(s_aucRootKey, aucPvrKey, CONAX_PVR_KEY_SIZE);

	return ERR_OK;
}

HERROR CXAPI_PVR_GenerateRootKey(void)
{
	HUINT32 ulChipId, ulCRC32;
	HUINT8	aucPvrKey[CONAX_PVR_KEY_SIZE] = {0,};
	HUINT8	aucHASH[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucKeyWithHash[CONAX_PVR_KEY_SIZE + CONAX_PVR_HASH_SIZE] = {0,};

	ENTER_FUNCTION;

#if 1
	// CHIP ID¸¦ º£ÀÌ½º·Î ÇÏ¿© SEED °ªÀ¸·Î ÇÏ°í Chipset Unique Key¸¦ ÀÌ¿ëÇÏ¿© ENCRYPTÇÑ °á°ú¸¦ PVR KEY·Î »ç¿ëÇÑ´Ù.
	if(svc_cas_DevPortSysGetChipId(&ulChipId) != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysGetChipId() error\n");
		return ERR_FAIL;
	}

	ulCRC32 = HLIB_MATH_GetCrc32(&ulChipId, 4);

	CX_memcpy(aucPvrKey, &ulChipId, 4);
	CX_memcpy(aucPvrKey + 4, &ulCRC32, 4);
	CX_memcpy(aucPvrKey + 8, aucPvrKey, 8);
#else
	// DI¿¡ HW baseÀÇ PRNG¸¦ ¸¸µéÀÚ...
#endif

	if(_cxcore_pvr_AesCipherProcess(NULL, 0, aucPvrKey, aucPvrKey, CONAX_PVR_KEY_SIZE, TRUE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		return ERR_FAIL;
	}

	// PVR Root KeyÀÇ verificationÀ» À§ÇÑ HASH¸¦ °è»êÇÑ´Ù.
	svc_cas_DevPortChiperSha1(aucPvrKey, CONAX_PVR_KEY_SIZE, aucHASH);

	// Persistent Storage¿¡ ÀúÀåÇÏ±â Àü¿¡ Chipset Unique Key·Î Proection ÇÑ´Ù.
	if(_cxcore_pvr_AesCipherProcess(NULL, 0, aucPvrKey, aucPvrKey, CONAX_PVR_KEY_SIZE, TRUE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		return ERR_FAIL;
	}

	CX_memcpy(aucKeyWithHash, aucPvrKey, CONAX_PVR_KEY_SIZE);
	CX_memcpy(aucKeyWithHash + CONAX_PVR_KEY_SIZE, aucHASH, CONAX_PVR_HASH_SIZE);

	if(CXNVM_WritePvrKeyWithHash(aucKeyWithHash, CONAX_PVR_KEY_SIZE + CONAX_PVR_HASH_SIZE) != ERR_OK)
	{
		HxLOG_Error("CXNVM_WritePvrKeyWithHash() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CXAPI_PVR_ReadPvrKey(HUINT8* pucPvrKey)
{
	if (pucPvrKey == NULL)
	{
		HxLOG_Error("pucPvrKey is NULL\n");
		return ERR_FAIL;
	}

	CX_memcpy(pucPvrKey, s_aucRootKey, CONAX_PVR_KEY_SIZE);

	return ERR_OK;
}

HERROR CXAPI_PVR_Rec_GetSessionKey(Handle_t hAction, HUINT8* pucRecKey)
{
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;
	HERROR	hError = ERR_OK;

	if (pucRecKey == NULL)
	{
		HxLOG_Error("pucRecKey or pucEncKey NULL\n");
		return ERR_FAIL;
	}

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n", pstContext);
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(pstContext->eState == eConaxPvrState_Close)
	{
		HxLOG_Error("Conax PVR Context State (%d)\n", pstContext->eState);
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(_cxcore_pvr_AddSessionToContext(pstContext) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AddSessionToContext() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	hError = _cxcore_pvr_GetRandomNumber(pstSession->aucRecKey, CONAX_PVR_KEY_SIZE);
	if(hError != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_GetRandomNumber() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CX_memcpy(pucRecKey, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE);

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

HERROR CXAPI_PVR_Pb_GetSessionKey(Handle_t hAction, HUINT8* pucPbKey)
{
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;

	if (pucPbKey == NULL)
	{
		HxLOG_Error("pucEncKey or pucPbKey NULL\n");
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(pstContext->eState == eConaxPvrState_Close)
	{
		HxLOG_Error("Conax PVR Context State (%d)\n", pstContext->eState);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CX_memcpy(pucPbKey, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE);

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}


#define __________CONAX_PVR_META_DATA_API__________
HERROR CXAPI_PVR_Rec_UpdateSessionKeyToMetaFile(Handle_t hAction, HUINT32 ulTimecode, HUINT8* pucRecKey)
{
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

#if defined(CONAX_REC_KEY_ENCRYPT)
	if(_cxcore_pvr_AesCipherProcess(_cxcore_pvr_GetRootKey(), CONAX_PVR_KEY_SIZE, pstSession->aucRecKey, pstSession->stMetaFile.aucRecKey, CONAX_PVR_KEY_SIZE, TRUE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() fail\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}
#else
	CX_memcpy(pstSession->stMetaFile.aucRecKey, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE);
#endif

	pstSession->stMetaFile.ulApplyTime = pstContext->ulTimecode;

	// Á¦ÀÏ Ã³À½...
	if(pstSession->pstPrev == NULL)
	{
		pstSession->ulStartTime = 0;
	}
	else
	{
		pstSession->pstPrev->ulEndTime = pstContext->ulTimecode;
		pstSession->ulStartTime = pstContext->ulTimecode;
	}

	pstSession->ulEndTime = 0xFFFFFFFF;

	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Session | eConaxFileType_Meta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() fail\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}


#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)


HERROR CXAPI_PVR_UpdateFingerprintToMetaFile(Handle_t hAction, CxFingerprint_t* pstCxFingerprint)
{
	ConaxContext_t			*pstContext;
	ConaxSession_t			*pstSession;
	ConaxMeta_t				stNewMeta;

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	stNewMeta.d.ulTime = pstContext->ulTimecode;
	{
		HUINT32 ulActionId;
		HERROR	hErr;
		HUINT32	ulRecordId;
		HUINT32	ulRecDuration;

		if (pstContext->eRecType == eConaxRecType_Record)
		{
			ulActionId = PAL_PIPE_GetActionId(hAction);
			hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_HDD_REC, &ulRecordId);
			if (hErr == ERR_OK)
			{
				hErr = PAL_REC_GetDuration(ulRecordId, &ulRecDuration);
				if (hErr == ERR_OK)
				{
					stNewMeta.d.ulTime = ulRecDuration;
				}
			}
		}
	}

	stNewMeta.d.eMetaType = eMetaType_FinterPrint;

	stNewMeta.d.u.stFingerPrint.ulTime = pstCxFingerprint->ulTime;
	stNewMeta.d.u.stFingerPrint.usDuration = pstCxFingerprint->usDuration;
	stNewMeta.d.u.stFingerPrint.usPosX = pstCxFingerprint->usPosX;
	stNewMeta.d.u.stFingerPrint.usPosY = pstCxFingerprint->usPosY;
	stNewMeta.d.u.stFingerPrint.usHeight = pstCxFingerprint->usHeight;
	CX_memcpy(stNewMeta.d.u.stFingerPrint.szText, pstCxFingerprint->szText, CX_MAX_FINGERPRINT);
//	pstFingerprint->bFPFromProtectedData = pstCxFingerprint->bFPFromProtectedData;

	if(_cxcore_pvr_AddMetaToSession(pstContext, &stNewMeta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AddMetaToSession() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Meta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}
#endif

HERROR CXAPI_PVR_GetMaturityRateValue(Handle_t hAction, HUINT8 *pucMaturityLevel)
{
	ConaxContext_t	*pstContext;

	CXPB_GET_SEMAPHORE;
	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(_cxcore_pvr_GetStrictMaturityRateValue(hAction, pstContext->hSvc, pucMaturityLevel) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_GetStrictMaturityRateValue() fail !!!\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}

HERROR CXAPI_PVR_GetMaturityRateValueFromURI(Handle_t hAction, HUINT32 ulUriTime, HUINT8* pucMaturityRate)
{
	ConaxContext_t* pstContext;
	ConaxSession_t	*pstSession = NULL;
	ConaxMeta_t		*pstMeta;

	CXPB_GET_SEMAPHORE;
	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstMeta = pstSession->stMetaFile.pstMetaList;
	while(pstMeta)
	{
		if(pstMeta->d.eMetaType == eMetaType_URI)
			break;

		pstMeta = pstMeta->pstNext;
	}

	if(pstMeta == NULL)
	{
		HxLOG_Error("pstMeta NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	*pucMaturityRate = pstMeta->d.u.stUri.eMR;

	CXPB_RELEASE_SEMAPHORE;

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_CONAX_URI)
HERROR CXAPI_PVR_UpdateUriToMetaFile(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo)
{
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;
	ConaxMeta_t		stMeta;
	SvcMeta_Record_t stRecInfo;

	CXREC_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetRecContext(hAction);
	pstSession = _cxcore_pvr_GetRecCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	stMeta.d.ulTime = pstContext->ulTimecode;
	stMeta.d.eMetaType = eMetaType_URI;

	CX_memcpy(&stMeta.d.u.stUri, pstDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	// convert emi
	stMeta.d.u.stUri.eEmiCopyCtrl = _cxcore_pvr_ConvertEmiValueForRecording(stMeta.d.u.stUri.eEmiCopyCtrl);

	if(_cxcore_pvr_AddMetaToSession(pstContext, &stMeta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AddMetaToSession() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(_cxcore_pvr_UpdateMetaFile(pstContext, eConaxFileType_Meta) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_UpdateMetaFile() error\n");
		CXREC_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(pstDrmInfo->eMR != eDxCopyrightControl_ConaxMaturityRating_Off)
	{
		stRecInfo.u1cScrambled = FALSE;

		if(SVC_META_GetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
		{
			HxLOG_Error("SVC_META_GetRecordInfo() error\n");
		}

		if(stRecInfo.u1cScrambled != TRUE)
		{
			stRecInfo.u1cScrambled = TRUE;
			if(SVC_META_SetRecordInfo(pstContext->ulContentId, &stRecInfo) != ERR_OK)
			{
				HxLOG_Error("SVC_META_SetRecordInfo() error\n");
			}
		}
	}

	CXREC_RELEASE_SEMAPHORE;
	return ERR_OK;
}

#if 0
HERROR CXAPI_PVR_GetCurUriFromMetaFile(Handle_t hAction, UNIXTIME curTime, DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo, UNIXTIME* pulTime)
{
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;
	ConaxMeta_t		*pstMeta;

	if(pstDrmInfo == NULL || pulTime == NULL)
	{
		HxLOG_Error("Invalid parameter !!!\n");
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstMeta = pstSession->pstUriPos;
	while(pstMeta)
	{
		if(pstMeta->d.eMetaType != eMetaType_URI)
			continue;

		if(pstMeta->d.ulTime <= curTime)
			break;
	}

	if(pstMeta == NULL)
	{
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	*pulTime = pstMeta->d.ulTime;
	CX_memcpy(pstDrmInfo, &pstMeta->d.u.stUri, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}
#endif
#endif

#define __________CONAX_PVR_PLAYBACK_API__________
#define __________CONAX_PVR_RECORD_API__________

HERROR CXAPI_PVR_Rec_Open(Handle_t hAction, HUINT32 ulContentId, Handle_t hSvc, SvcCas_ActionType_e eActionType)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_RecOpen;
	aulMsg[1] = hAction;
	aulMsg[2] = ulContentId;
	aulMsg[3] = hSvc;
	aulMsg[4] = eActionType;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_RecOpen(hAction, ulContentId, hSvc, eActionType);
#endif

	return hError;
}

HERROR CXAPI_PVR_Rec_ChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_RecTimecode;
	aulMsg[1] = hAction;
	aulMsg[2] = ulTimecode;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_RecChangeTimecode(hAction, ulTimecode, pbUpdateKey);
#endif

	return hError;
}

HERROR CXAPI_PVR_Rec_Close(Handle_t hAction)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_RecClose;
	aulMsg[1] = hAction;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_RecClose(hAction);
#endif

	return hError;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXAPI_PVR_Rec_StartDelayed(Handle_t hAction, HUINT32 ulContentId, HUINT8 *pucOldPath)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 ulSize;
	HUINT8	*pucParam;
	HUINT32 aulMsg[5] = {0,};

	ulSize = HxSTD_StrLen(pucOldPath) + 1;
	pucParam = CX_MEM_Calloc(ulSize);
	CX_memcpy(pucParam, pucOldPath, ulSize);

	aulMsg[0] = eCxPvrMsg_ChangeTsrToDelayedRec;
	aulMsg[1] = hAction;
	aulMsg[2] = ulContentId;
	aulMsg[3] = (HUINT32)pucParam;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_RecChangeTsrToDelayedRec(hAction, ulContentId, pucOldPath);
#endif

	if(CXACT_UpdateTsrActionTypeToRecord(hAction) != ERR_OK)
	{
		HxLOG_Error("CXACT_UpdateTsrActionTypeToRecord() error\n");
	}

	return hError;
}
#endif

HERROR CXAPI_PVR_Pb_Open(Handle_t hAction, HUINT32 ulContentId, HUINT32 hSvc)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_PbOpen;
	aulMsg[1] = hAction;
	aulMsg[2] = ulContentId;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_PbOpen(hAction, ulContentId);
#endif

	return hError;
}

HERROR CXAPI_PVR_Pb_ChangeTimecode(Handle_t hAction, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_PbTimecode;
	aulMsg[1] = hAction;
	aulMsg[2] = ulTimecode;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_PbChangeTimecode(hAction, ulTimecode, pbUpdateKey);
#endif

	return hError;
}

HERROR CXAPI_PVR_Pb_Close(Handle_t hAction)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_PbClose;
	aulMsg[1] = hAction;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_PbClose(hAction);
#endif

	return hError;
}

#if 0
HERROR CXAPI_PVR_Pb_UpdatePbSessionForTsr(Handle_t hAction, HINT32 pPbSession)
{
	/*
		³»¿ë¸¸ ºÁ¼­´Â TSR REC Áß¿¡ TSR PB°¡ µ¿ÀÛÇÒ ¶§ TSR REC¿Í TSR PBÀÇ META¸¦ sync ÇÏ´Â °ÍÀ¸·Î º¸ÀÌ´Âµ¥...
		ÇöÀç´Â Recording Session°ú Playback Session¸¦ °øÀ¯ÇÏ´Â °ÍÀ¸·Î ±¸ÇöÇÔ..
	*/
}
#endif

#define __________CONAX_PVR_MATURITY_RATING_API__________

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION) || defined(CONFIG_MW_CAS_CONAX_GERMANY_MR)

HERROR CXAPI_PVR_GetCaPin(HUINT8* szCxPinCode)
{
	CX_memcpy(szCxPinCode, s_aucCaPin, CONAX_PVR_CA_PIN_SIZE);

	return	ERR_OK;
}

HERROR CXAPI_PVR_ReadCaPin(void)
{
	HUINT8	aucCaPin[CONAX_PVR_CA_PIN_KEY_SIZE] = {0,};
	HUINT8	aucReadHash[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucCalcHash[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucCaPinWithHash[CONAX_PVR_CA_PIN_KEY_SIZE + CONAX_PVR_HASH_SIZE] = {0,};

	if(CXNVM_ReadCAPINCode(aucCaPinWithHash, CONAX_PVR_CA_PIN_KEY_SIZE + CONAX_PVR_HASH_SIZE) != ERR_OK)
	{
		return ERR_FAIL;
	}

	CX_memcpy(aucCaPin, aucCaPinWithHash, CONAX_PVR_CA_PIN_KEY_SIZE);
	CX_memcpy(aucReadHash, aucCaPinWithHash + CONAX_PVR_CA_PIN_KEY_SIZE, CONAX_PVR_HASH_SIZE);

	if(_cxcore_pvr_AesCipherProcess(NULL, 0, aucCaPin, aucCaPin, CONAX_PVR_CA_PIN_KEY_SIZE, FALSE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		return ERR_FAIL;
	}

	svc_cas_DevPortChiperSha1(aucCaPin, CONAX_PVR_CA_PIN_KEY_SIZE, aucCalcHash);
	if (CX_memcmp(aucCalcHash, aucReadHash, CONAX_PVR_HASH_SIZE))
	{
		HxLOG_Error("CXAPI_PVR_ReadCAPin invalid SHA-1...\n");
		return ERR_FAIL;
	}

	CX_memcpy(s_aucCaPin, aucCaPin, CONAX_PVR_CA_PIN_SIZE);
	HxLOG_Print("CXAPI_PVR_ReadCAPin %s\n", s_aucCaPin);

	return ERR_OK;
}


HERROR CXAPI_PVR_SetCaPin(HUINT8* szCAPin)
{
	HUINT8	aucCaPin[CONAX_PVR_CA_PIN_KEY_SIZE] = {0,};
	HUINT8	aucCalcHash[CONAX_PVR_HASH_SIZE] = {0,};
	HUINT8	aucCaPinWithHash[CONAX_PVR_CA_PIN_KEY_SIZE + CONAX_PVR_HASH_SIZE] = {0,};

	if(szCAPin == NULL)
	{
		HxLOG_Error("szCAPin is NULL\n");
		return ERR_FAIL;
	}

	CX_memcpy(aucCaPin, szCAPin, CONAX_PVR_CA_PIN_SIZE);
	aucCaPin[CONAX_PVR_CA_PIN_SIZE-1] = 0;

	svc_cas_DevPortChiperSha1(aucCaPin, CONAX_PVR_CA_PIN_KEY_SIZE, aucCalcHash);

	if(_cxcore_pvr_AesCipherProcess(NULL, 0, aucCaPin, aucCaPin, CONAX_PVR_CA_PIN_KEY_SIZE, TRUE) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_AesCipherProcess() error\n");
		return ERR_FAIL;
	}

	CX_memcpy(aucCaPinWithHash, aucCaPin, CONAX_PVR_CA_PIN_KEY_SIZE);
	CX_memcpy(aucCaPinWithHash + CONAX_PVR_CA_PIN_KEY_SIZE, aucCalcHash, CONAX_PVR_HASH_SIZE);

	CXNVM_WriteCAPINCode(aucCaPinWithHash, CONAX_PVR_CA_PIN_KEY_SIZE + CONAX_PVR_HASH_SIZE);

	CX_memcpy(s_aucCaPin, szCAPin, CONAX_PVR_CA_PIN_SIZE);
	HxLOG_Print("CXAPI_PVR_SetCAPin %s\n", s_aucCaPin);

	return ERR_OK;
}
#endif

HERROR CXAPI_PVR_ScInserted(Handle_t hSession)
{
	HERROR hError;
#ifdef USE_PVR_TASK
	HUINT32 aulMsg[5] = {0,};

	aulMsg[0] = eCxPvrMsg_ScInsert;
	aulMsg[1] = (HUINT32)hSession;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_ScInsert(hSession);
#endif

	return hError;
}

HERROR CXAPI_PVR_GetNextUriTimeFromMetaFile(Handle_t hAction, UNIXTIME* pulNextUriTime)
{
#if defined(CONFIG_MW_CAS_CONAX_URI)
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;
	ConaxMeta_t		*pstMeta;

	if (pulNextUriTime == NULL)
	{
		HxLOG_Error("Invalid parameter !!!\n");
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	if(pstContext->pstCurrentMeta == NULL)
	{
		HxLOG_Error("pstSession->pstCurrentMeta NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstMeta = pstContext->pstCurrentMeta->pstNext;
	while(pstMeta)
	{
		if(pstMeta->d.eMetaType == eMetaType_URI)
			break;

		pstMeta = pstMeta->pstNext;
	}

	if(pstMeta == NULL)
	{
		HxLOG_Error("can not find URI or nextURI !!\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	*pulNextUriTime = pstMeta->d.ulTime;

	CXPB_RELEASE_SEMAPHORE;
#endif
	return ERR_OK;
}

HERROR CXAPI_PVR_Pb_CheckAllUriRL(Handle_t hAction, UNIXTIME ulStartTime, HBOOL* pbExpiredAllUri)
{
#if defined(CONFIG_MW_CAS_CONAX_URI)
	ConaxContext_t	*pstContext;
	ConaxSession_t	*pstSession;
	ConaxMeta_t		*pstMeta;
	HERROR			hError			= ERR_OK;
	HBOOL			bExpiredUri		= TRUE;
	UNIXTIME		ulExpiredTime	= 0;
	DxCopyrightControl_t	stDrmInfo;

	if(pbExpiredAllUri == NULL)
	{
		HxLOG_Error("Invalid parameter !!!\n");
		return ERR_FAIL;
	}

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CX_memset(&stDrmInfo, 0x00, sizeof(DxCopyrightControl_t));

	stDrmInfo.bCasCc = TRUE;
	stDrmInfo.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Conax;

	pstMeta = pstSession->stMetaFile.pstMetaList;
	while(pstMeta)
	{
		if(pstMeta->d.eMetaType == eMetaType_URI)
		{
			CX_memcpy(&stDrmInfo.stCasCcInfo.info.stConaxDrmInfo, &pstMeta->d.u.stUri, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

			hError = SVC_META_CheckDrmExpired(&stDrmInfo, (ulStartTime + pstMeta->d.ulTime), &bExpiredUri, &ulExpiredTime);
			if (hError == ERR_OK && bExpiredUri == FALSE)
			{
				*pbExpiredAllUri = FALSE;
				CXPB_RELEASE_SEMAPHORE;
				return ERR_OK;
			}
		}

		pstMeta = pstMeta->pstNext;
	}

	*pbExpiredAllUri = TRUE;
	CXPB_RELEASE_SEMAPHORE;
#endif
	return ERR_OK;
}

HERROR CXAPI_PVR_Remove(HCHAR *pszPath, HCHAR *pszName)
{
	HERROR hError;

#ifdef USE_PVR_TASK
	HUINT32	ulLen;
	HCHAR	*pszParam1, *pszParam2;
	HUINT32 aulMsg[5] = {0,};

	ulLen = HxSTD_StrLen(pszPath) + 1;
	pszParam1 = CX_MEM_Calloc(ulLen);
	HLIB_STD_StrNCpySafe(pszParam1, pszPath, ulLen);

	ulLen = HxSTD_StrLen(pszName) + 1;
	pszParam2 = CX_MEM_Calloc(ulLen);
	HLIB_STD_StrNCpySafe(pszParam2, pszName, ulLen);

	aulMsg[0] = eCxPvrMsg_Remove;
	aulMsg[1] = (HUINT32)pszParam1;
	aulMsg[2] = (HUINT32)pszParam2;

	hError = CX_MSG_Send(s_ulCxPvrTaskMsgQId, &aulMsg, sizeof(aulMsg));
#else
	hError = _cxcore_pvr_Remove(pszPath, pszName);
#endif

	return hError;
}

HERROR CXPVR_GetMaturityRate(Handle_t hAction, CxMaturityRatingValue_t *pstMaturityRate)
{
	ConaxContext_t *pstContext;

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	CX_memcpy(pstMaturityRate, &pstContext->stMaturityRate, sizeof(CxMaturityRatingValue_t));

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}

HERROR CXPVR_SetMaturityRate(Handle_t hAction, CxMaturityRatingValue_t *pstMaturityRate)
{
	ConaxContext_t *pstContext;

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstContext->stMaturityRate.ucSmartcard = CXSC_GetMaturityRatingLevel(CX_SC_SLOT_0);

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}

HERROR CXAPI_PVR_GetEncryptionKey(SvcMeta_Record_t *pstRecInfo, HUINT32 ulTimecode, HUINT8 *pucKey)
{
	ConaxContext_t *pstContext = NULL;
	ConaxSession_t *pstSession, *pstNextSession;
	HERROR hError = ERR_FAIL;

	if(pucKey == NULL)
	{
		goto _GO_RETURN;
	}

	pstContext = CX_MEM_Calloc(sizeof(ConaxContext_t));
	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL\n");
		goto _GO_RETURN;
	}

	if(_cxcore_pvr_GetSessionFromContentFile(pstContext, pstRecInfo) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_GetSessionFromContentFile() error\n");
		goto _GO_RETURN;
	}

	if(_cxcore_pvr_GetMetaFromSessionFile(pstContext, pstRecInfo) != ERR_OK)
	{
		HxLOG_Error("_cxcore_pvr_GetMetaFromSessionFile() error\n");
		goto _GO_RETURN;
	}

	pstSession = pstContext->pstSessionList;
	while(pstSession)
	{
		pstNextSession = pstSession->pstNext;
		if(pstNextSession == NULL)
		{
			HxLOG_Print("end of session.\n");
			break;
		}

		if(((pstSession->stMetaFile.ulApplyTime) <= ulTimecode) && (ulTimecode < (pstNextSession->stMetaFile.ulApplyTime)))
		{
			HxLOG_Print("Found valid session...\n");
			break;
		}

		pstSession = pstSession->pstNext;
	}

	if(pstSession == NULL)
	{
		HxLOG_Error("pstSession NULL\n");
		goto _GO_RETURN;
	}

	CX_memcpy(pucKey, pstSession->aucRecKey, CONAX_PVR_KEY_SIZE);

	hError = ERR_OK;

_GO_RETURN:
	if(pstContext != NULL)
	{
		_cxcore_pvr_ContextCleanup(pstContext);
		CX_MEM_Free(pstContext);
	}

	return hError;
}

#if 0	// TODO: swlee3 - delete
HERROR CXAPI_PVR_CheckDisplayFingerprint(Handle_t hAction, HUINT32 ulTimecode, CxFingerprint_t* pstCxFingerprint)
{
	ConaxContext_t			*pstContext;
	ConaxSession_t			*pstSession;
	ConaxMeta_t				*pstMeta;
	ConaxMetaFingerprint_t	*pstMetaFingerPrint = NULL;

	CXPB_GET_SEMAPHORE;

	pstContext = _cxcore_pvr_GetPbContext(hAction);
	pstSession = _cxcore_pvr_GetPbCurrentSession(pstContext);
	if(pstContext == NULL || pstSession == NULL)
	{
		HxLOG_Error("pstContext (%p), pstSession (%p) error\n", pstContext, pstSession);
		CXPB_RELEASE_SEMAPHORE;
		return ERR_FAIL;
	}

	pstMeta = pstSession->pstFpPos;
	if(pstMeta->d.ulTime < ulTimecode)
	{	// forword
		pstMeta = pstMeta->pstNext;
		while(pstMeta)
		{
			if(pstMeta->d.eMetaType == eMetaType_FinterPrint)
			{
				if(pstMeta->d.ulTime >= ulTimecode)
					break;
			}
			pstMeta = pstMeta->pstNext;
		}
	}
	else
	{	// rewind
		pstMeta = pstMeta->pstPrev;
		while(pstMeta)
		{
			if(pstMeta->d.eMetaType == eMetaType_FinterPrint)
			{
				if(pstMeta->d.ulTime < ulTimecode)
					break;
			}
			pstMeta = pstMeta->pstPrev;
		}
	}

	if(pstMeta == NULL)
	{
		CXPB_RELEASE_SEMAPHORE;
		return ERR_OK;
	}

	pstMetaFingerPrint = &pstMeta->d.u.stFingerPrint;

	pstCxFingerprint->ulTime = pstMetaFingerPrint->ulTime;
	pstCxFingerprint->usDuration = pstMetaFingerPrint->usDuration;
	pstCxFingerprint->usPosX = pstMetaFingerPrint->usPosX;
	pstCxFingerprint->usPosY = pstMetaFingerPrint->usPosY;
	pstCxFingerprint->usHeight = pstMetaFingerPrint->usHeight;
	pstCxFingerprint->ulTime = pstMetaFingerPrint->ulTime;
	CX_memcpy(pstCxFingerprint->szText, pstMetaFingerPrint->szText, CX_MAX_FINGERPRINT);
//	pstCxFingerprint->bFPFromProtectedData = pstMetaFingerPrint->bFPFromProtectedData;

	CXPB_RELEASE_SEMAPHORE;
	return ERR_OK;
}
#endif
#if 0
#if defined(CONFIG_MW_MM_DLNA) && defined(CONFIG_MW_MM_DLNA_DMS)
#define __________CONAX_PVR_DLNA_PLAYBACK_API__________

STATIC ConaxContext_t* _cxpvr_dlna_PbAllocSession(void)
{
	HINT32			i;
	ConaxContext_t* pstContext	= NULL;

	for (i = 0 ; i < MAX_MEDIA_DLNACONTENT_USERNUM ; i++)
	{
		pstContext = &s_stCxDlnaSession[i];
		if (pstContext->bUsed == 0)
		{
			HxLOG_Print("== _cxpvr_dlna_PbAllocSession== Find Empty DLNA Session (%d)\n", i);
			break;
		}
	}

	if (i == MAX_MEDIA_DLNACONTENT_USERNUM)
	{
		HxLOG_Print("==_cxpvr_dlna_PbAllocSession== Fail!! Session is full\n");

		return NULL;
	}

	return pstContext;
}


STATIC ConaxContext_t* _cxpvr_dlna_PbGetSession(HUINT32 ulHandle)
{
	HUINT32			i;
	ConaxContext_t* pstContext	= NULL;

	for (i = 0 ; i < MAX_MEDIA_DLNACONTENT_USERNUM ; i++)
	{
		pstContext = &s_stCxDlnaSession[i];
		if ((pstContext->bUsed == 1) && (pstContext->hAction == ulHandle))
		{
			HxLOG_Print("== _cxpvr_dlna_PbGetSession== Find DLNA PB Session Idx:(%d), pstContext(%d)\n", i, pstContext->hAction);
			break;
		}
	}

	if (i == MAX_MEDIA_DLNACONTENT_USERNUM)
	{
		HxLOG_Print("==_cxpvr_dlna_PbGetSessionPath== Fail!! Session not found with Handle(%d)\n", ulHandle);
		return NULL;
	}

	return pstContext;
}


STATIC HERROR _cxpvr_dlna_PbReleaseSession(HUINT32 ulHandle)
{
	ConaxContext_t* pstContext	= NULL;

	pstContext = _cxpvr_dlna_PbGetSession(ulHandle);
	if (pstContext != NULL)
	{
		HxLOG_Print("==_cxpvr_dlna_PbReleaseSession== ulHandle(%d) close()...\n", pstContext->hAction);
		CX_memset(pstContext, 0, sizeof(ConaxContext_t));
	}

	return ERR_OK;
}

STATIC ConaxContext_t* _cxpvr_dlna_PbGetSessionPath(const char* pathname)
{
	HUINT32			i;
	ConaxContext_t* pstContext	= NULL;

	HxLOG_Print("==_cxpvr_dlna_PbGetSessionPath== Compare file path [%s]\n", pathname);
	for (i = 0 ; i < MAX_MEDIA_DLNACONTENT_USERNUM ; i++)
	{
		pstContext = &s_stCxDlnaSession[i];
		if (pstContext == NULL)
		{
			continue;
		}
		HxLOG_Print("Compare file path : Session : [%s]\n", pstContext->szMetaName);
		if ((pstContext->bUsed == 1) && (VK_strcmp(pathname, pstContext->szMetaName) == 0))
		{
			HxLOG_Print("==_cxpvr_dlna_PbGetSessionPath== Find same file path in sesstion(%d) \n", i);
			break;
		}
	}

	if (i == MAX_MEDIA_DLNACONTENT_USERNUM)
	{
		HxLOG_Print("==_cxpvr_dlna_PbGetSessionPath== Session not found the  file path\n");
		return NULL;
	}

	return pstContext;
}


HERROR CXAPI_PVR_DLNA_Pb_SetSessionKeyHandle(const char* pathname, HUINT32 ulHandle)
{
	ConaxContext_t* pstContext	= NULL;

	if (pathname == NULL)
	{
		HxLOG_Error("==CXAPI_PVR_DLNA_Pb_SetSessionKeyHandle== pathname NULL\n");
		return	ERR_FAIL;
	}

	pstContext = _cxpvr_dlna_PbGetSessionPath(pathname);
	if (pstContext == NULL)
	{
		HxLOG_Error("==CXAPI_PVR_DLNA_Pb_SetSessionKeyHandle== DLNA pstContext NULL!!!\n");
		return	ERR_FAIL;
	}

	pstContext->hAction = ulHandle;

	return	ERR_OK;
}



HERROR CXAPI_PVR_DLNA_Pb_GetSessionKey(HUINT32 ulHandle, HUINT8* pucPbKey)
{
	ConaxContext_t* pstContext	= NULL;

	if (pucPbKey == NULL)
	{
		HxLOG_Error("==CXAPI_PVR_DLNA_Pb_GetSessionKey== DLNA pucEncKey or pucPbKey NULL\n");
		return ERR_FAIL;
	}

	pstContext = _cxpvr_dlna_PbGetSession(ulHandle);
	if (pstContext == NULL)
	{
		HxLOG_Error("==CXAPI_PVR_DLNA_Pb_GetSessionKey== DLNA pstContext NULL!!!\n");
		return ERR_FAIL;
	}
	HxLOG_Print("== CXAPI_PVR_DLNA_Pb_GetSessionKey== : key IDX :%d ...\n", pstContext->ulSessionPos);
	CX_memcpy(pucPbKey, pstContext->stMetaInfo.stSession[pstContext->ulSessionPos].aucSessionKey, CONAX_PVR_KEY_SIZE);

	return ERR_OK;
}


HERROR CXAPI_PVR_DLNA_Pb_GetSessionKeyByPath(const char* pathname, HUINT8* pucPbKey)
{
	ConaxContext_t* pstContext;

	if (pathname == NULL)
	{
		HxLOG_Error("==CXAPI_PVR_DLNA_Pb_GetSessionKeyByPath== pathname NULL!!!\n");
		return ERR_FAIL;
	}

	pstContext = _cxpvr_dlna_PbGetSessionPath(pathname);
	if (pstContext == NULL)
	{
		HxLOG_Error("DLNA pstContext NULL!!!\n");
		return ERR_FAIL;
	}

	if (pucPbKey != NULL)
	{
		CX_memcpy(pucPbKey, pstContext->stMetaInfo.stSession[pstContext->ulSessionPos].aucSessionKey, CONAX_PVR_KEY_SIZE);
	}
	else
	{
		HxLOG_Error("DLNA pucPbKey is NULL!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}



HERROR CXAPI_PVR_DLNA_Pb_OpenMetaFile(const char* pathname)
{
	HERROR			hError				= ERR_FAIL;
	//	SvcMeta_Record_t	stRecInfo;
	ConaxContext_t* pstContext			= NULL;
	ConaxMetaInfo_t* 						pstCipherContents	= NULL,* pstPlainContents = NULL;
	HUINT8			aucPbKey[CONAX_PVR_KEY_SIZE];
	HUINT8* 		pucPvrKey			= NULL;
	int				i;
	HUINT8* 		pcPath				= NULL;
	HUINT8* 		pcTempPath			= NULL;


	if (pathname == NULL)
	{
		HxLOG_Error("pathname NULL!!!\n");
		return ERR_FAIL;
	}

	pstContext = _cxpvr_dlna_PbAllocSession();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		return ERR_FAIL;
	}

	pstContext->bUsed = TRUE;
	pstContext->hAction = 0;
	pstContext->ulTimecode = 0;
	pstContext->ulSessionPos = 0;

	pcPath = CX_MEM_Alloc(PAL_PVR_MAX_FILE_NAME_LENGTH);
	if (pcPath != NULL)
	{
		CX_memset(pcPath, 0x0, PAL_PVR_MAX_FILE_NAME_LENGTH);
		MWC_UTIL_DvbStrcpy(pcPath, pathname);
		pcTempPath = strstr((char*)pcPath, EXT_TS_FILE);
		if (pcTempPath != NULL)
		{
			pcTempPath[1] = 'c';
			pcTempPath[2] = 'x';
			pcTempPath[3] = 'm';
			pcTempPath[4] = '\0';
			HxLOG_Print("==> CXAPI_PVR_DLNA_Pb_OpenMetaFile EXT_TS_FILE call Path=%s \n", pcPath);
		}
		else
		{
			HxLOG_Print("==> CXAPI_PVR_DLNA_Pb_OpenMetaFile call Path=%s \n", pcPath);
		}
	}

	snprintf(pstContext->szMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s", pcPath);

	pstCipherContents = (ConaxMetaInfo_t *)CX_MEM_Alloc(sizeof(ConaxMetaInfo_t));
	if (pstCipherContents == NULL)
	{
		if (pcPath)
		{
			CX_MEM_Free(pcPath);
		}

		HxLOG_Print("pstCipherContents ConaxMetaInfo_t CX_MEM_Alloc() fail\n");
		return ERR_FAIL;
	}

	pstPlainContents = (ConaxMetaInfo_t *)CX_MEM_Alloc(sizeof(ConaxMetaInfo_t));
	if (pstPlainContents == NULL)
	{
		if (pstCipherContents)
		{
			CX_MEM_Free(pstCipherContents);
		}
		if (pcPath)
		{
			CX_MEM_Free(pcPath);
		}

		HxLOG_Print("pstPlainContents ConaxMetaInfo_t CX_MEM_Alloc() fail\n");
		return ERR_FAIL;
	}

	if(_cxpvr_CheckHmacValidity(pstCipherContents, pstPlainContents, pstContext) != ERR_OK)
	{
		HxLOG_Print("meta file corrupted.. \n");
		CX_MEM_Free(pstCipherContents);
		CX_MEM_Free(pstPlainContents);
		CX_MEM_Free(pcPath);
		return ERR_FAIL;
	}

	pucPvrKey = _cxcore_pvr_GetRootKey();

	for (i = 0 ; i < CONAX_PVR_SESSION_SLOT_NUM ; i++)
	{
		if (i != 0 && pstContext->stMetaInfo.stSession[i].ulApplySessionKeyTime == 0)
		{
			continue;
		}

		CX_memset(aucPbKey, 0, CONAX_PVR_KEY_SIZE);

		if (svc_cas_DevPortSysDecrypt(pucPvrKey, TRUE, pstContext->stMetaInfo.stSession[i].aucSessionKey, aucPbKey, CONAX_PVR_KEY_SIZE) != ERR_OK)
		{
			HxLOG_Error("Decryption fail, index(%d)\n", i);
		}

		CX_memcpy(pstContext->stMetaInfo.stSession[i].aucSessionKey, aucPbKey, CONAX_PVR_KEY_SIZE);
	}

	snprintf(pstContext->szMetaName, PAL_PVR_MAX_FILE_NAME_LENGTH, "%s", pathname);
	HxLOG_Print("==> CXAPI_PVR_DLNA_Pb_OpenMetaFile save Path=%s \n", pstContext->szMetaName);

#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CAS_CONAX_URI)
	{
		// print uri data
		HUINT32	ulIdx	= 0, ulCount = 0;

		VK_Print(" === mmm === (%s, %d) Load URI DATA \n", __FUNCTION__, __LINE__);
		for (ulIdx = 0; ulIdx < CONAX_PVR_URI_SLOT_NUM; ulIdx++)
		{
			if (pstContext->stMetaInfo.stUri[ulIdx].ucUriValid)
			{
				VK_Print("idx(%d) uriTime(%d) APS(%d) EMI(%d) ICT(%d) RDC(%d) RTL(%d) TRI(%d) MR(%d) AOC(%d) \n",
							ulIdx, pstContext->stMetaInfo.stUri[ulIdx].ulTime,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eApsCopyCtrl,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eEmiCopyCtrl,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eICT,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eRedistributionCtrl,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eRetentionLimit,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eTrickPlayCtrl,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eMR,
							pstContext->stMetaInfo.stUri[ulIdx].stConaxDrmInfo.eDisableAnalOutput);
				ulCount++;
			}
		}
		VK_Print(" === mmm === (%d) URI loaded \n", ulCount);
	}
#endif

	if (pstCipherContents)
	{
		CX_MEM_Free(pstCipherContents);
	}
	if (pstPlainContents)
	{
		CX_MEM_Free(pstPlainContents);
	}
	if (pcPath)
	{
		CX_MEM_Free(pcPath);
	}

	return ERR_OK;
}


HERROR CXAPI_PVR_DLNA_Pb_ChangeTimecode(HUINT32 ulHandle, HUINT32 ulTimecode, HBOOL* pbUpdateKey)
{
	ConaxContext_t* pstContext	= NULL;
	//	HUINT32 ulAVDecId = 0xffff;
	//	HBOOL	bVideoDecoding = TRUE;
	int				i;

	pstContext = _cxpvr_dlna_PbGetSession(ulHandle);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!!\n");
		return ERR_FAIL;
	}

	/*
		1. playback Áß¿¡ 1ÃÊ ³»¿¡ °°Àº timecode°¡ ¿©·¯°³ µé¾î¿Â´Ù...
		2. pause Áß...(È£ÃâÇÏ´Â °÷¿¡¼­ Á¶°ÇÀ¸·Î Ã³¸®ÇßÀ½... pause»óÅÂÀÏ ¶§ È£Ãâ ¾ÈµÇµµ·Ï...)
		3. X¹è¼Ó play½Ã¿¡ session key º¯°æ Á÷ÀüÀÇ timecode°¡ °è¼Ó µé¾î¿À³×...
	*/

	if (pstContext->ulTimecode == ulTimecode)
	{
		pstContext->ulSameTimecode++;

		if (pstContext->ulSameTimecode >= 30)
		{
			if ((pstContext->bIncreaseTimecode == TRUE) && (pstContext->ulSessionPos != CONAX_PVR_SESSION_SLOT_NUM - 1))
			{
				pstContext->ulSessionPos++;
				*pbUpdateKey = TRUE;
				pstContext->ulSameTimecode = 0;
			}

			if ((pstContext->bIncreaseTimecode == FALSE) && (pstContext->ulSessionPos > 0))
			{
				pstContext->ulSessionPos--;
				*pbUpdateKey = TRUE;
				pstContext->ulSameTimecode = 0;
			}
		}

		return ERR_OK;
	}

	if (pstContext->ulTimecode < ulTimecode)
	{
		pstContext->bIncreaseTimecode = TRUE;
	}
	else
	{
		pstContext->bIncreaseTimecode = FALSE;
	}

	pstContext->ulTimecode = ulTimecode;
	pstContext->ulSameTimecode = 0;

	for (i = 0 ; i < CONAX_PVR_SESSION_SLOT_NUM ; i++)
	{
		if (pstContext->stMetaInfo.stSession[i].ulApplySessionKeyTime < ulTimecode)
		{
			if (i != (CONAX_PVR_SESSION_SLOT_NUM - 1))
			{
				if ((pstContext->stMetaInfo.stSession[i + 1].ulApplySessionKeyTime >= ulTimecode) ||	//
					(pstContext->stMetaInfo.stSession[i + 1].ulApplySessionKeyTime == 0))
				{
					break;
				}
			}
		}
	}

	if (i == pstContext->ulSessionPos)
	{
		// °°Àº Å°´Ï±î ¼ÂÆÃÇÒ ÇÊ¿ä ¾ø³×...
		return ERR_OK;
	}

	if (i >= CONAX_PVR_SESSION_SLOT_NUM)
	{
		i = 0;
	}

	pstContext->ulSessionPos = i;

	*pbUpdateKey = TRUE;

	return ERR_OK;
}


HERROR CXAPI_PVR_DLNA_Pb_CloseMetaFile(HUINT32 ulHandle)
{
	return	_cxpvr_dlna_PbReleaseSession(ulHandle);
}


#endif
#endif

/*********************** End of File ******************************/

