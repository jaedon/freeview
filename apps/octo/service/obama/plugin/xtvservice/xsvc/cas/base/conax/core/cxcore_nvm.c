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
	@file     cxcore_nvm.c
	@brief    cxcore_nvm.c (CONAX NVM)

	Description: nvm 관리 등등 \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <tapi.h>

#include "../local_include/cx_nvram.h"

#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"

#include "../local_include/cxcore_ui.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_cpm.h"

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/

// nvm cache를 사용하려면 아래 define 사용
// init시 미리 read하고, 이후로는 cache에서만 read, write는 write-through (cache에도 쓰고 실제 메모리에도 쓰고)
//#define CX_NVM_CACHE

#define CX_NVM_USERTEXT_OFFSET					0
/*
 * pn_dsjeong :
 * CX_NVM_USERTEXT_SLOT_SIZE 값으로 쓸 때, 항상 4bytes align에 맞춰 CxUserText_t 구조체 bytes 값 제대로 계산해야 함!!!
 * 수정 전 CX_NVM_USERTEXT_SLOT_SIZE가 CxUserText_t 실제 bytes 수 보다 2 bytes 작게 define 되면서,
 * 쓰려고 하는 영역 포함 다음 index msg 영역 앞 부분까지 overwrite 되면서 다음 index msg가 날라가 버리는 케이스가 있었음.
 *
 *  (2014.08.07 기준 CxUserText_t 구조체 전체 bytes 계산 예시)
 *	szText : 256 + 1
 * 	ucDisplayed : 1
 * 	usDuration : 2 => 여기까지 4bytes align에 맞춰 256 + 4 bytes
 * 	ucSeqNo : 1
 * 	ucRepeat : 1
 * 	usInterval : 2 => 여기까지 4bytes align에 맞춰 4 bytes
 * 	HxDATETIME_t : (DATE_T)2 + 1 + 1 + (TIME_T)1 + 1 + 1 + 2 => 여기까지 4bytes align에 맞춰 12 bytes(3bytes 빈 공간 추가 됨).
 * 	UNIXTIME : 4 => 4bytes align에 맞춰 그대로 4 bytes
 * 	따라서 4bytes align에 맞춰 전체 사이즈를 계산하면 280 bytes.
 */
#define CX_NVM_USERTEXT_SLOT_SIZE				(CX_MAX_USER_TEXT_LEN + 1 + 1 + 2 + 1 + 1 + 2 + 12 + 4) //sizeof(CxUserText_t) 256 + 24
#define CX_NVM_USERTEXT_SLOT_NUM				CX_MAX_USERTEXT_SLOT_NUM
#define CX_NVM_USERTEXT_SIZE					(CX_NVM_USERTEXT_SLOT_SIZE * CX_NVM_USERTEXT_SLOT_NUM)
#define CX_NVM_USERTEXT_SLOT_PREVIOUS_SIZE		(CX_MAX_USER_TEXT_LEN + 12 + 6)	//(256 + 6)	//256	//sizeof(CxUserText_t)
#define CX_NVM_USERTEXT_PREVIOUS_SIZE			(CX_NVM_USERTEXT_SLOT_PREVIOUS_SIZE * CX_NVM_USERTEXT_SLOT_NUM)

#define CX_NVM_FINGERPRINT_OFFSET				(CX_NVM_USERTEXT_OFFSET + CX_NVM_USERTEXT_SIZE)
#define CX_NVM_FINGERPRINT_SLOT_SIZE			(64 + 4 + 4)	// greater than sizeof(CxFingerprint_t) (+ Cent Time + isProtectedData)
#define CX_NVM_FINGERPRINT_SLOT_NUM				1
#define CX_NVM_FINGERPRINT_SIZE					(CX_NVM_FINGERPRINT_SLOT_SIZE * CX_NVM_FINGERPRINT_SLOT_NUM)

#if defined(CONFIG_MW_CAS_CONAX_CPM)
#define	CX_NVM_KEYSTORE_OFFSET				(CX_NVM_FINGERPRINT_OFFSET + CX_NVM_FINGERPRINT_SIZE)
#define	CX_NVM_KEYSTORE_SLOT_SIZE			17408//1280 -> PT 요구조건 (최소 16k) => key number(256) * sizeof(CxCpmKey_t)보다 크게...
#define	CX_NVM_KEYSTORE_SLOT_NUM			1
#define	CX_NVM_KEYSTORE_SIZE				(CX_NVM_KEYSTORE_SLOT_SIZE * CX_NVM_KEYSTORE_SLOT_NUM)
#else
#define	CX_NVM_KEYSTORE_OFFSET				(CX_NVM_FINGERPRINT_OFFSET + CX_NVM_FINGERPRINT_SIZE)
#define	CX_NVM_KEYSTORE_SIZE				0
#endif

