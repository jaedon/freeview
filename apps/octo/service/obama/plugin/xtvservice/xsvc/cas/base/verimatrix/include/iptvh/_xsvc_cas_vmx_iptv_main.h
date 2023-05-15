/*
 * (c) 2011-2015 Humax Co., Ltd.
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
/********************************************************************
 * $Workfile:   _xsvc_cas_vmx_main.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef	__INT_XSVC_VMXIPTVHMAIN_H__
#define	__INT_XSVC_VMXIPTVHMAIN_H__
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "VMCallbackTypes.h"
#include "_xsvc_cas_vmx_psi.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/* �Ʒ� define�� Ǯ�� ������ �ش� log�� �� ��µȴ� */
#define VMX_DBG_MAIN					0
#define VMX_DBG_DMX						0
#define VMX_DBG_DSCR					0
#define VMX_DBG_PSI						0
#define VMX_DBG_PSI_ACTION				0
#define VMX_DBG_PSI_SESSION				0
#define VMX_DBG_PSI_STREAM				0
#define VMX_DBG_PSI_ECM					0
#define VMX_DBG_PSI_EMM					0
#define VMX_DBG_UI						0
#define VMX_DBG_VR_IPTVH_API			0
#define VMX_DBG_VR_IPTVH_MGR			0
#define VMX_DBG_VR_IPTVH_CC				0
#define VMX_DBG_VR_IPTVH_PSI_ECM		0
#define VMX_DBG_VR_IPTVH_PSI_EMM		0
#define VMX_DBG_VR_IPTVH_UI				0
#define VMX_DBG_VR_IPTVH_TESTCMD		0

#define GET_STBHANDLE(a,b)					((a & 0xFF000000) | (b & 0x00FFFFFF))
#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)

#define MAKE_BUF_VAL(val, shift, mask_num)	((val>>shift)&(mask_num==1?0x01:mask_num==2?0x03:mask_num==3?0x07:mask_num==4?0x0f:0x0)) // ���� �˰������� ��������.

// actionList�� ��� action id�� ������ �� �ֵ��� �̷���..  => stActionList[VMX_CAS_MAX_NUM_OF_ACTION];
#define VMX_CAS_MAX_NUM_OF_ACTION 		CAS_MAX_NUM_OF_ACTION

#if defined(CONFIG_PROD_YSR2000) || defined(CONFIG_PROD_YS1000)
/**
  *	vmx���� 10������ ���־��ٴµ� ��� Ȯ���ؾ� �ϳ�.. �������� Ȯ���� �� �ۿ� ����..
  * 	6�������� ����Ȯ���� (1 live + 2 fcc + 2 rec + 1 tsr)
**/
#define VMX_MAX_SESSION_NUM				10
#else
/**
  *	vmx ��翣 �⺻���� 3���� ��������
**/
#define VMX_MAX_SESSION_NUM				3
#endif

#define VMX_CAS_VALID_SVCIDX_START		(0x40)
#define VMX_CAS_VALID_SVCIDX_END		(VMX_CAS_VALID_SVCIDX_START + VMX_MAX_SESSION_NUM - 1)

/* *
  *	���ý� ��� actionId�� �����Ǵ� vmx context�� create�Ѵ� (actionId ���� = VMX_CAS_MAX_NUM_OF_ACTION)
  *	�̸� create���ѵ� ���� ������ ������ �ʴ� �Ͱ���
  *	������� �ʴ� actionId�� create context�� �ϸ鼭 �޸� �����ϴ� ���̴� ������� �ʴ´�
  *	�ʿ�� �׽�Ʈ �뵵�θ� ����� ��..
  *
  *	#define VMX_CREATE_ALL_CONTEXT_ON_BOOT	// booting�� ��� channel context�� vmclient lib�� ����Ѵ�.
**/


#define VMX_USE_MAIN_MGR_TASK			// main mgr task�� �����ϰ� ����Ѵ�

#if defined(CONFIG_OP_TDC)
/*
From Peter Haas, 20151120

Normally in TDC network we are using these NTP servers:
Name:    ntp03.eng.tdc.net
Address:  193.162.145.2
Name:    tix.ns.tele.dk
Address:  193.162.145.130
Name:    ns.tele.dk
Address:  193.162.159.194
*/

#define TDC_NTP_SERVER_ADDR			"ns.tele.dk"
//#define USE_NTP_COMMAND_TOOL		// tapi��� ntp client�� command�� ���� �� ���.  �׽�Ʈ �뵵 �ܿ��� ������� �� ��.
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eVmxMsg_ReqBootServerConnection,
	eVmxMsg_StartService,
	eVmxMsg_StopService,
	eVmxMsg_SetPid,

