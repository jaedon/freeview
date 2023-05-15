/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cxcore_stream.c
	@brief  	 stream table °ü¸® µîµî

	Description: File¿¡ ´ëÇÑ ¼³¸íÀ» Àû½À´Ï´Ù.		\n
	Module: Layer¿Í ¸ðµâ ¸íÀ» Àû½À´Ï´Ù.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Include_________________________________
/*******************************************************************/
#include <_svc_cas_sub_svc_api.h>

#include <cxmgr_action.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_ecm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cxcore_session.h"
#include "../local_include/cxcore_sc.h"

#if defined(CONFIG_MW_CAS_CONAX_CPM)
#include "../local_include/cxcore_cpm.h"
#endif

#include "../local_include/cx_dscr.h"
/*******************************************************************/
#define ________CXCORE_STREAM_Golbal_Value____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Define__________________________________
/*******************************************************************/
typedef enum
{
	eCxStream_NotUsed,
	eCxStream_Used,
} CxStreamState_t;

// information about each elementary stream
typedef struct
{
	HUINT16			usPid;
	//CAS_EsType_t 	eType;
	CxStreamState_t	eState;
	Handle_t		hSession;
	Handle_t		hEcm;
	Handle_t		hDescrambler;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT16			usCwpkId;
#endif
} CxStream_t;

/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Value____________________________
/*******************************************************************/
STATIC HULONG	s_ulCxStreamSemId;

// elementary stream table
STATIC CxStream_t		s_astCxStream[CX_MAX_NUM_OF_ES];

/*******************************************************************/
#define ________CXCORE_STREAM_Global_Value____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Prototype________________________
/*******************************************************************/
STATIC HERROR local_cxstream_OpenDescrambler(int nIdx, Handle_t hSession);
STATIC HERROR local_cxstream_CloseDescrambler(int nIdx);

STATIC HUINT16 local_cxstream_GetEcmPidFromPmt(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo);
STATIC HERROR local_cxstream_Add(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo);
STATIC HERROR local_cxstream_Delete(HINT32 nIdx);
STATIC HERROR local_cxstream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid);
STATIC HERROR local_cxstream_UnlinkEcm(HINT32 nIdx);
STATIC HERROR local_cxstream_StartEcmFilter(HINT32 nIdx);
STATIC HERROR local_cxstream_StopEcmFilter(HINT32 nIdx);
/*******************************************************************/
#define ________CXCORE_STREAM_Private_Static_Prototype_Body___________________
/*******************************************************************/
/*******************************************************************/
#define ________CXCORE_STREAM_Public_Functions_Body___________________________
/*******************************************************************/
#define INIT_________________________________________________

