/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp.c $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 7/25/12 9:45a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp.c $
 * 
 * Hydra_Software_Devel/26   7/25/12 9:45a btosi
 * SW7425-3558: added BXDM_PictureProvider_GetCurrentPicturePtr_isr
 * 
 * Hydra_Software_Devel/25   7/2/12 9:53a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/24   6/13/12 11:21a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.
 * 
 * Hydra_Software_Devel/23   3/26/12 1:56p btosi
 * SW7425-2536: removed
 * BXDM_PictureProvider_GetDefaultDecoderInterface_isr
 * 
 * Hydra_Software_Devel/22   3/16/12 11:56a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/21   3/1/12 1:28p btosi
 * SW7405-4736: removed BERR_TRACE from
 * BXDM_PictureProvider_GetNextPTSInfo_isr
 * 
 * Hydra_Software_Devel/20   2/1/12 1:56p btosi
 * SW7425-2270: add support for IgnoreNRTUnderflow
 * 
 * Hydra_Software_Devel/19   1/19/12 11:12a btosi
 * SW7425-1264: BXDM_PictureProvider_SetClockOverride_isr, print the new
 * STC value in hex
 * 
 * Hydra_Software_Devel/18   10/3/11 2:19p btosi
 * SW7425-1264:  support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/17   5/10/11 1:40p btosi
 * SW7405-4736: added range checking and debug messages for the input
 * parameters
 * 
 * Hydra_Software_Devel/16   12/16/10 9:12a btosi
 * SW7422-72: fixed warnings
 * 
 * Hydra_Software_Devel/15   11/17/10 9:37a btosi
 * SW7422-72: added new BXVD and XDM 3D API's
 * 
 * Hydra_Software_Devel/14   10/21/10 6:20p davidp
 * SWBLURAY-23012: Add NULL handle test in
 * BXDM_PictureProvider_GetMonitorRefreshRate_isr.
 * 
 * Hydra_Software_Devel/13   10/1/10 2:50p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 * 
 * Hydra_Software_Devel/12   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/11   9/15/10 12:06p btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetFrameRateOverride_isr
 * and supporting logic
 * 
 * Hydra_Software_Devel/10   9/14/10 11:56a btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetTrickMode_isr to
 * primarily disable the FIC logic when receiving video with sparse
 * pictures
 * 
 * Hydra_Software_Devel/9   8/5/10 8:29a delkert
 * SW7405-4703: Add HorizontalOverscanMode to allow bypass of Overscan
 * calculation.  Added appropriate Get/Set APIs
 * 
 * Hydra_Software_Devel/8   8/3/10 2:03p btosi
 * SW7400-2870: added BXVD_[Set/Get]PlaybackRate(_isr)
 * 
 * Hydra_Software_Devel/7   5/11/10 1:33p btosi
 * SW7405-4117: added usage note
 * 
 * Hydra_Software_Devel/6   4/29/10 10:47a btosi
 * SW7405-4117: added support for ...DisplayFieldMode_eAuto
 * 
 * Hydra_Software_Devel/5   3/1/10 10:12a nilesh
 * SW7405-2993: Fixed compiler warnings for unused parameters
 * 
 * Hydra_Software_Devel/4   2/24/10 11:56a nilesh
 * SW7405-2993: Fixed BDBG_MSG printout
 * 
 * Hydra_Software_Devel/3   2/19/10 2:40p nilesh
 * SW7405-2993: Removed warning BERR_TRACE around return code for
 * BXDM_PictureProvider_GetCurrentPicture_isr
 * 
 * Hydra_Software_Devel/2   2/19/10 11:09a nilesh
 * SW7405-2993: Made the DIH handle a property of the picture provider so
 * that the PDR callback can persist over a stop/start decode
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "btmr.h"

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"

BDBG_MODULE(BXDM_PP);

