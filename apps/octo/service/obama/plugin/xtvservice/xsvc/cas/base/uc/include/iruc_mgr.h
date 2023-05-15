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
	@file	   \n
	@brief	   \n

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2008 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__IRUCMGR_MAIN_H__
#define	__IRUCMGR_MAIN_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_svc_cas_mgr_main.h>
#include "octo_common.h"

#include "ir_api.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
// TODO: Hereby, you must set configuration of Univerclient manager
#define UC_MGR_CONFIG_NO_CA_TASK


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

} IRUC_ScInst_t; //it's used only for dummy instance.

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
void IRUCMGR_Init(void);
HUINT32 local_cas_iruc_MGR_Notify_InstanceChanged(HUINT16 usSlotId, HUINT16 *pusCaSysId, HUINT8 ucNumCaSysId, HBOOL bAvailable);
SvcCas_Context_t * CAS_IRUC_MGR_GetContextByAction(Handle_t hAction);

#endif /* !__IRUCMGR_MAIN_H__ */


