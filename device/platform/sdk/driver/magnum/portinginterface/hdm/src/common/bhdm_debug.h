/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bhdm_debug.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 7/12/12 4:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm_debug.h $
* 
* Hydra_Software_Devel/1   7/12/12 4:02p rgreen
* SW7425-2989: Merge changes
* 
* Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:21p rgreen
* SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
* 
***************************************************************************/
#ifndef BHDM_DEBUG_H__
#define BHDM_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

void BHDM_DEBUG_P_Monitor_FormatChangesCheck_isr(BHDM_Handle hHDMI)  ;
void BHDM_DEBUG_P_Monitor_FormatChangesStart(BHDM_Handle hHDMI) ;

#ifdef __cplusplus
}
#endif



#endif /* BHDM_DEBUG_H__ */

