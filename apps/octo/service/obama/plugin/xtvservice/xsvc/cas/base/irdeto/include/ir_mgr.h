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

#ifndef	__IRMGR_MAIN_H__
#define	__IRMGR_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_cas_mgr_main.h>
#include "octo_common.h"

#include "ir_svc.h"
#include "ir_msg_scell.h"



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* CAS Device¿« Context. */

typedef struct {
	unsigned short		usSlotId;

	unsigned short		usNumCaSysId;
	unsigned short		ausCaSysId[MAX_SC_SECTOR_COUNT];

	Handle_t			hCasDevice; //it's cas instance ID.

} IR_ScInst_t;

typedef struct {
	Handle_t			hAction;
	Handle_t			hSvc;
	SvcCas_ActionType_e	eActType;

	IR_ScInst_t 		*pScInst;

	HUINT32				unDmxId;
	HUINT16				usUniqueId;
	HUINT16				usSvcId;

	IR_SERVICE_INFO	IrSvcInfo;

} IR_Context_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HUINT32 CAS_IR_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable);
Handle_t CAS_IR_MGR_GetCasInstance(void);
SvcCas_Context_t * CAS_IR_MGR_GetContextByAction(Handle_t hAction);
void		IRMGR_UpdateNit(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucNit);
void		IRMGR_UpdateBat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucBat);
#endif /* !__IRMGR_MAIN_H__ */

