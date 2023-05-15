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

	Description: nvm ���� ��� \n
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

// nvm cache�� ����Ϸ��� �Ʒ� define ���
// init�� �̸� read�ϰ�, ���ķδ� cache������ read, write�� write-through (cache���� ���� ���� �޸𸮿��� ����)
//#define CX_NVM_CACHE

#define CX_NVM_USERTEXT_OFFSET					0
/*
 * pn_dsjeong :
 * CX_NVM_USERTEXT_SLOT_SIZE ������ �� ��, �׻� 4bytes align�� ���� CxUserText_t ����ü bytes �� ����� ����ؾ� ��!!!
 * ���� �� CX_NVM_USERTEXT_SLOT_SIZE�� CxUserText_t ���� bytes �� ���� 2 bytes �۰� define �Ǹ鼭,
 * ������ �ϴ� ���� ���� ���� index msg ���� �� �κб��� overwrite �Ǹ鼭 ���� index msg�� ���� ������ ���̽��� �־���.
 *
 *  (2014.08.07 ���� CxUserText_t ����ü ��ü bytes ��� ����)
 *	szText : 256 + 1
 * 	ucDisplayed : 1
 * 	usDuration : 2 => ������� 4bytes align�� ���� 256 + 4 bytes
 * 	ucSeqNo : 1
 * 	ucRepeat : 1
 * 	usInterval : 2 => ������� 4bytes align�� ���� 4 bytes
 * 	HxDATETIME_t : (DATE_T)2 + 1 + 1 + (TIME_T)1 + 1 + 1 + 2 => ������� 4bytes align�� ���� 12 bytes(3bytes �� ���� �߰� ��).
 * 	UNIXTIME : 4 => 4bytes align�� ���� �״�� 4 bytes
 * 	���� 4bytes align�� ���� ��ü ����� ����ϸ� 280 bytes.
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
#define	CX_NVM_KEYSTORE_SLOT_SIZE			17408//1280 -> PT �䱸���� (�ּ� 16k) => key number(256) * sizeof(CxCpmKey_t)���� ũ��...
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

// ERR_OK �� ������
// ERR_FAIL�̸� �ٸ���
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

// ERR_OK �� ������
// ERR_FAIL�̸� �ٸ���
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

// ERR_OK �� have been displayed.
// ERR_FAIL�̸� never have been displayed.
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

// UT�� display �ߴٰ� check �ϴ� �Լ�.
HERROR CXNVM_UserTextDisplayed(HUINT8 ucSeqNo, CxUTDisplayCount_t ulOpt)
{
	HERROR	nErr;
	HUINT8	ucSlotIdx;

#if defined(CX_NVM_CACHE)
	HUINT32	ulOffset;
#endif

	ENTER_FUNCTION;

	ucSlotIdx = ucSeqNo;

	VK_TASK_Sleep(200); 	// Smart card���� �ް� write �Ǳ� ���� read�ϴ� ��찡 �����Ͽ� STBY ���� UserText data�� ���� �ɷ� �Ǿ� �߸� ��µǴ� ������ ���� ����. CT R.7
	nErr = CXNVM_ReadUserText(ucSlotIdx, &s_stUserTextBuf);
	if (nErr == ERR_OK)
	{
		s_stUserTextBuf.ucDisplayed++;

		if (ulOpt == eCxUTDisplayCntMax)
		{
			// Display �ð��� ���� ���
			s_stUserTextBuf.ucDisplayed = s_stUserTextBuf.ucRepeat;
		}

#if defined(CX_NVM_CACHE)
		ulOffset = CX_NVM_USERTEXT_OFFSET + ucSlotIdx * CX_NVM_USERTEXT_SLOT_SIZE;
		CX_memcpy(&s_stUserTextBuf, pucCxNvmCache + ulOffset, sizeof(CxUserText_t));
#endif

		/**
		  *	�ٸ� task���� �����ϰ� �ϸ�, ui ó������ �ʰ� ����Ǵ� ��찡 �����
		  *	(ex) mail msg�� �о����� mail icon�� ���� ����
		  *	�Ͽ� msg�� ó������ �ʰ� ���� write�ϵ��� ����
		 **/
		//CXCORE_SendMsgToSubTask(eCxMsg_Nvm_WriteUserText, HANDLE_NULL, (HUINT32)&s_stUserTextBuf, (HUINT32)0); // �̴�� ����ϸ� s_stUserTextBuf�� free�Ϸ��ϹǷ� �ȵ�..
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
		  *	�ٸ� task���� �����ϰ� �ϸ�, ui ó������ �ʰ� ����Ǵ� ��찡 �����
		  *	�Ͽ� msg�� ó������ �ʰ� ���� write�ϵ��� ����
		 **/

		CXNVM_DelayedWriteUserText(&s_stUserTextBuf, FALSE);
	}

	LEAVE_FUNCTION;
	return nErr;
}

// ERR_FAIL�� return�Ǹ� ȭ�鿡 ǥ������ �ʴ´�.
// ���� ����� 2����
// 1. �⺻
//	: task�� msg ������ (text�� copy), task���� �ٽ� delayedWrite�� ȣ���Ѵ�.
// 2. CX_NVM_CACHE
//	: task�� msg�� ������ (text�� cache�� ��ϵ� �� ���), task���� �ٽ� delayedWrite�� ȣ���Ѵ�.
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

	// seq��ȣ�� ���� slot�� ã�´�. slot idx = seqNo % (max_slot_idx + 1)
	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;
	pstText->ucSeqNo = ucSlotIdx;

