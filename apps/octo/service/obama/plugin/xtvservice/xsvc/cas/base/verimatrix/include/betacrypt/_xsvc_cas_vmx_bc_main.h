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
 * $Workfile:   _xsvc_cas_vmx_bc_main.h  $
 * Project: 	 Verimatrix CAS
 * Author:		ahn
 * Description:	API for Verimatrix
 *
 *                              Copyright (c) 2015 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#ifndef	__INT_VMXMAIN_H__
#define	__INT_VMXMAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_psi.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/* 아래 define을 풀면 무조건 해당 log가 다 출력된다 */
#define VMX_DBG_MAIN						0
#define VMX_DBG_DMX						0
#define VMX_DBG_DSCR						0
#define VMX_DBG_PSI						0
#define VMX_DBG_PSI_ACTION				0
#define VMX_DBG_PSI_SESSION				0
#define VMX_DBG_PSI_STREAM				0
#define VMX_DBG_PSI_ECM					0
#define VMX_DBG_PSI_EMM					0
#define VMX_DBG_SC						0
#define VMX_DBG_UI							0
#define VMX_DBG_VR_BC_API				0
#define VMX_DBG_VR_BC_INDIV				0
#define VMX_DBG_VR_BC_MGR				0
#define VMX_DBG_VR_BC_CC				0
#define VMX_DBG_VR_BC_PSI_ECM			0
#define VMX_DBG_VR_BC_PSI_EMM			0
#define VMX_DBG_VR_BC_SC					0
#define VMX_DBG_VR_BC_UI					0
#define VMX_DBG_VR_BC_USERCMD			0

// TODO_VMX
// di_dsc.c DI_DSC_SetPid()에서 아래처럼 수정해주어야함
// s_stDscSlotInfo[i].pid = pid;

// 아래는 define 정리해주자
#define VMX_TEST
#define VMX_SVC_TEST

#define GET_STBHANDLE(a,b)						((a & 0xFF000000) | (b & 0x00FFFFFF))
#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)
#define VMX_CAS_MAX_NUM_OF_ACTION 		(CAS_MAX_NUM_OF_ACTION - NUM_BG_ACTION)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32 param1;
	HUINT32 param2;	
	HUINT32 param3;	
	HUINT32 param4;	
} Vmx_Message_t;

typedef struct 
{	
	HUINT8 			ucVmxServiceIdx;			// 일단 serviceidx를 3개로 보자.
	HBOOL			bConnect;
	Handle_t			hAction;
	Handle_t 		hSvc;
	SvcCas_ActionType_e	eActType;
} Vmx_ActionTable_t;

typedef struct
{
	Handle_t 			hHandle;	
	HUINT32				ulParam;
	HUINT32				ulParam2;
} Vmx_Msg_t;

typedef struct 
{
	Handle_t						hCasInst;
	SvcCas_CbFuncList_t 		stCbList;
	Vmx_ActionTable_t			stActionList[VMX_CAS_MAX_NUM_OF_ACTION];
} Vmx_Instance_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
void xsvc_vmx_main_PrintDump(char flag, char *title, void *buf, unsigned int len);
Vmx_Instance_t *xsvc_vmx_main_GetInstance(void);
Handle_t xsvc_vmx_main_GetActionHandleFromStbHdl(Handle_t hStbHandle);
Handle_t xsvc_vmx_main_GetActionHandleFromActionId(HUINT32 nActionId);
Vmx_ActionTable_t * xsvc_vmx_main_GetActionByActionHandle(Handle_t hAction);
Handle_t xsvc_vmx_main_GetLiveActionHandle(void);
Handle_t xsvc_vmx_main_GetActionHandleFromServiceIndex(HUINT8 usFilterId); // FilterId == ServiceIndex
Handle_t xsvc_vmx_main_GetStbHandleFormServiceIndex(HUINT8 usFilterId);
HERROR xsvc_vmx_main_GetServiceIdx(Vmx_ActionTable_t *pAct);
#endif
#endif /* !__INT_VMXMAIN_H__ */
