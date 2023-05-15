/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc_dbg.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 10/6/11 2:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/7425/brdc_dbg.h $
 * 
 * Hydra_Software_Devel/6   10/6/11 2:00p hongtaoz
 * SW7425-1425: added RDC per slot timestamp log;
 * 
 * Hydra_Software_Devel/5   3/24/09 5:29p albertl
 * PR52513: Moved RDC debug globals into hList structure.
 * 
 * Hydra_Software_Devel/4   3/13/09 11:45a rpan
 * PR52203: Add API for dynamically enable/disable RUL capture.
 * 
 * Hydra_Software_Devel/3   2/28/08 5:02p jessem
 * PR 38623: Added RDMA blockout support.
 * 
 * Hydra_Software_Devel/2   6/13/05 3:14p pntruong
 * PR15862: Added rdc rul logging for debug.
 * 
 * Hydra_Software_Devel/1   2/22/05 5:22p jasonh
 * PR 13937: Adding DBG functions to RDC so that a list can be traversed
 * and displayed.
 *
 ***************************************************************************/
#ifndef BRDC_DBG_H__
#define BRDC_DBG_H__

#include "brdc.h"

/* #define BRDC_USE_CAPTURE_BUFFER */

#ifdef __cplusplus
extern "C" {
#endif

/* describes the possible entries in a register DMA list */
typedef enum 
{
	BRDC_DBG_ListEntry_eCommand,
	BRDC_DBG_ListEntry_eRegister,
	BRDC_DBG_ListEntry_eData,
	BRDC_DBG_ListEntry_eEnd
	
} BRDC_DBG_ListEntry;

/* debug functions */

BERR_Code BRDC_DBG_SetList(
	BRDC_List_Handle  hList
	);

BERR_Code BRDC_DBG_SetList_isr(
	BRDC_List_Handle  hList
	);	

BERR_Code BRDC_DBG_GetListEntry(
	BRDC_List_Handle     hList,
	BRDC_DBG_ListEntry  *peEntry,
	uint32_t             aulArgs[4]
	);

BERR_Code BRDC_DBG_GetListEntry_isr(
	BRDC_List_Handle     hList,
	BRDC_DBG_ListEntry	*peEntry,
	uint32_t			 aulArgs[4]
	);
	
BERR_Code BRDC_DBG_DumpList(
	BRDC_List_Handle  hList
	);

typedef struct BRDC_DBG_CaptureBuffer {
	uint8_t *mem;
	int size; /* size of mem in bytes */
	int readptr, writeptr; /* offsets into mem */
	
	/* stats */
	int num_ruls;
	int total_bytes;

	bool enable; /* enable capture */
} BRDC_DBG_CaptureBuffer;

BERR_Code
BRDC_DBG_CreateCaptureBuffer(BRDC_DBG_CaptureBuffer *buffer, int size);
void
BRDC_DBG_DestroyCaptureBuffer(BRDC_DBG_CaptureBuffer *buffer);
void
BRDC_DBG_WriteCapture_isr(BRDC_DBG_CaptureBuffer *buffer, BRDC_Slot_Handle hSlot, BRDC_List_Handle hList);

/* prefixes */
#define BRDC_DBG_RUL            1
#define BRDC_DBG_BVN_ERROR      2
#define BRDC_DBG_RUL_TIMESTAMP  3

/* log errors from throughout the system */
void BRDC_DBG_LogErrorCode_isr(BRDC_Handle rdc, uint32_t prefix, const char *str);

/* called by application */
void BRDC_DBG_ReadCapture_isr(BRDC_Handle rdc, uint8_t *mem, int size, int *read);

void BRDC_DBG_EnableCapture_isr(BRDC_Handle rdc, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BRDC_DBG_H__ */


/* end of file */
