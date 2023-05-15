/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_intr.h $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 7/13/10 4:25p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_intr.h $
 * 
 * Hydra_Software_Devel/14   7/13/10 4:25p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/13   5/14/08 12:31p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/12   8/21/07 7:15p nilesh
 * PR22327: Changed PicDataReady interrupt behavior to the following:
 *   - If no channels are open or no channels are decoding, XVD delivers
 * a single muted picture to the VDC callback that has the correct
 * polarity
 *   - Otherwise, a picture list will contain pictures of only channels
 * that are actively decoding
 * 
 * Hydra_Software_Devel/11   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   5/8/07 4:02p davidp
 * PR29915: Add support for multi-decode (channels)
 * 
 * Hydra_Software_Devel/10   3/28/07 1:26p pblanco
 * PR27168: Changed hardwired constants to defined constants.
 * 
 * Hydra_Software_Devel/8   7/17/06 1:56p pblanco
 * PR22673: Removed 7401a0 conditionalized code and restructured some of
 * the remaining conditional code.
 * 
 * Hydra_Software_Devel/7   6/29/06 6:55p davidp
 * PR20017: Enable 7401 B0 watchdog support
 * 
 * Hydra_Software_Devel/6   6/14/06 5:18p davidp
 * PR20017: Add 7401 B0 support for MBX, SeqHdr and DecodeStillPicture FW
 * generated interrupts.
 * 
 * Hydra_Software_Devel/5   5/22/06 5:58p davidp
 * PR21712: Use 7400 decoder specific watchdog timers.
 * 
 * Hydra_Software_Devel/4   4/24/06 2:11p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 * 
 * Hydra_Software_Devel/3   3/27/06 6:33p davidp
 * PR20353: Add Decode Still Picture support for 7400
 * 
 * Hydra_Software_Devel/2   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 * 
 * Hydra_Software_Devel/1   3/8/06 11:17a pblanco
 * PR20077: Initial checkin.
 * 
 * 
  ***************************************************************************/
/*=************************ Module Overview ********************************
<verbatim>

Overview:

This module contains the funtion prototypes for the XVD interrupt service
routines. These are now separate from both bxvd.c and bxvd_priv.c to facilitate
easier modification and maintainence.

</verbatim>
****************************************************************************/
#ifndef BXVD_INTR_H__
#define BXVD_INTR_H__

#include "bxvd.h"

/* This value clear the whole low order word except interrupts 1 thru 4 */
#define BXVD_INTR_INTGEN_CLEAR_VALUE 0x0000ffe1

#ifdef __cplusplus
extern "C" {
#endif

void BXVD_P_AVD_MBX_isr(void *pvXvd,
		    int iParam2);

void BXVD_P_AVD_PicDataRdy_isr(void *pvXvd,
			       int iParam2);

void BXVD_P_PictureDataReady_isr(BXVD_Handle hXvd,
				 BXVD_ChannelHandle hXvdCh,
				 BAVC_XVD_Picture *pPicItem);

void BXVD_P_PictureDataRdy_NoDecode_isr(BXVD_Handle hXvd,
					BXVD_DisplayInterrupt eDisplayInterrupt,
					BAVC_XVD_Picture *pPicItem);

void BXVD_P_AVD_StillPictureRdy_isr(void *pvXvd,
				    int iParam2);

void BXVD_P_WatchdogInterrupt_isr(void *pvXvd,
				  int param2);

void BXVD_P_VidInstrChkr_isr(void *pvXvd, 
                             int param2);

void BXVD_P_StereoSeqError_isr(void *pvXvd, 
                               int param2);

#ifdef __cplusplus
}
#endif

#endif /* BXVD_INTR_H__ */