BERR_Code
BXDM_PictureProvider_SetDecoderInterface_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Decoder_Interface *pstDecoderInterface,
         void *pPrivateContext
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetDecoderInterface_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstDecoderInterface );

   BDBG_MSG(("BXDM_PictureProvider_SetDecoderInterface_isr(%p, 0x%08x, 0x%08x)",
               hXdmPP,
               pstDecoderInterface,
               pPrivateContext
               ));

   hXdmPP->stDMConfig.stDecoderInterface = *pstDecoderInterface;
   hXdmPP->stDMConfig.pDecoderPrivateContext = pPrivateContext;

   BDBG_LEAVE(BXDM_PictureProvider_SetDecoderInterface_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDefaultChannelChangeSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultChannelChangeSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstChannelChangeSettings );

   BSTD_UNUSED( hXdmPP );

   BKNI_Memset(pstChannelChangeSettings, 0, sizeof ( BXDM_PictureProvider_ChannelChangeSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultChannelChangeSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetChannelChangeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetChannelChangeSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstChannelChangeSettings );

   BDBG_MSG(("BXDM_PictureProvider_SetChannelChangeSettings_isr(%p, hlp:%d, fpp: %d)",
                  hXdmPP,
                  pstChannelChangeSettings->bHoldLastPicture,
                  pstChannelChangeSettings->bFirstPicturePreview
                  ));

   hXdmPP->stDMConfig.stChannelChangeSettings = *pstChannelChangeSettings;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_CHANNEL_CHANGE_SETTINGS;

   BDBG_LEAVE(BXDM_PictureProvider_SetChannelChangeSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetChannelChangeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ChannelChangeSettings *pstChannelChangeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetChannelChangeSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstChannelChangeSettings );

   *pstChannelChangeSettings = hXdmPP->stDMConfig.stChannelChangeSettings;

   BDBG_LEAVE(BXDM_PictureProvider_GetChannelChangeSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetMuteMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetMuteMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetMuteMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bMute = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_MUTE;

   BDBG_LEAVE(BXDM_PictureProvider_SetMuteMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetMuteMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetMuteMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bMute;

   BDBG_LEAVE(BXDM_PictureProvider_GetMuteMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetDisplayFieldMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayFieldMode eDisplayFieldMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetDisplayFieldMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetDisplayFieldMode_isr(%p, %d)",
                  hXdmPP,
                  eDisplayFieldMode
                  ));

   if ( eDisplayFieldMode >= BXDM_PictureProvider_DisplayFieldMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetDisplayFieldMode_isr:: eDisplayFieldMode value of %d is out of range", eDisplayFieldMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eDisplayFieldMode = eDisplayFieldMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_DISPLAY_FIELD_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetDisplayFieldMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDisplayFieldMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayFieldMode *peDisplayFieldMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDisplayFieldMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peDisplayFieldMode );

   *peDisplayFieldMode = hXdmPP->stDMConfig.eDisplayFieldMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetDisplayFieldMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetSourceFormatOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetSourceFormatOverride_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetSourceFormatOverride_isr(%p, %d)",
                  hXdmPP,
                  eSourceFormatOverride
                  ));

   if ( eSourceFormatOverride >= BXDM_PictureProvider_SourceFormatOverride_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetSourceFormatOverride_isr:: eSourceFormatOverride value of %d is out of range", eSourceFormatOverride ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eSourceFormatOverride = eSourceFormatOverride;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_SRC_FORMAT_OVERRIDE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetSourceFormatOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetSourceFormatOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_SourceFormatOverride *peSourceFormatOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetSourceFormatOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peSourceFormatOverride );

   *peSourceFormatOverride = hXdmPP->stDMConfig.eSourceFormatOverride;

   BDBG_LEAVE(BXDM_PictureProvider_GetSourceFormatOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetScanModeOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ScanModeOverride eScanModeOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetScanModeOverride_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetScanModeOverride_isr(%p, %d)",
                  hXdmPP,
                  eScanModeOverride
                  ));

   if ( eScanModeOverride >= BXDM_PictureProvider_ScanModeOverride_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetScanModeOverride_isr:: eScanModeOverride value of %d is out of range", eScanModeOverride ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eScanModeOverride = eScanModeOverride;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_SCAN_MODE_OVERRIDE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetScanModeOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetScanModeOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ScanModeOverride *peScanModeOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetScanModeOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peScanModeOverride );

   *peScanModeOverride = hXdmPP->stDMConfig.eScanModeOverride;

   BDBG_LEAVE(BXDM_PictureProvider_GetScanModeOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetFreeze_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetFreeze_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetFreeze_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bFreeze = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_FREEZE;

   BDBG_LEAVE(BXDM_PictureProvider_SetFreeze_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetFreeze_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetFreeze_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bFreeze;

   BDBG_LEAVE(BXDM_PictureProvider_GetFreeze_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Set240iScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_240iScanMode e240iScanMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Set240iScanMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_Set240iScanMode_isr(%p, %d)",
                  hXdmPP,
                  e240iScanMode
                  ));
   if ( e240iScanMode >= BXDM_PictureProvider_240iScanMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_Set240iScanMode_isr:: e240iScanMode value of %d is out of range", e240iScanMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.e240iScanMode = e240iScanMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_240I_SCAN_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_Set240iScanMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Get240iScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_240iScanMode *pe240iScanMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Get240iScanMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pe240iScanMode );

   *pe240iScanMode = hXdmPP->stDMConfig.e240iScanMode;

   BDBG_LEAVE(BXDM_PictureProvider_Get240iScanMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Set480pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode ePulldownMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Set480pPulldownMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_Set480pPulldownMode_isr(%p, %d)",
                  hXdmPP,
                  ePulldownMode
                  ));

   if ( ePulldownMode >= BXDM_PictureProvider_PulldownMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_Set480pPulldownMode_isr:: ePulldownMode value of %d is out of range", ePulldownMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.e480pPulldownMode = ePulldownMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_480P_PULLDOWN_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_Set480pPulldownMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Get480pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode *pePulldownMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Get480pPulldownMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pePulldownMode );

   *pePulldownMode = hXdmPP->stDMConfig.e480pPulldownMode;

   BDBG_LEAVE(BXDM_PictureProvider_Get480pPulldownMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Set1080pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode ePulldownMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Set1080pPulldownMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_Set1080pPulldownMode_isr(%p, %d)",
                  hXdmPP,
                  ePulldownMode
                  ));

   if ( ePulldownMode >= BXDM_PictureProvider_PulldownMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_Set1080pPulldownMode_isr:: ePulldownMode value of %d is out of range", ePulldownMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.e1080pPulldownMode = ePulldownMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_1080P_PULLDOWN_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_Set1080pPulldownMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Get1080pPulldownMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PulldownMode *pePulldownMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Get1080pPulldownMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pePulldownMode );

   *pePulldownMode = hXdmPP->stDMConfig.e1080pPulldownMode;

   BDBG_LEAVE(BXDM_PictureProvider_Get1080pPulldownMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPTSOffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiOffset
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPTSOffset_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetPTSOffset_isr(%p, 0x%08x)",
                  hXdmPP,
                  uiOffset
                  ));

   hXdmPP->stDMConfig.uiPTSOffset = uiOffset;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PTS_OFFSET;

   BDBG_LEAVE(BXDM_PictureProvider_SetPTSOffset_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPTSOffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiOffset
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPTSOffset_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( puiOffset );

   *puiOffset = hXdmPP->stDMConfig.uiPTSOffset;

   BDBG_LEAVE(BXDM_PictureProvider_GetPTSOffset_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetDisplayMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayMode eDisplayMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetDisplayMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetDisplayMode_isr(%p, %d)",
                  hXdmPP,
                  eDisplayMode
                  ));

   if ( eDisplayMode >= BXDM_PictureProvider_DisplayMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetDisplayMode_isr:: eDisplayMode value of %d is out of range", eDisplayMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eDisplayMode = eDisplayMode;

      hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_DISPLAY_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetDisplayMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDisplayMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_DisplayMode *peDisplayMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDisplayMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peDisplayMode );

   *peDisplayMode = hXdmPP->stDMConfig.eDisplayMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetDisplayMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDefaultTSMThresholdSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultTSMThresholdSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstTSMThresholdSettings );

   BSTD_UNUSED( hXdmPP );

   BKNI_Memset(pstTSMThresholdSettings, 0, sizeof ( BXDM_PictureProvider_TSMThresholdSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultTSMThresholdSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetTSMThresholdSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetTSMThresholdSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstTSMThresholdSettings );

   BDBG_MSG(("BXDM_PictureProvider_SetTSMThresholdSettings_isr(%p, e:0x%08x, l:0x%08x, d:0x%08x)",
                  hXdmPP,
                  pstTSMThresholdSettings->uiTooEarlyThreshold,
                  pstTSMThresholdSettings->uiTooLateThreshold,
                  pstTSMThresholdSettings->uiDeltaStcPtsDiffThreshold
                  ));

   hXdmPP->stDMConfig.stTSMThresholdSettings = *pstTSMThresholdSettings;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_TSM_THRESHOLD;

   BDBG_LEAVE(BXDM_PictureProvider_SetTSMThresholdSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetTSMThresholdSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_TSMThresholdSettings *pstTSMThresholdSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetTSMThresholdSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstTSMThresholdSettings );

   *pstTSMThresholdSettings = hXdmPP->stDMConfig.stTSMThresholdSettings;

   BDBG_LEAVE(BXDM_PictureProvider_GetTSMThresholdSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetSTCValid_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bValid
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetSTCValid_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetSTCValid_isr(%p, %d)",
                  hXdmPP,
                  bValid
                  ));

   hXdmPP->stDMConfig.bSTCValid = bValid;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_STC_VALID;

   BDBG_LEAVE(BXDM_PictureProvider_SetSTCValid_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetSTCValid_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbValid
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetSTCValid_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbValid );

   *pbValid = hXdmPP->stDMConfig.bSTCValid;

   BDBG_LEAVE(BXDM_PictureProvider_GetSTCValid_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDefaultClipTimeSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ClipTimeSettings *pstClipTimeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultClipTimeSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstClipTimeSettings );

   BSTD_UNUSED( hXdmPP );

   BKNI_Memset(pstClipTimeSettings, 0, sizeof ( BXDM_PictureProvider_ClipTimeSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultClipTimeSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetClipTimeSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_ClipTimeSettings *pstClipTimeSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetClipTimeSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstClipTimeSettings );

   BDBG_MSG(("BXDM_PictureProvider_SetClipTimeSettings_isr(%p, v: %d, t:%d, 0x%08x-0x%08x, id:0x%08x)",
                  hXdmPP,
                  pstClipTimeSettings->bValid,
                  pstClipTimeSettings->eType,
                  pstClipTimeSettings->uiStart,
                  pstClipTimeSettings->uiStop,
                  pstClipTimeSettings->uiId
                  ));

   hXdmPP->stDMConfig.stClipTimeSettings = *pstClipTimeSettings;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_CLIP_TIMING;

   BDBG_LEAVE(BXDM_PictureProvider_SetClipTimeSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetSoftwarePCROffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiOffset
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetSoftwarePCROffset_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetSoftwarePCROffset_isr(%p, 0x%08x)",
                  hXdmPP,
                  uiOffset
                  ));

   hXdmPP->stDMConfig.uiSoftwarePCROffset = uiOffset;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_SOFTWARE_PCR_OFFSET;

   BDBG_LEAVE(BXDM_PictureProvider_SetSoftwarePCROffset_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetSoftwarePCROffset_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiOffset
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetSoftwarePCROffset_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( puiOffset );

   *puiOffset = hXdmPP->stDMConfig.uiSoftwarePCROffset;

   BDBG_LEAVE(BXDM_PictureProvider_GetSoftwarePCROffset_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetHardwarePCROffsetMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetHardwarePCROffsetMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetHardwarePCROffsetMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bUseHardwarePCROffset = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_USE_HW_PCR_OFFSET;

   BDBG_LEAVE(BXDM_PictureProvider_SetHardwarePCROffsetMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetHardwarePCROffsetMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetHardwarePCROffsetMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bUseHardwarePCROffset;

   BDBG_LEAVE(BXDM_PictureProvider_GetHardwarePCROffsetMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPlaybackRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture_Rate *pstPlaybackRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPlaybackRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPlaybackRate );

   BDBG_MSG(("BXDM_PictureProvider_SetPlaybackRate_isr(%p, %d/%d)",
                  hXdmPP,
                  pstPlaybackRate->uiNumerator,
                  pstPlaybackRate->uiDenominator
                  ));

   /* Prevent a divide by '0'.*/
   if ( 0 == pstPlaybackRate->uiDenominator )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetPlaybackRate_isr:: uiDenominator == 0!"));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.stPlaybackRate = *pstPlaybackRate;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PLAYBACK_RATE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetPlaybackRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPlaybackRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_Rate *pstPlaybackRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPlaybackRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPlaybackRate );

   *pstPlaybackRate = hXdmPP->stDMConfig.stPlaybackRate;

   BDBG_LEAVE(BXDM_PictureProvider_GetPlaybackRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPictureDropMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PictureDropMode ePictureDropMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPictureDropMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetPictureDropMode_isr(%p, %d)",
                  hXdmPP,
                  ePictureDropMode
                  ));
   if ( ePictureDropMode >= BXDM_PictureProvider_PictureDropMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetPictureDropMode_isr:: ePictureDropMode value of %d is out of range", ePictureDropMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.ePictureDropMode = ePictureDropMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PICTURE_DROP_MODE;
   }
   
   BDBG_LEAVE(BXDM_PictureProvider_SetPictureDropMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPictureDropMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PictureDropMode *pePictureDropMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPictureDropMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pePictureDropMode );

   *pePictureDropMode = hXdmPP->stDMConfig.ePictureDropMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetPictureDropMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetCounters_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_Counters *pstCounters
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCounters_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstCounters );

   *pstCounters = hXdmPP->stDMStatus.stCounters;

   BDBG_LEAVE(BXDM_PictureProvider_GetCounters_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetCurrentPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCurrentPTSInfo_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pPTSInfo );

   *pPTSInfo = hXdmPP->stDMStatus.stCurrentPTS;

   BDBG_LEAVE(BXDM_PictureProvider_GetCurrentPTSInfo_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetLastCodedPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetLastCodedPTSInfo_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pPTSInfo );

   *pPTSInfo = hXdmPP->stDMStatus.stCodedPTS;

   BDBG_LEAVE(BXDM_PictureProvider_GetLastCodedPTSInfo_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetNextPTSInfo_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PTSInfo *pPTSInfo
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetNextPTSInfo_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pPTSInfo );

   if ( true == hXdmPP->stDMStatus.bNextPTSInfoValid )
   {
      *pPTSInfo = hXdmPP->stDMStatus.stNextPTS;
      rc = BERR_SUCCESS;
   }
   else
   {
      rc = BERR_UNKNOWN;
   }

   BDBG_LEAVE(BXDM_PictureProvider_GetNextPTSInfo_isr);
   return rc;
}

BERR_Code
BXDM_PictureProvider_GetIPictureFoundStatus_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbFound
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetIPictureFoundStatus_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbFound );

   *pbFound = hXdmPP->stDMStatus.bIPictureFound;

   BDBG_LEAVE(BXDM_PictureProvider_GetIPictureFoundStatus_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPictureTag_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPictureTag_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( puiValue );

   *puiValue = hXdmPP->stDMStatus.uiPictureTag;

   BDBG_LEAVE(BXDM_PictureProvider_GetPictureTag_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetCurrentTimeCode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_GopTimeCode *pstTimeCode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCurrentTimeCode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstTimeCode );

   *pstTimeCode = hXdmPP->stDMStatus.stGOPTimeCode;

   BDBG_LEAVE(BXDM_PictureProvider_GetCurrentTimeCode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetCurrentPicture_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture *pPicture
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCurrentPicture_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pPicture );

   if ( true == hXdmPP->stDMStatus.bCurrentPPBParameterInfoValid )
   {
      *pPicture = hXdmPP->stDMStatus.stCurrentPPBParameterInfo;
      rc = BERR_SUCCESS;
   }
   else
   {
      rc = BERR_UNKNOWN;
   }

   BDBG_LEAVE(BXDM_PictureProvider_GetCurrentPicture_isr);
   return rc;
}

/* SW7425-3558: return a pointer to "stCurrentPPBParameterInfo",
 * the picture currently selected for display.
 * In contract to BXDM_PictureProvider_GetCurrentPicture_isr, using
 * this routine saves both a copy operation and the need for the 
 * calling routine to have a local BXDM_Picture structure on the stack.
 * It also provides a way to access the linked picture of a
 * pair of pictures (say for MVC).
 * If the data isn't valid, "pPicture" is set to NULL.
 */
BERR_Code
BXDM_PictureProvider_GetCurrentPicturePtr_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture **pPicture
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCurrentPicture_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pPicture );

   if ( true == hXdmPP->stDMStatus.bCurrentPPBParameterInfoValid )
   {
      *pPicture = &(hXdmPP->stDMStatus.stCurrentPPBParameterInfo);
   }
   else
   {
      *pPicture = NULL;
   }

   BDBG_LEAVE(BXDM_PictureProvider_GetCurrentPicture_isr);
   return rc;
}