HERROR CXSTREAM_InitCore(void)
{
	int		i;
	HINT32	nResult			= 0;

	ENTER_FUNCTION;

	nResult = CX_SEM_Create("CxCStrSem", &s_ulCxStreamSemId);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memset(s_astCxStream, 0, sizeof(CxStream_t) * CX_MAX_NUM_OF_ES);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		s_astCxStream[i].usPid = PID_NULL;
		//		s_astCxStream[i].eType 			= eEsType_None;
		s_astCxStream[i].eState = eCxStream_NotUsed;
		s_astCxStream[i].hSession = HANDLE_NULL;
		s_astCxStream[i].hEcm = HANDLE_NULL;
		s_astCxStream[i].hDescrambler = HANDLE_NULL;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
		s_astCxStream[i].usCwpkId = 0;
#endif
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define DESCRAMBLER__________________________________________________

// ¹«Á¶°Ç descrambler slotÀ» open ÇÏ´Â °ÍÀº ¾Æ´Ï°í, ¿¬°üµÈ ecmÀÌ ÀÖ´ÂÁö (scrambled ¹æ¼ÛÀÎÁö)È®ÀÎ ÇÏ°í open ÇÔ.
// À¯È¿ÇÑ stream ÀÎÁö, ÀÌ¹Ì slotÀ» °¡Áö°í ÀÖ´Â Áö µîµµ È®ÀÎÇÔ.
// ÀÌ ÇÔ¼ö¸¦ È£ÃâÇÏ´Â ÂÊ¿¡¼­´Â Á¶°Ç È®ÀÎÀ» ÇÒ ÇÊ¿ä ¾øÀÌ, ÀÏ´Ü try ÇØº»´Ù´Â °³³äÀ¸·Î.. µÇ¸é µÇ´Â°Å°í ¾Æ´Ï¸é ¸»°í..
// nIdx´Â stream table »óÀÇ index
STATIC HERROR local_cxstream_OpenDescrambler(int nIdx, Handle_t hSession)
{
	HUINT32		ulHandle;
	HUINT8		aucCW[2][CX_CW_LENGTH];
	HUINT8* 	pucCWPK	= NULL;
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	HUINT8		aucCWPK[CX_CPM_KEY_VALUE_BYTES];
#endif

	ENTER_FUNCTION;

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx\r\n");
		return ERR_FAIL;
	}

	// À¯È¿ÇÑ ESÀÎÁö È®ÀÎ
	if (s_astCxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}
	if (s_astCxStream[nIdx].eState != eCxStream_Used)
	{
		HxLOG_Error("not used stream\r\n");
		return ERR_FAIL;
	}

	// ÀÌ¹Ì descrambler slot ÇÒ´ç ¹Þ¾Ò´ÂÁö È®ÀÎ
	if (s_astCxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		HxLOG_Error("descrambler slot is already assigned\r\n");
		return ERR_FAIL;
	}

	// hEcm ÀÖ´ÂÁö È®ÀÎ (hEcm == NULLÀÌ¸é free ¼­ºñ½ºÀÌ°í descrambler ÇÊ¿ä ¾ø´Â°Å °°Àºµ¥)
	if (s_astCxStream[nIdx].hEcm == HANDLE_NULL)
	{
		HxLOG_Error("free service\r\n");
		return ERR_FAIL;
	}

	// descrambler slot ÇÒ´ç
	// descrambler slot ÇÒ´ç
	ulHandle = CXDSCR_Open(s_astCxStream[nIdx].hEcm, nIdx, s_astCxStream[nIdx].usPid, CXACT_GetActionHandle(CXSESSION_GetStbHandle(hSession)));
	if (ulHandle == HANDLE_NULL)
	{
		HxLOG_Error("fail to open descrambler slot\r\n");
		return ERR_FAIL;
	}

	s_astCxStream[nIdx].hDescrambler = (Handle_t)ulHandle;

	// ±âÁ¸¿¡ ecmÀ» ¹Þ¾Æ³õÀº°Ô ÀÖÀ»Áöµµ ¸ô¶ó.. ¾îÂ÷ÇÇ ¾È³ª¿Íµµ ±×¸¸ÀÌ´Ï ÇÑ¹ø CW ¼¼ÆÃÀ» ÇØº¸ÀÚ..
#if defined(CONFIG_MW_CAS_CONAX_CPM)
	if (CXECM_GetCWPK(s_astCxStream[nIdx].hEcm, aucCWPK) == ERR_OK)
	{
		pucCWPK = aucCWPK;
	}
#endif
	CXECM_GetCW(s_astCxStream[nIdx].hEcm, aucCW[0], aucCW[1]);
	CXDSCR_SetKeys(s_astCxStream[nIdx].hDescrambler, CX_CW_LENGTH, aucCW[0], CX_CW_LENGTH, aucCW[1], pucCWPK);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// nIdx´Â stream table »óÀÇ index
STATIC HERROR local_cxstream_CloseDescrambler(int nIdx)
{
	ENTER_FUNCTION;

	// for removing prevent issue
	if (nIdx < 0 || nIdx >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("wrong idx\r\n");
		return ERR_FAIL;
	}

	// À¯È¿ÇÑ ESÀÎÁö È®ÀÎ
	if (s_astCxStream[nIdx].usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	// ÀÌ¹Ì descrambler slot ÇÒ´ç ¹Þ¾Ò´Ù¸é close
	if (s_astCxStream[nIdx].hDescrambler != HANDLE_NULL)
	{
		//## descrambler slot close
		CXDSCR_Close(s_astCxStream[nIdx].hDescrambler, s_astCxStream[nIdx].usPid);
		s_astCxStream[nIdx].hDescrambler = HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSTREAM_SetCW(Handle_t hEcm, HUINT32 ulOddLength, HUINT32 ulEvenLength, HUINT8* pucOddCw, HUINT8* pucEvenCw, HUINT8* pucEncKey, HUINT16 usEncKeyId)
{
	int		i;
	HUINT8	aucCW[2][CX_CW_LENGTH];

	ENTER_FUNCTION;

	if (ulOddLength && pucOddCw == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}
	if (ulEvenLength && pucEvenCw == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	CX_memset(aucCW, 0x00, sizeof(aucCW));

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// °°Àº ecm¿¡ linkµÈ ¸ðµç stream¿¡ CW¸¦ Á¦°øÇÑ´Ù
		if (s_astCxStream[i].hEcm == hEcm && s_astCxStream[i].hDescrambler != HANDLE_NULL)
		{
			//## descrambler slot¿¡ CW¸¦ ³Ö¾îÁØ´Ù
			if (ulOddLength)
			{
				CX_memcpy(aucCW[0], pucOddCw, CX_CW_LENGTH);
			}

			if (ulEvenLength)
			{
				CX_memcpy(aucCW[1], pucEvenCw, CX_CW_LENGTH);
			}

			// encrypted CW°¡ ¾Æ´Ï¸é entropy reduction ÇØÁØ´Ù !!
			if (pucEncKey == NULL)
			{
				if (ulOddLength)
				{
					aucCW[0][3] = aucCW[0][0] + aucCW[0][1] + aucCW[0][2];
					aucCW[0][7] = aucCW[0][4] + aucCW[0][5] + aucCW[0][6];
				}

				if (ulEvenLength)
				{
					aucCW[1][3] = aucCW[1][0] + aucCW[1][1] + aucCW[1][2];
					aucCW[1][7] = aucCW[1][4] + aucCW[1][5] + aucCW[1][6];
				}
			}

			CXDSCR_SetKeys(s_astCxStream[i].hDescrambler, ulOddLength, aucCW[0], ulEvenLength, aucCW[1], pucEncKey);

			// CW´Â ´ÙÀ½ ecmÀ» ¹Þ±âÀü¿¡ ¾µ ÀÏÀÌ ÀÖÀ» ¼ö ÀÖÀ¸´Ï º¸°üÀ» ÇØµÐ´Ù.
#if defined(CONFIG_MW_CAS_CONAX_CPM)
			s_astCxStream[i].usCwpkId = usEncKeyId;//remove key È£Ãâ½Ã »èÁ¦ÇÏ±â À§ÇØ º¸°ü...
			if (pucEncKey != NULL)
			{
				CXECM_SetCWPK(hEcm, pucEncKey);
			}
#endif

			CXECM_SetCW(hEcm, aucCW[0], ulOddLength, aucCW[1], ulEvenLength);

#ifdef CX_SUPPORT_SHARED_DESCRAMBLER_SLOT
			// descrambler slotÀ» °øÀ¯ÇÑ´Ù¸é, set keyµµ ÇÑ¹ø¸¸ ÇÏ¸é µÈ´Ù.
			break;
#endif
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

#define STREAM_TABLE__________________________________________________

// ÁÖ¾îÁø esÀÇ ecm pid¸¦ Ã£´Â´Ù.
STATIC HUINT16 local_cxstream_GetEcmPidFromPmt(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo)
{
	int		i;
	HUINT16	usPid;
	HUINT16	usCaSysId;
	HINT32	nRet;

	ENTER_FUNCTION;

	if (pstPmtInfo == NULL || pstEsInfo == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return PID_NULL;
	}

	nRet = CXSC_GetCaSysId(CXSESSION_GetScSlot(hSession));
	if (nRet == CX_ERR_INVALID_PARAMETER)
	{
		HxLOG_Error("fail to get ca_sys_id\r\n");
		return ERR_FAIL;
	}
	usCaSysId = (HUINT16)nRet;

	usPid = PID_NULL;

	if (pstEsInfo->bElementCaFlag)
	{
		for (i = 0; i < pstEsInfo->ulNumOfCaDescriptor; i++)
		{
			//## ca system id ºñ±³ (ca_system_id´Â Ä«µå¿¡¼­ ¾òÀ» ¼ö ÀÖÀ» °ÍÀÌ´Ù.. ±×·±µ¥ Ä«µå°¡ ¾ø´Ù¸é ¾î¶»°Ô ÇÏÁö?)
			if (pstEsInfo->stElementCaDescriptor[i].usCaSysId == usCaSysId)
			{
				usPid = pstEsInfo->stElementCaDescriptor[i].usCaPid;
				break;
			}
		}
	}

	// ¸¸¾à elementary stream level¿¡ ecm pid¸¦ ¸øÃ£À¸¸é (ca_Descriptor°¡ ¾ø´Ù¸é), program level¿¡¼­ Ã£´Â´Ù.
	if (usPid == PID_NULL)
	{
		if (pstPmtInfo->bProgramCaFlag)
		{
			for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
			{
				//## ca system id ºñ±³ (ca_system_id´Â Ä«µå¿¡¼­ ¾òÀ» ¼ö ÀÖÀ» °ÍÀÌ´Ù.. ±×·±µ¥ Ä«µå°¡ ¾ø´Ù¸é ¾î¶»°Ô ÇÏÁö?)
				if (pstPmtInfo->stProgramCaDescriptor[i].usCaSysId == usCaSysId)
				{
					usPid = pstPmtInfo->stProgramCaDescriptor[i].usCaPid;
					break;
				}
			}
		}
	}

	HxLOG_Print("[Exit:ecm pid=%x]\r\n", usPid);
	return usPid;
}

STATIC HERROR local_cxstream_Add(Handle_t hSession, SvcCas_PmtInfo_t* pstPmtInfo, SvcCas_PmtElement_t* pstEsInfo)
{
	int		i;
	HUINT16	usEcmPid;

	ENTER_FUNCTION;

	if (pstPmtInfo == NULL || pstEsInfo == NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// ÀÏ´Ü pid·Î ºó slotÀ» È®ÀÎÇØº¸ÀÚ
		if (s_astCxStream[i].usPid == PID_NULL)
		{
			s_astCxStream[i].usPid = pstEsInfo->usPid;
			//s_astCxStream[i].eType 	= pstEsInfo->eEsType;
			s_astCxStream[i].hSession = hSession;

			///////////////////////////////////////////////////////////////////////////////////////////////
			//
			// !!! ¸¸¾à setPid¸¦ ¾È¾²°í pmt »óÀÇ ¸ðµç streamÀ» Ã³¸®ÇØ¾ß ÇÑ´Ù¸é, ¿©±â¼­ state¸¦ ¹«Á¶°Ç used·Î ÇØÁÖ¸é µÈ´Ù.. Âü ½±ÁÒ~À×~
			// s_astCxStream[i].eState = eCxStream_Used;
			//
			///////////////////////////////////////////////////////////////////////////////////////////////

			usEcmPid = local_cxstream_GetEcmPidFromPmt(hSession, pstPmtInfo, pstEsInfo);
			if (usEcmPid != PID_NULL)
			{
				// ecm pid°¡ ÀÖ´Ù¸é hEcm µî·ÏÇÏ°í
				local_cxstream_LinkEcm(hSession, i, usEcmPid);

				// [improve speed]: ¾Æ·¡ ÇÔ¼ö ¾È¿¡ µé¾î°¡¼­µµ usedÀÎÁö È®ÀÎ ÇÔ
				if (s_astCxStream[i].eState == eCxStream_Used)
				{
					local_cxstream_StartEcmFilter(i);

					// descramblerµµ µî·ÏÇØ¾ßÁö
					local_cxstream_OpenDescrambler(i, hSession);
				}
			}
			break;
		}
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_Delete(HINT32 nIdx)
{
	ENTER_FUNCTION;

	if (s_astCxStream[nIdx].hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	// descrambler slotÀ» closeÇÑ´Ù
	local_cxstream_CloseDescrambler(nIdx);

	// ecm°ú link ²÷¾î¿ä
	local_cxstream_UnlinkEcm(nIdx);

	s_astCxStream[nIdx].hSession = HANDLE_NULL;
	s_astCxStream[nIdx].eState = eCxStream_NotUsed;
	//s_astCxStream[nIdx].eType 		= eEsType_None;
	s_astCxStream[nIdx].usPid = PID_NULL;

	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_LinkEcm(Handle_t hSession, HINT32 nIdx, HUINT16 usEcmPid)
{
	Handle_t	hEcm;

	ENTER_FUNCTION;

	//°°Àº ecm ÀÖ³ª Ã£¾Æº¸°í ÀÖÀ¸¸é ¿¬°á, ¾øÀ¸¸é Ãß°¡
	hEcm = CXECM_Add(hSession, usEcmPid);

	s_astCxStream[nIdx].hEcm = hEcm;

	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	//## ecm filter start ÇØ¾ßµÇ³ª ?
	//local_cxstream_StartEcmFilter(nIdx);
	LEAVE_FUNCTION;
	return ERR_OK;
}

STATIC HERROR local_cxstream_UnlinkEcm(HINT32 nIdx)
{
	ENTER_FUNCTION;

	// ecm table¿¡¼­ »èÁ¦ 5IAv4B ecmBJ?!<- >K>F<- GR 0E@S (filter stop55 >K>F<- GR 0M@S)
	CXECM_Delete(s_astCxStream[nIdx].hEcm);

	s_astCxStream[nIdx].hEcm = HANDLE_NULL;

	LEAVE_FUNCTION;
	return ERR_OK;
}

//streamÀÌ not usedÀÌ¸é filter start ÇÒ ÇÊ¿ä ¾øÀ½
STATIC HERROR local_cxstream_StartEcmFilter(HINT32 nIdx)
{
	Handle_t	hEcm;

	ENTER_FUNCTION;

	hEcm = s_astCxStream[nIdx].hEcm;
	if (hEcm == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (s_astCxStream[nIdx].eState != eCxStream_Used)
	{
		HxLOG_Error("not used stream\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (CXSESSION_IsSelected(s_astCxStream[nIdx].hSession) == FALSE)
	{
		//¾ÆÁ÷ descrambleÇÏµµ·Ï ¼±ÅÃµÇÁö ¾ÊÀº »óÅÂÀÌ¹Ç·Î ecm filter¸¦ °ÉÁö ¾Êµµ·Ï ÇÑ´Ù.
		HxLOG_Error("not selected\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// filter start: both·Î °Ç´Ù.. ÀÌÈÄ·Î´Â ÀÚµ¿ toggle
	CXECM_StartFilter(hEcm, eCxEcm_Both);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ´Ù¸¥ session¿¡¼­ ¾²°í ÀÖ´Ù¸é filter¸¦ ±×³É µÎ°í, ¾Æ´Ï¸é stop ½ÃÄÑ¿ä
HERROR local_cxstream_StopEcmFilter(HINT32 nIdx)
{
	int			i;
	Handle_t	hEcm;
	Handle_t	hSession;

	ENTER_FUNCTION;

	hSession = s_astCxStream[nIdx].hSession;
	hEcm = s_astCxStream[nIdx].hEcm;

	if (hSession == HANDLE_NULL || hEcm == HANDLE_NULL)
	{
		HxLOG_Error("wrong param\r\n");
		return ERR_FAIL;
	}

	//´Ù¸¥ session¿¡¼­ »ç¿ëµÇ´ÂÁö Ã£¾Æº¸ÀÚ
	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL &&			// À¯È¿ÇÑ slot
			s_astCxStream[i].hSession != hSession && 		// ´Ù¸¥ session
			s_astCxStream[i].hEcm == hEcm && 				// °°Àº ecm
			s_astCxStream[i].eState == eCxStream_Used)	// »ç¿ëÁßÀÎ stream
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		// ¸ø Ã£¾Ò´Ù¸é filter stop
		CXECM_StopFilter(hEcm);
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// °¢ type º°·Î ÇÏ³ªÀÇ stream¸¸ µî·ÏÇÑ´Ù Ä¡°í..
//## ´õ »ý°¢ÇØº¸ÀÚ... audio°¡ µÎ°³ÀÏ¼öµµ ÀÖ¾î...
#if 0
HERROR CXSTREAM_SetPid(Handle_t hSession, CxEsPidInfo_t *pstEsInfo)
{
	int i, j;
	HBOOL bExist;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL)
		{
			bExist = FALSE;

			for (j = 0; j < pstEsInfo->ulPidNum; j++)
			{
				if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid == pstEsInfo->usPid[j])
				{
					bExist = TRUE;
					break;
				}
			}

			if (bExist)
			{
VK_Print("SetPID:update[%d,type:%d,pid:%x,st:%d]pid:%x\n", i,s_astCxStream[i].eType, s_astCxStream[i].usPid, s_astCxStream[i].eState, pstEsInfo->usPid[j]);
				// ÀÌ¹Ì µî·ÏµÈ pid ÀÌ°í, ¾ÕÀ¸·Îµµ »ç¿ëÇÒ pidÀÓ
				if (s_astCxStream[i].eState != eCxStream_Used)
				{
					s_astCxStream[i].eState = eCxStream_Used;
					// ¾ÆÁ÷ descrambler slotÀÌ openµÇÁö ¾Ê¾Ò´Ù¸é open ½Ãµµ
					local_cxstream_OpenDescrambler(i);
					//## ecm filter start ÇØ¾ßµÇ³ª ?
					local_cxstream_StartEcmFilter(i);
				}
			}
			else
			{
VK_Print("SetPID:delete[%d,type:%d,pid:%x,st:%d]pid:%x\n", i,s_astCxStream[i].eType, s_astCxStream[i].usPid, s_astCxStream[i].eState, pstEsInfo->usPid[j]);
				//ÀÖ´ø °Çµ¥, ÀÌÁ¦ ¾ø¾îÁú pid
				if (s_astCxStream[i].eState != eCxStream_NotUsed)
				{
					// descrambler slotÀ» closeÇÑ´Ù
					local_cxstream_CloseDescrambler(i);
				}
				s_astCxStream[i].eState  = eCxStream_NotUsed;
			}
		}
	}

	for (j = 0; j < pstEsInfo->ulPidNum; j++)
	{
		if (pstEsInfo->usPid[j] != PID_NULL)
		{
			bExist = FALSE;
			for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
			{
				if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid == pstEsInfo->usPid[j])
				{
					bExist = TRUE;
					break;
				}
			}

			if (bExist)
			{
				// ÀÌ¹Ì µî·ÏµÈ pid, ¾Õ¿¡¼­ Ã³¸®ÇßÀ½.
			}
			else
			{
VK_Print("SetPID:add[%d,type:%d,pid:%x]\n", j,pstEsInfo->eType[j],pstEsInfo->usPid[j]);
				//±âÁ¸¿¡´Â ¾ø´ø °Çµ¥, »õ·Î Ãß°¡µÉ pid
				for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
				{
					// ÀÏ´Ü pid·Î ºó slotÀ» È®ÀÎÇØº¸ÀÚ
					if (s_astCxStream[i].usPid == PID_NULL)
					{
						s_astCxStream[i].usPid 	= pstEsInfo->usPid[j];
						s_astCxStream[i].eType 	= pstEsInfo->eType[j];
						s_astCxStream[i].eState 	= eCxStream_Used;
						s_astCxStream[i].hSession	= hSession;
						break;
					}
				}
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}
#else
//HERROR CXSTREAM_SetPid(Handle_t hSession, CAS_EsType_t eType, HUINT16 usPid, HBOOL bAdd)
HERROR CXSTREAM_SetPid(Handle_t hSession, HUINT16 usPid, HBOOL bAdd)
{
	int i;

	HxLOG_Print("[Enter(%x,%x)]\r\n", usPid, bAdd);

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	if (usPid == PID_NULL)
	{
		HxLOG_Error("null pid\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
//		if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].eType == eType)
// type ±¸ºÐÀ» ÇÏÁö ¸»¾Æº¸ÀÚ...
		if (s_astCxStream[i].hSession == hSession)
		{
			HxLOG_Print("SetPID[%d,pid:%x,st:%d]pid:%x\n", i, s_astCxStream[i].usPid, s_astCxStream[i].eState, usPid);

			if (s_astCxStream[i].usPid == usPid)
			{
				// ÀÌ¹Ì µî·ÏµÈ pid (¿©±â·Î ¿À¸é pmt¸¦ ¹Þ¾Ò´Ù´Â °ÍÀÓ)
				if (bAdd)
				{
					// µî·Ï
					if (s_astCxStream[i].eState != eCxStream_Used)
					{
						s_astCxStream[i].eState = eCxStream_Used;
						// ¾ÆÁ÷ descrambler slotÀÌ openµÇÁö ¾Ê¾Ò´Ù¸é open ½Ãµµ
						local_cxstream_OpenDescrambler(i, hSession);
						//## ecm filter start ÇØ¾ßµÇ³ª ?
						local_cxstream_StartEcmFilter(i);
					}
				}
				else
				{
					// Á¦°Å
					if (s_astCxStream[i].eState != eCxStream_NotUsed)
					{
						s_astCxStream[i].eState = eCxStream_NotUsed;
						// descrambler slotÀ» closeÇÑ´Ù
						local_cxstream_CloseDescrambler(i);
						// ecm filter Ã³¸® - ´Ù¸¥µ¥¼­ ¾²¸é ±×³É µÎ°í, ¾²Áö ¾ÊÀ¸¸é ¾ø¾Ø´Ù.
//						local_cxstream_UnlinkEcm(i);
					}
				}

				CX_SEM_Release(s_ulCxStreamSemId);
				HxLOG_Print("[Exit]1\r\n");
				return ERR_OK;
			}
#if 0
			else
			{
				// ´Ù¸¥ pid °¡ µî·ÏµÇ¾î ÀÖ´Â °æ¿ì (±âÁ¸ °ÍÀº ³»¸®°í »õ·Î µî·ÏÇÑ´Ù)
				if (s_astCxStream[i].eState == eCxStream_Used)
				{
					// descrambler slotÀ» closeÇÑ´Ù
					local_cxstream_CloseDescrambler(i);
					// ecm filter Ã³¸® - ´Ù¸¥µ¥¼­ ¾²¸é ±×³É µÎ°í, ¾²Áö ¾ÊÀ¸¸é ¾ø¾Ø´Ù.
					local_cxstream_UnlinkEcm(i);
				}

				s_astCxStream[i].eState  = eCxStream_NotUsed;
				break;
			}
#endif
		}
	}

	// ¾øÀ¸¸é Ãß°¡ (¿©±â·Î ¿Ô´Ù¸é ¾ÆÁ÷ pmt¸¦ ¸ø¹ÞÀº »óÅÂÀÓ)
	if (bAdd)
	{
		for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
		{
			// ÀÏ´Ü pid·Î ºó slotÀ» È®ÀÎÇØº¸ÀÚ
			if (s_astCxStream[i].usPid == PID_NULL)
			{
				s_astCxStream[i].usPid 	= usPid;
//				s_astCxStream[i].eType 	= eType;
				s_astCxStream[i].eState 	= eCxStream_Used;
				s_astCxStream[i].hSession	= hSession;
				CX_SEM_Release(s_ulCxStreamSemId);
				HxLOG_Print("[Exit]2\r\n");
				return ERR_OK;
			}
		}
	}
	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]3\r\n");
	return ERR_FAIL;
}
#endif

HBOOL CXSTREAM_IsConaxStream(Handle_t hSession)
{
	int					i, j;
	SvcCas_PmtInfo_t* 	pstPmtInfo;
	SvcCas_PmtElement_t* 							pstEsInfo;
	HBOOL				bFound			= FALSE;

	ENTER_FUNCTION;

	pstPmtInfo = CXSESSION_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("no pmt\r\n");
		return FALSE;
	}

	// program level¿¡¼­ Ã£´Â´Ù.
	if (pstPmtInfo->bProgramCaFlag)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			//## ca system id ºñ±³
			if ((pstPmtInfo->stProgramCaDescriptor[i].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_CX & 0xFF00))
			{
				bFound = TRUE;
				break;
			}
		}
	}

	// es levelµµ Ã£¾Æº»´Ù
	if (bFound == FALSE)
	{
		for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
		{
			pstEsInfo = &pstPmtInfo->stElementInfo[i];

			if (pstEsInfo->bElementCaFlag)
			{
				for (j = 0; j < pstEsInfo->ulNumOfCaDescriptor; j++)
				{
					//## ca system id ºñ±³
					if ((pstEsInfo->stElementCaDescriptor[j].usCaSysId & 0xFF00) == (eDxCAS_GROUPID_CX & 0xFF00))
					{
						bFound = TRUE;
						break;
					}
				}
			}

			// Ã£¾ÒÀ¸¸é ±×¸¸ ÇÏÀÚ.
			if (bFound)
			{
				break;
			}
		}
	}

	HxLOG_Print("[Exit(%d)]\r\n", bFound);
	return bFound;
}

// pmt parsingÇÏ´Â °÷
HERROR CXSTREAM_ParsePmt(Handle_t hSession)
{
	int					i, j;
	HBOOL				bFound;
	HUINT16				usEcmPid;
	SvcCas_PmtInfo_t* 	pstPmtInfo;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	pstPmtInfo = CXSESSION_GetPmtInfo(hSession);

	if (pstPmtInfo == NULL)
	{
		HxLOG_Error("null pmt info\r\n");
		return ERR_FAIL;
	}

	// smart card°¡ ¾øÀ¸¸é ÇÏÁö ¸¶¶ó..
	// ca_system_id ±¸ºÐÇÏ´Â °Íµµ ±×·¸°í.. ecm filterµµ °ÉÇÊ¿ä°¡ ¾øÀ¸´Ï.. sc µé¾î¿À¸é ±×¶§ ÇÏÀÚ
	// ÀÌ°Ô ¼Óµµ¿¡ ¿µÇâÀ» ÁØ´Ù¸é ³ªÁß¿¡ ¹Ù²Ù¼¼¿ä. (¿©±â¶û scInserted¿¡ pmt_parse ´Ù½Ã È£ÃâÇÏ´Â ºÎºÐ ¼öÁ¤ÇÏ¸é µÉ°ÅÀÓ)
	if (CXSESSION_GetScSlot(hSession) == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	//## 1. ÀÌ¹Ì µî·ÏµÈ stream Á¤º¸¸¦ updated pmtº¸°í ¾÷µ¥ÀÌÆ® È¤Àº »èÁ¦
	//##		hSession, pid, type °°À¸¸é update
	//##			hEcmµµ °°À¸¸é skip
	//##			hEcm ´Ù¸£¸é ±âÁ¸ hEcm close ½Ãµµ (´Ù¸¥ °÷¼­ »ç¿ëÁßÀÌ¸é Áö¿öÁöÁö´Â ¾ÊÀ» °Í), »õ·Î¿î hEcm Ãß°¡ (state=usedÀÌ¸é filter start)
	//##			descrambler open ½Ãµµ (hEcm ÀÖ´Âµ¥ descrambler ÇÒ´ç ¸ø¹ÞÀº °æ¿ì¶ó¸é)
	//##		°°Àº°Å ¹ß°ßµÇÁö ¾ÊÀ¸¸é delete
	//##			hEcm close ½Ãµµ
	//##			descrambler close ½Ãµµ
	//## 2. updated pmt¿¡ ³²Àº streamÀÌ ÀÖ´Ù¸é Ãß°¡
	//##		Ãß°¡ ÇÏ°í, hEcmµµ Ãß°¡ÇÏ°í.. ºÁ¼­ filter³ª descramblerµµ openÇÏ°í..

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].hSession == hSession && s_astCxStream[i].usPid != PID_NULL)
		{
			// ÀÌ¹Ì ÀúÀåµÈ elementary streamÀÇ º¯°æ/»èÁ¦ ¿©ºÎ¸¦ È®ÀÎÇÑ´Ù

			bFound = FALSE;
			for (j = 0; j < pstPmtInfo->ulNumOfElement; j++)
			{
				if (s_astCxStream[i].usPid == pstPmtInfo->stElementInfo[j].usPid)
				//&& s_astCxStream[i].eType == pstPmtInfo->stElementInfo[j].eEsType)
				{
					//µ¿ÀÏ es°¡ »õ·Î¿î pmt¿¡µµ Á¸ÀçÇÔ - º¯°æ ¿©ºÎ È®ÀÎ (free <-> scrambled, ecm pid º¯°æ ¿©ºÎ)
					HxLOG_Print("ALREADY EXIST(%x)\n", pstPmtInfo->stElementInfo[i].usPid);

					bFound = TRUE;
					usEcmPid = local_cxstream_GetEcmPidFromPmt(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[j]);

					if (usEcmPid == PID_NULL)
					{
						// hEcm = null ÀÌ¸é º¯µ¿ ¾ø´Â °ÍÀÓ, ¾Æ´Ï¶ó¸é update
						if (s_astCxStream[i].hEcm != HANDLE_NULL)
						{
							HxLOG_Print("UPDATE(free)\n");
							// scrambled --> free·Î º¯°æ
							// descrambler slot ³»¸°´Ù.
							local_cxstream_CloseDescrambler(i);
							// hEcm ³»¸°´Ù..
							local_cxstream_UnlinkEcm(i);
						}
					}
					else
					{
						// ecm pid °¡ °°´Ù¸é º¯°æÀÌ ¾ø´Ù´Â °Í, ¾Æ´Ï¶ó¸é update
						if (CXECM_GetPid(s_astCxStream[i].hEcm) != usEcmPid)
						{
							HxLOG_Print("UPDATE(scr)\n");
							// hEcm º¯°æ ÇÊ¿äÇÔ
							//±âÁ¸ hEcmÀº È®ÀÎÇØ¼­ ³»¸®µµ·ÏÇÏ°í, »õ hEcm Ãß°¡ÇÑ´Ù.
							if (s_astCxStream[i].hEcm != HANDLE_NULL)
							{
								local_cxstream_UnlinkEcm(i);
							}
							local_cxstream_LinkEcm(hSession, i, usEcmPid);

							// descrambler´Â ±×³É µÎ°í, ¸¸¾à free Ã¤³Î ÀÌ¾ú´Ù¸é, descrambler slot ¹Þ¾Æ¾ßÇÔ
							// sc Á¦°Å ÈÄ µ¿ÀÏÇÑ service°¡ stop -> start µÉ °æ¿ì descrambler µî·Ï°ú ecm filter°¡ µ¿ÀÛÇÏÁö ¾Ê¾Æ¿ä...
						}

						local_cxstream_OpenDescrambler(i, hSession);
						local_cxstream_StartEcmFilter(i);
					}
					break;
				}
			}

			if (bFound == FALSE)
			{
				HxLOG_Print("NOT FOUND-DELETE(%x)\n", s_astCxStream[i].usPid);
				// µ¿ÀÏ es°¡ »õ·Î¿î pmt¿¡ Á¸ÀçÇÏÁö ¾ÊÀ½ - »èÁ¦ Ã³¸®
				local_cxstream_Delete(i);
			}
		}
	}

	// »õ·Î¿î pmt¿¡ Ãß°¡µÈ streamÀÌ ÀÖ´ÂÁö È®ÀÎ
	for (i = 0; i < pstPmtInfo->ulNumOfElement; i++)
	{
		if (pstPmtInfo->stElementInfo[i].usPid != PID_NULL && pstPmtInfo->stElementInfo[i].usPid != 0x0)
		{
			// ÀÌ¹Ì Á¸ÀçÇÏ´Â pid¸é ¾Õ¿¡¼­ updateµÇ¾úÀ» °ÍÀÌ´Ï skipÇÏ°í, ¾ø´ø °Í¸¸ »õ·Î Ãß°¡
			for (j = 0; j < CX_MAX_NUM_OF_ES; j++)
			{
				if (s_astCxStream[j].hSession == hSession && s_astCxStream[j].usPid == pstPmtInfo->stElementInfo[i].usPid)
				{
					break;
				}
			}

			if (j == CX_MAX_NUM_OF_ES)
			{
				HxLOG_Print("NEW!(%x)\n", pstPmtInfo->stElementInfo[i].usPid);
				// µ¿ÀÏ pid ¹ß°ßµÇÁö ¾Ê¾ÒÀ½, Ãß°¡
				local_cxstream_Add(hSession, pstPmtInfo, &pstPmtInfo->stElementInfo[i]);
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXSTREAM_DeleteAll(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		// ÇØ´ç sessionÀÇ ¸ðµç streamÀ» Á¦°ÅÇÑ´Ù
		if (s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_Delete(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// session ¹× pid Á¤º¸¸¦ ÃÊ±âÈ­ ÇÑ´Ù
HERROR CXSTREAM_ResetStream(Handle_t hSession)
{
	HUINT32	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].hSession == hSession)
		{
			// descrambler slotÀ» closeÇÑ´Ù
			local_cxstream_CloseDescrambler(i);

			// session Á¤º¸ ¹× pid Á¤º¸¸¦ clear
			// descrambler´Â sc insert½Ã parsePMT ÇÏ¸é¼­ openµÈ´Ù
			s_astCxStream[i].hSession = HANDLE_NULL;
			s_astCxStream[i].usPid = PID_NULL;

			// setPid°¡ ´Ù½Ã ³»·Á¿ÀÁö ¾ÊÀ¸¹Ç·Î state´Â À¯Áö
			//s_astCxStream[i].eState	 	= eCxStream_NotUsed;
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// free or scrambled svc È®ÀÎ
HERROR CXSTREAM_IsScrambledSvc(Handle_t hSession, HBOOL* pbScrambled)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	*pbScrambled = FALSE;

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			//if (s_astCxStream[i].hEcm != HANDLE_NULL && s_astCxStream[i].eState == eCxStream_Used)
			if (s_astCxStream[i].hEcm != HANDLE_NULL)
			{
				//¿¬°áµÈ ecmÀÌ ÀÖ´Ù¸é scrambled svc·Î º»´Ù.
				*pbScrambled = TRUE;
				break;
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ÇØ´ç session (service)¿¡ ¼ÓÇÑ streamÀÌ »ç¿ëÇÏ´Â ¸ðµç ecm filter¸¦ start ½ÃÅ²´Ù.
HERROR CXSTREAM_StartAllEcmFilter(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("wrong handle\r\n");
		return ERR_FAIL;
	}

	// smart card°¡ ¾øÀ¸¸é filter start ÇÒ ÇÊ¿ä ¾ø´Ù
	if (CXSESSION_GetScSlot(hSession) == CX_SC_NONE)
	{
		HxLOG_Error("no smartcard\r\n");
		return ERR_OK;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_StartEcmFilter(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ÇØ´ç session (service)¿¡ ¼ÓÇÑ streamÀÌ »ç¿ëÇÏ´Â ¸ðµç ecm filter¸¦ stop ½ÃÅ²´Ù.
HERROR CXSTREAM_StopAllEcmFilter(Handle_t hSession)
{
	int	i;

	ENTER_FUNCTION;

	if (hSession == HANDLE_NULL)
	{
		HxLOG_Error("null handle\r\n");
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hSession == hSession)
		{
			local_cxstream_StopEcmFilter(i);
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ÇØ´ç session (service)¿¡ ¼ÓÇÑ streamÀÌ »ç¿ëÇÏ´Â ¸ðµç ecm filter¸¦ restart ½ÃÅ²´Ù.
HERROR CXSTREAM_RestartAllEcmFilter(Handle_t hSession)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	nErr = CXSTREAM_StopAllEcmFilter(hSession);

	if (nErr == ERR_OK)
	{
		nErr = CXSTREAM_StartAllEcmFilter(hSession);
	}

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXSTREAM_FindStreamBySessionAndEcm(Handle_t hSession, Handle_t hEcm)
{
	int	i;

	ENTER_FUNCTION;

	//	sem °É¸é ¾ÈµÊ (stream->ecm->stream call)
	//	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL 		//
		&& s_astCxStream[i].hSession == hSession	//
		&& s_astCxStream[i].hEcm == hEcm)			//
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

Handle_t CXSTREAM_GetFirstSessionByEcm(Handle_t hEcm)
{
	int	i;

	ENTER_FUNCTION;

	//	sem °É¸é ¾ÈµÊ (stream->ecm->stream call)
	//	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hEcm == hEcm)
		{
			break;
		}
	}

	if (i >= CX_MAX_NUM_OF_ES)
	{
		HxLOG_Error("not found\r\n");
		return HANDLE_NULL;
	}

	LEAVE_FUNCTION;
	return s_astCxStream[i].hSession;
}

HUINT32 CXSTREAM_GetAllSessionByEcm(Handle_t hEcm, Handle_t* phList)
{
	int		i, j;
	HUINT32	ulCount;

	ENTER_FUNCTION;

	CX_SEM_Get(s_ulCxStreamSemId);

	for (i = 0, ulCount = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usPid != PID_NULL && s_astCxStream[i].hEcm == hEcm)
		{
			// È¤½Ã °°Àº sessionÀ» ÀÌ¹Ì Ã£¾Ò¾ú´ÂÁö È®ÀÎ
			for (j = 0; j < ulCount; j++)
			{
				if (phList[j] == s_astCxStream[i].hSession)
				{
					break;
				}
			}

			if (j == ulCount)
			{
				phList[ulCount++] = s_astCxStream[i].hSession;
			}
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);
	LEAVE_FUNCTION;
	return ulCount;
}

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXSTREAM_RemoveCWPK(HUINT16 usKeyId)
{
	int	i;

	HxLOG_Print("[Enter]\n");

	if (usKeyId == 0)
	{
		return ERR_FAIL;
	}

	CX_SEM_Get(s_ulCxStreamSemId);

	// ÇØ´ç key id¸¦ »ç¿ëÇÏ´Â descramblerÀÇ CWPK¸¦ clear ÇÑ´Ù.
	for (i = 0; i < CX_MAX_NUM_OF_ES; i++)
	{
		if (s_astCxStream[i].usCwpkId == usKeyId && s_astCxStream[i].hDescrambler != HANDLE_NULL)
		{
			CXDSCR_ClearCWPK(s_astCxStream[i].hDescrambler);
			s_astCxStream[i].usCwpkId = 0;
		}
	}

	CX_SEM_Release(s_ulCxStreamSemId);

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}
#endif

#if defined(CONFIG_DEBUG)
STATIC HUINT8* local_cxstream_DEBUG_GetScStateStr(CxStreamState_t eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCxStream_NotUsed)
		ENUM_TO_STR(eCxStream_Used)
	}

	return "Unknown";
}

void CMD_CXSTREAM_PrintStreamContext(void)
{
	HUINT32	i;

	for (i = 0 ; i < CX_MAX_NUM_OF_ES ; i++)
	{
		HxLOG_Warning(HxANSI_COLOR_GREEN("CONAX STREAM (%d)\n"), i);

		HxLOG_Warning("\t hSession (%d, 0x%x)\n", CXSESSION_GetSessionIdx(s_astCxStream[i].hSession));
		HxLOG_Warning("\t eState (%s)\n", local_cxstream_DEBUG_GetScStateStr(s_astCxStream[i].eState));
		HxLOG_Warning("\t usPid (0x%x)\n", s_astCxStream[i].usPid);

		if(s_astCxStream[i].eState != eCxStream_Used)
			continue;

		HxLOG_Warning("\t hEcm (0x%x)\n", s_astCxStream[i].hEcm);
		HxLOG_Warning("\t hDescrambler (%d)\n", s_astCxStream[i].hDescrambler);
		HxLOG_Warning("\t usCwpkId (0x%x)\n", s_astCxStream[i].usCwpkId);
	}
}
#endif