#if defined(CONFIG_MW_MM_PVR)
#define CX_NVM_PVRKEY_OFFSET				(CX_NVM_KEYSTORE_OFFSET + CX_NVM_KEYSTORE_SIZE)
#define CX_NVM_PVRKEY_SIZE					36	// key(16 bytes) + sha1 hash (20bytes)
#else
#define CX_NVM_PVRKEY_OFFSET				(CX_NVM_KEYSTORE_OFFSET + CX_NVM_KEYSTORE_SIZE)
#define CX_NVM_PVRKEY_SIZE					0
#endif

#define CX_NUM_TOTAL_SIZE					(CX_NVM_USERTEXT_SIZE + CX_NVM_FINGERPRINT_SIZE + CX_NVM_KEYSTORE_SIZE + CX_NVM_PVRKEY_SIZE)

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/
static HERROR CXNVM_DelayedWriteUserText(CxUserText_t* pstText, HBOOL bAsync);
static HERROR CXNVM_DelayedWriteUserTextForRemove(CxUserText_t* pstText);
/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/

STATIC CxUserText_t		s_stUserTextBuf;
STATIC CxFingerprint_t	s_stFingerprintBuf;
#if defined(CX_NVM_CACHE)
STATIC HUINT8* 			pucCxNvmCache;
#endif /* CX_NVM_CACHE */

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

#define INIT_________________________________________________

HERROR CXNVM_InitCore(void)
{
	ENTER_FUNCTION;
#if defined(CONFIG_MW_CAS_CONAX_TEXT) || defined(CONFIG_MW_CAS_CONAX_FINGERPRINT) || defined(CONFIG_MW_CAS_CONAX_CPM)
#if defined(CX_NVM_CACHE)
	pucCxNvmCache = (HUINT8 *)CX_MEM_Alloc(CX_NUM_TOTAL_SIZE);

	if (pucCxNvmCache)
	{
		int i;
		HUINT32 ulOffset, ulSize;

		for(i = 0 ; i < eCxNvmFile_Max ; i++)
		{
			switch(i)
			{
				case eCxNvmFile_UserText:
					ulOffset = CX_NVM_USERTEXT_OFFSET;
					ulSize = CX_NVM_USERTEXT_SIZE;
					break;

				case eCxNvmFile_Fingerprint:
					ulOffset = CX_NVM_FINGERPRINT_OFFSET;
					ulSize = CX_NVM_FINGERPRINT_SIZE;
					break;

				case eCxNvmFile_KeyStore:
					ulOffset = CX_NVM_KEYSTORE_OFFSET;
					ulSize = CX_NVM_KEYSTORE_SIZE;
					break;

				case eCxNvmFile_PvrKey:
					ulOffset = CX_NVM_PVRKEY_OFFSET;
					ulSize = CX_NVM_PVRKEY_SIZE;
					break;

				default:
					return;
			}

			CXNVM_Read(i, 0, ulSize, pucCxNvmCache + ulOffset);
		}
	}
#endif
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}

#define UserText_________________________________________________

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#if defined(CONFIG_DEBUG)
static HERROR local_cas_cx_DebugPrintCurrentTime(CxUserText_t* pstText)
{
	UNIXTIME	utCurrTime, utDBStartTime;
	HERROR		hErr;

	hErr = VK_CLOCK_GetTime((HULONG *)&utCurrTime);
	if (hErr == ERR_OK)
	{
		HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&(pstText->stStartDataTime), &utDBStartTime);
		HxLOG_Error("utCurrTime(%d), utDBStartTime(%d)\n", utCurrTime, utDBStartTime);
	}

	return ERR_OK;
}
#endif

// ERR_OK 면 같은것
// ERR_FAIL이면 다른것
HERROR local_cxnvm_CompareUserText(HUINT8 ucSlotIdx, CxUserText_t* pstText)
{
	ENTER_FUNCTION;

	// prevent #123047 fix.
	(void)CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);

#if 0
	if (pstText->stUnixTime != s_stUserTextBuf.stUnixTime)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