#if defined(CONFIG_DEBUG)
	local_cas_cx_DebugPrintCurrentTime(pstText);
#endif

	// ���� ���� �ִ��� Ȯ���Ѵ�.
	if (local_cxnvm_CompareUserText(ucSlotIdx, pstText) == ERR_OK)
	{
		// ���� ���� ������ �������� �ʰ� fail return�Ѵ�. -> ȭ�鿡 ǥ������ �ʴ´�.
		HxLOG_Error("Same MSG is already stored\r\n");
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if (bStartTimeOmit == TRUE)
	{
		CXUI_SetRecvUTTimeStamp();
	}

	// CXHD-5100C 2011-08 1st Conax PT. USER_TEXT ���� �ð��� ��쿡�� ���� ��� ǥ�ø� �ؾ��Ѵٰ� ��. by Teppo in Labwise
	// �Ʒ� �ڵ忡�� �ϴ� ���� �����̰ų� SI�� ���� �ð��� ������ ���� ��� block �ϴ� �ǵ� �ǹ̰� ���ٰ� �����Ǿ� ����. (�Ʒ� mailbox message�� ���� �ּ��� �ڵ尡 ���� ��ġ���� ����.)
	// SI ���� �ð� �޴� ���� CXUI_UserTextReceived()���� ��.  File�� ������ USER_TEXT�� ���⼭ �Ÿ��� �ʰ� ������.
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
			// do nothing. ������ �ʰ�, Display�� �ѱ�.
		}
	}
	// Mailbox Message
	else if(bStartTimeOmit != FALSE)
	{
		// Mail Message && �̷� �ð� case
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

//�� �� ����� �Ϸ��� main task�� ������ task���� write�ؾ� �ȴ�.
// ���� ����� data�� ������ Ȯ���� ���Ѵ�.
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

	// seq��ȣ�� ���� slot�� ã�´�. slot idx = seqNo % (max_slot_idx + 1)
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

	// seq��ȣ�� ���� slot�� ã�´�. slot idx = seqNo % (max_slot_idx + 1)
	ucSlotIdx = pstText->ucSeqNo % CX_NVM_USERTEXT_SLOT_NUM;
	LEAVE_FUNCTION;
	return local_cxnvm_IsAlreadyDisplayed(ucSlotIdx, pstText);
}
#endif

#define Fingerprint_________________________________________________

#if defined(CONFIG_MW_CAS_CONAX_FINGERPRINT)
static HERROR local_cas_cx_CheckFPCurrentTime(void)
{
	// ����, current time�� Invalid �� ���� �¹����鼭 Error case ���߽�Ų��. �׷���, CheckPastTime ����.

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
	// ���Žð��� �Ҵ�Ǿ� ������, Timer �����ȵǼ� player���� loop ���� ���� �ð��� �Ǿ
	// �ش� Fingerprint�� display���� �ʴ´�...
	// ��� Ignore.....

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
	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrentTime, &stFPDateTime);		// FP �� �ð� ������ ������, ������� Current���� �޴´�.

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

// ERR_OK �� ������
// ERR_FAIL�̸� �ٸ���
#if 0 //not used..
static HERROR local_cxnvm_CompareFingerprintDisplayed(CxFingerprint_t *pstFingerprint)
{
	ENTER_FUNCTION;

	CXNVM_ReadFingerprint(&s_stFingerprintBuf);

	// ���� Display ���� ���� �Ϳ� ���� �켱������ ������.
	// (s_stFingerprintBuf.ucDisplayed == FALSE)
	// Display �� �Ŀ��� �켱���� �ǹ� ����
	if(s_stFingerprintBuf.ucDisplayed == TRUE)
	{
		// ��� ����
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	if(pstFingerprint->bFPFromProtectedData == TRUE)
	{
		if(s_stFingerprintBuf.bFPFromProtectedData == FALSE)
		{
			//���� ���� ���� �켱������ FP�� ��ü�Ѵ�.
			LEAVE_FUNCTION;
			return ERR_FAIL;
		}
	}
	else
	{
		if(s_stFingerprintBuf.bFPFromProtectedData == TRUE)
		{
			//���� ���� ���� �켱������ FP�� ��ü���� ���Ѵ�.
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

		// Write ��ƾ �и� -> Write Ÿ�̹��� ��ġ�� ������...
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

// ERR_FAIL�� return�Ǹ� ȭ�鿡 ǥ������ �ʴ´�.
// ���� ����� 2����
// 1. �⺻
//	: task�� msg ������ (text�� copy), task���� �ٽ� delayedWrite�� ȣ���Ѵ�.
// 2. CX_NVM_CACHE
//	: task�� msg�� ������ (text�� cache�� ��ϵ� �� ���), task���� �ٽ� delayedWrite�� ȣ���Ѵ�.
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
		// �̷� �ð� case
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

//�� �� ����� �Ϸ��� main task�� ������ task���� write�ؾ� �ȴ�.
// ���� ����� data�� ������ Ȯ���� ���Ѵ�.
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
// �ѹ��� Read Write �ϱ⿡ cache �۾��� �ʿ� ���� ��...
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
// �ѹ��� Read Write �ϱ⿡ cache �۾��� �ʿ� ���� ��...
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
	// TODO: �׸� �߰��� �Ʒ� ��� �߰��ϼ���...
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