BERR_Code
BXDM_PictureProvider_SetFrameAdvanceMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameAdvanceMode eFrameAdvanceMode
      )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetFrameAdvanceMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetFrameAdvanceMode_isr(%p, %d)",
                  hXdmPP,
                  eFrameAdvanceMode
                  ));
   if ( eFrameAdvanceMode >= BXDM_PictureProvider_FrameAdvanceMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetFrameAdvanceMode_isr:: eFrameAdvanceMode value of %d is out of range", eFrameAdvanceMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eFrameAdvanceMode = eFrameAdvanceMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_FRAME_ADVANCE_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetFrameAdvanceMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDefaultPreserveStateSettings(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PreserveStateSettings *pstPreserveStateSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultPreserveStateSettings);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPreserveStateSettings );

   BSTD_UNUSED( hXdmPP );

   BKNI_Memset(pstPreserveStateSettings, 0, sizeof ( BXDM_PictureProvider_PreserveStateSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultPreserveStateSettings);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPreserveStateSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_PictureProvider_PreserveStateSettings *pstPreserveStateSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPreserveStateSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPreserveStateSettings );

   BDBG_MSG(("BXDM_PictureProvider_SetPreserveStateSettings_isr(%p, d:%d, c:%d)",
                  hXdmPP,
                  pstPreserveStateSettings->bDisplay,
                  pstPreserveStateSettings->bCounters
                  ));

   hXdmPP->stDMConfig.stPreserveStateSettings = *pstPreserveStateSettings;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_PRESERVE_SETTINGS;

   BDBG_LEAVE(BXDM_PictureProvider_SetPreserveStateSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPreserveStateSettings_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_PreserveStateSettings *pstPreserveStateSettings
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPreserveStateSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPreserveStateSettings );

   *pstPreserveStateSettings = hXdmPP->stDMConfig.stPreserveStateSettings;

   BDBG_LEAVE(BXDM_PictureProvider_GetPreserveStateSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetReverseFieldsMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetReverseFieldsMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetReverseFieldsMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bReverseFields = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_REVERSE_FIELDS;

   BDBG_LEAVE(BXDM_PictureProvider_SetReverseFieldsMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetReverseFieldsMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetReverseFieldsMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bReverseFields;

   BDBG_LEAVE(BXDM_PictureProvider_GetReverseFieldsMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
      )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr(%p, %d",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bAutoValidateStcOnPause = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_AUTO_VALIDATE_ON_PAUSE;

   BDBG_LEAVE(BXDM_PictureProvider_SetAutoValidateStcOnPauseMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetAutoValidateStcOnPauseMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
      )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetAutoValidateStcOnPauseMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bAutoValidateStcOnPause;

   BDBG_LEAVE(BXDM_PictureProvider_GetAutoValidateStcOnPauseMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetProtocol_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_VideoCompressionStd eProtocol
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetProtocol_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetProtocol_isr(%p, %d)",
                  hXdmPP,
                  eProtocol
                  ));

   if ( eProtocol >= BAVC_VideoCompressionStd_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetProtocol_isr:: eProtocol value of %d is out of range", eProtocol ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eProtocol = eProtocol;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PROTOCOL;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetProtocol_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetProtocol_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_VideoCompressionStd *peProtocol
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetProtocol_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peProtocol );

   *peProtocol = hXdmPP->stDMConfig.eProtocol;

   BDBG_LEAVE(BXDM_PictureProvider_GetProtocol_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bJitterToleranceImprovement = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_JITTER_TOLERANCE;

   BDBG_LEAVE(BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetJitterToleranceImprovementMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetJitterToleranceImprovementMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bJitterToleranceImprovement;

   BDBG_LEAVE(BXDM_PictureProvider_GetJitterToleranceImprovementMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetMonitorRefreshRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_MonitorRefreshRate eMonitorRefreshRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetMonitorRefreshRate_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetMonitorRefreshRate_isr(%p, %d)",
                  hXdmPP,
                  eMonitorRefreshRate
                  ));

   /* Determine the monitor refresh rate */
   if ( eMonitorRefreshRate == BXDM_PictureProvider_MonitorRefreshRate_eUnknown 
         || eMonitorRefreshRate >= BXDM_PictureProvider_MonitorRefreshRate_eMax 
      )
   {
      BXVD_DBG_WRN(hXdmPP,("Monitor Refresh Rate Override: Unsupported(%d->%d)", eMonitorRefreshRate, BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz));
      hXdmPP->stDMConfig.eMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz;
   }
   else
   {
      hXdmPP->stDMConfig.eMonitorRefreshRate = eMonitorRefreshRate;
   }

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_MONITOR_REFRESH_RATE;

   BDBG_LEAVE(BXDM_PictureProvider_SetMonitorRefreshRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetMonitorRefreshRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_MonitorRefreshRate *peMonitorRefreshRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetMonitorRefreshRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peMonitorRefreshRate );

   /* Check for NULL handle */
   if (hXdmPP != NULL)
   {
      *peMonitorRefreshRate = hXdmPP->stDMConfig.eMonitorRefreshRate;
   }
   else
   {
      rc = BERR_INVALID_PARAMETER;
   } 

   BDBG_LEAVE(BXDM_PictureProvider_GetMonitorRefreshRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Set1080pScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_1080pScanMode eScanMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Set1080pScanMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_Set1080pScanMode_isr(%p, %d)",
                  hXdmPP,
                  eScanMode
                  ));

   if ( eScanMode >= BXDM_PictureProvider_1080pScanMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_Set1080pScanMode_isr:: eScanMode value of %d is out of range", eScanMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.e1080pScanMode = eScanMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_1080P_SCAN_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_Set1080pScanMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Get1080pScanMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_1080pScanMode *peScanMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Get1080pScanMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peScanMode );

   *peScanMode = hXdmPP->stDMConfig.e1080pScanMode;

   BDBG_LEAVE(BXDM_PictureProvider_Get1080pScanMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetSTCIndex_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiValue
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetSTCIndex_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetSTCIndex_isr(%p, 0x%08x)",
                  hXdmPP,
                  uiValue
                  ));

   hXdmPP->stDMConfig.uiSTCIndex = uiValue;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_STC_INDEX;

   BDBG_LEAVE(BXDM_PictureProvider_SetSTCIndex_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetSTCIndex_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetSTCIndex_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( puiValue );

   *puiValue = hXdmPP->stDMConfig.uiSTCIndex;

   BDBG_LEAVE(BXDM_PictureProvider_GetSTCIndex_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetCRCMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetCRCMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetCRCMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bCRCMode= bEnable;

   BDBG_LEAVE(BXDM_PictureProvider_SetCRCMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetCRCMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetCRCMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bCRCMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetCRCMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetRemovalDelay_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiValue
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetRemovalDelay_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetRemovalDelay_isr(%p, 0x%08x)",
                  hXdmPP,
                  uiValue
                  ));

   hXdmPP->stDMConfig.uiRemovalDelay = uiValue;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_REMOVAL_DELAY;
   
   BDBG_LEAVE(BXDM_PictureProvider_SetRemovalDelay_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetRemovalDelay_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t *puiValue
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetRemovalDelay_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( puiValue );

   *puiValue = hXdmPP->stDMConfig.uiRemovalDelay;

   BDBG_LEAVE(BXDM_PictureProvider_GetRemovalDelay_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPreRollRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture_Rate *pstPreRollRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPreRollRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPreRollRate );

   BDBG_MSG(("BXDM_PictureProvider_SetPreRollRate_isr(%p, %d/%d)",
                  hXdmPP,
                  pstPreRollRate->uiNumerator,
                  pstPreRollRate->uiDenominator
                  ));

   if ( 0 == pstPreRollRate->uiDenominator )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetPreRollRate_isr:: uiDenominator == 0!"));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.stPreRollRate = *pstPreRollRate;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PRE_ROLL_RATE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetPreRollRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPreRollRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_Rate *pstPreRollRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPreRollRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstPreRollRate );

   *pstPreRollRate = hXdmPP->stDMConfig.stPreRollRate;

   BDBG_LEAVE(BXDM_PictureProvider_GetPreRollRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetPlaybackMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetPlaybackMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetPlaybackMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bPlayback = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_PLAYBACK;
   
   BDBG_LEAVE(BXDM_PictureProvider_SetPlaybackMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetPlaybackMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetPlaybackMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bPlayback;

   BDBG_LEAVE(BXDM_PictureProvider_GetPlaybackMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetDefaultFrameRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_FrameRateCode eFrameRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetDefaultFrameRate_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetDefaultFrameRate_isr(%p, %d)",
                  hXdmPP,
                  eFrameRate
                  ));

   /* SW7425-3177: this code could be anywhere in XDM or XVD.  Forces a 
    * complie time warning when new BAVC_FrameRateCode enums are added.
    */
#ifdef BDBG_CWARNING
   BDBG_CWARNING( BXDM_PictureProvider_P_MAX_FRAMERATE == BAVC_FrameRateCode_eMax );
#endif

   if ( eFrameRate >= BXDM_PictureProvider_P_MAX_FRAMERATE )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetDefaultFrameRate_isr:: eFrameRate value of %d is out of range", eFrameRate ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eDefaultFrameRate = eFrameRate;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_DEFAULT_FRAME_RATE;
   }
   
   BDBG_LEAVE(BXDM_PictureProvider_SetDefaultFrameRate_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDefaultFrameRate_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BAVC_FrameRateCode *peFrameRate
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultFrameRate_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peFrameRate );

   *peFrameRate = hXdmPP->stDMConfig.eDefaultFrameRate;

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultFrameRate_isr);
   return BERR_TRACE( rc );
}
BERR_Code
BXDM_PictureProvider_SetFrameRateDetectionMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameRateDetectionMode eFrameRateDetectionMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetFrameRateDetectionMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetFrameRateDetectionMode_isr(%p, %d)",
                  hXdmPP,
                  eFrameRateDetectionMode
                  ));

   if ( eFrameRateDetectionMode >= BXDM_PictureProvider_FrameRateDetectionMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetFrameRateDetectionMode_isr:: eFrameRateDetectionMode value of %d is out of range", eFrameRateDetectionMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eFrameRateDetectionMode = eFrameRateDetectionMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_FRAME_RATE_DETECTION_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetFrameRateDetectionMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetFrameRateDetection_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_FrameRateDetectionMode *peFrameRateDetectionMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetFrameRateDetection_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peFrameRateDetectionMode );

   *peFrameRateDetectionMode = hXdmPP->stDMConfig.eFrameRateDetectionMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetFrameRateDetection_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetASTMMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetASTMMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetASTMMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bAstmMode = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_ATSM_MODE;
   
   BDBG_LEAVE(BXDM_PictureProvider_SetASTMMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetASTMMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetASTMMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bAstmMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetASTMMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bVirtualTSMOnPCRDiscontinuity = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_VTSM_ON_PCR_DISCON;
   
   BDBG_LEAVE(BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetVirtualTSMOnPCRDiscontinuityMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetVirtualTSMOnPCRDiscontinuityMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXdmPP->stDMConfig.bVirtualTSMOnPCRDiscontinuity;

   BDBG_LEAVE(BXDM_PictureProvider_GetVirtualTSMOnPCRDiscontinuityMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetErrorHandlingMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ErrorHandlingMode eErrorHandlingMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetErrorHandlingMode_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetErrorHandlingMode_isr(%p, %d)",
                  hXdmPP,
                  eErrorHandlingMode
                  ));

   if ( eErrorHandlingMode >= BXDM_PictureProvider_ErrorHandlingMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetErrorHandlingMode_isr:: eErrorHandlingMode value of %d is out of range", eErrorHandlingMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eErrorHandlingMode = eErrorHandlingMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_ERROR_HANDLING_MODE;
   }
   
   BDBG_LEAVE(BXDM_PictureProvider_SetErrorHandlingMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetErrorHandlingMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_ErrorHandlingMode *peErrorHandlingMode
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetErrorHandlingMode_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( peErrorHandlingMode );

   *peErrorHandlingMode = hXdmPP->stDMConfig.eErrorHandlingMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetErrorHandlingMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetTimerHandle_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BTMR_TimerHandle hTimer
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetTimerHandle_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetTimerHandle_isr(%p, h:%p)",
                  hXdmPP,
                  hTimer
                  ));

   hXdmPP->hTimer = hTimer;

   BDBG_LEAVE(BXDM_PictureProvider_SetTimerHandle_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetTimerHandle_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BTMR_TimerHandle *phTimer
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetTimerHandle_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(phTimer);

   *phTimer = hXdmPP->hTimer;

   BDBG_LEAVE(BXDM_PictureProvider_GetTimerHandle_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetChannelSyncMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetChannelSyncMode_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetChannelSyncMode_isr(%p, %d)",
                  hXdmPP,
                  bEnable
                  ));

   hXdmPP->stDMConfig.bChannelSyncMode = bEnable;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_CHANNEL_SYNC_MODE;   

   BDBG_LEAVE(BXDM_PictureProvider_SetChannelSyncMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetChannelSyncMode_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetChannelSyncMode_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(pbEnable);

   *pbEnable = hXdmPP->stDMConfig.bChannelSyncMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetChannelSyncMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_SetDIH(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_DisplayInterruptHandler_Handle hXdmDih
         )
{
   BDBG_ENTER( BXDM_PictureProvider_SetDIH );

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(hXdmDih);

   hXdmPP->stDMState.stChannel.hXdmDih = hXdmDih;
   
   BDBG_LEAVE( BXDM_PictureProvider_SetDIH );

   return BERR_TRACE( BERR_SUCCESS );
}


BERR_Code
BXDM_PictureProvider_GetDIH(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_DisplayInterruptHandler_Handle *phXdmDih
         )
{
   BDBG_ENTER( BXDM_PictureProvider_GetDIH );

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(phXdmDih);

   *phXdmDih = hXdmPP->stDMState.stChannel.hXdmDih;
   
   BDBG_LEAVE( BXDM_PictureProvider_GetDIH );

   return BERR_TRACE( BERR_SUCCESS );
}

/* SW7405-4117: uiMaxHeightSupportedByDeinterlacer is used in conjuction with 
 * BXDM_PictureProvider_DisplayFieldMode_eAuto to choose either eSingleField or eBothField 
 * based on the steam height during slow motion (and preroll).
 */
BERR_Code
BXDM_PictureProvider_SetDeinterlacerMaxHeight(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiMaxHeight
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetDeinterlacerMaxHeight);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetDeinterlacerMaxHeight(%p, %d)",
                  hXdmPP,
                  uiMaxHeight
                  ));

   hXdmPP->stDMConfig.uiMaxHeightSupportedByDeinterlacer = uiMaxHeight;

   hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_MAX_DEINT_HEIGHT;

   BDBG_LEAVE(BXDM_PictureProvider_SetDeinterlacerMaxHeight);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetDeinterlacerMaxHeight(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t * puiMaxHeight
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDeinterlacerMaxHeight);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(puiMaxHeight);

   *puiMaxHeight = hXdmPP->stDMConfig.uiMaxHeightSupportedByDeinterlacer;

   BDBG_LEAVE(BXDM_PictureProvider_GetDeinterlacerMaxHeight);
   return BERR_TRACE( rc );
}

