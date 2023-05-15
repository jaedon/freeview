/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_dip.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/30/11 4:25p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_dip.h $
 * 
 * Hydra_Software_Devel/3   6/30/11 4:25p davidp
 * SW7425-826: Clear vsync interrupt but when registering vdc mask with
 * decoder.
 * 
 * Hydra_Software_Devel/2   6/27/11 4:18p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/7/11 5:51p davidp
 * SW7425-542: Add support for AVD FW Host Interface Memory API.
 * 
 * Hydra_Software_Devel/1   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 ***************************************************************************/

#ifndef BXVD_DIP_H_
#define BXVD_DIP_H_

#include "bxdm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BXVD_DisplayInterruptProvider_P_ChannelContext *BXVD_DisplayInterruptProvider_P_ChannelHandle;

typedef struct BXVD_DisplayInterruptProvider_P_ChannelSettings
{
      BXVD_Handle hXvd;
      BINT_Handle hInterrupt;
      BREG_Handle hRegister;
      BINT_Id interruptId;
      uint32_t uiInterruptClearRegister;
      uint32_t uiInterruptMaskRegister;
      BXVD_DisplayInterrupt eDisplayInterrupt;
#if BXVD_P_FW_HIM_API
      uint32_t  uiDisplayInfoOffset;
#else
      BXVD_P_DisplayInfo *pstDisplayInfo;
#endif
} BXVD_DisplayInterruptProvider_P_ChannelSettings;

typedef struct BXVD_DisplayInterruptProvider_P_RULIDMasks
{
  uint32_t ui32TopFieldRULIDMask;
  uint32_t ui32BottomFieldRULIDMask;
  uint32_t ui32ProgressiveFieldRULIDMask;
} BXVD_DisplayInterruptProvider_P_RULIDMasks;

typedef struct BXVD_DisplayInterruptProvider_P_InterruptSettings
{
      BXVD_DisplayInterruptProvider_P_RULIDMasks stRULIDMasks;
} BXVD_DisplayInterruptProvider_P_InterruptSettings;

BERR_Code BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings( BXVD_DisplayInterruptProvider_P_ChannelSettings *pstXvdDipChSettings );

BERR_Code BXVD_DisplayInterruptProvider_P_OpenChannel( BXVD_DisplayInterruptProvider_P_ChannelHandle *phXvdDipCh,
                                                       const BXVD_DisplayInterruptProvider_P_ChannelSettings *pstXvdDipChSettings );

BERR_Code BXVD_DisplayInterruptProvider_P_CloseChannel( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh );

BERR_Code BXVD_DisplayInterruptProvider_P_ProcessWatchdog( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh );

BERR_Code BXVD_DisplayInterruptProvider_P_GetDefaultInterruptSettings( BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings );

BERR_Code BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
                                                                     const BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings);

BERR_Code BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
                                                                     BXVD_DisplayInterruptProvider_P_InterruptSettings *pstXvdDipIntSettings);

BERR_Code BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
                                                                          BXDM_DisplayInterruptHandler_isr fCallback,
                                                                          void *pPrivateContext );

BERR_Code BXVD_DisplayInterruptProvider_GetDisplayInterruptInfo_isr( BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh,
                                                                     BXDM_DisplayInterruptInfo *pstXvdDisplayInterruptInfo );


#ifdef __cplusplus
}
#endif

#endif /* BXVD_DIP_H_ */