#endif

	if (pstText->ucRepeat != s_stUserTextBuf.ucRepeat)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstText->usDuration != s_stUserTextBuf.usDuration)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstText->usInterval != s_stUserTextBuf.usInterval)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (CX_strcmp((char*)pstText->szText, (char*)s_stUserTextBuf.szText))
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (CX_memcmp(&(pstText->stStartDataTime), &(s_stUserTextBuf.stStartDataTime), sizeof(HxDATETIME_t)))
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ERR_OK 면 같은것
// ERR_FAIL이면 다른것
HERROR local_cxnvm_CompareUserTextWithoutStartTime(HUINT8 ucSlotIdx, CxUserText_t* pstText)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	CX_memset(&s_stUserTextBuf, 0, sizeof(s_stUserTextBuf));
	nErr = CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);
	if (nErr == ERR_FAIL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstText->ucRepeat != s_stUserTextBuf.ucRepeat)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
	if (pstText->usDuration != s_stUserTextBuf.usDuration)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
	if (pstText->usInterval != s_stUserTextBuf.usInterval)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
	if (CX_memcmp((char*)pstText->szText, (char*)s_stUserTextBuf.szText, CX_MAX_USER_TEXT_LEN))
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

// ERR_OK 면 have been displayed.
// ERR_FAIL이면 never have been displayed.
HERROR local_cxnvm_IsAlreadyDisplayed(HUINT8 ucSlotIdx, CxUserText_t* pstText)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	CX_memset(&s_stUserTextBuf, 0, sizeof(s_stUserTextBuf));
	nErr = CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);
	if (nErr == ERR_FAIL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (s_stUserTextBuf.ucDisplayed == 0)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}
	LEAVE_FUNCTION;
	return ERR_OK;
}

HUINT32 CXNVM_GetUserTextSlotNum(void)
{
	return CX_NVM_USERTEXT_SLOT_NUM;
}

HERROR CXNVM_ReadUserText(HUINT8 ucSlotIdx, CxUserText_t* pstText)
{
	HERROR	nErr;
	HUINT32	ulOffset;

	ENTER_FUNCTION;

	if (ucSlotIdx > CX_NVM_USERTEXT_SLOT_NUM)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstText == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	ulOffset = ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;

#if defined(CX_NVM_CACHE)
	CX_memcpy(pstText, pucCxNvmCache + CX_NVM_USERTEXT_OFFSET + ulOffset, sizeof(CxUserText_t));
	nErr = ERR_OK;
#else
	nErr = CXNVM_Read(eCxNvmFile_UserText, ulOffset, sizeof(CxUserText_t), (HUINT8 *)pstText);
#endif

	LEAVE_FUNCTION;
	return nErr;
}

// UT를 display 했다고 check 하는 함수.
HERROR CXNVM_UserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
	HERROR	nErr;
	HUINT8	ucSlotIdx;

#if defined(CX_NVM_CACHE)
	HUINT32	ulOffset;
#endif

	ENTER_FUNCTION;

	ucSlotIdx = ucSeqNo;

	VK_TASK_Sleep(200); 	// Smart card에서 받고 write 되기 전에 read하는 경우가 존재하여 STBY 이후 UserText data가 이전 걸로 되어 잘못 출력되는 현상을 막기 위함. CT R.7
	nErr = CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);
	if (nErr == ERR_OK)
	{
		s_stUserTextBuf.ucDisplayed++;

		if (ulOpt == eCxUTDisplayCntMax)
		{
			// Display 시간이 지난 경우
			s_stUserTextBuf.ucDisplayed = s_stUserTextBuf.ucRepeat;
		}

#if defined(CX_NVM_CACHE)
		ulOffset = CX_NVM_USERTEXT_OFFSET + ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;
		CX_memcpy(&s_stUserTextBuf, pucCxNvmCache + ulOffset, sizeof(CxUserText_t));
#endif

		/**
		  *	다른 task에서 저장하게 하면, ui 처리보다 늦게 저장되는 경우가 생긴다
		  *	(ex) mail msg를 읽었으나 mail icon이 남아 있음
		  *	하여 msg로 처리하지 않고 직접 write하도록 수정
		 **/
		//CXCORE_SendMsgToSubTask(eCxMsg_Nvm_WriteUserText, HANDLE_NULL, (HUINT32)&s_stUserTextBuf, (HUINT32)0); // 이대로 사용하면 s_stUserTextBuf를 free하려하므로 안됨..
		CXNVM_DelayedWriteUserText(&s_stUserTextBuf, FALSE);
	}

	LEAVE_FUNCTION;
	return nErr;
}

