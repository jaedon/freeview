/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: nexus_record_module.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 1/18/08 2:36p $
 *
 * Module Description:
 *  PVR Record
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/record/src/nexus_record_module.h $
 * 
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/2   11/9/07 6:03p vsilyaev
 * PR 36788: Added timeshifting
 * 
 * Nexus_Devel/1   11/7/07 5:07p vsilyaev
 * PR 36788: PVR record module
 * 
 **************************************************************************/
#ifndef NEXUS_RECORD_MODULE_H__
#define NEXUS_RECORD_MODULE_H__

#include "nexus_base.h"
#include "nexus_record_thunks.h"
#include "nexus_record.h"
#include "nexus_record_init.h"

#ifdef __cplusplus
extern "C" {
#endif

extern NEXUS_ModuleHandle NEXUS_RecordModule;

#define NEXUS_MODULE_SELF NEXUS_RecordModule

typedef struct NEXUS_Record_P_ModuleState {
    NEXUS_RecordModuleSettings cfg;
} NEXUS_Record_P_ModuleState;

extern NEXUS_Record_P_ModuleState g_NEXUS_Record_P_ModuleState;


#ifdef __cplusplus
}
#endif

#endif /* NEXUS_RECORD_MODULE_H__ */

