/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_docsis.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/5/06 6:29p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7118/A0/docsis/bhsm_docsis.h $
 * 
 * Hydra_Software_Devel/1   9/5/06 6:29p dbayani
 * PR 24090: Add for docsis support.
 * 
 *
 ***************************************************************************/

#ifndef BHSM_DOCSIS_H__
#define BHSM_DOCSIS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Module Specific Functions */

typedef struct BHSM_DocsisIO {
	/* input */
	uint32_t	unW1Mask;
	uint32_t	unW1BaseIn;
	uint32_t	unW1BaseOut;
	uint32_t	unW2Mask;
	uint32_t	unW2BaseIn;
	uint32_t	unW2BaseOut;
	
	/* Output */
	uint32_t					unStatus;

	
} BHSM_DocsisIO_t;

BERR_Code BHSM_SetDocsisMipsBoot (
		BHSM_Handle				in_handle,
		BHSM_DocsisIO_t	*inoutp_DocsisIO
);




/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_DOCSIS_H__ */







