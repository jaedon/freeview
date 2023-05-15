/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_status.inc $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/17/12 10:23a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_status.inc $
 * 
 * Hydra_Software_Devel/2   10/17/12 10:23a nilesh
 * SW7445-54: Fixed missing newline compiler warning
 * 
 * Hydra_Software_Devel/1   7/18/12 4:53p nilesh
 * SW7425-3156: Updated BVCE_Channel_ClearStatus behavior to be more
 * intuitive
 *
 ***************************************************************************/

BVCE_STATUS_FIELD(uiTotalErrors)
BVCE_STATUS_FIELD(uiTotalPicturesReceived)
BVCE_STATUS_FIELD(uiTotalPicturesDroppedDueToFrameRateConversion)
BVCE_STATUS_FIELD(uiTotalPicturesDroppedDueToErrors)
BVCE_STATUS_FIELD(uiTotalPicturesEncoded)
BVCE_STATUS_FIELD(uiLastPictureIdEncoded)
#undef BVCE_STATUS_FIELD