#if 0	// old case, should be removed
	eVmxIptv_Msg_CreateAllChannelContext,

	eVmxIptv_Msg_Ecm,
	eVmxIptv_Msg_Ecm_FilterRestart,

	eVmxIptv_Msg_Emm,
	eVmxIptv_Msg_Emm_Ready,

	eVmxIptv_Msg_StartService,
	eVmxIptv_Msg_StopService,

	eVmxIptv_Msg_SetOverrideRatingLevel,
#endif

	eVmxMsg_Msg_Max
} VmxIptv_MsgType_t;

typedef enum
{
	eVmxIptv_Decrypt_SW,
	eVmxIPtv_Decrypt_HW,
} VmxIptv_DecryptType_t;

typedef struct
{
	tDecodeFailureCallback 	pfnCbDecodeFail;
	tUserDataCallback		pfnCbUserData;
	tControlWordCallback		pfnCbCW;
	tFingerPrintingCallback	pfnCbFingerPrinting;
	tCopyControlCallback		pfnCbCopyControl;
	tOsdMessageCallback	pfCbOsdMsg;
	tIrdOpaqueDataCallback	pfCbOpaqueData;
} VmxIptv_CbFuncList_t;

typedef struct
{
	HUINT8 					ucVmxServiceIdx; 	/* live 4�� .. rec 4�� .. tsr 1�� */
	void 					*pContext;			/* live 4�� .. rec 4�� .. tsr 1�� */
	HBOOL					bServiceStarted;
	eEncryptionAlgorithm_t 	eEncrAlgo;
	VmxIptv_CbFuncList_t 	stCbList;
	VmxIptv_DecryptType_t	stDecryptType;
	HUINT8					ucChannelRatingLevel;
	HBOOL					bConnect;
	Handle_t				hAction;
	Handle_t 				hSvc;
	SvcCas_ActionType_e		eActType;
} Vmx_ActionTable_t;

typedef struct
{
	VmxIptv_MsgType_t 		eMsgType;
	Handle_t 		hHandle;	// hSession Ȥ�� hEcm�� ���޵ȴ�.
	HUINT32			ulParam;
	HUINT32			ulParam2;
} Vmx_Msg_t;

typedef struct
{
	VmxIptv_MsgType_t		eMsg;
	Handle_t 				hAction;
	Handle_t 				hSvc;
	SvcCas_ActionType_e 	eActType;
}Vmx_StartSessionMsg_t;

typedef struct
{
	VmxIptv_MsgType_t		eMsg;
	Handle_t 				hAction;
	Handle_t 				hNextSvc;
}Vmx_StopSessionMsg_t;

typedef struct
{
	VmxIptv_MsgType_t		eMsg;
	Handle_t 				hAction;
	SvcCas_SetPidInfo_t 	stCaDscInfo;
}Vmx_SetPidMsg_t;

typedef union
{
	VmxIptv_MsgType_t		eMsg;
	Vmx_StartSessionMsg_t	stStartSession;
	Vmx_StopSessionMsg_t	stStopSession;
	Vmx_SetPidMsg_t			stSetPid;
} Vmx_Message_t;

typedef struct
{
	Handle_t						hCasInst;
	HUINT8							ucCurrentRatingLevel;
	SvcCas_CbFuncList_t 			stCbList;
	Vmx_ActionTable_t				stActionList[VMX_CAS_MAX_NUM_OF_ACTION];
} Vmx_Instance_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
void xsvc_vmx_main_PrintDump(char flag, char *title, void *buf, unsigned int len);
void xsvc_vmx_main_PrintContext(void);
Vmx_Instance_t *xsvc_vmx_main_GetInstance(void);
Handle_t xsvc_vmx_main_GetActionHandleFromStbHdl(Handle_t hStbHandle);
Handle_t xsvc_vmx_main_GetActionHandleFromActionId(HUINT32 nActionId);
Vmx_ActionTable_t * xsvc_vmx_main_GetActionByActionHandle(Handle_t hAction);
Handle_t xsvc_vmx_main_GetLiveActionHandle(void);
Handle_t xsvc_vmx_main_GetStbHandleFormServiceIndex(HUINT8 usFilterId);
HUINT8 xsvc_vmx_main_GetServiceIdx(HUINT32 ulActionId);
HERROR xsvc_vmx_main_DefaultSetting(void);

#endif /* !__INT_XSVC_VMXIPTVHMAIN_H__ */
