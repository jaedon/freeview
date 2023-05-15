/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/satsa_sci/satsa_sci.h-arc   1.1   Jun 12 2007 14:10:02   chlim  $ */
/******************************************************************************
 * $Workfile:   satsa_sci.h  $
 * $Modtime:   Jun 11 2007 16:50:30  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

#ifndef _SATSA_SCI_H_
#define _SATSA_SCI_H_

/********************************************************************
 Header Files
 ********************************************************************/

#include "htype.h"
#include "vkernel.h"

/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
// Messages
#define		MSG_SCI_FROM_UI			0x1000
#define		MSG_SCI_FROM_CAI		0x2000
#define		MSG_SCI_FROM_SCI		0x3000
#define		MSG_SCI_FROM_SC			0x4000

#define		SET_SECURE_STATUS		0x1001

#define		SCI_FROM_SCMGR_INSERT			0x4001
#define		SCI_FROM_SCMGR_EXTRACT			0x4002
#define		SCI_FROM_SCMGR_INVALID			0X4003
#define		SCI_FROM_SCMGR_MUTE				0x4004
#define		SCI_FROM_SCMGR_DENIED			0x4005

#define SATSA_SEM_Create(a,b)				VK_SEM_Create(a, b, VK_SUSPENDTYPE_PRIORITY)		// name,id --> id, name
#define SATSA_SEM_Destroy(a)				VK_SEM_Destroy(a)
#define SATSA_SEM_Get(a)					VK_SEM_Get(a)
#define SATSA_SEM_Release(a)				VK_SEM_Release(a)

#define SATSA_MSG_Create(a,b,c,d)			VK_MSG_Create(a,b,c,d,VK_SUSPENDTYPE_PRIORITY)
#define SATSA_MSG_Destroy(a)				VK_MSG_Destroy(a)
#define SATSA_MSG_Send(a,b,c)				VK_MSG_Send(a,b,c)
#define SATSA_MSG_Receive(a,b,c)			VK_MSG_Receive(a,b,c)
#define SATSA_MSG_ReceiveTimeout(a,b,c,d)	VK_MSG_ReceiveTimeout(a,b,c,d)

#define SATSA_MEM_Alloc(a)					OxCAS_Malloc(a)
#define SATSA_MEM_Free(a)					OxCAS_Free(a)

#define SATSA_TIMER_EventAfter(a,b,c,d,e)	VK_TIMER_EventAfter(a,b,c,d,e)

#define SATSA_memset(a,b,c)					HxSTD_memset(a,b,c)
#define SATSA_memcpy(a,b,c)				HxSTD_memcpy(a,b,c)
#define SATSA_memcmp(a,b,c)				memcmp(a,b,c)
#define SATSA_strcmp(a,b)					strcmp(a,b)

/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef struct tagSATSA_MSG{
	HUINT16	wMsg;
	HUINT16	nMsg;
	HUINT8 	lParam1;
	HUINT16	lParam2;
	HUINT32	lParam3;
	HUINT32	lParam4;
}	SATSA_MSG;


typedef	struct SATSA_SCI_INST_STRUCT
{
	HUINT8	ucSlotId;
	HUINT8	ucCasVersion;
	HUINT16	wCaSysId;

	HUINT8	bCasMenuLayer;
	HUINT8	bCardMenuLang;

	HUINT8	bIsConaxCard;
	HUINT8	bIsConaxCardInit;

	HUINT8	bWaitForPincode;
	HUINT8	bIgnorePincode;

}	SATSA_SCI_INST;

/********************************************************************
 Global Variable
 ********************************************************************/

/********************************************************************
 Global Function
 ********************************************************************/
HUINT32 Get_satsaTaskId();

HUINT32 Get_satsaQueueId();

HUINT32 Get_satsaSemId();


void local_Satsa_Sci_Init(void);

#endif /* _SATSA_SCI_H_ */