HERROR CXNVM_OverWriteUserText(HUINT8 ucSeqNo, HUINT32 ulCurrentUnixTime)
{
	HERROR	nErr;
	HUINT8	ucSlotIdx;

#if defined(CX_NVM_CACHE)
	HUINT32	ulOffset;
#endif

	ENTER_FUNCTION;

	ucSlotIdx = ucSeqNo;

	nErr = CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);
	if (nErr == ERR_OK)
	{
		s_stUserTextBuf.ulImmediateStartTime = (UNIXTIME)ulCurrentUnixTime;

#if defined(CX_NVM_CACHE)
		ulOffset = CX_NVM_USERTEXT_OFFSET + ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;
		CX_memcpy(&s_stUserTextBuf, pucCxNvmCache + ulOffset, sizeof(CxUserText_t));
#endif

		/**
		  *	다른 task에서 저장하게 하면, ui 처리보다 늦게 저장되는 경우가 생긴다
		  *	하여 msg로 처리하지 않고 직접 write하도록 수정
		 **/

		CXNVM_DelayedWriteUserText(&s_stUserTextBuf, FALSE);
	}

	LEAVE_FUNCTION;
	return nErr;
}

// ERR_FAIL이 return되면 화면에 표시하지 않는다.
// 동작 방식은 2가지
// 1. 기본
//	: task로 msg 던지고 (text는 copy), task에서 다시 delayedWrite를 호출한다.
// 2. CX_NVM_CACHE
//	: task로 msg를 던지고 (text는 cache에 기록된 것 사용), task에서 다시 delayedWrite를 호출한다.
HERROR CXNVM_WriteUserText(CxUserText_t* pstText, HBOOL bStartTimeOmit)
{
	HUINT8			ucSlotIdx;
#if defined(CX_NVM_CACHE)
	HUINT32			ulOffset;
#endif
	//	HERROR				hErr;

	ENTER_FUNCTION;

	if (pstText == NULL)
	{
		HxLOG_Error("There is no Text Data.\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// seq번호를 보고 slot을 찾는다. slot idx = seqNo % (max_slot_idx + 1)
	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;
	pstText->ucSeqNo = ucSlotIdx;

#if defined(CONFIG_DEBUG)
	local_cas_cx_DebugPrintCurrentTime(pstText);
#endif

	// 같은 것이 있는지 확인한다.
	if (local_cxnvm_CompareUserText(ucSlotIdx, pstText) == ERR_OK)
	{
		// 같은 것이 있으면 저장하지 않고 fail return한다. -> 화면에 표시하지 않는다.
		HxLOG_Error("Same MSG is already stored\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (bStartTimeOmit == TRUE)
	{
		CXUI_SetRecvUTTimeStamp();
	}

	// CXHD-5100C 2011-08 1st Conax PT. USER_TEXT 과거 시간일 경우에는 받은 즉시 표시를 해야한다고 함. by Teppo in Labwise
	// 아래 코드에서 하는 일이 과거이거나 SI로 부터 시간을 얻어오지 못한 경우 block 하는 건데 의미가 없다고 생각되어 막음. (아래 mailbox message의 경우는 주석과 코드가 서로 일치하지 않음.)
	// SI 에서 시간 받는 일은 CXUI_UserTextReceived()에서 함.  File에 저장한 USER_TEXT는 여기서 거르지 않고 보낸다.
#if 0
	//Immediate Message
	if((pstText->usDuration != 0))
	{
		// if PI_START is
		if(bStartTimeOmit == FALSE)
		{
			if(local_cas_cx_CheckPastTime(&(pstText->stStartDataTime)))
			{
				HxLOG_Error("The DateTime is past time....\r\n");
				LEAVE_FUNCTION;
				return ERR_FAIL;
			}
		}
		// if PI_START is not
		else
		{
			// do nothing. 비교하지 않고, Display로 넘김.
		}
	}
	// Mailbox Message
	else if(bStartTimeOmit != FALSE)
	{
		// Mail Message && 미래 시간 case
		hErr = local_cas_cx_CheckCurrentTime();
		if(hErr != ERR_OK)
		{
			HxLOG_Error("The Current Time isn't set by SI....\r\n");
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
#endif

	HxLOG_Error("[UTInfo] Displayed(%d), Repeat(%d), SeqNo(%d), Duration(%d), Interval(%d)\r\n", pstText->ucDisplayed, pstText->ucRepeat, (pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM), pstText->usDuration, pstText->usInterval);

#if defined(CX_NVM_CACHE)
	ulOffset = CX_NVM_USERTEXT_OFFSET + ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;
	CX_memcpy(pucCxNvmCache + ulOffset, pstText, sizeof(CxUserText_t));
#endif

	CXNVM_DelayedWriteUserText(pstText, TRUE);
	HxLOG_Error("###### Write OK ######\n");
	LEAVE_FUNCTION;
	return ERR_OK;
}

//좀 더 제대로 하려면 main task와 별개의 task에서 write해야 된다.
// 기존 저장된 data와 같은지 확인은 안한다.
HERROR CXNVM_DelayedWriteUserText(CxUserText_t* pstText, HBOOL bAsync)
{
	HUINT8			ucSlotIdx		= 0;
	HERROR			hErr			= ERR_FAIL;
	HUINT32			ulOffset		= 0;
	CxUserText_t* 	pstWriteBuf		= NULL;

	ENTER_FUNCTION;
	if (pstText == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;
	ulOffset = ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;

	if (bAsync == TRUE)
	{
		pstWriteBuf = (CxUserText_t *)CX_MEM_Alloc(sizeof(CxUserText_t));
		if (pstWriteBuf == NULL)
		{
			LEAVE_FUNCTION;
			return hErr;
		}

		CX_memcpy(pstWriteBuf, pstText, sizeof(CxUserText_t));

		hErr = CXNVM_AsyncWrite(eCxNvmFile_UserText, ulOffset, sizeof(CxUserText_t), (HUINT8 *)pstWriteBuf);
	}
	else
	{
		hErr = CXNVM_Write(eCxNvmFile_UserText, ulOffset, sizeof(CxUserText_t), (HUINT8 *)pstText);
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Error("CXNVM_Write() Failed.. err(%d) \n", hErr);
	}
	LEAVE_FUNCTION;

	return hErr;
}

HERROR CXNVM_DelayedWriteUserTextForRemove(CxUserText_t* pstText)
{
	return CXNVM_DelayedWriteUserText(pstText, TRUE);
}

HERROR CXNVM_RemoveUserText(HUINT8 ucSeqNo)
{
	HUINT8			ucSlotIdx;
#if defined(CX_NVM_CACHE)
	HUINT32			ulOffset;
#endif
	CxUserText_t	stTmpUserText;

	ENTER_FUNCTION;

	ucSlotIdx = ucSeqNo;
	CX_memset(&stTmpUserText, 0, sizeof(CxUserText_t));
	stTmpUserText.ucSeqNo = ucSlotIdx;

#if defined(CX_NVM_CACHE)
	ulOffset = CX_NVM_USERTEXT_OFFSET + ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;
	CX_memcpy(pucCxNvmCache + ulOffset, &stTmpUserText, sizeof(CxUserText_t));
#endif

	CXNVM_DelayedWriteUserTextForRemove(&stTmpUserText);

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXNVM_CompareUserTextWithoutStartTime(CxUserText_t* pstText)
{
	HUINT8	ucSlotIdx;

	if (pstText == NULL)
	{
		HxLOG_Print("There is no Text Data.\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// seq번호를 보고 slot을 찾는다. slot idx = seqNo % (max_slot_idx + 1)
	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;

	return local_cxnvm_CompareUserTextWithoutStartTime(ucSlotIdx, pstText);
}

HERROR CXNVM_IsAlreadyDisplayed(CxUserText_t* pstText)
{
	HUINT8	ucSlotIdx;

	ENTER_FUNCTION;
	if (pstText == NULL)
	{
		HxLOG_Print("There is no Text Data.\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	// seq번호를 보고 slot을 찾는다. slot idx = seqNo % (max_slot_idx + 1)
	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;
	LEAVE_FUNCTION;
	return local_cxnvm_IsAlreadyDisplayed(ucSlotIdx, pstText);
}
#endif

#define Fingerprint_________________________________________________

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
static HERROR local_cas_cx_CheckFPCurrentTime(void)
{
	// 가끔, current time이 Invalid 한 경우와 맞물리면서 Error case 유발시킨다. 그래서, CheckPastTime 돌림.

	TxSOURCE_e eSource = TxSOURCE_SET_BY_UNKNOWN;

	TAPI_GetTimeSource(&eSource);

	if (eSource != TxSOURCE_SET_BY_SI)
	{
		HxLOG_Error(" Time source is not by SI (%d)!!\n", eSource);
		return ERR_FAIL;
	}

	return ERR_OK;
}
static HERROR local_cas_cx_CheckFPPastTime(HUINT8* ulFPtime)
{
	// 과거시간이 할당되어 버리면, Timer 가동안되서 player에서 loop 돌고 지정 시간이 되어도
	// 해당 Fingerprint는 display되지 않는다...
	// 계속 Ignore.....

	HUINT32			ulError;
	HUINT32			ulCurrentTime		= 0;
	HxDATETIME_t	stDateTime;
	HxDATETIME_t	stFPDateTime;
	HUINT32			ulFPUnixTime		= 0;
	UNIXTIME		ulCurrentUnixTime	= 0;

	// Get Current Time
	ulError = VK_CLOCK_GetTime((HULONG *)&ulCurrentTime);
	if (ulError != VK_OK)
	{
		return	ERR_FAIL;
	}

	CX_memset(&stFPDateTime, 0, sizeof(HxDATETIME_t));
	CX_memset(&stDateTime, 0, sizeof(HxDATETIME_t));

	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulCurrentTime, &stDateTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrentTime, &stFPDateTime);		// FP 는 시간 정보만 들어오니, 년월일은 Current에서 받는다.

	stFPDateTime.stTime.ucHour = ulFPtime[0];
	stFPDateTime.stTime.ucMinute = ulFPtime[1];
	stFPDateTime.stTime.ucSecond = ulFPtime[2];
	stFPDateTime.stTime.usMillisecond = (HUINT16)ulFPtime[3];

	HxLOG_Print("[FP] Check FP Date(%d/%d/%d), Time(%2d:%2d:%2d:%2d)\n",
					stFPDateTime.stDate.usYear,
					stFPDateTime.stDate.ucMonth,
					stFPDateTime.stDate.ucDay,
					stFPDateTime.stTime.ucHour,
					stFPDateTime.stTime.ucMinute,
					stFPDateTime.stTime.ucSecond,
					stFPDateTime.stTime.usMillisecond);
	HxLOG_Print("[FP] Check Cur Date(%d/%d/%d), Time(%2d:%2d:%2d:%2d)\n",
					stDateTime.stDate.usYear,
					stDateTime.stDate.ucMonth,
					stDateTime.stDate.ucDay,
					stDateTime.stTime.ucHour,
					stDateTime.stTime.ucMinute,
					stDateTime.stTime.ucSecond,
					stDateTime.stTime.usMillisecond);

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulCurrentUnixTime);
	//HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset(&stFPDateTime, &ulFPUnixTime);
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stFPDateTime, &ulFPUnixTime);

	if (ulFPUnixTime < ulCurrentUnixTime)
	{
		HxLOG_Print(" [Time] Current(%d), ulFPUnixTime(%d)!!\n", ulCurrentUnixTime, ulFPUnixTime);
		return	ERR_FAIL;
	}

	return	ERR_OK;
}

// ERR_OK 면 같은것
// ERR_FAIL이면 다른것
#if 0 //not used..
static HERROR local_cxnvm_CompareFingerprintDisplayed(CxFingerprint_t *pstFingerprint)
{
	ENTER_FUNCTION;

	CXNVM_ReadFingerprint(&s_stFingerprintBuf);

	// 아직 Display 되지 않은 것에 대해 우선순위를 따진다.
	// (s_stFingerprintBuf.ucDisplayed == FALSE)
	// Display 된 후에는 우선순위 의미 없음
	if(s_stFingerprintBuf.ucDisplayed == TRUE)
	{
		// 계속 진행
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if(pstFingerprint->bFPFromProtectedData == TRUE)
	{
		if(s_stFingerprintBuf.bFPFromProtectedData == FALSE)
		{
			//높은 것이 하위 우선순위의 FP를 대체한다.
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
	else
	{
		if(s_stFingerprintBuf.bFPFromProtectedData == TRUE)
		{
			//낮은 것이 상위 우선순위의 FP를 대체하지 못한다.
			LEAVE_FUNCTION;
			return ERR_OK;
		}
	}
	LEAVE_FUNCTION;
	return ERR_FAIL;
}
#endif

HERROR CXNVM_FingerprintDisplayed(void)
{
	HERROR	nErr;
#if defined(CX_NVM_CACHE)
	HUINT32	ulOffset;
#endif

	nErr = CXNVM_ReadFingerprint(&s_stFingerprintBuf);
	if (nErr == ERR_OK && s_stFingerprintBuf.ucDisplayed == FALSE)
	{
		s_stFingerprintBuf.ucDisplayed = TRUE;

		// Write 루틴 분리 -> Write 타이밍이 겹치는 문제로...
		//nErr = CXNVM_WriteFingerprint(&s_stFingerprintBuf);
#if defined(CX_NVM_CACHE)
		ulOffset = CX_NVM_FINGERPRINT_OFFSET;
		CX_memcpy(pucCxNvmCache + ulOffset, &s_stFingerprintBuf, sizeof(CxFingerprint_t));
#endif
		CXNVM_DelayedWriteFingerprint(&s_stFingerprintBuf);
	}

	return ERR_OK;
}

HERROR CXNVM_ReadFingerprint(CxFingerprint_t* pstFingerprint)
{
	HERROR	nErr;

	ENTER_FUNCTION;

	if (pstFingerprint == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

#if defined(CX_NVM_CACHE)
	CX_memcpy(pstFingerprint, pucCxNvmCache + CX_NVM_FINGERPRINT_OFFSET, sizeof(CxFingerprint_t));
	nErr = ERR_OK;
#else
	nErr = CXNVM_Read(eCxNvmFile_Fingerprint, 0, sizeof(CxFingerprint_t), (HUINT8 *)pstFingerprint);
#endif

	LEAVE_FUNCTION;
	return nErr;
}

// ERR_FAIL이 return되면 화면에 표시하지 않는다.
// 동작 방식은 2가지
// 1. 기본
//	: task로 msg 던지고 (text는 copy), task에서 다시 delayedWrite를 호출한다.
// 2. CX_NVM_CACHE
//	: task로 msg를 던지고 (text는 cache에 기록된 것 사용), task에서 다시 delayedWrite를 호출한다.
HERROR CXNVM_WriteFingerprint(CxFingerprint_t* pstFingerprint)
{
#if defined(CX_NVM_CACHE)
	HUINT32	ulOffset;
#endif
	//	HERROR			nDisplayed = 0;
	HERROR	hErr			= 0;

	ENTER_FUNCTION;

	if (pstFingerprint == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (pstFingerprint->ulTime != CX_FINGER_PRINT_IMMEDIATELY_TIME)
	{
		// 미래 시간 case
		hErr = local_cas_cx_CheckFPCurrentTime();
		if (hErr != ERR_OK)
		{
			HxLOG_Print("The Current Time isn't set by SI....\r\n");
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}

		hErr = local_cas_cx_CheckFPPastTime(pstFingerprint->ucCentTime);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("The FP Time is past time....\r\n");
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}

	HxLOG_Print("[FP] Time(0x%x / %d)\n", pstFingerprint->ulTime, pstFingerprint->ulTime);

#if defined(CX_NVM_CACHE)
	ulOffset = CX_NVM_FINGERPRINT_OFFSET;
	CX_memcpy(pucCxNvmCache + ulOffset, pstFingerprint, sizeof(CxFingerprint_t));
#endif
	CXNVM_DelayedWriteFingerprint(pstFingerprint);

	HxLOG_Print("####### Write OK #######\n");
	LEAVE_FUNCTION;
	return ERR_OK;
}

//좀 더 제대로 하려면 main task와 별개의 task에서 write해야 된다.
// 기존 저장된 data와 같은지 확인은 안한다.
HERROR CXNVM_DelayedWriteFingerprint(CxFingerprint_t* pstFingerprint)
{
	HERROR			hErr			= ERR_FAIL;
	CxFingerprint_t* 						pstWriteBuf		= NULL;

	ENTER_FUNCTION;

	if (pstFingerprint == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pstWriteBuf = (CxFingerprint_t *)CX_MEM_Alloc(sizeof(CxFingerprint_t));
	if (pstWriteBuf == NULL)
	{
		LEAVE_FUNCTION;
		return hErr;
	}

	CX_memcpy(pstWriteBuf, pstFingerprint, sizeof(CxFingerprint_t));

	hErr = CXNVM_AsyncWrite(eCxNvmFile_Fingerprint, 0, sizeof(CxFingerprint_t), (HUINT8 *)pstWriteBuf);
	LEAVE_FUNCTION;

	return hErr;
}
#endif

#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR CXNVM_WriteKeyStore(void* pvData, HUINT32 ulOffset, HUINT32 ulLength)
{
	HERROR	err;

	ENTER_FUNCTION;

#if defined(CX_NVM_CACHE)
	CX_memcpy(pucCxNvmCache + CX_NVM_KEYSTORE_OFFSET + ulOffset, pvData, ulLength);
#endif
	err = CXNVM_Write(eCxNvmFile_KeyStore, ulOffset, ulLength, (HUINT8 *)pvData);

	LEAVE_FUNCTION;

	return err;
}

HERROR CXNVM_ReadKeyStore(void* pvData, HUINT32 ulOffset, HUINT32 ulLength)
{
	HERROR	err;

	ENTER_FUNCTION;


#if defined(CX_NVM_CACHE)
	CX_memcpy(pvData, pucCxNvmCache + CX_NVM_KEYSTORE_OFFSET + ulOffset, ulLength);
	err = ERR_OK;
#else
	err = CXNVM_Read(eCxNvmFile_KeyStore, ulOffset, ulLength, (HUINT8 *)pvData);
#endif

	LEAVE_FUNCTION;

	return err;
}
#endif

#if defined(CONFIG_MW_MM_PVR)
// 한번만 Read Write 하기에 cache 작업이 필요 없을 듯...
HERROR CXNVM_WritePvrKeyWithHash(HUINT8* pucKey, HUINT32 ulLength)
{
	HERROR	err;

	ENTER_FUNCTION;

	err = CXNVM_Write(eCxNvmFile_PvrKey, 0, ulLength, pucKey);

	LEAVE_FUNCTION;

	return err;
}

HERROR CXNVM_ReadPvrKeyWithHash(HUINT8* pucKey, HUINT32 ulLength)
{
	HERROR	err				= ERR_OK;

	ENTER_FUNCTION;

	err = CXNVM_Read(eCxNvmFile_PvrKey, 0, ulLength, pucKey);

	LEAVE_FUNCTION;

	return err;
}
#endif


#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION) || defined(CONFIG_MW_CAS_CONAX_GERMANY_MR)
// 한번만 Read Write 하기에 cache 작업이 필요 없을 듯...
HERROR CXNVM_WriteCAPINCode(HUINT8* pucKey, HUINT32 ulLength)
{
	HERROR	err;

	ENTER_FUNCTION;

	err = CXNVM_Write(eCxNvmFile_CaPin, 0, ulLength, pucKey);

	LEAVE_FUNCTION;

	return err;
}

HERROR CXNVM_ReadCAPINCode(HUINT8* pucKey, HUINT32 ulLength)
{
	HERROR	err				= ERR_OK;

	ENTER_FUNCTION;

	err = CXNVM_Read(eCxNvmFile_CaPin, 0, ulLength, pucKey);

	LEAVE_FUNCTION;

	return err;
}
#endif

void CXNVM_ClearConaxUserData(void)
{
	// TODO: 항목 추가시 아래 계속 추가하세요...
	HUINT32				ulSize;
	CxFingerprint_t* 	pstFingerprint;
	CxUserText_t* 		pstUserText;

	ulSize = sizeof(CxFingerprint_t);
	pstFingerprint = (CxFingerprint_t *)CX_MEM_Alloc(ulSize);
	if (pstFingerprint != NULL)
	{
		CX_memset(pstFingerprint, 0, ulSize);
		CXNVM_Write(eCxNvmFile_Fingerprint, 0, ulSize, (HUINT8 *)pstFingerprint);
		CX_MEM_Free(pstFingerprint);
	}

	ulSize = sizeof(CxUserText_t) * CX_NVM_USERTEXT_SLOT_NUM;
	pstUserText = (CxUserText_t *)CX_MEM_Alloc(ulSize);
	if (pstUserText != NULL)
	{
		CX_memset(pstUserText, 0, ulSize);
		CXNVM_Write(eCxNvmFile_UserText, 0, ulSize, (HUINT8 *)pstUserText);
		CX_MEM_Free(pstUserText);
	}
}

void CXAPI_NVM_ClearConaxUserData(void)
{
	return CXNVM_ClearConaxUserData();
}
/*********************** End of File ******************************/

