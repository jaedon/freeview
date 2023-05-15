/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_status_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/20/11 3:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_status_priv.h $
 * 
 * Hydra_Software_Devel/2   7/20/11 3:07p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/1   11/6/07 12:36p nilesh
 * PR36741: Added XVD status reporting via BDBG console messages
 *
 ***************************************************************************/

#ifndef BXVD_STATUS_PRIV_H__
#define BXVD_STATUS_PRIV_H__

#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

typedef struct BXVD_P_Status_Context
{
      BXVD_Handle hXvd;
      
      uint32_t auiOpenChannelCount[BXVD_MAX_VIDEO_CHANNELS];
      uint32_t auiCloseChannelCount[BXVD_MAX_VIDEO_CHANNELS];
      uint32_t auiStartDecodeCount[BXVD_MAX_VIDEO_CHANNELS];
      uint32_t auiStopDecodeCount[BXVD_MAX_VIDEO_CHANNELS];
      
      BXVD_ChannelStatus astChannelStatus[BXVD_MAX_VIDEO_CHANNELS];
} BXVD_P_Status_Context;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXVD_STATUS_PRIV_H__ */
/* End of File */
