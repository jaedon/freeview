/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_decoder_dbg.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 1/12/12 3:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_decoder_dbg.h $
 * 
 * Hydra_Software_Devel/5   1/12/12 3:23p btosi
 * SW7405-4736: added debug support for new PPB field flags_ext0
 * 
 * Hydra_Software_Devel/4   7/28/11 11:39a btosi
 * SW7405-4736: added the PPB flags to the BXVD_UP message
 * 
 * Hydra_Software_Devel/3   4/15/11 10:50a btosi
 * SW7405-4736: added support to print the SEI messages
 * 
 * Hydra_Software_Devel/2   4/4/11 2:21p btosi
 * SW7405-4736: added BDBG_DEBUG_BUILD
 * 
 * Hydra_Software_Devel/1   4/4/11 11:26a btosi
 * SW7405-4736: initial check in
 * 
 *
 ***************************************************************************/

#ifndef BXVD_DECODER_DBG_H__
#define BXVD_DECODER_DBG_H__

#include "bxvd.h"

#ifdef __cplusplus
extern "C" {
#endif

#if BDBG_DEBUG_BUILD

void BXVD_DecoderDbg_P_PrintUnifiedPicture(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiPPBFlags,
   uint32_t uiPPBFlagsExt0,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext,
   bool bDropped
   );

void BXVD_DecoderDbg_P_PrintSeiMessage(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_SEI_Message * pSEIMessage,
   uint32_t uiSerialNumber
   );

#else

#define BXVD_DecoderDbg_P_PrintUnifiedPicture( hXvdCh, uiPPBFlags, uiPPBFlagsExt0, pstUnifiedContext, bDropped )
#define BXVD_DecoderDbg_P_PrintSeiMessage( hXvdCh, pSEIMessage, uiSerialNumber )

#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXVD_DECODER_DBG_H__ */