/* SW7405-4703: API to allow bypass of BXDM_PPOUT_S_CalculateHorizontalOverscan */
BERR_Code BXDM_PictureProvider_SetHorizontalOverscanMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_HorizontalOverscanMode eHorizOverscanMode)
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetHorizontalOverscanMode_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetHorizontalOverscanMode_isr(%p, %d)",
                  hXdmPP,
                  eHorizOverscanMode
                  ));

   if ( eHorizOverscanMode >= BXDM_PictureProvider_HorizontalOverscanMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetHorizontalOverscanMode_isr:: eHorizOverscanMode value of %d is out of range", eHorizOverscanMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eHorizontalOverscanMode = eHorizOverscanMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_HORIZON_OVERSCAN_MODE;
   }
   
   BDBG_LEAVE(BXDM_PictureProvider_SetHorizontalOverscanMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetHorizontalOverscanMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_HorizontalOverscanMode *peHorizOverscanMode)
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetHorizontalOverscanMode_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(peHorizOverscanMode);

   *peHorizOverscanMode = hXdmPP->stDMConfig.eHorizontalOverscanMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetHorizontalOverscanMode_isr);
   return BERR_TRACE( rc );
}

/* SWDEPRECATED-1003: needed to turn off the FIC logic during certain trick modes 
 */
