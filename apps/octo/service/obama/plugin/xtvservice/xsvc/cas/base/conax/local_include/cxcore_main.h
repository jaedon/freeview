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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_main.h
@brief			cxcore_main.h (Conax)

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_MAIN_H__
#define	__CXCORE_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <conax.h>

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/

#define CX_CORE_VERSION_STR				"7.6.0.1" // Conax CAS7, STB integration kit 6.0, CxCore: 1st release

// ��� ����.
#define CX_MAX_NUM_OF_ES					(9*4)		// 9 ES * 4 (live + TSR + REC1/2), see the CX_DSCR_SLOT_NUM
#define CX_MAX_NUM_OF_ECM					(9*3)		// live = TSR �ϰͰ���..����
#define CX_MAX_NUM_OF_EMM					2
#define CX_MAX_NUM_OF_SESSION				(NUM_VIEW_ACTION + NUM_MAX_REC_ACTION)		// FCC�� ����Ǹ鼭 �ش� ���ǵ� ��������. // 5 session * 1 sc : ��Ȯ�ϰԴ� smartcard���� session number�� �ش�....

#define CX_NUM_OF_EMM_FILTER				10
#define CX_EMM_FILTER_LEN					7

#define CX_ECM_FILTER_LEN					8


#define CX_INVALID_SESSION_NUM				(-1)

// Macro Tool
#define VALID_PID(x)						(x > 0 && x < 0x2000)
#define INVALID_PID(x)						!VALID_PID(x)
#define GET_ACTION_VERSION(c) 				(c & 0x00FFFFFF)
#define GET_SERVICE_ID(x)					(x & 0xffff)
#define GET_UNIQUE_ID(x) 					((x & 0xffff0000) >> 16)
#define MWCAS_UNUSED(X)

/**
  *	tsr session ó�� ����
  *
  *	�⺻ ���� - MR svc���� PIN �Է��ϰ� live ��û �� �ٸ� svc�� zapping���� ������ pin �ٽ� �� ���´�
  *			��, Normal/TSR/Chase PB �� �ص� pin�� �ٽ� �� ���´�
  *
  *	1. MR svc�� �̵��� Live���� CA PIN �Է����� �ʾƵ� TSR buffer ���̴� ���� ���� (���� tsr session ����Ǹ鼭 �߻��ϰ�, session reset �������� ������)
  *	2. MR svc���� PB������ ���Ŀ� �Ҵ�Ǵ� tsr session�� �ٸ���� mr override ���� �ʾ� tsr buffer �� ���̴� ���� ����
  *	   (208���� PIN �Է��� Instant rec���� -> 210���� PIN �Է� (tsr session idx 2) 	-> 208 ��ȭ �ߴ� -> PB ���� �� ����
  *		=> tsr session�� idx 1�� �Ҵ� -> �ű� session�� tsr�� �Ҵ������ ���� MR override �� �Ȼ��°� �� -> tsr buffer �� ����)
  *	3. ���� ��û���� �ƴ� �ٸ� ��ȭ���� svc�� chase pb�ÿ� pin ������ ����
  *
**/
/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef enum
{
	eCxMsg_Sc_Reset,
	eCxMsg_Sc_RecoveryReset,
	eCxMsg_Sc_Extracted,
	eCxMsg_Sc_Ready,
	eCxMsg_Sc_CatReceived,
	eCxMsg_Ecm,
	eCxMsg_Ecm_FilterRestart,
	eCxMsg_Emm,
	eCxMsg_PullEmm_EmmConfig,
	eCxMsg_PullEmm_Emm,
	eCxMsg_ScheduleActionDone,
	eCxMsg_Ui_CheckUserTextAtBoot,
	eCxMsg_Ui_RetryUserText,
	eCxMsg_Ui_CheckFingerprintAtBoot,
	eCxMsg_Ui_RetryFingerprint,
	eCxMsg_Ui_CheckUserTextAtStandby,
	eCxMsg_Ui_CheckFingerprintAtStandby,
	eCxMsg_Svc_Start,
	eCxMsg_Svc_Stop,
	eCxMsg_Svc_SetPid,
	eCxMsg_Svc_UpdatePmt,
	eCxMsg_Svc_UpdateCat,
	eCxMsg_Svc_Select,
	eCxMsg_Svc_Move,
	eCxMsg_Svc_SetMainView,
	eCxMsg_UriTimeout,
	eCxMsg_Max
} CxMsgType_t;



typedef struct
{
	CxMsgType_t	eMsgType;
	Handle_t	hHandle;	// hSession Ȥ�� hEcm�� ���޵ȴ�.
	HUINT32		ulParam;
	HUINT32		ulParam2;
} CxMsg_t;

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXCORE_SendMsg(CxMsgType_t eMsgType, Handle_t hHandle, HUINT32 ulParam, HUINT32 ulParam2);

// API function (which can be invoked from outside of the conax core)
/* move to conax.h
HERROR CXAPI_StartTask(void);
HERROR CXAPI_StopTask(void);
HERROR CXAPI_Initialize(void);
*/
#endif /* !__CXCORE_MAIN_H__ */
