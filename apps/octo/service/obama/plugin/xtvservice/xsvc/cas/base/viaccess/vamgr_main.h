/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__VAMGR_MAIN_H__
#define	__VAMGR_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "vkernel.h"
#include "va_def.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define VA_TASK_PRIORITY 						(VK_TASK_PRIORITY_MW_REF+3)
#define VA_TASK_STACK_SIZE						SIZE_32K

#define GET_STBHANDLE(a,b)						((a & 0xFF000000) | (b & 0x00FFFFFF))
#define GET_ACTION_ID_FROM_STBHANDLE(a)		((a >> 24) & 0xFF)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct {
	HBOOL			bConnect;	// CasMgr로부터 선택 받았느냐를 표시..
	HBOOL			bPmt;		// pmt를 받았는지 표시
	Handle_t			hAction;
	Handle_t 			hSvc;
	SvcCas_ActionType_e	eActType;
} VaActionTable_t;

typedef struct {
	Handle_t				hCasInst;
	SvcCas_CbFuncList_t 	stCbList;
	VaActionTable_t		stActionList[CAS_MAX_NUM_OF_ACTION];
} VaInstance_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
VaInstance_t *VAMGR_GetInstance();
Handle_t VAMGR_GetVaHandle(Handle_t hAction);
Handle_t VAMGR_GetActionHandle(Handle_t hStbHandle);
SvcCas_ActionType_e VAMGR_GetActType(Handle_t hVaHandle);
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
HBOOL VAMGR_IsLive(Handle_t hVaHandle);
HBOOL VAMGR_IsSameSvcWithLive(Handle_t hVaHandle);
#endif
HERROR VAMGR_GetTripleID(Handle_t hVaHandle, HUINT16 *pusOrgNetId, HUINT16 *pusTsId, HUINT16 *pusUniqueId, HUINT16 *pusServiceId);

void VAMGR_Task_Init();

#endif /* !__VAMGR_MAIN_H__ */
