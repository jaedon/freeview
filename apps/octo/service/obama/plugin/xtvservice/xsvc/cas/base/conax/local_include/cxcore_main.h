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

// 사양 관리.
#define CX_MAX_NUM_OF_ES					(9*4)		// 9 ES * 4 (live + TSR + REC1/2), see the CX_DSCR_SLOT_NUM
#define CX_MAX_NUM_OF_ECM					(9*3)		// live = TSR 일것같음..ㅎㅎ
#define CX_MAX_NUM_OF_EMM					2
#define CX_MAX_NUM_OF_SESSION				(NUM_VIEW_ACTION + NUM_MAX_REC_ACTION)		// FCC가 적용되면서 해당 정의도 무너진다. // 5 session * 1 sc : 정확하게는 smartcard에서 session number를 준다....

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
  *	tsr session 처리 수정
  *
  *	기본 가정 - MR svc에서 PIN 입력하고 live 시청 중 다른 svc로 zapping하지 않으면 pin 다시 안 묻는다
  *			즉, Normal/TSR/Chase PB 을 해도 pin을 다시 안 묻는다
  *
  *	1. MR svc간 이동시 Live에서 CA PIN 입력하지 않아도 TSR buffer 쌓이는 문제 수정 (기존 tsr session 재사용되면서 발생하고, session reset 해줌으로 수정함)
  *	2. MR svc에서 PB이전과 이후에 할당되는 tsr session이 다를경우 mr override 되지 않아 tsr buffer 안 쌓이는 문제 수정
  *	   (208에서 PIN 입력후 Instant rec시작 -> 210에서 PIN 입력 (tsr session idx 2) 	-> 208 녹화 중단 -> PB 시작 및 종료
  *		=> tsr session이 idx 1로 할당 -> 신규 session이 tsr에 할당됨으로 인해 MR override 안 된상태가 됨 -> tsr buffer 안 쌓임)
  *	3. 현재 시청중이 아닌 다른 녹화중인 svc의 chase pb시엔 pin 묻도록 수정
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
	Handle_t	hHandle;	// hSession 혹은 hEcm이 전달된다.
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
