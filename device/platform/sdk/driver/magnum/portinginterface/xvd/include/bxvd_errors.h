/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_errors.h $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 4/30/10 1:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_errors.h $
 * 
 * Hydra_Software_Devel/17   4/30/10 1:43p pblanco
 * SW7400-2753: Fix kernel oops when user data packet size exceeds maximum
 * buffer size.
 * 
 * Hydra_Software_Devel/16   6/16/08 1:20p pblanco
 * PR29915: Added better error handling and recovery to enqueueing and
 * read_isr routines.
 * 
 * Hydra_Software_Devel/15   5/21/08 1:00p pblanco
 * PR42910: Added invalid handle error code.
 * 
 * Hydra_Software_Devel/14   4/11/08 4:53p davidp
 * PR40234: Merge decoder debug logging from branch to
 * Hydra_Software_Devel/.
 * 
 * Hydra_Software_Devel/PR40234/1   4/10/08 3:35p davidp
 * PR40234: Add Decoder Debug log errors.
 * 
 * Hydra_Software_Devel/13   3/12/07 10:19a pblanco
 * PR26433: Added two new relocation engine errors to avoid compiler
 * errors when set to very picky.
 * 
 * Hydra_Software_Devel/12   1/17/07 12:09p pblanco
 * PR27168: Incorporated changes suggested in code review on 16-Jan-2007
 * 
 * Hydra_Software_Devel/11   12/11/06 3:15p pblanco
 * PR26433: Added new reloc engine errors and moved existing ones to their
 * own section.
 * 
 * Hydra_Software_Devel/10   9/22/06 9:31a pblanco
 * PR23959: Added new error code for EOC not found (relocation engine).
 * 
 * Hydra_Software_Devel/9   4/28/06 11:14a pblanco
 * PR19877: Added new error for already active channel.
 * 
 * Hydra_Software_Devel/8   4/27/06 4:15p pblanco
 * PR19877: Added two new error codes for FW busy and decoder inactive.
 * 
 * Hydra_Software_Devel/7   4/4/06 10:29a pblanco
 * PR19877: Added error definitions for new relocation code.
 * 
 * Hydra_Software_Devel/6   1/10/06 9:23a pblanco
 * PR16052: Added two new user data error code.
 * 
 * Hydra_Software_Devel/5   12/2/05 3:42p pblanco
 * PR16052: Added BXVD_ERR_BAD_DEVICE_STATE for new API implementation.
 * 
 * Hydra_Software_Devel/4   8/25/05 9:33a pblanco
 * PR16052: Added new error code for mmap failures.
 * 
 * Hydra_Software_Devel/3   7/26/05 10:15a pblanco
 * PR16052: Added invalid ack error.
 * 
 * Hydra_Software_Devel/2   7/25/05 4:00p pblanco
 * PR16052: Moved FW EOF error here. Was in bxvd_priv.c
 * 
 * Hydra_Software_Devel/1   7/6/05 11:01a pblanco
 * PR16052: Initial check in
 * 
 ***************************************************************************/
#ifndef BXVD_ERRORS_H__
#define BXVD_ERRORS_H__

#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes unique to BXVD.  Error has been reserved by berr module.
 * Each module has 0xfffff (64k) of errors. 
	*/

/* Video decoder subsystem related errors */
#define BXVD_ERR_QUEUE_CORRUPTED         BERR_MAKE_CODE(BERR_XVD_ID, 0x000)
#define BXVD_ERR_QUEUE_EMPTY             BERR_MAKE_CODE(BERR_XVD_ID, 0x001)
#define BXVD_ERR_QUEUE_FULL              BERR_MAKE_CODE(BERR_XVD_ID, 0x002)
#define BXVD_ERR_USERDATA_NONE           BERR_MAKE_CODE(BERR_XVD_ID, 0x003)
#define BXVD_ERR_USERDATA_INVALID        BERR_MAKE_CODE(BERR_XVD_ID, 0x004)
#define BXVD_ERR_USERDATA_DISABLED       BERR_MAKE_CODE(BERR_XVD_ID, 0x005)
#define BXVD_ERR_USERDATA_UNINITED       BERR_MAKE_CODE(BERR_XVD_ID, 0x006)
#define BXVD_ERR_USERDATA_USRBFROFL      BERR_MAKE_CODE(BERR_XVD_ID, 0x007)
#define BXVD_ERR_USERDATA_ITEM_TOO_LARGE BERR_MAKE_CODE(BERR_XVD_ID, 0x008)

/* FW image handling errors */
#define BXVD_ERR_EOF                 BERR_MAKE_CODE(BERR_XVD_ID, 0x008)

/* FW communication errors */
#define BXVD_ERR_FW_IS_BUSY          BERR_MAKE_CODE(BERR_XVD_ID, 0x009)

/* State machine errors */
#define BXVD_ERR_DECODER_INACTIVE    BERR_MAKE_CODE(BERR_XVD_ID, 0x00A)
#define BXVD_ERR_DECODER_ACTIVE      BERR_MAKE_CODE(BERR_XVD_ID, 0x00B)

/* Relocation engine errors */
#define BXVD_ERR_RELF_BAD_INPUT      BERR_MAKE_CODE(BERR_XVD_ID, 0x00C)
#define BXVD_ERR_RELF_BAD_HEADER     BERR_MAKE_CODE(BERR_XVD_ID, 0x00D)
#define BXVD_ERR_RELF_BAD_SECTION    BERR_MAKE_CODE(BERR_XVD_ID, 0x00E)
#define BXVD_ERR_RELF_BAD_RELOC_TYPE BERR_MAKE_CODE(BERR_XVD_ID, 0x00F)
#define BXVD_ERR_RELF_NO_EOC_FOUND   BERR_MAKE_CODE(BERR_XVD_ID, 0x010)
#define BXVD_ERR_MULT_SYM_TABLE_REFS BERR_MAKE_CODE(BERR_XVD_ID, 0x011)
#define BXVD_ERR_CANT_READ_SYMTAB    BERR_MAKE_CODE(BERR_XVD_ID, 0x012)

/* Decoder debug logging errors */
#define BXVD_ERR_DEBUG_LOG_NOBUFFER  BERR_MAKE_CODE(BERR_XVD_ID, 0x013)
#define BXVD_ERR_DEBUG_LOG_OVERFLOW  BERR_MAKE_CODE(BERR_XVD_ID, 0x014)

/* Invalid handle error code */
#define BXVD_ERR_INVALID_HANDLE      BERR_MAKE_CODE(BERR_XVD_ID, 0x015)

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXVD_ERRORS_H__ */
/* End of file. */