BERR_Code BXDM_PictureProvider_SetTrickMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_TrickMode eTrickMode
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetTrickMode_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetTrickMode_isr(%p, %d)",
                  hXdmPP,
                  eTrickMode
                  ));
   if ( eTrickMode >= BXDM_PictureProvider_TrickMode_eMax )
   {
      BDBG_WRN(("BXDM_PictureProvider_SetTrickMode_isr:: eTrickMode value of %d is out of range", eTrickMode ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.eTrickMode = eTrickMode;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_TRICK_MODE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetTrickMode_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetTrickMode_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_TrickMode *peTrickMode
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetTrickMode_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(peTrickMode);

   *peTrickMode = hXdmPP->stDMConfig.eTrickMode;

   BDBG_LEAVE(BXDM_PictureProvider_GetTrickMode_isr);
   return BERR_TRACE( rc );
}

/* SWDEPRECATED-1003: provide an API to override the frame rate during trick modes.
*/
BERR_Code
BXDM_PictureProvider_SetFrameRateOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         const BXDM_Picture_FrameRateOverride *pstFrameRateOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetFrameRateOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstFrameRateOverride );

   BDBG_MSG(("BXDM_PictureProvider_SetFrameRateOverride_isr(%p, valid:%d %d/%d)",
                  hXdmPP,
                  pstFrameRateOverride->bValid,
                  pstFrameRateOverride->stRate.uiNumerator,
                  pstFrameRateOverride->stRate.uiDenominator
                  ));

   /* Prevent a divide by '0'.*/
   if ( ( true == pstFrameRateOverride->bValid )
         && ( 0 == pstFrameRateOverride->stRate.uiDenominator ) )
   {
      hXdmPP->stDMConfig.stFrameRateOverride.bValid = false;
      BDBG_WRN(("BXDM_PictureProvider_SetFrameRateOverride_isr:: uiDenominator == 0!"));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.stFrameRateOverride = *pstFrameRateOverride;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_FRAMERATE_OVERRIDE;
   }

   BDBG_LEAVE(BXDM_PictureProvider_SetFrameRateOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetFrameRateOverride_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_Picture_FrameRateOverride *pstFrameRateOverride
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetFrameRateOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstFrameRateOverride );

   *pstFrameRateOverride = hXdmPP->stDMConfig.stFrameRateOverride;

   BDBG_LEAVE(BXDM_PictureProvider_GetFrameRateOverride_isr);
   return BERR_TRACE( rc );
}

