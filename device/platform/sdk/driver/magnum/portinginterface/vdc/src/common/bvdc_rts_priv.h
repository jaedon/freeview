/***************************************************************************
 *     Copyright (c) 2006-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_rts_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/3/07 7:01p $
 *
 * Module Description:

* Revision History:

 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_rts_priv.h $
 * 
 * Hydra_Software_Devel/2   5/3/07 7:01p pntruong
 * PR29662: Added c0 dynamics rts settings.
 *
 * Hydra_Software_Devel/1   1/25/07 10:05a yuxiaz
 * PR 23202: Added for RTS reconfig in VDC.
 *
 ***************************************************************************/
#ifndef BVDC_RTS_PRIV_H__
#define BVDC_RTS_PRIV_H__

#include "bavc.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * private functions
 ***************************************************************************/
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData );

#ifdef __cplusplus
}
#endif



#endif /* #ifndef BVDC_RTS_PRIV_H__ */

/* end of file */
