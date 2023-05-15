/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_action.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

#ifndef __OAPI_CAS_ACTION_H__
#define __OAPI_CAS_ACTION_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define RPC_OAPI_CAS_ACTION_Init					"oapi.cas.action.Init"

#define RPC_OBAMA_CAS_ACTION_onStatusChanged		"obama.cas.action.onStatusChanged"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/* start of CAS */
#define	APKS_CAS_SUB_DATA_LENG		256

typedef enum
{
	eCasEvent_None,
	eCasEvent_Action,
	eCasEvent_MAX
} APKS_CAS_EventType_e;


typedef enum
{
	eCasGroupID_NONE		= 0x0000,
	eCasGroupID_MG			= 0x0100,
	eCasGroupID_VA			= 0x0500,
	eCasGroupID_IR			= 0x0600,
	eCasGroupID_CX			= 0x0B00,
	eCasGroupID_CR			= 0x0D00,
	eCasGroupID_NA			= 0x1800,
	eCasGroupID_ND			= 0x0900,
	eCasGroupID_SATSA		= 0x4400,
	eCasGroupID_COMMON	 	= 0xfffe,			// CAS+OP System Message ó����.
	eCasGroupID_CAM			= 0xffff
} APKS_CAS_GroupId_e;

typedef enum
{
	eCasActionType_None,
	// Tune ���� Action...
		// OBAMA -> Web APP
		eCasActionType_TuneRequest,		// CAS�� ���� Tune ������ �ʿ��� ��Ȳ �� ��, ����ڿ��� Tune �� ������ ����� ���� UI ����� ���� ���.
		eCasActionType_ForcedTune,		// CAS�� ���� Tune ������ �ﰢ������ �ʿ��� ��Ȳ �� ��, ����ڿ��� Tune ������ ���۵ȴٰ� Notify ���ִ� UI ����� ���� ���.
		eCasActionType_ReservedTune,	// CAS�� ���� Tune ������ ���� �Ǿ��� ��, �ش� ������ ����ڿ��� Notify ���ִ� UI ����� ���� ���.
		// Web APP -> OBAMA
		eCasActionType_ReplyToTuneReq,	// Tuning ��û�� ���Ͽ�, User�� �����̳� Ȥ�� ���� UI�� �ѷ��ְ� �׿� ���� ������ obama�ʿ� �����ֱ� ���Ͽ� ���.
	// Update ���� Action...
		// OBAMA -> Web APP
		eCasActionType_UpdateStart,		// CAS�� ��û���� CAS update ���� �� UI ó�� �ؾ� �� ��� ���
		eCasActionType_UpdateStop,		// CAS�� ��û���� CAS update ���� �� UI ó�� �ؾ� �� ��� ���

	eCasActionType_MAX

} APKS_CAS_ActionType_e;

typedef struct
{
	APKS_CAS_ActionType_e	eCasActionType;		// action type
	HUINT32					unSlotId;			// slot id
	APKS_CAS_GroupId_e		eCasGrpId;			// cas type
	HCHAR					szData[APKS_CAS_SUB_DATA_LENG];	// pointer alloc ���� �ص� ������, alloc �κа� free �κп� ���� �ڵ� ���󰡱Ⱑ ����� �� �����ϰ� ��.
} APKS_CAS_ActionInfo_t;
/* End of CAS */


typedef void (*OxCasAction_Notifier_t) (void);
typedef	void	(*CBOapiCasActionObjectHandler)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR 		OAPI_CAS_ACTION_Init(OxCasAction_Notifier_t fnApkNotifier);
HERROR 		OAPI_CAS_ACTION_GetStatus(APKS_CAS_ActionInfo_t *pstStatus);

#endif // __OAPI_CAS_ACTION_H__