/* SW7405-4736: Set the XDM instance ID. This is to help debug multi channel issues, 
 * i.e. PIP and mosaic mode.  Many of the BXVD_DBG_MSG statements will print this ID 
 * to help associate messages with a particular channel.  This can be set to any value, 
 * perhaps what makes the most sense is to set it equal to the VDC rectangle number.
 */
BERR_Code
BXDM_PictureProvider_SetInstanceID_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t uiInstanceID
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetInstanceID_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_SetInstanceID(%p, %d)",
                  hXdmPP,
                  uiInstanceID
                  ));

   hXdmPP->stDMConfig.uiInstanceID = uiInstanceID;

   BDBG_LEAVE(BXDM_PictureProvider_SetInstanceID_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetInstanceID_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         uint32_t * puiInstanceID
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetInstanceID_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(puiInstanceID);

   *puiInstanceID = hXdmPP->stDMConfig.uiInstanceID;

   BDBG_LEAVE(BXDM_PictureProvider_GetInstanceID_isr);
   return BERR_TRACE( rc );
}

/* SW7422-72: API's to allow the middleware/application to specify an
 * orientation for each picture.
 */

BERR_Code 
BXDM_PictureProvider_Set3D_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_3DSettings * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Set3D_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(pst3DSettings);

   if ( true == pst3DSettings->bOverrideOrientation
         && pst3DSettings->eOrientation >= BXDM_PictureProvider_Orientation_eMax
      )
   {
      BDBG_WRN(("BXDM_PictureProvider_Set3D_isr:: bOverrideOrientation is out of range %d", pst3DSettings->bOverrideOrientation ));
      rc = BERR_INVALID_PARAMETER;
   }
   else
   {
      hXdmPP->stDMConfig.st3DSettings = *pst3DSettings;

      hXdmPP->stDMConfig.uiDirtyBits_1 |= BXDM_PictureProvider_P_DIRTY_1_3D_SETTINGS;
   }

   BDBG_LEAVE(BXDM_PictureProvider_Set3D_isr);
   return BERR_TRACE( rc );
}

BERR_Code 
BXDM_PictureProvider_Get3D_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_3DSettings * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Get3D_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(pst3DSettings);

   *pst3DSettings = hXdmPP->stDMConfig.st3DSettings;

   BDBG_LEAVE(BXDM_PictureProvider_Get3D_isr);
   return BERR_TRACE( rc );
}

BERR_Code 
BXDM_PictureProvider_GetDefault3D(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_3DSettings * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefault3D);

   BSTD_UNUSED(hXdmPP);
   BDBG_ASSERT(pst3DSettings);

   BKNI_Memset( pst3DSettings, 0, sizeof( BXDM_PictureProvider_3DSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefault3D);
   return BERR_TRACE( rc );
}

/*
 * SW7425-1264: support for a synthesized STC; can create a clock that run backwards.
 */
BERR_Code
BXDM_PictureProvider_SetClockOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetClockOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstClockOverride );

   BDBG_MSG(("BXDM_PictureProvider_SetClockOverride_isr(%p) Enable:%d  initialValue:%08x  load:%d stcDelta:%d)",
                  hXdmPP,
                  pstClockOverride->bEnableClockOverride,
                  pstClockOverride->uiStcValue,
                  pstClockOverride->bLoadSwStc,
                  pstClockOverride->iStcDelta
                  ));

   hXdmPP->stDMConfig.stClockOverride = *pstClockOverride;

   /* Effectively a one-shot load.  Whenever called with this flag set,
    * SW STC will be reloaded.
    */
   hXdmPP->stDMConfig.bInitializeSwStc = pstClockOverride->bLoadSwStc;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_CLOCK_OVERRIDE;
 
   BDBG_LEAVE(BXDM_PictureProvider_SetClockOverride_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetClockOverride_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetClockOverride_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstClockOverride );

   *pstClockOverride = hXdmPP->stDMConfig.stClockOverride;

   BDBG_LEAVE(BXDM_PictureProvider_GetClockOverride_isr);
   return BERR_TRACE( rc );
}

/* SW7425-2270:
 * The application will call SetIgnoreNRTUnderflow when it determines that an NRT underflow 
 * is actually a gap in the content (e.g. slideshow or end of stream) and the repeated picture 
 * should actually be encoded.
 *
 * When SetIgnoreNRTUnderflow=true, the "decoder underflow" scenario should be ignored until either: 
 * - the underflow condition ends 
 * - the app explicitly sets SetIgnoreNRTUnderflow=false 
 *
 * Note: only the "decoder underflow" condition is ignored. All other NRT scenarios 
 * (e.g. "Other Transcode Stalled", "FIC Stall", etc) are still in effect. 
 */
BERR_Code
BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   bool bIgnoreNRTUnderflow
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr);

   BDBG_ASSERT( hXdmPP );

   BDBG_MSG(("BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr(%p) bIgnoreNRTUnderflow:%d",
                  hXdmPP,
                  bIgnoreNRTUnderflow
                  ));

   hXdmPP->stDMConfig.bIgnoreNRTUnderflow = bIgnoreNRTUnderflow;

   /* If "bIgnoreNRTUnderflow" has been disabled, blindly clear "bIgnoringUnderflow"
    * just in case the system was currently ignoring underflows.
    */
   if ( false == hXdmPP->stDMConfig.bIgnoreNRTUnderflow )
   {
      hXdmPP->stDMState.stDecode.stNonRealTime.bIgnoringUnderflow = false;
   }

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_IGNORE_NRT_UNDERFLOW;
 
   BDBG_LEAVE(BXDM_PictureProvider_SetIgnoreNRTUnderflow_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetIgnoreNRTUnderflow_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   bool * pbIgnoreNRTUnderflow
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetIgnoreNRTUnderflow_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pbIgnoreNRTUnderflow );

   *pbIgnoreNRTUnderflow = hXdmPP->stDMConfig.bIgnoreNRTUnderflow;

   BDBG_LEAVE(BXDM_PictureProvider_GetIgnoreNRTUnderflow_isr);
   return BERR_TRACE( rc );
}

/*
 * SW7425-3358: support for FNRT.
 */
BERR_Code
BXDM_PictureProvider_SetFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_SetFNRTSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstFNRTSettings );

   BDBG_MSG(("BXDM_PictureProvider_SetFNRTSettings_isr(%p) bEnable:%d  uiPreChargeCount:%08x",
                  hXdmPP,
                  pstFNRTSettings->bEnabled,
                  pstFNRTSettings->uiPreChargeCount
                  ));

   hXdmPP->stDMConfig.stFNRTSettings = *pstFNRTSettings;

   hXdmPP->stDMConfig.uiDirtyBits_2 |= BXDM_PictureProvider_P_DIRTY_2_FNRT_SETTINGS;
 
   BDBG_LEAVE(BXDM_PictureProvider_SetFNRTSettings_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_GetFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetFNRTSettings_isr);

   BDBG_ASSERT( hXdmPP );
   BDBG_ASSERT( pstFNRTSettings );

   *pstFNRTSettings = hXdmPP->stDMConfig.stFNRTSettings;

   BDBG_LEAVE(BXDM_PictureProvider_GetFNRTSettings_isr);
   return BERR_TRACE( rc );
}


BERR_Code
BXDM_PictureProvider_GetDefaultFNRTSettings_isr(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_FNRTSettings * pstFNRTSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_GetDefaultFNRTSettings_isr);

   BSTD_UNUSED(hXdmPP);
   BDBG_ASSERT( pstFNRTSettings );

   BKNI_Memset( pstFNRTSettings, 0, sizeof( BXDM_PictureProvider_FNRTSettings ));

   BDBG_LEAVE(BXDM_PictureProvider_GetDefaultFNRTSettings_isr);
   return BERR_TRACE( rc );
}


/* 
 * Callback 
 */
BERR_Code
BXDM_PictureProvider_Callback_SetEnable_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_Callback ePictureProviderCallback,
         bool bEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Callback_SetEnable_isr);

   BDBG_ASSERT(hXdmPP);

   BDBG_MSG(("BXDM_PictureProvider_Callback_SetEnable_isr(%p, cb:%d, e:%d)",
                  hXdmPP,
                  ePictureProviderCallback,
                  bEnable
                  ));

   hXdmPP->stCallbacks[ePictureProviderCallback].bEnable = bEnable;

   BDBG_LEAVE(BXDM_PictureProvider_Callback_SetEnable_isr);
   return BERR_TRACE( rc );
}

BERR_Code
BXDM_PictureProvider_Callback_GetEnable_isr(
         BXDM_PictureProvider_Handle hXdmPP,
         BXDM_PictureProvider_Callback ePictureProviderCallback,
         bool *pbEnable
         )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER(BXDM_PictureProvider_Callback_GetEnable_isr);

   BDBG_ASSERT(hXdmPP);
   BDBG_ASSERT(pbEnable);

   *pbEnable = hXdmPP->stCallbacks[ePictureProviderCallback].bEnable;

   BDBG_LEAVE(BXDM_PictureProvider_Callback_GetEnable_isr);
   return BERR_TRACE( rc );
}

#define BXDM_PP_CALLBACK_ENTRY(_name)\
BERR_Code \
BXDM_PictureProvider_Callback_Install_##_name##_isr(\
         BXDM_PictureProvider_Handle hXdmPP,\
         BXDM_PictureProvider_Callback_##_name##_isr fCallback,\
         void *pPrivateContext,\
         int32_t iPrivateParam\
)\
{\
   BERR_Code rc = BERR_SUCCESS;\
   BXDM_PictureProvider_Callback ePictureProviderCallback = BXDM_PictureProvider_Callback_e##_name;\
   BDBG_ENTER(BXDM_PictureProvider_Callback_Install_##_name##_isr);\
   BDBG_ASSERT(hXdmPP);\
   BDBG_MSG(("BXDM_PictureProvider_Callback_Install_"#_name"_isr(%p, f:%p, pp:%p, pi:%d)",\
                  hXdmPP,\
                  fCallback,\
                  pPrivateContext,\
                  iPrivateParam\
                  ));\
   hXdmPP->stCallbacks[ePictureProviderCallback].stFunction.f##_name = fCallback;\
   hXdmPP->stCallbacks[ePictureProviderCallback].pPrivateContext = pPrivateContext;\
   hXdmPP->stCallbacks[ePictureProviderCallback].iPrivateParam = iPrivateParam;\
   BDBG_LEAVE(BXDM_PictureProvider_Callback_Install_##_name##_isr);\
   return BERR_TRACE( rc );\
}
#include "bxdm_pp_callback.def"

#define BXDM_PP_CALLBACK_ENTRY(_name)\
BERR_Code \
BXDM_PictureProvider_Callback_UnInstall_##_name##_isr(\
         BXDM_PictureProvider_Handle hXdmPP\
         )\
{\
   BERR_Code rc = BERR_SUCCESS;\
   BXDM_PictureProvider_Callback ePictureProviderCallback = BXDM_PictureProvider_Callback_e##_name;\
   BDBG_ENTER(BXDM_PictureProvider_Callback_Install_##_name##_isr);\
   BDBG_ASSERT(hXdmPP);\
   BDBG_MSG(("BXDM_PictureProvider_Callback_UnInstall_"#_name"_isr(%p)",\
                  hXdmPP\
                  ));\
   hXdmPP->stCallbacks[ePictureProviderCallback].stFunction.f##_name = NULL;\
   hXdmPP->stCallbacks[ePictureProviderCallback].pPrivateContext = NULL;\
   hXdmPP->stCallbacks[ePictureProviderCallback].iPrivateParam = 0;\
   BDBG_LEAVE(BXDM_PictureProvider_Callback_Install_##_name##_isr);\
   return BERR_TRACE( rc );\
}

#include "bxdm_pp_callback.def"
