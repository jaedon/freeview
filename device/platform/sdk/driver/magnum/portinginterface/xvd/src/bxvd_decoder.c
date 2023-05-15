/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/62 $
 * $brcm_Date: 9/21/12 4:04p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_decoder.c $
 * 
 * Hydra_Software_Devel/62   9/21/12 4:04p davidp
 * SW7346-963: Access FW picture queues using cached memory space, fix
 * still picture release issue.
 * 
 * Hydra_Software_Devel/61   9/6/12 8:55a btosi
 * FWAVD-289: added logic to ensure that the PPBReceived callback is only
 * executed once per PPB
 * 
 * Hydra_Software_Devel/60   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/59   8/20/12 9:12a btosi
 * SW7405-4736: added wrappers for queue macros to aid with debug
 * 
 * Hydra_Software_Devel/58   8/16/12 2:11p btosi
 * SW7405-4736: (Enhance XDM debug capabilities) added BXVD_QDBG message
 * to help detect issues with the delivery queue write offset
 * 
 * Hydra_Software_Devel/57   7/27/12 11:29a btosi
 * SW7425-1064: added support for BXVD_Decoder_OpenChannel
 * 
 * Hydra_Software_Devel/56   7/11/12 8:56a btosi
 * SW7425-3358: use the picture_tag for the chunkId
 * 
 * Hydra_Software_Devel/55   7/2/12 9:56a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/54   6/13/12 11:15a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.  Reintroduced
 * BXVD_P_PPB_Protocol since the AVD and bavc.h protocol enums have
 * diverged.
 * 
 * Hydra_Software_Devel/53   5/8/12 12:02p btosi
 * SW7125-1285: read back PPB and offset to ensure that the values are
 * written to memory
 * 
 * Hydra_Software_Devel/52   5/4/12 11:01a btosi
 * SW7125-1285: declare uiReleaseWriteOffset and pReleaseQue to be
 * volatile to ensure the write order
 * 
 * Hydra_Software_Devel/51   4/12/12 1:03p btosi
 * SW7125-1281: Add more range checking for bad data.
 * 
 * Hydra_Software_Devel/50   3/16/12 11:58a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/49   2/10/12 11:18a btosi
 * SW7425-2247: add support for bar data
 * 
 * Hydra_Software_Devel/48   1/13/12 9:48a btosi
 * SW7405-4736: fixed warning on the 7630 build
 * 
 * Hydra_Software_Devel/47   1/12/12 3:23p btosi
 * SW7405-4736: added debug support for new PPB field flags_ext0
 * 
 * Hydra_Software_Devel/46   1/10/12 1:32p btosi
 * SW7425-1001: added support for bLast flag
 * 
 * Hydra_Software_Devel/45   12/16/11 8:32a btosi
 * SWDTV-8681: add support for VP8 display size
 * 
 * Hydra_Software_Devel/44   12/5/11 2:54p btosi
 * SWCHECKFRAME-80: execute the PPBReceived callback prior to processing
 * the PPB data.
 * 
 * Hydra_Software_Devel/43   12/1/11 1:12p pblanco
 * SW7425-1780: Implemented picture id support for transcode userdata.
 * 
 * Hydra_Software_Devel/42   9/20/11 3:32p btosi
 * SW7425-1261: only clear bTruncatingGop in ..._ReleaseGopTail
 * 
 * Hydra_Software_Devel/41   7/28/11 11:38a btosi
 * SW7405-4736: added the PPB flags to the BXVD_UP message
 * 
 * Hydra_Software_Devel/40   7/27/11 12:08p btosi
 * SWDTV-7795: added support for RV9 cropping
 * 
 * Hydra_Software_Devel/39   7/20/11 3:07p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/38   6/27/11 4:18p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/24/11 8:53a davidp
 * SW7425-542: Add BXVD_P_DBG_PIC_QUES to enable queue processing log.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/14/11 10:40a btosi
 * SW7425-542: cleaned up warning
 * 
 * Hydra_Software_Devel/37   6/8/11 1:04p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/36   6/1/11 3:15p btosi
 * SW7425-542: added macros for accessing the AVD queue read and write
 * offsets
 * 
 * Hydra_Software_Devel/35   5/20/11 11:55a btosi
 * SW7425-609: check bPictureDropped before linking the dependent to the
 * base picture
 * 
 * Hydra_Software_Devel/34   5/3/11 1:32p btosi
 * SW7405-4736: reset stUnifiedContext in ..._UpdateReadOffset, fixes
 * crash in StereoSeqError check test
 * 
 * Hydra_Software_Devel/33   4/18/11 9:33a btosi
 * SW7405-4736: pre increment uiPPBSerialNumber so that the Decoder and
 * XDM debug messages match
 * 
 * Hydra_Software_Devel/32   4/15/11 3:31p btosi
 * SW7405-4736: backed out change for incrementing uiPPBSerialNumber
 * 
 * Hydra_Software_Devel/31   4/15/11 10:50a btosi
 * SW7405-4736: added support to print the SEI messages
 * 
 * Hydra_Software_Devel/30   4/4/11 11:28a btosi
 * SW7405-4736: added support for BXVD_DecoderDbg_P_PrintUnifiedPicture
 * 
 * Hydra_Software_Devel/29   3/15/11 10:17a btosi
 * SW7425-160: updated to use Broadcom standard types
 * 
 * Hydra_Software_Devel/28   2/25/11 11:14a btosi
 * SW7425-106: work around the issue with the DQ read offset increment
 * 
 * Hydra_Software_Devel/27   2/17/11 2:01p btosi
 * SW7405-5135: if the SEI arrangement_cancel flag is set, set both the
 * Unified and MFD picture orientations to 2D
 * 
 * Hydra_Software_Devel/26   2/8/11 12:51p btosi
 * SW7425-82: use frame_packing_arrangement_type to determine the type of
 * frame packing.
 * 
 * Hydra_Software_Devel/25   2/2/11 1:41p btosi
 * SW7125-808: fixed the issue of the drop count going negative in
 * BXVD_Decoder_GetPictureDropPendingCount_isr when the deliver queue
 * is empty
 * 
 * Hydra_Software_Devel/24   1/25/11 3:56p davidp
 * SW7422-22: Add support for SVC Picture Order Count.
 * 
 * Hydra_Software_Devel/23   12/16/10 9:20a btosi
 * SW7422-72: fixed warning
 * 
 * Hydra_Software_Devel/22   12/10/10 1:22p btosi
 * SWBLURAY-23835: added check for a picture being released multiple times
 * 
 * Hydra_Software_Devel/21   11/24/10 1:22p btosi
 * SW7422-72: added BXVD_LEGACY_MVC_SUPPORT and bSetNextPointer for MVC on
 * older DVD chips
 * 
 * Hydra_Software_Devel/20   11/23/10 10:25a btosi
 * SW7422-72: with the exception of older DVD chips, 2D is now the default
 * orientation for MVC streams
 * 
 * Hydra_Software_Devel/19   10/29/10 11:39a btosi
 * SW7422-72: merging 3D changes to mainline
 * 
 * Hydra_Software_Devel/SW7422-72/2   10/29/10 11:34a btosi
 * SW7422-72: added support for AVC 3D
 * 
 * Hydra_Software_Devel/SW7422-72/1   10/19/10 10:29a btosi
 * SW7422-72: expanded support for 3D
 * 
 * Hydra_Software_Devel/18   10/19/10 9:08a delkert
 * SW7405-4044: Add support for
 * BAVC_TransferCharacteristics_eIec_61966_2_4 to support xvYCC
 *
 * Hydra_Software_Devel/17   8/9/10 1:24p btosi
 * SW7405-4733: set eSequence to eInterlaced instead of defaulting to
 * eUnknown
 *
 * Hydra_Software_Devel/16   7/29/10 11:22a btosi
 * SW7405-4560: added support for the SEI "new message" flag.
 *
 * Hydra_Software_Devel/15   7/27/10 11:36a btosi
 * SW7401-4426: added support for the H264 timecode
 *
 * Hydra_Software_Devel/14   7/7/10 2:40p btosi
 * SWBLURAY-21461: for MPEG and VC1, use the display width/height provided
 * in the PPB
 *
 * Hydra_Software_Devel/13   6/28/10 2:48p btosi
 * SW7405-4365: added support to selectively compile the function timing
 * code.
 *
 * Hydra_Software_Devel/12   6/25/10 2:13p btosi
 * SW7405-4365: added use of local variables in
 * BXVD_Decoder_S_UnifiedQ_ValidatePicture to
 * improve performance
 *
 * Hydra_Software_Devel/11   6/16/10 12:56p btosi
 * SW7405-4365: added support for using the new timer functions
 *
 * Hydra_Software_Devel/10   6/11/10 10:54a davidp
 * SW7405-3925: Fix compiler warning.
 *
 * Hydra_Software_Devel/9   6/7/10 10:23a delkert
 * SW7405-3925: When retrieving pending drop count, first check that there
 * are pictures in the Q to avoid "ping-ponging" between vTSM and TSM
 * selection mode.  Modify request drop API to update the request count.
 * Removed duplicated assertion
 *
 * Hydra_Software_Devel/8   6/3/10 11:55a btosi
 * SW7630-83: fixed handling of mismatched pairs of pictures as well as
 * general cleanup
 *
 * Hydra_Software_Devel/7   5/12/10 2:44p btosi
 * SWBLURAY-20606: fixed assignment of
 * stBluRay3DGraphicsOffset.puiOffsetData
 *
 * Hydra_Software_Devel/6   5/4/10 10:41a btosi
 * SW7405-3996: added support for new SEI message format
 *
 * Hydra_Software_Devel/5   5/4/10 9:12a pblanco
 * SW7125-386: Fixed malformed BXVD_DBG_ERR calls.
 *
 * Hydra_Software_Devel/4   4/29/10 4:12p btosi
 * SW7630-79: set stBufferInfo.bValid to false if a pictureless PPB
 *
 * Hydra_Software_Devel/3   4/16/10 10:50a pblanco
 * SW7405-2993: Added some conditionalized debugging output.
 *
 * Hydra_Software_Devel/2   3/1/10 4:58p nilesh
 * SW7405-2993: Fix coverity warnings
 *
 * Hydra_Software_Devel/1   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"                /* Dbglib */
#include "bmth_fix.h"

#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_reg.h"
#include "bxdm_decoder.h"
#include "bxvd_decoder.h"
#include "bxvd_decoder_dbg.h"

#include "bxvd_vdec_info.h"

#if BXVD_DM_ENABLE_YUV_GRAB_MODE
#include <stdio.h>
#include <stdlib.h>
#include "bxvd_reg.h"
#endif


BDBG_MODULE(BXVD_DECODER);
BDBG_FILE_MODULE(BXVD_QDBG);

/***************************************************************************
 *
 *   Local constant and type definitions.
 *
 ***************************************************************************/

/* Number of pictures in this set. Should use the data provided with the
 * picture, i.e. "pstPicCntxt->uiSetCount".  Waiting on the firmware.
 */
#define BXVD_DECODER_S_PICTURES_PER_SET      2

/*
 * Set this to non-zero for extra AVD debugging help
 */
#define BXVD_DECODER_AVD_DEBUG_HELP 0


/* sAVCStripeWidthLUT maps the stripe width from XVD to the proper
 * BAVC_StripeWidth enum
 */
static const BAVC_StripeWidth sAVCStripeWidthLUT[BXVD_P_STRIPE_WIDTH_NUM] =
{
   BAVC_StripeWidth_e64Byte,
   BAVC_StripeWidth_e128Byte

}; /* end of sAVCStripeWidthLUT[] */


static const BXDM_Picture_Rate sFrameRateEnumToIntLUT[BXVD_P_PPB_FrameRate_eMax] =
{
   { 30000, 1000},   /* BXVD_P_PPB_FrameRate_eUnknown */
   { 24000, 1001},   /* BXVD_P_PPB_FrameRate_e23_976  */
   { 24000, 1000},   /* BXVD_P_PPB_FrameRate_e24      */
   { 25000, 1000},   /* BXVD_P_PPB_FrameRate_e25      */
   { 30000, 1001},   /* BXVD_P_PPB_FrameRate_e29_97   */
   { 30000, 1000},   /* BXVD_P_PPB_FrameRate_e30      */
   { 50000, 1000},   /* BXVD_P_PPB_FrameRate_e50      */
   { 60000, 1001},   /* BXVD_P_PPB_FrameRate_e59_94   */
   { 60000, 1000},   /* BXVD_P_PPB_FrameRate_e60      */
   { 15000, 1001},   /* BXVD_P_PPB_FrameRate_e14_985  */
   {  7500, 1001},   /* BXVD_P_PPB_FrameRate_e7_493   */
   { 15000, 1000},   /* BXVD_P_PPB_FrameRate_e15      */
   { 10000, 1000},   /* BXVD_P_PPB_FrameRate_e10      */
   { 20000, 1000}    /* BXVD_P_PPB_FrameRate_e20      */
};


typedef struct BXVD_Decoder_S_SampleAspectRatio
{
      uint16_t uiSampleAspectRatioX;
      uint16_t uiSampleAspectRatioY;
} BXVD_Decoder_S_SampleAspectRatio;

#define BXVD_DECODER_S_SAMPLE_ASPECT_RATIO_LUT_SIZE BXVD_P_PPB_AspectRatio_e221_1 + 1

static const BXVD_Decoder_S_SampleAspectRatio sSampleAspectRatioLUT[BXVD_DECODER_S_SAMPLE_ASPECT_RATIO_LUT_SIZE] =
{
   {0,0},
   {1,1},
   {12,11},
   {10,11},
   {16,11},
   {40,33},
   {24,11},
   {20,11},
   {32,11},
   {80,33},
   {18,11},
   {15,11},
   {64,33},
   {160,99},
   {4,3},
   {3,2},
   {2,1},
   {16,9},
   {221,1}
}; /* end of sSampleAspectRatioLUT[] */

/* DNR Related Defines */
#define BXVD_Decoder_S_DNR_INTRA_DEFAULT              2114
#define BXVD_Decoder_S_DNR_INTER_DEFAULT              964

#define BXVD_Decoder_S_DNR_INTRA_DEFAULT_AVC          1680
#define BXVD_Decoder_S_DNR_INTER_DEFAULT_AVC          1507

/* Lookup Tables */
#define BXVD_DECODER_S_MATRIX_COEFFICIENTS_LUT_SIZE (BXVD_P_PPB_MatrixCoeff_eSMPTE293M + 1)
static const BAVC_MatrixCoefficients sMatrixCoefficientsLUT[BXVD_DECODER_S_MATRIX_COEFFICIENTS_LUT_SIZE] =
{
   BAVC_MatrixCoefficients_eUnknown,           /* BXVD_P_PPB_MatrixCoeff_eUnknown */
   BAVC_MatrixCoefficients_eItu_R_BT_709,      /* BXVD_P_PPB_MatrixCoeff_eBT709 */
   BAVC_MatrixCoefficients_eUnknown,           /* BXVD_P_PPB_MatrixCoeff_eUnspecified */
   BAVC_MatrixCoefficients_eUnknown,           /* BXVD_P_PPB_MatrixCoeff_eReserved */
   BAVC_MatrixCoefficients_eFCC,               /* BXVD_P_PPB_MatrixCoeff_eFCC */
   BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG, /* BXVD_P_PPB_MatrixCoeff_eBT740_2BG */
   BAVC_MatrixCoefficients_eSmpte_170M,        /* BXVD_P_PPB_MatrixCoeff_eSMPTE170M */
   BAVC_MatrixCoefficients_eSmpte_240M,        /* BXVD_P_PPB_MatrixCoeff_eSMPTE240M */
   BAVC_MatrixCoefficients_eUnknown            /* BXVD_P_PPB_MatrixCoeff_eSMPTE293M */
}; /* end of sMatrixCoefficientsLUT[] */

#define BXVD_DECODER_S_COLOR_PRIMARIES_LUT_SIZE (BXVD_P_PPB_ColorPrimaries_eBT1361 + 1)
static const BAVC_ColorPrimaries sColorPrimariesLUT[BXVD_DECODER_S_COLOR_PRIMARIES_LUT_SIZE] =
{
   BAVC_ColorPrimaries_eUnknown,           /* BXVD_P_PPB_ColorPrimaries_eUnknown */
   BAVC_ColorPrimaries_eItu_R_BT_709,      /* BXVD_P_PPB_ColorPrimaries_eBT709 */
   BAVC_ColorPrimaries_eUnknown,           /* BXVD_P_PPB_ColorPrimaries_eUnspecified */
   BAVC_ColorPrimaries_eUnknown,           /* BXVD_P_PPB_ColorPrimaries_eReserved */
   BAVC_ColorPrimaries_eItu_R_BT_470_2_M,  /* BXVD_P_PPB_ColorPrimaries_eBT470_2M */
   BAVC_ColorPrimaries_eItu_R_BT_470_2_BG, /* BXVD_P_PPB_ColorPrimaries_eBT470_2BG */
   BAVC_ColorPrimaries_eSmpte_170M,        /* BXVD_P_PPB_ColorPrimaries_eSMPTE170M */
   BAVC_ColorPrimaries_eSmpte_240M,        /* BXVD_P_PPB_ColorPrimaries_eSMPTE240M */
   BAVC_ColorPrimaries_eGenericFilm,       /* BXVD_P_PPB_ColorPrimaries_eGenericFilm */
   BAVC_ColorPrimaries_eUnknown            /* BXVD_P_PPB_ColorPrimaries_eBT1361 */
}; /* end of sColorPrimariesLUT[] */

#define BXVD_DECODER_S_TRANSFER_CHARACTERISICS_LUT_SIZE (BXVD_P_PPB_TransferChar_eIec_61966_2_4 + 1)
static const BAVC_TransferCharacteristics sTransferCharacteristicsLUT[BXVD_DECODER_S_TRANSFER_CHARACTERISICS_LUT_SIZE] =
{
   BAVC_TransferCharacteristics_eUnknown,           /* BXVD_P_PPB_TransferChar_eUnknown */
   BAVC_TransferCharacteristics_eItu_R_BT_709,      /* BXVD_P_PPB_TransferChar_eBT709 */
   BAVC_TransferCharacteristics_eUnknown,           /* BXVD_P_PPB_TransferChar_eUnspecified */
   BAVC_TransferCharacteristics_eUnknown,           /* BXVD_P_PPB_TransferChar_eReserved */
   BAVC_TransferCharacteristics_eItu_R_BT_470_2_M,  /* BXVD_P_PPB_TransferChar_eBT479_2M */
   BAVC_TransferCharacteristics_eItu_R_BT_470_2_BG, /* BXVD_P_PPB_TransferChar_eBT479_2BG */
   BAVC_TransferCharacteristics_eSmpte_170M,        /* BXVD_P_PPB_TransferChar_eSMPTE170M */
   BAVC_TransferCharacteristics_eSmpte_240M,        /* BXVD_P_PPB_TransferChar_eSMPTE240M */
   BAVC_TransferCharacteristics_eLinear,            /* BXVD_P_PPB_TransferChar_eLinear */
   BAVC_TransferCharacteristics_eUnknown,           /* BXVD_P_PPB_TransferChar_eLog100_1 */
   BAVC_TransferCharacteristics_eUnknown,           /* BXVD_P_PPB_TransferChar_eLog31622777_1 */
   BAVC_TransferCharacteristics_eIec_61966_2_4      /* BXVD_P_PPB_TransferChar_eIec_61966_2_4 */
}; /* end of sTransferCharacteristicsLUT[] */

/***************************************************************************
 *
 *   Function prototypes.
 *
 ***************************************************************************/

static void BXVD_Decoder_S_Increment_0BasedOffset(
   uint32_t * pOffset,
   uint32_t uiCount,
   uint32_t uiMaxValue
   );

static void BXVD_Decoder_S_Decrement_0BasedOffset(
   uint32_t * pOffset,
   uint32_t uiCount,
   uint32_t uiMaxValue
   );

static void BXVD_Decoder_S_Decrement_2BasedOffset(
   uint32_t * pOffset,
   uint32_t uiCount,
   uint32_t uiMaxValue
   );

static void BXVD_Decoder_S_DeliveryQ_UpdateReadOffset(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState* pstLocalState,
   uint32_t uiDeltaChange
   );

static void BXVD_Decoder_S_UnifiedQ_ValidatePicture(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext * pstPictureCntxt,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext,
   bool bPictureDropped
   );

static bool  BXVD_Decoder_S_DeliveryQ_CheckForDeadlock(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiNumPicturesToProcess
   );

static void BXVD_Decoder_S_DeliveryQ_ReleaseGopTail(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState * pstLocalState
   );

static void BXVD_Decoder_S_ReleaseQ_AddPPB(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_PPB *pPPBPhysical
   );

static void BXVD_Decoder_S_UnifiedQ_Initialize(
   BXVD_ChannelHandle hXvdCh
   );

static void BXVD_Decoder_S_UnifiedQ_AddPicture(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext * pstPictureCntxt,
   bool bBasePicture
   );

static void BXVD_Decoder_S_UnifiedQ_GetNextFreeElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext ** ppstUnifiedContext
   );

static void BXVD_Decoder_S_UnifiedQ_GetLastElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext ** ppstUnifiedContext,
   bool bBumpReadOffset
   );

static void BXVD_Decoder_S_UnifiedQ_DumpElement(
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext,
   bool bDependentPicture
   );

static void BXVD_Decoder_S_UnifiedQ_ReleaseElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext
   );

static uint32_t BXVD_Decoder_S_UnifiedQ_GetFreeCount(
   BXVD_ChannelHandle hXvdCh
   );

static uint32_t BXVD_Decoder_S_UnifiedQ_GetPictureCount(
   BXVD_ChannelHandle hXvdCh
   );

static void BXVD_Decoder_S_ComputeAspectRatio(
   BXVD_P_PPB * pPPB,
   BXDM_Picture * pstXdmPicture,
   BXVD_P_PPB_Protocol   eProtocol
   );

#if BXVD_DM_ENABLE_YUV_GRAB_MODE
void BXVD_Decoder_S_Dbg_DumpPPB( BXVD_P_PPB *ppb );
void BXVD_Decoder_S_Dbg_DumpRVC( BXVD_ChannelHandle hXvdCh );
void BXVD_Decoder_S_Dbg_DumpYUV( BXVD_ChannelHandle hXvdCh, BXVD_P_PPB *ppb );
#endif

/***************************************************************************
 *
 *   Local macros.
 *
 ***************************************************************************/

/***************************************************************************
 *
 *   Private functions.
 *
 ***************************************************************************/

#if BXVD_P_DBG_PIC_QUES
static void BXVD_Decoder_S_Printf( char * pszFormat, ... )
{
   va_list  pArg;

   va_start( pArg, pszFormat );
   BKNI_Vprintf( pszFormat, pArg );
   va_end( pArg );
}

#else
static void BXVD_Decoder_S_Printf( char * pszFormat, ... )
{
   BSTD_UNUSED( pszFormat );
   BDBG_NOP();
}
#endif

static void BXVD_Decoder_S_Increment_0BasedOffset(
   uint32_t* pOffset,
   uint32_t  uiCount,
   uint32_t  uiMaxValue
   )
{
   uint32_t i;

   for ( i=0; i < uiCount; i++ )
   {
      /* If the offset is about to go off the end of the queue,
       * set it back to the beginning.  Otherwise just increment it.
       */
      if( *pOffset >= uiMaxValue - 1/*(BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE - 1)*/ )
      {
         *pOffset = 0;
      }
      else
      {
         (*pOffset)++;
      }
   }

   return;
}


static void BXVD_Decoder_S_Decrement_0BasedOffset(
   uint32_t* pOffset,
   uint32_t  uiCount,
   uint32_t  uiMaxValue
   )
{
   uint32_t i;

   for ( i=0; i < uiCount; i++ )
   {
      /* If the offset is about to go off the beginning of the queue,
       * set it to the end.  Otherwise just decrement it.
       */
      if( *pOffset == 0 )
      {
         *pOffset = uiMaxValue - 1/*BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE - 1*/;
      }
      else
      {
         (*pOffset)--;
      }
   }

   return;
}

static void BXVD_Decoder_S_Decrement_2BasedOffset(
   uint32_t* pOffset,
   uint32_t  uiCount,
   uint32_t  uiMaxValue
   )
{
   uint32_t i;

   for ( i=0; i < uiCount; i++ )
   {
      /* If the offset is about to go off the beginning of the queue,
       * set it to the end.  Otherwise just decrement it.
       */
      if( *pOffset <=  BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE )
      {
         *pOffset = uiMaxValue - 1 /*BXVD_P_DISP_FIFO_DEPTH - 1*/;
      }
      else
      {
         (*pOffset)--;
      }
   }

   return;
}

/***************************************************************************
 *
 *   Routines for managing the delivery queue.
 *
 ***************************************************************************/

static bool BXVD_Decoder_S_ContextQ_PeekAtNextElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext ** pstPicCntxt
   )
{
   bool bReturn;
   uint32_t uiContextQReadOffset0Based;

   BXVD_Decoder_P_PictureContextQueue * pstContextQueue = &(hXvdCh->stDecoderContext.stPictureContextQueue);

   BDBG_ENTER(BXVD_Decoder_S_ContextQ_PeekAtNextElement);

   /* Return the appropriate display queue read offset based on the state of DQT.
    * If playing backwards, return "uiReverseReadOffset" which is walking backwards
    * through each GOP.  Else, return the standard "read offset".
    */
   if ( true == hXvdCh->stDecoderContext.stDqtCntxt.bDqtEnabled )
   {
      uiContextQReadOffset0Based = hXvdCh->stDecoderContext.stDqtCntxt.uiReverseReadOffset;
   }
   else
   {
      BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiContextQReadOffset0Based );
   }

   uiContextQReadOffset0Based -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

   /* If the read offset is equal to the context queue write offset,
    * both the deliver and context queues are empty, i.e. all the
    * pictures have been processed and are on the unified picture queue.
    */
   if ( uiContextQReadOffset0Based == pstContextQueue->uiWriteOffset )
   {
      bReturn = false;
      *pstPicCntxt = NULL;
   }
   else
   {
      /* Queue has data, simply return a pointer to the next element. */

      bReturn = true;
      *pstPicCntxt = &(pstContextQueue->astPictureContext[ uiContextQReadOffset0Based ]);
   }


   BDBG_LEAVE(BXVD_Decoder_S_ContextQ_PeekAtNextElement);

   return bReturn;
}

/****************************************************************************************************
**
** Local "utility" routines.
**
*****************************************************************************************************/

static void BXVD_Decoder_S_Log_Init(
   BXVD_ChannelHandle hXvdCh
   )
{
   BDBG_ENTER( BXVD_Decoder_S_Log_Init );

   BKNI_Memset( &(hXvdCh->stDecoderContext.stLogData), 0, sizeof( BXVD_Decoder_P_LogData ));

   /* Want to print out the results roughly once per second.
    * We'll count vsyncs to measure time.
    */
   switch ( hXvdCh->sChSettings.ui32MonitorRefreshRate )
   {
      case BXVD_MONITOR_REFRESH_RATE_50Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 50;
         break;

      case BXVD_MONITOR_REFRESH_RATE_59_94Hz:
      case BXVD_MONITOR_REFRESH_RATE_60Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 60;
         break;

      case BXVD_MONITOR_REFRESH_RATE_23_976Hz:
      case BXVD_MONITOR_REFRESH_RATE_24Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 24;
         break;

      case BXVD_MONITOR_REFRESH_RATE_25Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 25;
         break;

      case BXVD_MONITOR_REFRESH_RATE_30Hz:
      case BXVD_MONITOR_REFRESH_RATE_29_97Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 30;
         break;

      case BXVD_MONITOR_REFRESH_RATE_48Hz:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 48;
         break;

      case BXVD_MONITOR_REFRESH_RATE_INVALID:
      default:
         hXvdCh->stDecoderContext.stLogData.uiVsyncsPerSecond = 60;
         break;
   }

   BDBG_LEAVE( BXVD_Decoder_S_Log_Init );

   return;

}  /* end of BXVD_Decoder_S_Log_Init */

static void BXVD_Decoder_S_Log_Print(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState * pstLocalState,
   bool bForcePrint
   )
{
   BXVD_Decoder_P_LogData * pstLogData = &(hXvdCh->stDecoderContext.stLogData);

   BDBG_ENTER( BXVD_Decoder_S_Log_Print );

   /* Print out the results roughly once per second or when the calling routine 
    * forces the issue (for example BXVD_Decoder_StopDecode_isr).
    */
   if ( pstLogData->uiCallbackCount >= pstLogData->uiVsyncsPerSecond  
         || true == bForcePrint )
   {
      uint32_t    uiReadOffset;
      uint32_t    uiWriteOffset;
      uint32_t    uiDeliveryQDepth;
      uint32_t    uiReleaseQDepth;
      uint32_t    uiUnifiedQDepth;
      uint32_t    uiMaskHighWord, uiMaskLowWord;
      int32_t     i;

      BXVD_P_PictureDeliveryQueue * pstDeliveryQueue;
      BXVD_P_PictureReleaseQueue *  pstReleaseQueue;

      BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue;

      /* Retrieve the read/write offsets for the delivery queue.
       * Adjust them to be '0' based.
       */
      BXVD_P_DELIVERY_QUEUE_GET_ADDR( hXvdCh, pstDeliveryQueue );

#if BXVD_P_FW_HIM_API
      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)pstDeliveryQueue, sizeof(BXVD_P_PictureReleaseQueue))); 
#endif
      BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiReadOffset );

      uiReadOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      uiWriteOffset = pstLocalState->uiDeliveryQueueWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

      BXVD_P_GET_QUEUE_DEPTH( uiReadOffset, uiWriteOffset, uiDeliveryQDepth );

      /* Retrieve the read/write offsets for the release queue.
       * Adjust them to be '0' based.
       */
      BXVD_P_RELEASE_QUEUE_GET_ADDR( hXvdCh, pstReleaseQueue );

      BXVD_P_RELEASE_QUEUE_GET_READ_OFFSET( hXvdCh, uiReadOffset );
      BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( hXvdCh, uiWriteOffset );

      uiReadOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      uiWriteOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

      BXVD_P_GET_QUEUE_DEPTH( uiReadOffset, uiWriteOffset, uiReleaseQDepth );

      uiUnifiedQDepth = BXVD_Decoder_S_UnifiedQ_GetPictureCount( hXvdCh );

      uiMaskHighWord = uiMaskLowWord = 0;
      pstUnifiedQueue = &(hXvdCh->stDecoderContext.stUnifiedPictureQueue);

      /* Generate a mask of the Unified Queue elements in use. */
      for ( i = (BXVD_P_DECODER_PICTURE_QUEUE_DEPTH - 1); i >= 0; i-- )
      {
         if ( pstUnifiedQueue->astUnifiedContext[ i ].bInUse )
         {
            if ( i > 31 )
            {
               uiMaskHighWord |= 1;
            }
            else
            {
               uiMaskLowWord |= 1;
            }
         }

         if ( i > 32 )
         {
            uiMaskHighWord <<= 1;
         }
         else if ( i < 32 && i > 0 )
         {
            uiMaskLowWord <<= 1;
         }
      }

      BXVD_DBG_MSG( hXvdCh, ("AVD: %d/%d Dec: %d/%d Dq: %d Rq: %d Uq: %d InUse: %08x%08x",
                  pstLogData->uiPicturesFromAvd,
                  pstLogData->uiPicturesToAvd,
                  pstLogData->uiPicturesToPP,
                  pstLogData->uiPicturesFromPP,
                  uiDeliveryQDepth,
                  uiReleaseQDepth,
                  uiUnifiedQDepth,
                  uiMaskHighWord,
                  uiMaskLowWord
                  ));

      pstLogData->uiCallbackCount = 0;
   }

   pstLogData->uiCallbackCount++;

   BDBG_LEAVE( BXVD_Decoder_S_Log_Print );

   return;

}  /* end of BXVD_Decoder_S_Log_Print */

/*
 * A wrapper routine to be called by the still picture ISR handler.
 */
BERR_Code BXVD_Decoder_P_ComputeAspectRatio(
   BXVD_P_PPB * pPPB,
   BXDM_Picture * pstXdmPicture
   )
{
   BDBG_ASSERT( pPPB );
   BDBG_ASSERT( pstXdmPicture );

   BDBG_ENTER(BXVD_Decoder_P_ComputeAspectRatio);

   BXVD_Decoder_S_ComputeAspectRatio( pPPB, pstXdmPicture, pPPB->protocol );

   BDBG_LEAVE(BXVD_Decoder_P_ComputeAspectRatio);

   return BERR_SUCCESS;
}

/*
 *
 */
static void BXVD_Decoder_S_ComputeAspectRatio(
   BXVD_P_PPB * pPPB,
   BXDM_Picture * pstXdmPicture,
   BXVD_P_PPB_Protocol   eProtocol
   )
{
   BDBG_ENTER(BXVD_Decoder_S_ComputeAspectRatio);

   BDBG_ASSERT( pPPB );
   BDBG_ASSERT( pstXdmPicture );

   pstXdmPicture->stAspectRatio.uiSampleAspectRatioX = 0;
   pstXdmPicture->stAspectRatio.uiSampleAspectRatioY = 0;

   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         /* In MPEG2 streams, the decoder sends a subset of SAR
          * values (Sample Aspect Ratio).  The subset of SAR values
          * maps directly to explicit Display Aspect Ratio (DAR)
          * values that the VDC supports */
         switch ( pPPB->aspect_ratio )
         {
            case BXVD_P_PPB_AspectRatio_eSquare:
               pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_eSquarePxl;
               pstXdmPicture->stAspectRatio.bValid = true;
               break;

            case BXVD_P_PPB_AspectRatio_e4_3:
               pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_e4_3;
               pstXdmPicture->stAspectRatio.bValid = true;
               break;

            case BXVD_P_PPB_AspectRatio_e16_9:
               pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_e16_9;
               pstXdmPicture->stAspectRatio.bValid = true;
               break;

            case BXVD_P_PPB_AspectRatio_e221_1:
               pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_e221_1;
               pstXdmPicture->stAspectRatio.bValid = true;
               break;

            default:
               pstXdmPicture->stAspectRatio.bValid = false;
         }
         break;

      default:
         /* In H.264 streams, the decoder sends SAR values (Sample
          * Aspect Ratio). VDC normally expects DAR values (Display
          * Aspect Ratio) that are in the MPEG2 streams, so for
          * H.264, we need to use the BFMT_AspectRatio_eSAR aspect
          * ratio and set the ratios explicitly */

         if ( BXVD_P_PPB_AspectRatio_eOther == pPPB->aspect_ratio  )
         {
            pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_eSAR;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioX = pPPB->custom_aspect_ratio_width_height & 0xFFFF;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioY = (pPPB->custom_aspect_ratio_width_height >> 16) & 0xFFFF;
            pstXdmPicture->stAspectRatio.bValid = true;
         }
         else if ( BXVD_P_PPB_AspectRatio_eUnknown == pPPB->aspect_ratio )
         {
            pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_eUnknown;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioX = sSampleAspectRatioLUT[pPPB->aspect_ratio].uiSampleAspectRatioX;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioY = sSampleAspectRatioLUT[pPPB->aspect_ratio].uiSampleAspectRatioY;
            pstXdmPicture->stAspectRatio.bValid = true;
         }
         else if (( pPPB->aspect_ratio >= BXVD_P_PPB_AspectRatio_eSquare )
                  && ( pPPB->aspect_ratio <= BXVD_P_PPB_AspectRatio_e221_1 ))
         {
            pstXdmPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_eSAR;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioX = sSampleAspectRatioLUT[pPPB->aspect_ratio].uiSampleAspectRatioX;
            pstXdmPicture->stAspectRatio.uiSampleAspectRatioY = sSampleAspectRatioLUT[pPPB->aspect_ratio].uiSampleAspectRatioY;
            pstXdmPicture->stAspectRatio.bValid = true;
         }
         else
         {
            pstXdmPicture->stAspectRatio.bValid = false;
         }
         break;
   }

   BDBG_LEAVE(BXVD_Decoder_S_ComputeAspectRatio);

} /* end of BXVD_Decoder_S_ComputeAspectRatio() */

static void BXVD_Decoder_S_ComputeDigitalNoiseReduction(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_PPB * pPPB,
   BXDM_Picture * pUnifiedPicture,
   BXVD_P_PPB_Protocol   eProtocol
   )
{
   bool                  ref_pic_flag;
   unsigned long         avg_qp_cur;
   unsigned long         avg_qp_ref;
   unsigned long         intra_mb_cnt;
   unsigned long         inter_coded_mb_cnt;
   unsigned long         total_mb_cnt;
   unsigned long         intra_low_sum;
   unsigned long         intra_hi_sum;
   unsigned long         inter_low_sum;
   unsigned long         inter_hi_sum;

   unsigned long         act_qp_ref;
   unsigned long         adj_Qp = 0;
   unsigned long         intra_default;
   unsigned long         inter_default;

   signed long           wf1, wf2, sf1, sf2;
   signed long           var1,var2;
   signed long           var3,var4,var5,var6;
   signed long           Quo, Rem, Divd, Divs;

   bool                 p_forwarding;
   signed long          alpha;

   BDBG_ENTER(BXVD_Decoder_S_ComputeDigitalNoiseReduction);

   BDBG_ASSERT(pPPB);
   BDBG_ASSERT(pUnifiedPicture);

   if ( NULL == pPPB )
   {
      pUnifiedPicture->stDigitalNoiseReduction.bValid = false;
   }
   else
   {
      pUnifiedPicture->stDigitalNoiseReduction.bValid = true;

      /* Prepare DNR parameters */
      if (BXVD_P_PPB_Protocol_eH264 == eProtocol)
      {
         intra_default = BXVD_Decoder_S_DNR_INTRA_DEFAULT_AVC;
         inter_default = BXVD_Decoder_S_DNR_INTER_DEFAULT_AVC;
      }
      else{

         intra_default = BXVD_Decoder_S_DNR_INTRA_DEFAULT;
         inter_default = BXVD_Decoder_S_DNR_INTER_DEFAULT;

      }

      /* Ref Picture Flag is set if it is I or P picture */
      if ( (pPPB->dnr.pic_flags & BXVD_P_PPB_DNR_FLAG_REFPIC) == BXVD_P_PPB_DNR_FLAG_REFPIC )
      {
         ref_pic_flag = 1;
      }
      else
      {
         ref_pic_flag = 0;
      }

      /* Calculate the avg QP for current picture */
      if ( pPPB->dnr.num_mb_cur != 0 )
      {
         /* avg_qp_cur = (pPPB->dnr.sum_qp_cur / pPPB->dnr.num_mb_cur) ;*/
         Divd = pPPB->dnr.sum_qp_cur;
         Divs = pPPB->dnr.num_mb_cur;

         if (Divs != 0)
         {
            Quo = Divd/Divs;
         }
         else
         {
            Quo = Divd;
         }

         Rem = Divd - (Quo * Divs);

         Quo  = BMTH_FIX_SIGNED_ITOFIX(Quo, 26, 5);
         Rem  = BMTH_FIX_SIGNED_ITOFIX(Rem, 16, 15);
         Divs = BMTH_FIX_SIGNED_ITOFIX(Divs, 31, 0);

         if (Divs != 0)
         {
            avg_qp_cur = BMTH_FIX_SIGNED_DIV(Rem, Divs, 16, 15, 31, 0, 16, 15);
            avg_qp_cur = BMTH_FIX_SIGNED_CONVERT(avg_qp_cur, 16, 15, 26, 5);

            avg_qp_cur = avg_qp_cur + Quo;
         }
         else
         {
            avg_qp_cur = Quo;
         }

         /* avg_qp_cur (26,5)*/

      }
      else
      {
         avg_qp_cur = 0;
      }

      /* Calculate the avg QP for reference picture */
      if ( pPPB->dnr.num_mb_ref != 0 )
      {
         /*     avg_qp_ref = pPPB->dnr.sum_qp_ref /
                pPPB->dnr.num_mb_ref;*/

         Divd = pPPB->dnr.sum_qp_ref;
         Divs = (pPPB->dnr.num_mb_ref);

         if (Divs != 0)
         {
            Quo = Divd/Divs;
         }
         else
         {
            Quo = Divd;
         }

         Rem = Divd - (Quo * Divs);

         Quo  = BMTH_FIX_SIGNED_ITOFIX(Quo, 26, 5);
         Rem  = BMTH_FIX_SIGNED_ITOFIX(Rem, 16, 15);
         Divs = BMTH_FIX_SIGNED_ITOFIX(Divs, 31, 0);

         if (Divs != 0)
         {
            avg_qp_ref = BMTH_FIX_SIGNED_DIV(Rem, Divs, 16, 15, 31, 0, 16, 15);
            avg_qp_ref = BMTH_FIX_SIGNED_CONVERT(avg_qp_cur, 16, 15, 26, 5);

            avg_qp_ref = avg_qp_ref + Quo;
         }
         else
         {
            avg_qp_ref = Quo;
         }

         /* avg_qp_ref (26,5) */

      }
      else
      {
         avg_qp_ref = 0;
      }


      /* Extract the intra and inter MB count */
      intra_mb_cnt = pPPB->dnr.mb_count_intra_inter >> 16;
      inter_coded_mb_cnt = (pPPB->dnr.mb_count_intra_inter & 0xffff);

      /* Store the total number of MB in current picture */
      total_mb_cnt = pPPB->dnr.num_mb_cur;

      /* Extract the Intra sum */
      intra_low_sum =  pPPB->dnr.intra_qmat_sum_lo_hi >> 16;
      intra_hi_sum = pPPB->dnr.intra_qmat_sum_lo_hi & 0xffff;

      /* Extract the Inter sum */
      inter_low_sum = pPPB->dnr.inter_qmat_sum_lo_hi >> 16;
      inter_hi_sum  = pPPB->dnr.inter_qmat_sum_lo_hi & 0xffff;


      /* Initializing few parameters */
      p_forwarding = 1;
      /* alpha = 0.5; */
      alpha = 0x8;      /* Q4 for 0.5 */


      if ( (pPPB->dnr.pic_flags & BXVD_P_PPB_DNR_FLAG_GOPHEADER) == BXVD_P_PPB_DNR_FLAG_GOPHEADER )
      {
         /* PR 36460 : XVD : DM in PI : Reset mechanism of DNR on  */
         /* closed GOP and /or video sequence header, is erroneous.*/
         /*                                                        */
         /* At the beginning of each video sequence or closed GOP  */
         /* boundary, old wrong code initializes these variables:  */
         /*          adj_Qp_pre = 0;                               */
         /*          adj_Qp_ref = 0;                               */
         /*                                                        */
         /* but the correct reset mechanism is :                   */
         /*                                                        */
         /* At the beginning of each video sequence or closed GOP  */
         /* boundary, initialize these variables:                  */
         /*       adj_Qp_pre = adj_Qp;                             */
         /*       adj_Qp_ref = 0;                                  */
         /*                                                        */
         /* hXdmPP->stDMContextState.stDNRInfo.adj_Qp_pre = 0; */
         hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_ref = 0;
      }

      /* To avoid Divided By Zero Exception */
      if ( total_mb_cnt == 0 )
      {
         total_mb_cnt = hXvdCh->stDecoderContext.stDNRInfo.saved_mb_cnt;
      }
      else
      {
         hXvdCh->stDecoderContext.stDNRInfo.saved_mb_cnt = total_mb_cnt;
      }
      if ( (1 == p_forwarding) &&
           (0 != hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_ref) )
      {
         act_qp_ref = hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_ref;
      }
      else
      {
         act_qp_ref = avg_qp_ref;
      }

      if ( ref_pic_flag == 1 )
      {
         /*     w1 = (float)intra_mb_cnt / (float)total_mb_cnt;
                s1 = (float)(intra_low_sum + intra_hi_sum) / BXVD_P_INTRA_DEFAULT;
                w2 = (float)inter_coded_mb_cnt / (float)total_mb_cnt;
                s2 = (float)(inter_low_sum + inter_hi_sum) / BXVD_P_INTER_DEFAULT;
                adj_Qp   = ((float)(w1 * s1 * avg_qp_cur +w2 * s2 * avg_qp_cur +(1 - w1 - w2) * act_qp_ref)+ 0.5);
         */

         var1 = BMTH_FIX_SIGNED_ITOFIX(intra_mb_cnt, 16, 15);
         var2 = BMTH_FIX_SIGNED_ITOFIX(total_mb_cnt, 31, 0);
         if ( var2 != 0 )
         {
            wf1 = BMTH_FIX_SIGNED_DIV(var1, var2, 16, 15, 31, 0, 16, 15) ;
         }
         else
         {
            wf1 = var1;
         }

         var1 = BMTH_FIX_SIGNED_ITOFIX((intra_low_sum + intra_hi_sum), 17, 14);
         var2 = BMTH_FIX_SIGNED_ITOFIX(intra_default, 31, 0);
         sf1 = BMTH_FIX_SIGNED_DIV(var1, var2, 17, 14, 31, 0, 17, 14) ;

         var1 = BMTH_FIX_SIGNED_ITOFIX(inter_coded_mb_cnt, 16, 15);
         var2 = BMTH_FIX_SIGNED_ITOFIX(total_mb_cnt, 31, 0);
         if ( var2 != 0 )
         {
            wf2 = BMTH_FIX_SIGNED_DIV(var1, var2, 16, 15, 31, 0, 16, 15) ;
         }
         else
         {
            wf2 = var1;
         }

         var1 = BMTH_FIX_SIGNED_ITOFIX((inter_low_sum + inter_hi_sum), 17, 14);
         var2 = BMTH_FIX_SIGNED_ITOFIX(inter_default, 31, 0);
         sf2 = BMTH_FIX_SIGNED_DIV(var1, var2, 17, 14, 31, 0, 17, 14) ;

         var2 = BMTH_FIX_SIGNED_MUL(wf1, avg_qp_cur, 16, 15, 26, 5, 11, 20);
         var2 = BMTH_FIX_SIGNED_CONVERT(var2, 11, 20, 21, 10);
         sf1 =  BMTH_FIX_SIGNED_CONVERT(sf1, 17, 14, 21, 10);
         var3 = BMTH_FIX_SIGNED_MUL(sf1, var2, 21, 10, 21, 10, 11, 20);

         var2 = BMTH_FIX_SIGNED_MUL(wf2, avg_qp_cur, 16, 15, 26, 5, 11, 20);
         var2 = BMTH_FIX_SIGNED_CONVERT(var2, 11, 20, 21, 10);
         sf2 =  BMTH_FIX_SIGNED_CONVERT(sf2, 17, 14, 21, 10);
         var4 = BMTH_FIX_SIGNED_MUL(sf2, var2, 21, 10, 21, 10, 11, 20);

         var1 = (wf1 + wf2);
         var2 = BMTH_FIX_SIGNED_ITOFIX(1, 16, 15);
         var2 = var2 - var1;
         var5 = BMTH_FIX_SIGNED_MUL(var2, act_qp_ref, 16, 15, 26, 5, 11, 20);

         var6 = var3 + var4 + var5;
         var6 = BMTH_FIX_SIGNED_CONVERT(var6, 11, 20, 15, 16);

         adj_Qp = var6;

         /* hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_ref = adj_Qp; */
         hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_ref = BMTH_FIX_SIGNED_CONVERT(adj_Qp, 15, 16, 26, 5);
      }
      else
      {
         /* w1 = (float)intra_mb_cnt / (float)total_mb_cnt;
            s1 = (float)(intra_low_sum + intra_hi_sum) / BXVD_P_INTRA_DEFAULT;
            w2 = (float)inter_coded_mb_cnt / (float)total_mb_cnt;
            s2 = (float)(inter_low_sum + inter_hi_sum) / BXVD_P_INTER_DEFAULT;
            adj_Qp   = ((float)(w1 * s1 * avg_qp_cur +w2 * s2 * avg_qp_cur +(1 - w1 - w2) * act_qp_ref)+ 0.5);
         */
         var1 = BMTH_FIX_SIGNED_ITOFIX(intra_mb_cnt, 16, 15);
         var2 = BMTH_FIX_SIGNED_ITOFIX(total_mb_cnt, 31, 0);
         if ( var2 != 0 )
         {
            wf1 = BMTH_FIX_SIGNED_DIV(var1, var2, 16, 15, 31, 0, 16, 15) ;
         }
         else
         {
            wf1 = var1;
         }

         var1 = BMTH_FIX_SIGNED_ITOFIX((intra_low_sum + intra_hi_sum), 17, 14);
         var2 = BMTH_FIX_SIGNED_ITOFIX(intra_default, 31, 0);
         sf1 = BMTH_FIX_SIGNED_DIV(var1, var2, 17, 14, 31, 0, 17, 14) ;

         var1 = BMTH_FIX_SIGNED_ITOFIX(inter_coded_mb_cnt, 16, 15);
         var2 = BMTH_FIX_SIGNED_ITOFIX(total_mb_cnt, 31, 0);
         if ( var2 != 0 )
         {
            wf2 = BMTH_FIX_SIGNED_DIV(var1, var2, 16, 15, 31, 0, 16, 15) ;
         }
         else
         {
            wf2 = var1;
         }

         var1 = BMTH_FIX_SIGNED_ITOFIX((inter_low_sum + inter_hi_sum), 17, 14);
         var2 = BMTH_FIX_SIGNED_ITOFIX(inter_default, 31, 0);
         sf2 = BMTH_FIX_SIGNED_DIV(var1, var2, 17, 14, 31, 0, 17, 14) ;

         var2 = BMTH_FIX_SIGNED_MUL(wf1, avg_qp_cur, 16, 15, 26, 5, 11, 20);
         var2 = BMTH_FIX_SIGNED_CONVERT(var2, 11, 20, 21, 10);
         sf1 =  BMTH_FIX_SIGNED_CONVERT(sf1, 17, 14, 21, 10);
         var3 = BMTH_FIX_SIGNED_MUL(sf1, var2, 21, 10, 21, 10, 11, 20);

         var2 = BMTH_FIX_SIGNED_MUL(wf2, avg_qp_cur, 16, 15, 26, 5, 11, 20);
         var2 = BMTH_FIX_SIGNED_CONVERT(var2, 11, 20, 21, 10);
         sf2 =  BMTH_FIX_SIGNED_CONVERT(sf2, 17, 14, 21, 10);
         var4 = BMTH_FIX_SIGNED_MUL(sf2, var2, 21, 10, 21, 10, 11, 20);

         var1 = (wf1 + wf2);
         var2 = BMTH_FIX_SIGNED_ITOFIX(1, 16, 15);
         var2 = var2 - var1;
         var5 = BMTH_FIX_SIGNED_MUL(var2, act_qp_ref, 16, 15, 26, 5, 11, 20);

         var6 = var3 + var4 + var5;
         var6 = BMTH_FIX_SIGNED_CONVERT(var6, 11, 20, 15, 16);

         adj_Qp = var6;
      }

      /* adj_Qp = (float)(alpha * adj_Qp) + (float)((1 - alpha) * (hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_pre));
         hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_pre = adj_Qp;*/

      /* PR 36460 : XVD : DM in PI : Reset mechanism of DNR on  */
      /* closed GOP and /or video sequence header, is erroneous.*/
      /*                                                        */
      /* At the beginning of each video sequence or closed GOP  */
      /* boundary, old wrong code initializes these variables:  */
      /*          adj_Qp_pre = 0;                               */
      /*          adj_Qp_ref = 0;                               */
      /*                                                        */
      /* but the correct reset mechanism is :                   */
      /*                                                        */
      /* At the beginning of each video sequence or closed GOP  */
      /* boundary, initialize these variables:                  */
      /*       adj_Qp_pre = adj_Qp;                             */
      /*       adj_Qp_ref = 0;                                  */
      /*                                                        */
      if ( (pPPB->dnr.pic_flags & BXVD_P_PPB_DNR_FLAG_GOPHEADER) == BXVD_P_PPB_DNR_FLAG_GOPHEADER )
      {
         hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_pre = adj_Qp;
      }

      var1 = BMTH_FIX_SIGNED_MUL(alpha, adj_Qp, 27, 4, 15, 16, 11, 20);
      var2 = BMTH_FIX_SIGNED_CONVERT(1, 31, 0, 27, 4);

      var2 = var2-alpha;
      var2 = BMTH_FIX_SIGNED_MUL(hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_pre, var2, 15, 16, 27, 4, 11, 20);

      adj_Qp = var1 + var2;
      hXvdCh->stDecoderContext.stDNRInfo.adj_Qp_pre = BMTH_FIX_SIGNED_CONVERT(adj_Qp, 11, 20, 15, 16);
      adj_Qp = BMTH_FIX_SIGNED_FIXTOI(adj_Qp, 11, 20);

      pUnifiedPicture->stDigitalNoiseReduction.uiAdjustedQuantizationParameter = adj_Qp;
   }

   BDBG_LEAVE(BXVD_Decoder_S_ComputeDigitalNoiseReduction);
   return;
} /* end of BXVD_Decoder_S_ComputeDigitalNoiseReduction() */

/********************************************************************************
**
** Routines for managing the unified queue.
**
********************************************************************************/

static void BXVD_Decoder_S_UnifiedQ_DumpElement(
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext,
   bool bDependentPicture
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_DumpElement );

   if ( NULL != pstUnifiedContext )
   {
      if ( true == bDependentPicture )
      {
         BXVD_Decoder_S_Printf("\t", NULL );
      }

      BXVD_Decoder_S_Printf("pUniCntxt: 0x%x index %d pPPB: 0x%x\n",
                  pstUnifiedContext,
                  pstUnifiedContext->uiIndex,
                  pstUnifiedContext->pPPBPhysical
                  );

      if ( NULL != pstUnifiedContext->pstDependentUniContext )
      {
         BXVD_Decoder_S_UnifiedQ_DumpElement( pstUnifiedContext->pstDependentUniContext, true );
      }
   }

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_DumpElement );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_DumpElement */

static void BXVD_Decoder_S_UnifiedQ_Initialize(
   BXVD_ChannelHandle hXvdCh
   )
{
   BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue;

   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_Initialize );

   /*
    * Initialize the Unified Picture Queue
    */
   pstUnifiedQueue = &(hXvdCh->stDecoderContext.stUnifiedPictureQueue);

   BKNI_Memset( pstUnifiedQueue, 0, sizeof( BXVD_Decoder_P_UnifiedPictureQueue ) );

   pstUnifiedQueue->pstContextTail = &(pstUnifiedQueue->astUnifiedContext[0]);
   pstUnifiedQueue->pstContextHead = &(pstUnifiedQueue->astUnifiedContext[0]);

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_Initialize );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_Initialize */

static void BXVD_Decoder_S_UnifiedQ_InvalidateElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext
   )
{
   BDBG_ENTER(BXVD_Decoder_S_UnifiedQ_InvalidateElement);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pstUnifiedContext);

   hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumUsedElements--;

   /* The element count should never go below 0.
    * If it does, these is a bug in the logic.
    */
   BDBG_ASSERT(hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumUsedElements >= 0);

   pstUnifiedContext->bInUse = false;

   pstUnifiedContext->pstNextUniContext = NULL;
   pstUnifiedContext->pstPreviousUniContext = NULL;
   pstUnifiedContext->pstDependentUniContext = NULL;

   BDBG_LEAVE(BXVD_Decoder_S_UnifiedQ_InvalidateElement);

   return;
}

static void BXVD_Decoder_S_UnifiedQ_ParseSeiFramePackMsg(
   BXVD_P_SEI_FramePacking * pstAvdSEIData,
   BXDM_Picture_3D * pst3D
   )
{
   /* (SW7405-5135) The H264 specification was not completely rigorous in the definition of the
    * "arrangement_cancel" bit in the flags works.  We are assuming that if this bit is set, none 
    * of the other fields in the message are valid.   This then implies that the picture is a 2D 
    * picture.  The orientation will be set to "2D" in both the Unified and MFD picture structures.
    */
   if ( pstAvdSEIData->flags & BXVD_P_PPB_FLAG_SEI_FRAMEPACK_ARRANGEMENT_CANCEL )
   {
      pst3D->eOrientation = BXDM_Picture_Orientation_e2D;
      pst3D->eFrameRelationship = BXDM_Picture_FrameRelationship_eUnknown;
      pst3D->uiFrameNum = 0;
   }
   else 
   {
      /* Cancel bit is NOT set, parse the message and set the picture
       * orientation accordingly.
       */
      switch( pstAvdSEIData->frame_packing_arrangement_type )
      {
         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_CHECKER:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_Checker;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_COLUMN:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_Column;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_ROW:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_Row;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_SIDE_BY_SIDE:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_SideBySide;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_OVER_UNDER:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_TopBottom;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_TYPE_ALTERNATING:
            pst3D->eOrientation = BXDM_Picture_Orientation_e3D_FullFrame;
            break;

         default:
            pst3D->eOrientation = BXDM_Picture_Orientation_e2D;
            break;
      }

      switch( pstAvdSEIData->content_interpretation_type )
      {
         case BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_FRAME0_LEFT:
            pst3D->eFrameRelationship = BXDM_Picture_FrameRelationship_eFrame0Left;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_FRAME0_RIGHT:
            pst3D->eFrameRelationship = BXDM_Picture_FrameRelationship_eFrame0Right;
            break;

         case BXVD_P_PPB_SEI_FRAMEPACK_INTERPRET_UNSPECIFIED:
         default:
            pst3D->eFrameRelationship = BXDM_Picture_FrameRelationship_eUnknown;
            break;
      }

      if ( pstAvdSEIData->flags & BXVD_P_PPB_FLAG_SEI_FRAMEPACK_CURRENT_FRAME_IS_FRAME0 )
      {
         pst3D->uiFrameNum = 0;
      }
      else
      {
         pst3D->uiFrameNum = 1;
      }

   }     /* end else (cancel bit is not set ) */

   return;
   
}     /* end of BXVD_Decoder_S_UnifiedQ_ParseSeiFramePackMsg() */

static void BXVD_Decoder_S_UnifiedQ_ValidatePicture(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext * pstPictureCntxt,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext,
   bool bPictureDropped
   )
{
   bool     bMpegProtocol;
   uint32_t uiTemp;
   BXVD_P_PPB *   pPPB;
   BXDM_Picture * pstXdmPicture;

   /* SW7405-4365: local variables to improve performance. */
   uint32_t                   uiFlags;
   uint32_t                   uiFlagsExt0=0;
   BXVD_P_PPB_PullDown        ePulldown;
   BXVD_P_PPB_Protocol        eProtocol;
   BXVD_P_PPB_FrameRate       eFrameRate;

   BDBG_ENTER(BXVD_Decoder_S_UnifiedQ_ValidatePicture);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pstPictureCntxt);
   BDBG_ASSERT(pstUnifiedContext);

   pstXdmPicture = &(pstUnifiedContext->stUnifiedPicture);

   /* Reset the unified picture */
   BKNI_Memset( pstXdmPicture, 0 , sizeof( BXDM_Picture ) );

   /* Need to preserve the PPB address for use in BXVD_Decoder_ReleasePicture_isr
    * when releasing the picture.
    */
   BDBG_ASSERT(pstPictureCntxt->stPPB.pPPBPhysical);

   pstUnifiedContext->pPPBPhysical = pstPictureCntxt->stPPB.pPPBPhysical;
   pstUnifiedContext->pPPB = pstPictureCntxt->stPPB.pPPB;
   pstXdmPicture->pPrivateContext = pstUnifiedContext;

   BDBG_ASSERT(pstPictureCntxt->stPPB.pPPB);

   /* SW7405-4365: Originally this routine was taking 35-40 usec's.  Mapping
    * the PPB pointer to a cached address reduced this to ~20 usec's.  However when the
    * call to invalidate the cache was added, the execution time was ~26 usec's.
    * If the non-cached PPB address is used and the most frequently used PPB elements
    * are copied to local variables, the execution time is in the 28-30 usec's range.
    * While not the most aesthetic solution, going with the local variables avoids any
    * potential performance hit from invalidating the cache for every picture.
    */

   pPPB = pstPictureCntxt->stPPB.pPPB; 

   /* SW7405-4365: Copy the most frequently used PPB elements to local variables.
    */
   uiFlags     = pPPB->flags;
   ePulldown   = pPPB->pulldown;
   eProtocol   = pPPB->protocol;
   eFrameRate  = pPPB->frame_rate;

#if BXVD_P_PPB_EXTENDED
   uiFlagsExt0 = pPPB->flags_ext0;
#else
   uiFlagsExt0 = 0;  /* need to avoid a warning on some non-debug builds. */
#endif

#if BXVD_DECODER_AVD_DEBUG_HELP
   BKNI_Printf("%d,%d\n",
	       pPPB->il_perf_data.decode_cycle_count,
	       pPPB->il_perf_data.inst_cache_miss);
#endif
   /* SW7425-1064: handle of the channel that created the picture. 
    * Needed by XMO when going from 2D->3D and the channel change
    * mode is "hold last picture"
    */
   pstXdmPicture->hChannel = hXvdCh;

   /***************************/
   /* BXDM_Picture_BufferInfo */
   /***************************/

   /* If a pictureless PPB, the buffer info will not be valid. */
   pstXdmPicture->stBufferInfo.bValid = ( uiFlags & BXVD_P_PPB_FLAG_PICTURE_LESS_PPB ) ? false : true ;

   /* Set the picture heap */
   BDBG_ASSERT(hXvdCh->hPictureHeap);
   pstXdmPicture->stBufferInfo.hHeap = hXvdCh->hPictureHeap;

   /* pLuminanceFrameBufferAddress */
   BMEM_ConvertOffsetToAddress(
      pstXdmPicture->stBufferInfo.hHeap,
      pPPB->luma_video_address,
      &(pstXdmPicture->stBufferInfo.pLumaBufferVirtualAddress)
      );

   /* pChrominanceFrameBufferAddress */
   BMEM_ConvertOffsetToAddress(
      pstXdmPicture->stBufferInfo.hHeap,
      pPPB->chroma_video_address,
      &(pstXdmPicture->stBufferInfo.pChromaBufferVirtualAddress)
      );

   /* source size */
   pstXdmPicture->stBufferInfo.stSource.bValid     = true;
   pstXdmPicture->stBufferInfo.stSource.uiWidth    = pPPB->video_width;
   pstXdmPicture->stBufferInfo.stSource.uiHeight   = pPPB->video_height;

   /* display size
    * SWBLURAY-21461: handle MPEG and AVS in a similar manner as VC1.
    */
   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eVC1:
      case BXVD_P_PPB_Protocol_eVC1SimpleMain:
         pstXdmPicture->stBufferInfo.stDisplay.bValid    = true;
         pstXdmPicture->stBufferInfo.stDisplay.uiWidth   = pPPB->other.vc1.display_horizontal_size;
         pstXdmPicture->stBufferInfo.stDisplay.uiHeight  = pPPB->other.vc1.display_vertical_size;
         break;

      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG1:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         pstXdmPicture->stBufferInfo.stDisplay.bValid    = true;
         pstXdmPicture->stBufferInfo.stDisplay.uiWidth   = pPPB->other.mpeg.display_horizontal_size;
         pstXdmPicture->stBufferInfo.stDisplay.uiHeight  = pPPB->other.mpeg.display_vertical_size;
         break;

      case BXVD_P_PPB_Protocol_eAVS:
         pstXdmPicture->stBufferInfo.stDisplay.bValid    = true;
         pstXdmPicture->stBufferInfo.stDisplay.uiWidth   = pPPB->other.avs.display_horizontal_size;
         pstXdmPicture->stBufferInfo.stDisplay.uiHeight  = pPPB->other.avs.display_vertical_size;
         break;

      /* SWDTV-8681: add support for VP8 display size */
      case BXVD_P_PPB_Protocol_eVP8:
         pstXdmPicture->stBufferInfo.stDisplay.bValid    = true;
         pstXdmPicture->stBufferInfo.stDisplay.uiWidth   = pPPB->other.vp8.display_horizontal_size;
         pstXdmPicture->stBufferInfo.stDisplay.uiHeight  = pPPB->other.vp8.display_vertical_size;
         break;

      default:
         pstXdmPicture->stBufferInfo.stDisplay.bValid    = false;
         pstXdmPicture->stBufferInfo.stDisplay.uiWidth   = 0;
         pstXdmPicture->stBufferInfo.stDisplay.uiHeight  = 0;
         break;
   }

   /* BAVC_StripeWidth */

   /* SW7125-1281: range check for bad data. */
   if ( hXvdCh->pXvd->uiDecode_StripeWidth > BXVD_P_STRIPE_WIDTH_NUM )
   {
      BXVD_DBG_ERR(hXvdCh, ("hXvdCh->pXvd->uiDecode_StripeWidth of %d is out of range, defaulting to BAVC_StripeWidth_e64Byte",
                                       hXvdCh->pXvd->uiDecode_StripeWidth ));
      pstXdmPicture->stBufferInfo.eStripeWidth = BAVC_StripeWidth_e64Byte;
   }
   else
   {
      pstXdmPicture->stBufferInfo.eStripeWidth = sAVCStripeWidthLUT[hXvdCh->pXvd->uiDecode_StripeWidth];
   }

   /* Number of macroblocks size in the vertical direction
    * Should the shift happen here or in the output code?
    */
   pstXdmPicture->stBufferInfo.uiLumaStripeHeight =   pPPB->luma_stripe_height /* >> 4*/;
   pstXdmPicture->stBufferInfo.uiChromaStripeHeight = pPPB->chroma_stripe_height /* >> 4*/;

   /* Set the pulldown, start by range check.
    * (This is needed in case corrupted data is delivered.)
    * TODO: is the range checking appropriate?
    * TODO: is BXVD_DMTSM_S_ApplyCDTOverride handled in the Picture Provider?
    */
   if (( ePulldown > BXVD_P_PPB_PullDown_eFrameX4 )
        || ( ePulldown < BXVD_P_PPB_PullDown_eTop ) )
   {
      pstXdmPicture->stBufferInfo.ePulldown = BXVD_P_PPB_PullDown_eTopBottom;
   }
   else
   {
      pstXdmPicture->stBufferInfo.ePulldown = ePulldown;
   }

   /* SW7601-180: conditionally override TB->TBT and BT->BTB,
    * if
    * - the application specified "BXVD_MPEGPulldownOverride_eRFF"
    * - AND an MPEG stream
    * - AND the video decoder set "repeat_first_field"
    */
   bMpegProtocol = ( BXVD_P_PPB_Protocol_eMPEG2 == eProtocol );
   bMpegProtocol |= ( BXVD_P_PPB_Protocol_eMPEG1 == eProtocol );
   bMpegProtocol |= ( BXVD_P_PPB_Protocol_eMPEG2DTV == eProtocol );
   bMpegProtocol |= ( BXVD_P_PPB_Protocol_eMPEG2_DSS_PES == eProtocol );

   if ( BXVD_MPEGPulldownOverride_eRFF == hXvdCh->stDecoderContext.ePulldownOverride
         && true == bMpegProtocol )
   {
      if ( 0 != pPPB->other.mpeg.repeat_first_field )
      {
         switch( pstXdmPicture->stBufferInfo.ePulldown )
         {
         case BXVD_P_PPB_PullDown_eTopBottom:
            pstXdmPicture->stBufferInfo.ePulldown = BXVD_P_PPB_PullDown_eTopBottomTop;
            break;

         case BXVD_P_PPB_PullDown_eBottomTop:
            pstXdmPicture->stBufferInfo.ePulldown = BXVD_P_PPB_PullDown_eBottomTopBottom;
            break;

         default:
            break;
         }
      }
   }

   /* Set the chroma location based on the protocol. (PR50994)
    * - if:: AVC and the VUI (Video Usability Information) parameters are
    * valid, copy the appropriate value from the PPB.
    * - else if:: MPEG1, use "BAVC_MpegType_eMpeg1".
    * - else:: default to "BAVC_MpegType_eMpeg2".
    */
    if (( BXVD_P_PPB_Protocol_eH264 == eProtocol )
         && ( BXVD_P_PPB_H264_VALID_VUI & pPPB->other.h264.valid ))
   {
      /* If pulldown indicates
       * - only a bottom field, use the chroma bottom location
       * - an interlaced field, the Picture Provider will need to select the location based on the interrupt polarity
       * - only a top field or a frame, use the chroma top location
       */
      switch( pstXdmPicture->stBufferInfo.ePulldown )
      {
         case BXVD_P_PPB_PullDown_eBottom:
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].eMpegType = pPPB->other.h264.chroma_bottom;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].bValid = true;
            break;

         case BXVD_P_PPB_PullDown_eTop:
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].eMpegType = pPPB->other.h264.chroma_top;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].bValid = true;
            break;

         case BXVD_P_PPB_PullDown_eTopBottom:
         case BXVD_P_PPB_PullDown_eBottomTop:
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].eMpegType = pPPB->other.h264.chroma_bottom;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].bValid = true;

            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].eMpegType = pPPB->other.h264.chroma_top;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].bValid = true;

            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = pPPB->other.h264.chroma_top;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
            break;

         default:
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = pPPB->other.h264.chroma_top;
            pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
            break;
      }
   }
   else if ( BXVD_P_PPB_Protocol_eMPEG1 == eProtocol )
   {
      pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = BAVC_MpegType_eMpeg1;
      pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
   }
   else
   {
      pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = BAVC_MpegType_eMpeg2;
      pstXdmPicture->stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
   }

   /* eYCbCrType, always the same for AVD
    * TODO: make this configurable?
    */
   pstXdmPicture->stBufferInfo.eYCbCrType = BAVC_YCbCrType_e4_2_0;


   /* eSourceFormat: from the AVD PPB document
    * Bit 3:2
    * - Value 00 indicates Progressive Source format
    * - Value 01 indicates Interlace Source format
    * - Value 1x indicates Unknown Source format
    */
   uiTemp = ( uiFlags & BXVD_P_PPB_FLAG_EXTRACT_SOURCE_FORMAT_MASK ) >> BXVD_P_PPB_FLAG_EXTRACT_SOURCE_FORMAT_SHIFT;

   if ( 0 == uiTemp )
   {
      pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eProgressive;
   }
   else if ( 1 == uiTemp )
   {
      pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eInterlaced;
   }
   else
   {
      pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eUnknown;
   }

   /*************************/
   /* BXDM_Picture_Clipping */
   /*************************/
   if ( BXVD_P_PPB_Protocol_eH264 == eProtocol )
   {
      /* In H264, clipping information may be present, so we need to
       * tell VDC to read in the cropped region */
      if ( pPPB->other.h264.valid & BXVD_P_PPB_H264_VALID_SPS_CROP )
      {
         pstXdmPicture->stClipping.bValid = true;
         pstXdmPicture->stClipping.uiLeft = (pPPB->other.h264.sps_crop_horiz >> 16) & 0xFFFF;
         pstXdmPicture->stClipping.uiRight = pPPB->other.h264.sps_crop_horiz & 0xFFFF;
         pstXdmPicture->stClipping.uiTop = (pPPB->other.h264.sps_crop_vert >> 16) & 0xFFFF;
         pstXdmPicture->stClipping.uiBottom = pPPB->other.h264.sps_crop_vert & 0xFFFF;
      }
   }
   else if ( BXVD_P_PPB_Protocol_eRV9 == eProtocol )
   {
      /* SWDTV-7795: For RV9, clipping information will always be valid.  
       * Set the clipping information so that VDC will crop.
       */
      pstXdmPicture->stClipping.bValid = true;
      pstXdmPicture->stClipping.uiLeft = (pPPB->other.rv9.crop_horiz >> 16) & 0xFFFF;
      pstXdmPicture->stClipping.uiRight = pPPB->other.rv9.crop_horiz & 0xFFFF;
      pstXdmPicture->stClipping.uiTop = (pPPB->other.rv9.crop_vert >> 16) & 0xFFFF;
      pstXdmPicture->stClipping.uiBottom = pPPB->other.rv9.crop_vert & 0xFFFF;
   }



   /*************************/
   /* BXDM_Picture_Protocol */
   /*************************/
   pstXdmPicture->stProtocol.eProtocol = eProtocol;
   pstXdmPicture->stProtocol.eLevel = (( pPPB->profile_level & BXVD_P_PPB_PROTOCOL_LEVEL_MASK ) >> BXVD_P_PPB_PROTOCOL_LEVEL_SHIFT );
   pstXdmPicture->stProtocol.eProfile = (( pPPB->profile_level & BXVD_P_PPB_PROTOCOL_PROFILE_MASK ) >> BXVD_P_PPB_PROTOCOL_PROFILE_SHIFT );

   /*********************/
   /* BXDM_Picture_Type */
   /*********************/
   switch ( uiFlags & BXVD_P_PPB_FLAG_PICTURE_TYPE_MASK )
   {
      case BXVD_P_PPB_FLAG_I_PICTURE:
         pstXdmPicture->stPictureType.eCoding = BXDM_Picture_Coding_eI;
         break;

      case BXVD_P_PPB_FLAG_P_PICTURE:
         pstXdmPicture->stPictureType.eCoding = BXDM_Picture_Coding_eP;
         break;

      case BXVD_P_PPB_FLAG_B_PICTURE:
         pstXdmPicture->stPictureType.eCoding = BXDM_Picture_Coding_eB;
         break;

      default:
         pstXdmPicture->stPictureType.eCoding = BXDM_Picture_Coding_eUnknown;
   }

   if ( uiFlags & BXVD_P_PPB_FLAG_PROG_SEQUENCE )
   {
      pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eProgressive;
   }
   else
   {
      pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eInterlaced;
   }

   if (uiFlags & BXVD_P_PPB_FLAG_REF_PICTURE)
   {
      pstXdmPicture->stPictureType.bReference = true;
   }

   if (uiFlags & BXVD_P_PPB_FLAG_RAP_PICTURE)
   {
      pstXdmPicture->stPictureType.bRandomAccessPoint = true;
   }

   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG1:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         pstXdmPicture->stPictureType.bLowDelay = (( pPPB->other.mpeg.low_delay_video_format >> 16 ) & 0x3 ) ? true : false;
         break;

      default:
         pstXdmPicture->stPictureType.bLowDelay = false;
         break;
   }

#if BXVD_P_PPB_EXTENDED

   /* SW7425-1001: effectively an EOS flag.  Currently defined to 
    * only be delivered with a "picture-less" PPB. 
    */
   if ( uiFlagsExt0 & BXVD_P_PPB_EXT0_FLAG_LAST_PICTURE )
   {
      if ( uiFlags & BXVD_P_PPB_FLAG_PICTURE_LESS_PPB ) 
      {
         pstXdmPicture->stPictureType.bLastPicture = true;
      }
      else
      {
         BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_ValidatePicture:: BXVD_P_PPB_EXT0_FLAG_LAST_PICTURE set for a standard PPB"));
      }
   }
#endif

   /**********************/
   /* BXDM_Picture_Error */
   /**********************/
   if( uiFlags & BXVD_P_PPB_FLAG_DECODE_ERROR )
   {
      pstXdmPicture->stError.bThisPicture = true;
   }

   if( uiFlags & BXVD_P_PPB_FLAG_DECODE_REF_ERROR )
   {
      pstXdmPicture->stError.bPreviousRefPic = true;
   }

   /* If the decoder input has overflowed, bump the count. */

   if ( uiFlags & BXVD_P_PPB_FLAG_INPUT_OVERFLOW )
   {
      hXvdCh->stDecoderContext.stCounters.uiDecoderInputOverflow++;
   }


   /**********************************/
   /* BXDM_Picture_PictureOrderCount */
   /**********************************/
   if ( BXVD_P_PPB_Protocol_eH264 == eProtocol )
   {
      int32_t iPocBottom   = pPPB->other.h264.poc_bottom;
      int32_t iPocTop      = pPPB->other.h264.poc_top;

      pstXdmPicture->stPOC.uiPictureId = pPPB->other.h264.idr_pic_id;
      pstXdmPicture->stPOC.bValid = true;

      if ( iPocTop < iPocBottom )
      {
         pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].iValue = iPocTop;
      }
      else
      {
         pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].iValue = iPocBottom;
      }
      pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].bValid = true;

      pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eTopField].iValue = iPocTop;
      pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eTopField].bValid = true;

      pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eBotField].iValue = iPocBottom;
      pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eBotField].bValid = true;
   }
   else if ( BXVD_P_PPB_Protocol_eSVC == eProtocol )
   {
       int32_t iPocBottom   = pPPB->other.svc.poc_bottom;
       int32_t iPocTop      = pPPB->other.svc.poc_top;

       pstXdmPicture->stPOC.uiPictureId = pPPB->other.svc.idr_pic_id;
       pstXdmPicture->stPOC.bValid = true;

       if ( iPocTop < iPocBottom )
       {
          pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].iValue = iPocTop;
       }
       else
       {
          pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].iValue = iPocBottom;
       }
       pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eFrame].bValid = true;

       pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eTopField].iValue = iPocTop;
       pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eTopField].bValid = true;

       pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eBotField].iValue = iPocBottom;
       pstXdmPicture->stPOC.stPictureOrderCount[BAVC_Polarity_eBotField].bValid = true;
   }


   /**************************/
   /* BXDM_Picture_FrameRate */
   /**************************/
   if ( eFrameRate == BXVD_P_PPB_FrameRate_eUnknown
         || eFrameRate >= BXVD_P_PPB_FrameRate_eMax
      )
   {
      pstXdmPicture->stFrameRate.bValid = true;

      pstXdmPicture->stFrameRate.stRate.uiNumerator = 0;
      pstXdmPicture->stFrameRate.stRate.uiDenominator = 0;
   }
   else
   {
      pstXdmPicture->stFrameRate.bValid = true;

      pstXdmPicture->stFrameRate.stRate = sFrameRateEnumToIntLUT[ eFrameRate ];
   }

   /* SW3556-836: dereference the fixed_frame_rate_flag */
   pstXdmPicture->stFrameRate.eType = ( uiFlags & BXVD_P_PPB_FLAG_FIXED_FRAME_RATE ) ? BXDM_Picture_FrameRateType_eFixed : BXDM_Picture_FrameRateType_eUnknown;

   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG1:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         /* frame_rate_extension:: [6:5]=frame_rate_extn_n, [4:0]=frame_rate_extn_d */
         pstXdmPicture->stFrameRate.stExtension.uiNumerator = (( pPPB->other.mpeg.frame_rate_extension >> 5 ) & 0x3 );
         pstXdmPicture->stFrameRate.stExtension.uiDenominator = ( pPPB->other.mpeg.frame_rate_extension & 0x1F );
         break;

      default:
         pstXdmPicture->stFrameRate.stExtension.uiNumerator = 0;
         pstXdmPicture->stFrameRate.stExtension.uiDenominator = 0;
         break;
   }

   /********************/
   /* BXDM_Picture_PTS */
   /********************/
   if ( uiFlags & BXVD_P_PPB_FLAG_PTS_PRESENT )
   {
      pstXdmPicture->stPTS.bValid = true;
      pstXdmPicture->stPTS.uiValue = pPPB->pts;

      /* "n_drop" is a running tally from AVD of the number of pictures dropped
       * in response to a request from the Picture Provider to drop pictures.
       * The "pending drop count" is the difference between the requested number
       * and "n_drop".  While the "pending drop count" is non-zero, the Picture
       * Provider will run in VSYNC mode.  When it does become zero, the PP will
       * switch to TSM mode.  For this reason, we want to hold off reporting the
       * "pending drop count" as zero until a coded PTS has been received.  Hence
       * this logic to only sample "n_drop" for pictures with a coded PTS.
       */
      if ( BXVD_Decoder_P_PictureSet_eDependent != pstPictureCntxt->eSetType )
      {
         hXvdCh->stDecoderContext.uiDropCountSnapshot = pPPB->n_drop;
      }
   }

   /* The drop count for this picture is the one that came with the last coded PTS.
    */
   pstUnifiedContext->uiDropCount = hXvdCh->stDecoderContext.uiDropCountSnapshot;

   /**************************/
   /* BXDM_Picture_PCROffset */
   /**************************/
   if ( uiFlags & BXVD_P_PPB_FLAG_PCR_OFFSET_PRESENT )
   {
      pstXdmPicture->stPCROffset.bValid = true;
      pstXdmPicture->stPCROffset.uiValue = pPPB->pcr_offset;
   }

   if ( uiFlags & BXVD_P_PPB_FLAG_DISCONT_PCR_OFFSET )
   {
      pstXdmPicture->stPCROffset.bDiscontinuity = true;
   }

   /****************************/
   /* BXDM_Picture_GopTimeCode */
   /****************************/
   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG1:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         pstXdmPicture->stGOPTimeCode.uiHours = ( pPPB->other.mpeg.gop_time_code & BXVD_P_PPB_MPEG_GOP_HOUR_MASK ) >> BXVD_P_PPB_MPEG_GOP_HOUR_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiMinutes = ( pPPB->other.mpeg.gop_time_code & BXVD_P_PPB_MPEG_GOP_MINUTE_MASK ) >> BXVD_P_PPB_MPEG_GOP_MINUTE_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiSeconds = ( pPPB->other.mpeg.gop_time_code & BXVD_P_PPB_MPEG_GOP_SECOND_MASK ) >> BXVD_P_PPB_MPEG_GOP_SECOND_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiPictures = ( pPPB->other.mpeg.gop_time_code & BXVD_P_PPB_MPEG_GOP_PICTURE_MASK );
         pstXdmPicture->stGOPTimeCode.bValid = true;
         break;

      /* SW7401-4426: Extract the H264 time code. */
      case BXVD_P_PPB_Protocol_eH264:
         pstXdmPicture->stGOPTimeCode.uiHours = ( pPPB->time_code & BXVD_P_PPB_H264_GOP_HOUR_MASK ) >> BXVD_P_PPB_H264_GOP_HOUR_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiMinutes = ( pPPB->time_code & BXVD_P_PPB_H264_GOP_MINUTE_MASK ) >> BXVD_P_PPB_H264_GOP_MINUTE_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiSeconds = ( pPPB->time_code & BXVD_P_PPB_H264_GOP_SECOND_MASK ) >> BXVD_P_PPB_H264_GOP_SECOND_SHIFT;
         pstXdmPicture->stGOPTimeCode.uiPictures = ( pPPB->time_code & BXVD_P_PPB_H264_GOP_PICTURE_MASK );
         pstXdmPicture->stGOPTimeCode.bValid = true;
         break;

      default:
         pstXdmPicture->stGOPTimeCode.bValid = false;
         break;
   }

   /********************/
   /* BXDM_Picture_Tag */
   /********************/
   if ( uiFlags & BXVD_P_PPB_FLAG_PIC_TAG_PRESENT )
   {
      pstXdmPicture->stPictureTag.uiValue = pPPB->picture_tag;
      pstXdmPicture->stPictureTag.bValid = true;

      /* SW7425-3358: snap the picture_tag to GOP boundaries to
       * create the chunk Id.  Needed due to open GOP B's.
       */
      if ( uiFlags & BXVD_P_PPB_FLAG_RAP_PICTURE )
      {
         hXvdCh->stDecoderContext.uiCurrentChunkId = pPPB->picture_tag;
      }
   }

   /* Copy the saved chunkId to the Unified Picture. */
   pstXdmPicture->uiChunkId = hXvdCh->stDecoderContext.uiCurrentChunkId;

   if ( uiFlags & BXVD_P_PPB_NEW_PIC_TAG_AVAIL )
   {
      pstXdmPicture->stPictureTag.bNewTagAvailable = true;
   }

   /***********************/
   /* BXDM_Picture_Marker */
   /***********************/
   pstXdmPicture->stPictureMarker.uiValue = pPPB->timing_marker;
   pstXdmPicture->stPictureMarker.bValid = true;

   /*************************/
   /* BXDM_Picture_Overscan */
   /*************************/
   /* PR54677: Expose the overscan PPB flags to the app */
   pstXdmPicture->stOverscan.bValid = ( uiFlags & BXVD_P_PPB_OVERSCAN_FLAG ) ? true : false;
   pstXdmPicture->stOverscan.bOverscanAppropriate = ( uiFlags & BXVD_P_PPB_OVERSCAN_APPROPRIATE_FLAG ) ? true : false;

   /****************************/
   /* BXDM_Picture_DisplayInfo */
   /****************************/
   pstXdmPicture->stDisplayInfo.bValid = true;
   {
      uint8_t uiMatrixCoefficients, uiColorPrimaries, uiTransferCharacteristics;

      uiMatrixCoefficients = ((pPPB->display_info >> 24) & 0xFF);
      uiColorPrimaries = ((pPPB->display_info >> 16) & 0xFF);
      uiTransferCharacteristics = ((pPPB->display_info >> 8) & 0xFF);

      /* Set Matrix Coefficients */
      if ( uiMatrixCoefficients >= BXVD_DECODER_S_MATRIX_COEFFICIENTS_LUT_SIZE )
      {
         pstXdmPicture->stDisplayInfo.eMatrixCoefficients = BAVC_MatrixCoefficients_eUnknown;
         BXVD_DBG_MSG(hXvdCh, ("Unknown matrix coefficients: %#x", uiMatrixCoefficients));
      }
      else
      {
         pstXdmPicture->stDisplayInfo.eMatrixCoefficients = sMatrixCoefficientsLUT[uiMatrixCoefficients];
      }

      /* Set Color Primaries */
      if ( uiColorPrimaries >= BXVD_DECODER_S_COLOR_PRIMARIES_LUT_SIZE )
      {
         pstXdmPicture->stDisplayInfo.eColorPrimaries = BAVC_ColorPrimaries_eUnknown;
         BXVD_DBG_MSG(hXvdCh,("Unknown color primaries: %#x", uiColorPrimaries));
      }
      else
      {
         pstXdmPicture->stDisplayInfo.eColorPrimaries = sColorPrimariesLUT[uiColorPrimaries];
      }

      /* Set Transfer Characteristics */
      if ( uiTransferCharacteristics >= BXVD_DECODER_S_TRANSFER_CHARACTERISICS_LUT_SIZE )
      {
         pstXdmPicture->stDisplayInfo.eTransferCharacteristics = BAVC_TransferCharacteristics_eUnknown;
         BXVD_DBG_MSG(hXvdCh,("Unknown transfer characterisics: %#x", uiTransferCharacteristics));
      }
      else
      {
         pstXdmPicture->stDisplayInfo.eTransferCharacteristics = sTransferCharacteristicsLUT[uiTransferCharacteristics];
      }
   }

   switch ( eProtocol )
   {
       case BXVD_P_PPB_Protocol_eMPEG2:
       case BXVD_P_PPB_Protocol_eMPEG1:
       case BXVD_P_PPB_Protocol_eMPEG2DTV:
       case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
         /* low_delay_video_format:: [17:16]=low_delay, [2:0]=video_format */
         pstXdmPicture->stDisplayInfo.eVideoFormat = ( pPPB->other.mpeg.low_delay_video_format & 0x7 );
         break;

       default:
          pstXdmPicture->stDisplayInfo.eVideoFormat = BXDM_Picture_VideoFormat_eUnknown;
          break;
   }

   /************************/
   /* BXDM_Picture_PanScan */
   /************************/
   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eMPEG2:
      case BXVD_P_PPB_Protocol_eMPEG1:
      case BXVD_P_PPB_Protocol_eMPEG2DTV:
      case BXVD_P_PPB_Protocol_eMPEG2_DSS_PES:
      {
         if ( pPPB->other.mpeg.offset_count > 0 )
         {
            uint32_t uiPanScanIndex;

            pstXdmPicture->stPanScan.uiCount = pPPB->other.mpeg.offset_count;

            /* SW7125-1281: range check for bad data. */
            if ( pstXdmPicture->stPanScan.uiCount > BXDM_PICTURE_MAX_PAN_SCAN_VECTOR )
            {
               pstXdmPicture->stPanScan.uiCount = BXDM_PICTURE_MAX_PAN_SCAN_VECTOR;
            }

            for ( uiPanScanIndex = 0; uiPanScanIndex < pstXdmPicture->stPanScan.uiCount; uiPanScanIndex++ )
            {
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].eType = BXDM_Picture_PanScanVectorType_eSourceWindow;
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iHorizontal = pPPB->other.mpeg.horizontal_offset[uiPanScanIndex];
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iVertical = pPPB->other.mpeg.vertical_offset[uiPanScanIndex];
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiWidth = pPPB->other.mpeg.display_horizontal_size;
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiHeight = pPPB->other.mpeg.display_vertical_size;
            }
         }
      }
      break;

      case BXVD_P_PPB_Protocol_eH264:
      {
         if ( pPPB->other.h264.valid & BXVD_P_PPB_H264_VALID_PANSCAN )
         {
            int16_t iLeftOffset, iRightOffset, iTopOffset, iBottomOffset;

            /* Determine pan-scan index. According to H264 spec, the 3
             * sets of pan-scan values are used depending on which field
             * of the frame is displayed. When
             * BXVD_P_PPB.other.BXVD_P_PPB_H264.pan_scan_count is 1, then
             * the one set of values is valid for all fields displayed of
             * the current frame. Otherwise, the index of the values map
             * to the field that is displayed. In 3:2 pull-down cases,
             * there should be 3 values for frames which are displayed
             * for 3 fields. */
            if ( pPPB->other.h264.pan_scan_count > 0 )
            {
               uint32_t uiPanScanIndex;

               pstXdmPicture->stPanScan.uiCount = pPPB->other.h264.pan_scan_count;

               /* SW7125-1281: range check for bad data. */
               if ( pstXdmPicture->stPanScan.uiCount > BXDM_PICTURE_MAX_PAN_SCAN_VECTOR )
               {
                  pstXdmPicture->stPanScan.uiCount = BXDM_PICTURE_MAX_PAN_SCAN_VECTOR;
               }

               for ( uiPanScanIndex = 0; uiPanScanIndex < pstXdmPicture->stPanScan.uiCount; uiPanScanIndex++ )
               {
                  iLeftOffset = (int16_t) ((pPPB->other.h264.pan_scan_horiz[uiPanScanIndex] >> 16) & 0xFFFF);
                  iRightOffset = (int16_t) (pPPB->other.h264.pan_scan_horiz[uiPanScanIndex] & 0xFFFF);
                  iTopOffset = (int16_t) ((pPPB->other.h264.pan_scan_vert[uiPanScanIndex] >> 16) & 0xFFFF);
                  iBottomOffset = (int16_t) (pPPB->other.h264.pan_scan_vert[uiPanScanIndex] & 0xFFFF);
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].eType = BXDM_Picture_PanScanVectorType_eSourceCrop;
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iHorizontal = -(iLeftOffset + iRightOffset)/2;
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iVertical = -(iTopOffset + iBottomOffset)/2;
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiWidth = ((iLeftOffset - iRightOffset) >> 4);
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiHeight = ((iTopOffset - iBottomOffset) >> 4);
               }
            }
         }
      }
      break;

      case BXVD_P_PPB_Protocol_eVC1:
      case BXVD_P_PPB_Protocol_eVC1SimpleMain:
      {
         if ( uiFlags & BXVD_P_PPB_VC1_VALID_PANSCAN )
         {
            int32_t iDisplayHorizontalCenterSP, iDisplayVerticalCenterSP, iPanScanHorizontalCenterSP, iPanScanVerticalCenterSP;
            uint32_t uiPanScanWidth, uiPanScanHeight;

            /* Determine pan-scan index. According to VC1 spec, the 4
             * sets of pan-scan values are used depending on which field
             * of the frame is displayed. When
             * BXVD_P_PPB.other.BXVD_P_PPB_VC1.num_panscan_windows is 1,
             * then the one set of values is valid for all fields
             * displayed of the current frame. Otherwise, the index of
             * the values map to the field that is displayed. In 3:2
             * pull-down cases, there should be 3 values for frames which
             * are displayed for 3 fields. */
            if ( pPPB->other.vc1.num_panscan_windows > 0 )
            {
               uint32_t uiPanScanIndex;

               pstXdmPicture->stPanScan.uiCount = pPPB->other.vc1.num_panscan_windows;

               /* SW7125-1281: range check for bad data. */
               if ( pstXdmPicture->stPanScan.uiCount > BXDM_PICTURE_MAX_PAN_SCAN_VECTOR )
               {
                  pstXdmPicture->stPanScan.uiCount = BXDM_PICTURE_MAX_PAN_SCAN_VECTOR;
               }

               for ( uiPanScanIndex = 0; uiPanScanIndex < pstXdmPicture->stPanScan.uiCount; uiPanScanIndex++ )
               {
                  /* Note: VC1 pan scan values are relative to the
                   * _display_ size and NOT the _source_ size.  VDC
                   * requires the latter, so we need to scale the
                   * values accordingly.
                   *
                   * The offsets define the upper left corner of the
                   * pan scan rectange, and NOT the offset from
                   * center, so we need to calculate the center offset
                   * first.
                   *
                   * The offsets are in sub pixel precision but the
                   * dimensions are in pixel precision, so we need to
                   * convert accordingly.
                  */

                  /* Calculate center coordinates relative to the
                   * display region using 1/16 sub pixel prevision.
                   * We need to multiply by 16 to get sub-pixel
                   * precision and then divide by 2 to get the center,
                   * hence the resulting left shift of 3 */
                  iDisplayHorizontalCenterSP = pPPB->other.vc1.display_horizontal_size << 3;
                  iDisplayVerticalCenterSP = pPPB->other.vc1.display_vertical_size << 3;
                  uiPanScanWidth = (pPPB->other.vc1.ps_horiz_offset_width[uiPanScanIndex] & 0x3FFF);
                  uiPanScanHeight = (pPPB->other.vc1.ps_vert_offset_height[uiPanScanIndex] & 0x3FFF);
                  iPanScanHorizontalCenterSP = ((pPPB->other.vc1.ps_horiz_offset_width[uiPanScanIndex] >> 14) & 0x3FFFF) + (uiPanScanWidth << 3);
                  iPanScanVerticalCenterSP = ((pPPB->other.vc1.ps_vert_offset_height[uiPanScanIndex] >> 14) & 0x3FFFF) + (uiPanScanHeight << 3);

                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].eType = BXDM_Picture_PanScanVectorType_eDisplayWindow;
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iHorizontal = (iPanScanHorizontalCenterSP - iDisplayHorizontalCenterSP);
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iVertical = (iPanScanVerticalCenterSP - iDisplayVerticalCenterSP);
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiWidth = uiPanScanWidth;
                  pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiHeight = uiPanScanHeight;
               }
            }
         }
      }
      break;

      case BXVD_P_PPB_Protocol_eAVS:
      {
         if ( pPPB->other.avs.offset_count > 0 )
         {
            uint32_t uiPanScanIndex;

            pstXdmPicture->stPanScan.uiCount = pPPB->other.avs.offset_count;

            /* SW7125-1281: range check for bad data. */
            if ( pstXdmPicture->stPanScan.uiCount > BXDM_PICTURE_MAX_PAN_SCAN_VECTOR )
            {
               pstXdmPicture->stPanScan.uiCount = BXDM_PICTURE_MAX_PAN_SCAN_VECTOR;
            }

            for ( uiPanScanIndex = 0; uiPanScanIndex < pstXdmPicture->stPanScan.uiCount; uiPanScanIndex++ )
            {
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].eType = BXDM_Picture_PanScanVectorType_eSourceWindow;
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iHorizontal = pPPB->other.avs.horizontal_offset[uiPanScanIndex];
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].iVertical = pPPB->other.avs.vertical_offset[uiPanScanIndex];
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiWidth = pPPB->other.avs.display_horizontal_size;
               pstXdmPicture->stPanScan.stVector[uiPanScanIndex].uiHeight = pPPB->other.avs.display_vertical_size;
            }
         }
      }
      break;

      default:
         break;
   }

   /****************************/
   /* BXDM_Picture_AspectRatio */
   /****************************/
   BXVD_Decoder_S_ComputeAspectRatio(
            pPPB,
            pstXdmPicture,
            eProtocol );

   /**************************************/
   /* BXDM_Picture_DigitalNoiseReduction */
   /**************************************/
   BXVD_Decoder_S_ComputeDigitalNoiseReduction(
            hXvdCh,
            pPPB,
            pstXdmPicture,
            eProtocol );

   /* BXDM_Picture_RangeRemapping */
   switch ( eProtocol )
   {
      case BXVD_P_PPB_Protocol_eVC1:
      case BXVD_P_PPB_Protocol_eVC1SimpleMain:
         pstXdmPicture->stRangeRemapping.bValid = true;
         pstXdmPicture->stRangeRemapping.uiLuma = (pPPB->other.vc1.range_remapping_ratio >> 16) & 0xFFFF;
         pstXdmPicture->stRangeRemapping.uiChroma = pPPB->other.vc1.range_remapping_ratio & 0xFFFF;
         break;

      default:
         pstXdmPicture->stRangeRemapping.bValid = false;
         break;
   }

   /****************************************/
   /* BXDM_Picture_ActiveFormatDescription */
   /****************************************/
   if ( uiFlags & BXVD_P_PPB_FLAG_AFD_VALID )
   {
      pstXdmPicture->stActiveFormatDescription.bValid = true;
      pstXdmPicture->stActiveFormatDescription.uiValue = pPPB->afd_val;
   }
   else
   {
      pstXdmPicture->stActiveFormatDescription.bValid = false;
   }

   /************************/
   /* BXDM_Picture_BarData */
   /************************/

   /* SW7425-2247: support for bar data */
   if ( pPPB->bar_data & BXVD_P_PPB_BAR_DATA_FLAG_VALID )
   {
      pstXdmPicture->stBarData.eBarDataType = 
         ( pPPB->bar_data & BXVD_P_PPB_BAR_DATA_FLAG_TOP_BOTTOM ) ? BAVC_BarDataType_eTopBottom : BAVC_BarDataType_eLeftRight ;

      pstXdmPicture->stBarData.uiTopLeftBarValue  = ( pPPB->bar_data & BXVD_P_PPB_BAR_DATA_TOP_LEFT_MASK ) >> BXVD_P_PPB_BAR_DATA_TOP_LEFT_SHIFT ;
      pstXdmPicture->stBarData.uiBotRightBarValue = ( pPPB->bar_data & BXVD_P_PPB_BAR_DATA_BOT_RIGHT_MASK ) >> BXVD_P_PPB_BAR_DATA_BOT_RIGHT_SHIFT ;
   }
   else
   {
      pstXdmPicture->stBarData.eBarDataType = BAVC_BarDataType_eInvalid;
   }

   /**********************/
   /* BXDM_Picture_Stats */
   /**********************/
   pstXdmPicture->stStats.stMacroBlockCount.bValid = true;
   pstXdmPicture->stStats.stMacroBlockCount.uiInterCoded = ( pPPB->dnr.mb_count_intra_inter >> 16 );
   pstXdmPicture->stStats.stMacroBlockCount.uiIntraCoded = ( pPPB->dnr.mb_count_intra_inter & 0xffff );
   pstXdmPicture->stStats.stMacroBlockCount.uiTotal = pPPB->dnr.num_mb_cur;

   if ( BXVD_P_PPB_Protocol_eMPEG2 == eProtocol
        || BXVD_P_PPB_Protocol_eMPEG1 == eProtocol
        || BXVD_P_PPB_Protocol_eMPEG2DTV == eProtocol
        || BXVD_P_PPB_Protocol_eMPEG2_DSS_PES  == eProtocol
       )
   {
      pstXdmPicture->stStats.uiBitRate = pPPB->other.mpeg.bit_rate_value;
   }

   pstXdmPicture->stStats.uiDeltaPicturesSeen = pstUnifiedContext->pPPB->delta_pic_seen;

   /* Use a pre-increment so that the serial numbers start at '1' instead of '0'. */
   pstXdmPicture->uiSerialNumber = ++hXvdCh->uiPPBSerialNumber;

   /************/
   /* CRC Mode */
   /************/
   if ( true == hXvdCh->stDecoderContext.bCRCMode )
   {
      switch ( uiFlags & BXVD_P_PPB_FLAG_BUFFER_TYPE_MASK)
      {
         case BXVD_P_PPB_FLAG_FRAME:
            pstXdmPicture->stBufferInfo.ePulldown = BXDM_Picture_PullDown_eFrameX1;
            pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eProgressive;
            pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eProgressive;
            break;

         case BXVD_P_PPB_FLAG_FIELDPAIR:
            pstXdmPicture->stBufferInfo.ePulldown = BXDM_Picture_PullDown_eTopBottom;
            pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eInterlaced;
            pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eInterlaced;
            break;

         case BXVD_P_PPB_FLAG_TOPFIELD:
            pstXdmPicture->stBufferInfo.ePulldown = BXDM_Picture_PullDown_eTop;
            pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eInterlaced;
            pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eInterlaced;
            break;

         case BXVD_P_PPB_FLAG_BOTTOMFIELD:
            pstXdmPicture->stBufferInfo.ePulldown = BXDM_Picture_PullDown_eBottom;
            pstXdmPicture->stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eInterlaced;
            pstXdmPicture->stPictureType.eSequence = BXDM_Picture_Sequence_eInterlaced;
            break;
      }
   }

   /* Linked list of dependent unified pictures
    * TODO: add error checking for type and pointers?
    * That was the motivation for saving the settype and count.
    */
   pstUnifiedContext->eSetType = pstPictureCntxt->eSetType;
   pstUnifiedContext->uiSetCount = pstPictureCntxt->uiSetCount;

   /* validate any linked dependent pictures.
    */
   if ( NULL != pstPictureCntxt->pDependentPicture )
   {
      BXVD_Decoder_S_UnifiedQ_AddPicture( hXvdCh, pstPictureCntxt->pDependentPicture, false );
   }

#if 0
   /* for debug */
   pstUnifiedContext->eSetType = BXVD_Decoder_P_PictureSet_eSingle;
   pstUnifiedContext->uiSetCount = 1;
#endif

   /* link the dependent unified picture to the preceeding picture
    * Currently we only support picture sets of 2, the preceeding
    * picture should be a base picture.
    *
    * SW7425-609: With a MVC stream, it is possible to startup on the dependent 
    * (enhanced) picture.   This is an error condition and the picture will be dropped.  
    * The twist here is that preceding picture context will not have been validated, 
    * i.e. pstPreviousUniContext will be NULL.  Why is this routine being executed 
    * if the picture is going to be dropped?  For debug, solely to fill in the 
    * Unified Picture structure and then call BXVD_DecoderDbg_P_PrintUnifiedPicture.  
    * To avoid a crash if pstPreviousUniContext is NULL, check " bPictureDropped" 
    * in the following "if" statement.
    */
   if ( BXVD_Decoder_P_PictureSet_eDependent == pstUnifiedContext->eSetType
         && false == bPictureDropped
      )
   {
      /* Link the unified pictures for use by XDM... */
      pstUnifiedContext->pstPreviousUniContext->stUnifiedPicture.pNextPicture = pstXdmPicture;

      /* Link the picture context's for use by the decoder. */
      pstUnifiedContext->pstPreviousUniContext->pstDependentUniContext = pstUnifiedContext;

      /* The error handling logic only looks at the base picture.  Conditionally
       * set the error bits on the base picture.
       */
      if( true == pstXdmPicture->stError.bThisPicture )
      {
          pstUnifiedContext->pstPreviousUniContext->stUnifiedPicture.stError.bThisPicture = true;
      }

      if( true == pstXdmPicture->stError.bPreviousRefPic )
      {
         pstUnifiedContext->pstPreviousUniContext->stUnifiedPicture.stError.bPreviousRefPic = true;
      }

      /* Link the dependent extension data to the base.  This will allow an application
       * to access both blocks of data in the ePictureExtensionData callback.
       */
      pstUnifiedContext->pstPreviousUniContext->stUnifiedPicture.stExtensionInfo.pNext = 
                                                &(pstUnifiedContext->stUnifiedPicture.stExtensionInfo);
      
   }

   /******************************/
   /* BXDM_Picture_ExtensionInfo */
   /* BXDM_Picture_3D            */
   /******************************/

   /* default to a 2D picture. */
   pstXdmPicture->st3D.eOrientation = BXDM_Picture_Orientation_e2D;
   pstXdmPicture->st3D.eFrameRelationship = BXDM_Picture_FrameRelationship_eUnknown;


   /* SW7405-3996: add support for SEI frame packing. */
   if ( BXVD_P_PPB_Protocol_eH264 == eProtocol
         || BXVD_P_PPB_Protocol_eMVC == eProtocol
      )
   {
      uint32_t             uiSEIOffset=0;
      BXVD_P_SEI_Message * pSEIMessage;
      uint32_t             uiSerialNumber = pstXdmPicture->uiSerialNumber; /* For debug. */

      if ( BXVD_P_PPB_Protocol_eH264 == eProtocol )
      {
         /* Check for an SEI message on a single picture. */

         uiSEIOffset = (uint32_t)pPPB->other.h264.pstSEIMessageOffset;
      }
      else if ( BXVD_P_PPB_Protocol_eMVC == eProtocol
                && BXVD_Decoder_P_PictureSet_eBase == pstUnifiedContext->eSetType
                && NULL != pstPictureCntxt->pDependentPicture
               )
      {
         /* This case is for retrieving graphics offsets which might be delivered in an MVC stream.
          * The offsets are delivered by AVD with the dependent (or extended) pictures.
          * The middleware is expecting the offsets to be bound to the base pictures.
          * We hold off parsing these messages until this routine is finishing up with the base picture.
          * Note: this routine is called recursively.  The preceding call to "BXVD_Decoder_S_UnifiedQ_AddPicture" 
          * will have processed the dependent/extended picture.
          */
         uiSEIOffset = (uint32_t)pstPictureCntxt->pDependentPicture->stPPB.pPPB->other.h264.pstSEIMessageOffset;

         /* For debug. */
         if ( pstXdmPicture->pNextPicture )
         {
            uiSerialNumber = ((BXDM_Picture *)(pstXdmPicture->pNextPicture))->uiSerialNumber;
         }

#if BXVD_LEGACY_MVC_SUPPORT
         /* SW7422-72: to support the original MVC behavior on the older DVD chips, mark the set as
          * full frame 3D with frame 0 as the left image.  On newer chips, the application/middleware
          * will need to call "BXDM_PictureProvider_Set3D_isr()" to specify the orientation.  If this
          * call is not made, XDM will deliver the content as 2D to VDC.
          */
         pstXdmPicture->st3D.eOrientation = BXDM_Picture_Orientation_e3D_FullFrame;
         pstXdmPicture->st3D.eFrameRelationship = BXDM_Picture_FrameRelationship_eFrame0Left;
#endif
      }

      while ( 0 != uiSEIOffset )
      {
         BXDM_Picture_ExtensionData * pstExtensionData;
         uint32_t uiIndex;

         if ( pstXdmPicture->stExtensionInfo.uiCount >= BXDM_MAX_PICTURE_EXTENSION_INFO )
         {
            BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_ValidatePicture:: number of SEI messages is greater than BXDM_MAX_PICTURE_EXTENSION_INFO"));
            break;
         }


         /* Convert the BXVD_P_SEI_Message offset to a virtual address. */
         BMEM_ConvertOffsetToAddress(
            hXvdCh->hSystemHeap,
            uiSEIOffset,
            (void *)&( pSEIMessage )
            );

         pSEIMessage = (BXVD_P_SEI_Message *)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(pSEIMessage, hXvdCh);

         BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, pSEIMessage, sizeof( BXVD_P_SEI_Message)));

         switch( pSEIMessage->uiMsgType )
         {
            case BXVD_P_PPB_SEI_MSG_FRAMEPACKING:
            {
               /* Needed pstAvdSEIData to prevent the compiler warning
                * "dereferencing type-punned pointer will break strict-aliasing rules"
                */
               BXVD_P_SEI_FramePacking * pstAvdSEIData = (BXVD_P_SEI_FramePacking *)&(pSEIMessage->data.stSEIFramePacking);

               /* Dereference the offset data. */
               uiIndex = pstXdmPicture->stExtensionInfo.uiCount;
               pstExtensionData = &(pstXdmPicture->stExtensionInfo.astExtensionData[uiIndex]);

               /* Set the appropriate values. */
               pstExtensionData->eType = BXDM_Picture_ExtensionType_eSEIMsg_FramePacking;
               pstExtensionData->data.stSEIFramePacking.pstSeiData = (BXDM_Picture_Extension_SEIFramePacking *)pstAvdSEIData;

               if ( uiFlags & BXVD_P_PPB_FLAG_NEW_SEI_MSG )
               {
                  pstExtensionData->data.stSEIFramePacking.uiFlags = BXDM_PICTURE_SEIMSG_FRAMEPACK_NEW_MESSAGE_FLAG;
               }

               /* For 3D, fill in the pstXdmPicture->st3D.eOrientation
                * and pstXdmPicture->st3D.eFrameRelationship fields.
                */
               BXVD_Decoder_S_UnifiedQ_ParseSeiFramePackMsg( pstAvdSEIData, &(pstXdmPicture->st3D) );

               /* Conditionally print the SEI message */
               BXVD_DecoderDbg_P_PrintSeiMessage( hXvdCh, pSEIMessage, uiSerialNumber );

               /* Bump the extension count.*/
               pstXdmPicture->stExtensionInfo.uiCount++;
               break;
            }

            case BXVD_P_PPB_SEI_MSG_MVC_GRAPHICS_OFFSET:
            {

               /* NOTE: This code only grabs data from the first dependent picture.
                * Perhaps that is all we will need.
                */
               BXVD_P_MVC_Offset_Meta * pMetaData = (BXVD_P_MVC_Offset_Meta *)&(pSEIMessage->data.stOffsetMeta);

               /* Dereference the offset data. */
               uiIndex = pstXdmPicture->stExtensionInfo.uiCount;
               pstExtensionData = &(pstXdmPicture->stExtensionInfo.astExtensionData[uiIndex]);


               /* Set the appropriate values. */
               pstExtensionData->eType = BXDM_Picture_ExtensionType_eBluRay3DGraphicsOffset;
               pstExtensionData->data.stBluRay3DGraphicsOffset.uiCount = pMetaData->size;
               pstExtensionData->data.stBluRay3DGraphicsOffset.puiOffsetData = &(pMetaData->offset[0]);

               /* Conditionally print the SEI message */
               BXVD_DecoderDbg_P_PrintSeiMessage( hXvdCh, pSEIMessage, uiSerialNumber );

               /* Bump the extension count.*/
               pstXdmPicture->stExtensionInfo.uiCount++;
               break;
            }

            default:
               BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_ValidatePicture:: invalid SEI message type %d", pSEIMessage->uiMsgType ));
               break;
         }

         /* Check for another message. Note: while there are provisions to support multiple 
          * messages per PPB, the current thinking is that this will not happen.
          */
         uiSEIOffset = (uint32_t)pSEIMessage->pstNextSEIMsgOffset;

      }  /* end of while ( 0 != uiSEIOffset )            */
   }     /* end of if ( BXVD_P_PPB_Protocol_eH264 ) */


   /* Conditionally print the Unified Picture. 
    * If this is the 2nd picture of a pair of pictures, hold off printing until we hit
    * this point for the 1st (base) picture of the pair.  However if the picture is being 
    * dropped, go ahead and do the print.
    */
   if ( BXVD_Decoder_P_PictureSet_eSingle == pstUnifiedContext->eSetType
         || BXVD_Decoder_P_PictureSet_eBase == pstUnifiedContext->eSetType
         || true == bPictureDropped )
   {
      BXVD_Decoder_P_UnifiedPictureContext * pstTempContext = pstUnifiedContext;

      while ( 0 != pstTempContext )
      {      
         BXVD_DecoderDbg_P_PrintUnifiedPicture( hXvdCh, uiFlags, uiFlagsExt0, pstTempContext, bPictureDropped );
         pstTempContext = pstTempContext->pstDependentUniContext;
      }
   }

   BDBG_LEAVE(BXVD_Decoder_S_UnifiedQ_ValidatePicture);

   return;

}  /* BXVD_Decoder_S_UnifiedQ_ValidatePicture() */

/*
 * The picture count would take into account picture sets.
 */
static uint32_t BXVD_Decoder_S_UnifiedQ_GetFreeCount(
   BXVD_ChannelHandle hXvdCh
   )
{
   uint32_t uiFreeCount;

   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_GetFreeCount );

   uiFreeCount = BXVD_P_DECODER_PICTURE_QUEUE_DEPTH - hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumUsedElements;

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_GetFreeCount );

   return uiFreeCount;
}

static uint32_t BXVD_Decoder_S_UnifiedQ_GetPictureCount(
   BXVD_ChannelHandle hXvdCh
   )
{
   return hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumPictures;
}

/* Gets the entry at the tail of the unified context queue.
 * TODO: return a bool; false if the queue is empty?
 */
static void BXVD_Decoder_S_UnifiedQ_UnifiedContextGet(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext ** ppstUnifiedContext,
   bool bBumpReadOffset
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_UnifiedContextGet );


   if ( true == bBumpReadOffset )
   {
      hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumPictures--;
   }

   /* The picture count should never go below 0.
    * If it does, there is a bug in the logic.
    */
   BDBG_ASSERT( hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumPictures >= 0 );

   BXVD_Decoder_S_UnifiedQ_GetLastElement( hXvdCh, ppstUnifiedContext, bBumpReadOffset );

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_UnifiedContextGet );

   return;

} /* end of BXVD_Decoder_S_UnifiedQ_UnifiedContextGet() */

static void BXVD_Decoder_S_UnifiedQ_GetLastElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext ** ppstUnifiedContext,
   bool bBumpReadOffset
   )
{
   BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue = &hXvdCh->stDecoderContext.stUnifiedPictureQueue;

   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_GetLastElement );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( ppstUnifiedContext );

   /* TODO: what to return if the queue is empty? */

   *ppstUnifiedContext = pstUnifiedQueue->pstContextTail;

   if ( true == bBumpReadOffset )
   {
      BXVD_Decoder_S_UnifiedQ_DumpElement( *ppstUnifiedContext, false );

      /* Increment picture context queue read pointer */

      /* For debug / error checking. */
      if ( BXVD_Decoder_P_PictureSet_eSingle == (*ppstUnifiedContext)->eSetType
            && 1 != (*ppstUnifiedContext)->uiSetCount
         )
      {
         BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_GetNextPicture_isr:: BXVD_Decoder_P_PictureSet_eSingle uiSetCount = %d", (*ppstUnifiedContext)->uiSetCount ));
      }
      else if ( BXVD_Decoder_P_PictureSet_eBase == (*ppstUnifiedContext)->eSetType
                  && 2 != (*ppstUnifiedContext)->uiSetCount
               )
      {
         BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_GetNextPicture_isr:: BXVD_Decoder_P_PictureSet_eBase uiSetCount = %d", (*ppstUnifiedContext)->uiSetCount ));
      }

      /* Walk through all the linked dependent pictures. */
      while( NULL != pstUnifiedQueue->pstContextTail->pstDependentUniContext )
      {
         pstUnifiedQueue->pstContextTail = pstUnifiedQueue->pstContextTail->pstDependentUniContext;
      }

      /* Move the tail pointer if there is another picture. */
      if ( NULL != pstUnifiedQueue->pstContextTail->pstNextUniContext )
      {
         pstUnifiedQueue->pstContextTail = pstUnifiedQueue->pstContextTail->pstNextUniContext;

         /* Do we need to Break the link between the elements?
          * Note: the element is actually release in the call to BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease.
          */
         pstUnifiedQueue->pstContextTail->pstPreviousUniContext->pstNextUniContext = NULL;
         pstUnifiedQueue->pstContextTail->pstPreviousUniContext->pstPreviousUniContext = NULL;
      }

   }

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_GetLastElement );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_GetLastElement() */
#if 0
static void BXVD_Decoder_S_UnifiedQ_InitializeElement(
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_InitializeElement );

   BDBG_ASSERT( pstUnifiedContext );

   BKNI_Memset( pstUnifiedContext, 0, sizeof( BXVD_Decoder_P_UnifiedPictureContext ) );

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_InitializeElement );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_InitializeElement() */
#endif

/*
 * Sets the pointer to the head of the unified picture queue.
 */
static void BXVD_Decoder_S_UnifiedQ_GetNextFreeElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext ** ppstUnifiedContext
   )
{
   uint32_t uiElementsEvaluated=0;
   bool     bFoundFreeElement=false;

   BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue = &hXvdCh->stDecoderContext.stUnifiedPictureQueue;

   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_GetNextFreeElement );

   /* The call to "BXVD_Decoder_S_UnifiedQ_GetFreeCount()" in "BXVD_Decoder_S_UnifiedQ_Update()"
    * should ensure that the queue has enough room. Keep this check as a safety valve.
    */
   if ( !BXVD_Decoder_S_UnifiedQ_GetFreeCount( hXvdCh ) )
   {
      BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_GetNextFreeElement:: queue is full.", NULL));
      BDBG_ASSERT( 0 );
   }

   /* Set the pointer to the free element. */
   *ppstUnifiedContext = &(pstUnifiedQueue->astUnifiedContext[ pstUnifiedQueue->uiSearchIndex ]);

   /* Mark the entry as used. */
   (*ppstUnifiedContext)->bInUse = true;

   hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumUsedElements++;
   BDBG_ASSERT( hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumUsedElements <= BXVD_P_DECODER_PICTURE_QUEUE_DEPTH );

   /* uiElementsEvaluated To check if the code has looped around,
    * should not happen if "iNumUsedElements" is correct.
    */

   /* Step through the queue until the next free element is found. */

   while ( uiElementsEvaluated < BXVD_P_DECODER_PICTURE_QUEUE_DEPTH )
   {
      BXVD_Decoder_S_Increment_0BasedOffset(
               &(pstUnifiedQueue->uiSearchIndex),
               1,
               BXVD_P_DECODER_PICTURE_QUEUE_DEPTH
               );

      if ( false == pstUnifiedQueue->astUnifiedContext[ pstUnifiedQueue->uiSearchIndex ].bInUse )
      {
         bFoundFreeElement = true;
         break;
      }


      uiElementsEvaluated++;
   }

   if ( true == bFoundFreeElement )
   {
      /* Set the head of the list to this new context. */
      pstUnifiedQueue->pstContextHead = &(pstUnifiedQueue->astUnifiedContext[ pstUnifiedQueue->uiSearchIndex ]);

      /* for debug */
      pstUnifiedQueue->pstContextHead->uiIndex = pstUnifiedQueue->uiSearchIndex;
   }
   else
   {
      /* The preceeding calls to "BXVD_Decoder_S_UnifiedQ_GetFreeCount()"
       * should ensure that the queue has enough room. Keep this check as a safety valve.
       */
      BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_GetNextFreeElement:: did not find a free element.", NULL));
      BDBG_ASSERT( 0 );
   }

   /* Link this element to the preceeding. */
   pstUnifiedQueue->pstContextHead->pstPreviousUniContext = *ppstUnifiedContext;

   /* Link the preceeding element to this one. */
   (*ppstUnifiedContext)->pstNextUniContext = pstUnifiedQueue->pstContextHead;
#if 0
   BXVD_Decoder_S_Printf("this: %d ", (*ppstUnifiedContext)->uiIndex );

   if ( NULL == (*ppstUnifiedContext)->pstNextUniContext )
      BXVD_Decoder_S_Printf("next: NULL ", NULL );
   else
      BXVD_Decoder_S_Printf("next: %d ", (*ppstUnifiedContext)->pstNextUniContext->uiIndex );

   if ( NULL == (*ppstUnifiedContext)->pstPreviousUniContext )
      BXVD_Decoder_S_Printf("prev: NULL ", NULL );
   else
      BXVD_Decoder_S_Printf("prev: %d ", (*ppstUnifiedContext)->pstPreviousUniContext->uiIndex );

   BXVD_Decoder_S_Printf("head: %d ", pstUnifiedQueue->pstContextHead->uiIndex );

   if ( NULL == pstUnifiedQueue->pstContextHead->pstNextUniContext )
      BXVD_Decoder_S_Printf("hnext: NULL ", NULL );
   else
      BXVD_Decoder_S_Printf("hnext: %d ", pstUnifiedQueue->pstContextHead->pstNextUniContext->uiIndex );
   if ( NULL == pstUnifiedQueue->pstContextHead->pstPreviousUniContext )
      BXVD_Decoder_S_Printf("hprev: NULL\n", NULL );
      else
         BXVD_Decoder_S_Printf("hprev: %d\n", pstUnifiedQueue->pstContextHead->pstPreviousUniContext->uiIndex );
#endif

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_GetNextFreeElement );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_GetNextFreeElement() */


/*
 *
 */
static void BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstUnifiedContext );

   BXVD_Decoder_S_UnifiedQ_DumpElement( pstUnifiedContext, false );

   BXVD_Decoder_S_UnifiedQ_ReleaseElement( hXvdCh, pstUnifiedContext );

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease );

   return;
}

static void BXVD_Decoder_S_UnifiedQ_ReleaseElement(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_ReleaseElement );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstUnifiedContext );

   /* Release the unified picture(s) as well as the the associated PPB(s).
    * SWBLURAY-23835: Each picture should only be release once.  To 
    * accommodate poorly behaving software, check "bInUse" to see if a 
    * picture was previously released. 
    */
   if ( true == pstUnifiedContext->bInUse )
   {
      if ( NULL != pstUnifiedContext->pstDependentUniContext )
      {
         BXVD_Decoder_S_UnifiedQ_ReleaseElement( hXvdCh, pstUnifiedContext->pstDependentUniContext );
      }

      BXVD_Decoder_S_ReleaseQ_AddPPB( hXvdCh, pstUnifiedContext->pPPBPhysical );
      BXVD_Decoder_S_UnifiedQ_InvalidateElement( hXvdCh, pstUnifiedContext );
   }
   else
   {
      BXVD_DBG_ERR(hXvdCh,("BXVD_Decoder_S_UnifiedQ_ReleaseElement:: picture released multiple times.", NULL));
   }

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_ReleaseElement );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_ReleaseElement() */

#if 0
/*
 *
 */
static void BXVD_Decoder_S_UnifiedQ_UnifiedContextAdd(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext * pstPictureCntxt
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_UnifiedContextAdd );

   hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumPictures++;

   /* TODO: add a return value here? */
   BXVD_Decoder_S_UnifiedQ_AddPicture( hXvdCh, pstPictureCntxt );

   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_UnifiedContextAdd );

   return;

} /* end of BXVD_Decoder_S_UnifiedQ_UnifiedContextAdd() */
#endif

static void BXVD_Decoder_S_UnifiedQ_AddPicture(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_PictureContext * pstPictureCntxt,
   bool bBasePicture
   )
{
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext=NULL;

   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_AddPicture );

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_AddPic );

   /* Bump the count of sets of pictures on the Unified Queue when
    * the base picture is added.
    */
   if ( true == bBasePicture )
   {
      hXvdCh->stDecoderContext.stUnifiedPictureQueue.iNumPictures++;
   }

   /* Get the BXDM_Picture element at the head of the unified queue.
    * The call to "BXVD_Decoder_S_UnifiedQ_GetFreeCount()" in "BXVD_Decoder_S_UnifiedQ_Update()"
    * should ensure that the queue has enough room.
    */
   BXVD_Decoder_S_UnifiedQ_GetNextFreeElement( hXvdCh, &pstUnifiedContext );

   /* This should never happen. We should only get here if there is a free
    * element on the Unified Queue.
    */
   if ( NULL == pstUnifiedContext )
   {
      BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_AddPicture:: failed to get an element.", NULL));
      BDBG_ASSERT( 0 );
   }

   /* Dereference the PPB and fill in the BXDM_Picture structure.
    */
   BXVD_Decoder_S_UnifiedQ_ValidatePicture( hXvdCh, pstPictureCntxt, pstUnifiedContext, false );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_AddPic );


   BDBG_LEAVE( BXVD_Decoder_S_UnifiedQ_AddPicture );

   return;

} /* end of BXVD_Decoder_S_UnifiedQ_AddPicture() */

static void BXVD_Decoder_S_PPBReceived(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_PPB **pstPPB
   )
{
   bool bSignalPPBReceived;

   BDBG_ENTER(BXVD_Decoder_S_PPBReceived);

   BDBG_ASSERT( hXvdCh );

   /* First check that a callback function has been registered */
   bSignalPPBReceived = ( NULL != hXvdCh->stInterruptCallbackInfo[BXVD_Interrupt_ePPBReceived].BXVD_P_pAppIntCallbackPtr );

   /* Then check that the callback mask bit is set. */
   bSignalPPBReceived &= ( true == hXvdCh->stCallbackReq.bPPBReceived );

   /* If all the conditions have been met, execute the callback. */
   if( bSignalPPBReceived )
   {
      BXVD_DecoderTimer_P_SnapshotCallbackStartTime( hXvdCh, BXVD_DecoderTimer_P_eCbPPBReceived );

      hXvdCh->stInterruptCallbackInfo[BXVD_Interrupt_ePPBReceived].BXVD_P_pAppIntCallbackPtr (
                                 hXvdCh->stInterruptCallbackInfo[BXVD_Interrupt_ePPBReceived].pParm1,
                                 hXvdCh->stInterruptCallbackInfo[BXVD_Interrupt_ePPBReceived].parm2,
                                 pstPPB
                                 ) ;

      BXVD_DecoderTimer_P_SnapshotCallbackEndTime( hXvdCh, BXVD_DecoderTimer_P_eCbPPBReceived );

   }

   BDBG_LEAVE(BXVD_Decoder_S_PPBReceived);

   return;

}    /* end of BBXVD_Decoder_S_PPBReceived() */

static void BXVD_Decoder_S_UnifiedQ_GetSetType(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState * pstLocalState,
   BXVD_Decoder_P_PictureContext * pstPicCntxt
   )
{
   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_GetSetType );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstPicCntxt );
   BSTD_UNUSED( pstLocalState );

   if ( BXVD_P_PPB_Protocol_eH264 == pstPicCntxt->stPPB.pPPB->protocol )
   {
      /* If AVC, check the SEI messages to determine if 3D. */

      uint32_t             uiSEIOffset=0;
      BXVD_P_SEI_Message * pSEIMessage;

      /* SW7405-4883: set a default of 2D. */
      pstPicCntxt->st3D.eOrientation = BXDM_Picture_Orientation_e2D;
      pstPicCntxt->st3D.eFrameRelationship = BXDM_Picture_FrameRelationship_eUnknown;

      /* TODO: the following is a partial duplicate of the SEI parsing in
       * the validate routine, clean this up.
       */
      uiSEIOffset = (uint32_t)pstPicCntxt->stPPB.pPPB->other.h264.pstSEIMessageOffset;

      while ( 0 != uiSEIOffset )
      {
         /* Convert the BXVD_P_SEI_Message offset to a virtual address. */
         BMEM_ConvertOffsetToAddress(
            hXvdCh->hSystemHeap,
            uiSEIOffset,
            (void *)&( pSEIMessage )
            );

         pSEIMessage = (BXVD_P_SEI_Message *)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(pSEIMessage, hXvdCh);
         
         BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, pSEIMessage, sizeof( BXVD_P_SEI_Message)));

         if ( BXVD_P_PPB_SEI_MSG_FRAMEPACKING == pSEIMessage->uiMsgType )
         {
            /* Needed pstAvdSEIData to prevent the compiler warning
             * "dereferencing type-punned pointer will break strict-aliasing rules"
             */
            BXVD_P_SEI_FramePacking * pstAvdSEIData = (BXVD_P_SEI_FramePacking *)&(pSEIMessage->data.stSEIFramePacking);

            /* For 3D, fill in the pstXdmPicture->st3D.eOrientation
             * and pstXdmPicture->st3D.eFrameRelationship fields.
             */
            BXVD_Decoder_S_UnifiedQ_ParseSeiFramePackMsg( pstAvdSEIData, &(pstPicCntxt->st3D) );

            /* TODO: if multiple messages, break on the first?*/
            break;
         }

         /* Check for another message. */
         uiSEIOffset = (uint32_t)pSEIMessage->pstNextSEIMsgOffset;

      }  /* end of while ( 0 != uiSEIOffset )            */

      /* Convert SEI parameters into the appropriate "set" information.
       */
      switch( pstPicCntxt->st3D.eOrientation )
      {
         case BXDM_Picture_Orientation_e3D_FullFrame:
            /* Frame 0 is the base picture. */ 

            if ( 0 == pstPicCntxt->st3D.uiFrameNum )
            {
               pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eBase;
            }
            else
            {
               pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eDependent;
            }

            /* Expecting two pictures for full frame 3D */
            pstPicCntxt->uiSetCount = 2;       

            break;

         case BXDM_Picture_Orientation_e2D:
         case BXDM_Picture_Orientation_e3D_Checker:
         case BXDM_Picture_Orientation_e3D_Column:
         case BXDM_Picture_Orientation_e3D_Row:
         case BXDM_Picture_Orientation_e3D_SideBySide:
         case BXDM_Picture_Orientation_e3D_TopBottom:
         default:
            pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eSingle;
            pstPicCntxt->uiSetCount = 1;
            break;
      }

   }
   else
   {
      /* Extract the picture set type and set count. This is to support MVC and 
       * perhaps other multipicture protocols.
       */
      if ( !(pstPicCntxt->stPPB.pPPB->flags & BXVD_P_PPB_MULTIVIEW_FIELD_MASK) )
      {
         pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eSingle;
         pstPicCntxt->uiSetCount = 1;
      }
      else
      {
         if ( pstPicCntxt->stPPB.pPPB->flags & BXVD_P_PPB_MULTIVIEW_BASE_FLAG )
         {
            pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eBase;
            pstPicCntxt->uiSetCount = (( pstPicCntxt->stPPB.pPPB->flags & BXVD_P_PPB_MULTIVIEW_COUNT_MASK ) >> BXVD_P_PPB_MULTIVIEW_COUNT_SHIFT );

            /* Should be generalized to use "pstPicCntxt->uiSetCount", waiting on the firmware.*/
            pstPicCntxt->uiSetCount = BXVD_DECODER_S_PICTURES_PER_SET;
         }
         else
         {
            pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eDependent;
            pstPicCntxt->uiSetCount = BXVD_DECODER_S_PICTURES_PER_SET;
         }
      }
   }


   BDBG_ENTER( BXVD_Decoder_S_UnifiedQ_GetSetType );

   return;
}


/*
 *
 */
static void BXVD_Decoder_S_UnifiedQ_Update(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState * pstLocalState
   )
{
   BXVD_P_PictureDeliveryQueue * pstDeliveryQueue;
   BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue = &hXvdCh->stDecoderContext.stUnifiedPictureQueue;
   uint32_t uiDeliveryQWriteOffset0Based;
   uint32_t uiDeliveryQReadOffset0Based;
   BXVD_Decoder_P_PictureContext * pstPicCntxt = NULL;
   BXVD_Decoder_P_PictureContext * pstPrevPicCntxt = NULL;

   BXVD_Decoder_P_PictureContextQueue * pstContextQueue = &(hXvdCh->stDecoderContext.stPictureContextQueue);

   uint32_t    uiNumPicturesToProcess;

   uint32_t    uiDeliveryQ_PicturesToProcess;
   uint32_t  uiUniQFreeElements;

   BXVD_Decoder_P_DQTContext * pDQTCntxt = &( hXvdCh->stDecoderContext.stDqtCntxt );
   bool  bSearchForStartOfNextGop;
   bool  bDeadlock = false;
   bool  bFoundEndOfGop = false;
   bool  bDqtMvcError = false;

   /* Update the Unified Picture Queue.
    *
    * There are three queues in play here:
    * - Delivery Queue, contains the PPB's from the video decoder
    * - Picture Context Queue, temporary values needed for processing pictures (e.g. PPB virtual addresses)
    * - Unified Picture Queue, the "cooked" pictures for consumption by the Picture Provider
    *
    * The conversion of PPB's into Unified Pictures is handled in three phases.
    *
    * Phase 1: vetting of the PPB's
    * - calculate the virtual address of the PPB
    * - check that there are matched sets of base and dependent pictures (e.g. MVC protocol)
    * - when DQT is enabled, search for the start of the next GOP
    * - generate the PPBReceived callback
    *
    * Phase 2: DQT cleanup (if DQT is enabled)
    * - determine if a GOP needs to be truncated
    * - set the appropriate pointers for processing the PPB's in reverse order
    * - handle MVC errors
    *   (will this ever be supported? needs to be tested more thoroughly!!)
    *
    * Phase 3: validate Unified Picture Queue.
    * - interpret the PPB's fields to set appropriate Unified Picture elements
    * - link the dependent pictures to the base pictures
    */

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_Vet );

   BXVD_Decoder_S_Printf("\n> ", NULL );

   /* Cleanup pictures at the end of a GOP that are not going to be displayed.
    */
   if ( true == pDQTCntxt->bTruncatingGop )
   {
      BXVD_Decoder_S_DeliveryQ_ReleaseGopTail( hXvdCh, pstLocalState );
   }

   /* If ...
    * - DQT is enabled
    * - NOT in the middle of displaying a GOP
    * - NOT throwing away pictures at the end of the GOP
    * then it is time to look for the start of the next GOP.
    */
   bSearchForStartOfNextGop = ( true == pDQTCntxt->bDqtEnabled );
   bSearchForStartOfNextGop &= ( false == pDQTCntxt->bConvertGopToUniPics );
   bSearchForStartOfNextGop &= ( false == pDQTCntxt->bTruncatingGop );

   if ( true == bSearchForStartOfNextGop )
   {
      BXVD_Decoder_S_Printf(" findNext ", NULL );
   }

   /* Retrieve the read/write offsets for the delivery queue.
    * Adjust them to be '0' based.
    */
   BXVD_P_DELIVERY_QUEUE_GET_ADDR( hXvdCh, pstDeliveryQueue );

#if BXVD_P_FW_HIM_API
   BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)pstDeliveryQueue, sizeof(BXVD_P_PictureReleaseQueue)));
#endif

   BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiDeliveryQReadOffset0Based );

   uiDeliveryQReadOffset0Based -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

   uiDeliveryQWriteOffset0Based = pstLocalState->uiDeliveryQueueWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;


   /* SW7405-3137: calculate the number of pictures on the delivery queue.
    * Hold off the processing of pictures until a complete "set" has
    * been delivered. (i.e. detects incomplete sets at the head of the queue.)
    * TODO: does this work with DQT?  Handle in the search routine?
    */
   BXVD_P_GET_QUEUE_DEPTH( uiDeliveryQReadOffset0Based, uiDeliveryQWriteOffset0Based, uiDeliveryQ_PicturesToProcess );

   /* SW7405-3137: calculate the number of pictures to be processed on this call.
    * Hold off processing the one(s) at the tail of the queue until a complete
    * "set" has been delivered.  (i.e detects incomplete sets at the tail of the queue.)
    */
   BXVD_P_GET_QUEUE_DEPTH( pstContextQueue->uiWriteOffset, uiDeliveryQWriteOffset0Based, uiNumPicturesToProcess );

   /* If in the middle of processing a GOP, hold off processing more pictures
    * until done with this GOP.  This check should not be needed since the queue's are
    * reconcile as soon as the next GOP is delivered. The only way this could
    * happen, is if the Unified Picture Queue was full and the validation
    * need to be stalled until the Unified Queue was drained.
    */
   if ( true == pDQTCntxt->bConvertGopToUniPics )
   {
      BXVD_Decoder_S_Printf("rPlay ", NULL );
      goto ProcessGop;
   }

   /* If DQT is enabled, check for a dead lock condition, i.e. the DM is waiting for the next
    * picture to be delivered and the decoder is waiting for a picture to be freed.
    * Don't bother with the check if the queue is empty; this will prevent false deadlock
    * detections when simply waiting for the video decoder.
    */
   if ( true == bSearchForStartOfNextGop
       && uiDeliveryQReadOffset0Based != uiDeliveryQWriteOffset0Based
      )
   {
      bDeadlock = BXVD_Decoder_S_DeliveryQ_CheckForDeadlock( hXvdCh, uiNumPicturesToProcess );
      bFoundEndOfGop = bDeadlock;
   }


   BXVD_Decoder_S_Printf("start vetting:: DQrd:%d DQwr:%d CQwr:%d trnc:%d\n",
            uiDeliveryQReadOffset0Based,
            uiDeliveryQWriteOffset0Based,
            pstContextQueue->uiWriteOffset,
            pDQTCntxt->bTruncatingGop
            );

   /* Phase 1: walk through the deliver queue vetting the PPB's.
    *
    * Loop through until:
    * - the PictureContextQueue write offset is equal to the DeliveryQueue write offset
    * - OR an incomplete set of pictures is encountered at either the tail or head of the DeliveryQueue.
    * - OR an error condition, i.e. an incomplete set of pictures is encountered in
    *   the middle of the DeliveryQueue.
    * - OR DQT is enabled and the start of the next GOP has been found
    *
    * Note: "uiNumPicturesToProcess" will equal "0" during a deadlock condtion.  Checking the
    * deadlock flag was added to the while statement for documentation purposes.
    */
   while ((0 != uiNumPicturesToProcess) && (false == bDeadlock))
   {
      /* Get the next element on the picture context queue. */
      pstPicCntxt = &pstContextQueue->astPictureContext[pstContextQueue->uiWriteOffset];

      /* Reset the picture parameters and other elements */
      BKNI_Memset( pstPicCntxt, 0, sizeof( BXVD_Decoder_P_PictureContext ) );

      /* Get the PPB address */
      pstPicCntxt->stPPB.pPPBPhysical = (BXVD_P_PPB *)( pstDeliveryQueue->display_elements[ pstContextQueue->uiWriteOffset ]);

      BMEM_ConvertOffsetToAddress(
         hXvdCh->hSystemHeap,
         (uint32_t)( pstPicCntxt->stPPB.pPPBPhysical ),
         (void *)&( pstPicCntxt->stPPB.pPPB )
         );

      pstPicCntxt->stPPB.pPPB = (BXVD_P_PPB *)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(pstPicCntxt->stPPB.pPPB, hXvdCh); 

      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, pstPicCntxt->stPPB.pPPB, sizeof(BXVD_P_PPB)));

      /* Just for debug. */
      if ( true == pDQTCntxt->bDqtEnabled )
      {
         BXVD_Decoder_S_Printf("\n\tsearch:: DQ:%d/%d CQ:x/%d curTag=%u  picTag=%u ",
                     uiDeliveryQReadOffset0Based,
                     uiDeliveryQWriteOffset0Based,
                     pstContextQueue->uiWriteOffset,
                     pDQTCntxt->uiCurrentPicTag,
                     pstPicCntxt->stPPB.pPPB->picture_tag
                     );
      }

      /* SWCHECKFRAME-80: execute the PPBReceived callback prior to processing the PPB data.
       *
       * FWAVD-289: It is possible to go through this loop more than once for any given PPB.
       * For instance, when playing an MVC stream, only one picture of the pair may be in the 
       * delivery queue when the PictureDataReady callback fires.  To avoid generating the 
       * PPBReceived callback multiple times for the same picture, use the queue index to 
       * detect when a picture is being evaluated for the first time.
       */
      if ( hXvdCh->stDecoderContext.uiPreviousWriteOffset != pstContextQueue->uiWriteOffset )
      {
         BXVD_Decoder_S_PPBReceived( hXvdCh, &(pstPicCntxt->stPPB.pPPB) );

         hXvdCh->stDecoderContext.uiPreviousWriteOffset = pstContextQueue->uiWriteOffset;
      }

      /* Extract the picture set type and set count.  This is to 
       * support 3D and any multipicture protocol.
       */
      BXVD_Decoder_S_UnifiedQ_GetSetType( hXvdCh, pstLocalState, pstPicCntxt );

      if ( BXVD_Decoder_P_PictureSet_eSingle == pstPicCntxt->eSetType )
      {
         BXVD_Decoder_S_Printf("S/%d ", pstContextQueue->uiWriteOffset );
      }
      else if ( BXVD_Decoder_P_PictureSet_eBase == pstPicCntxt->eSetType )
      {
         BXVD_Decoder_S_Printf("B/%d ", pstContextQueue->uiWriteOffset );

         /* An incomplete set of pictures at the head of the delivery queue. */

         if ( uiDeliveryQ_PicturesToProcess < pstPicCntxt->uiSetCount )
         {
            BXVD_Decoder_S_Printf("break at head\n", NULL );
            break;
         }
      }
      else if ( BXVD_Decoder_P_PictureSet_eDependent == pstPicCntxt->eSetType )
      {
         BXVD_Decoder_S_Printf("d/%d ", pstContextQueue->uiWriteOffset );
      }
      else
      {
         BXVD_DBG_ERR(hXvdCh,("BXVD_Decoder_S_UnifiedQ_Update:: unknown set type %d", pstPicCntxt->eSetType));

         pstPicCntxt->eSetType = BXVD_Decoder_P_PictureSet_eSingle;
         pstPicCntxt->uiSetCount = 1;
      }


      /* If DQT is enabled, check for the start of the next GOP.
       * Don't look at the picture tag on dependent pictures.
       * GOP's should always start on a "base" or single picture.
       */
      if ( BXVD_Decoder_P_PictureSet_eDependent != pstPicCntxt->eSetType
            && true == pDQTCntxt->bDqtEnabled
          )
      {
         /* If "bValidPicTag" is false, the start of a GOP was detected the previous time
          * through this loop.  Snapshot the new picture tag.
          */
         if ( false == pDQTCntxt->bValidPicTag )
         {
            pDQTCntxt->uiCurrentPicTag = pstPicCntxt->stPPB.pPPB->picture_tag;
            pDQTCntxt->bValidPicTag = true;
            BXVD_Decoder_S_Printf(" set tag %d ", pDQTCntxt->uiCurrentPicTag );
         }

         /* If the new picture tag doesn't match the old one, the start of
          * the next GOP has been found.  Break out of this loop and begin
          * displaying the GOP in reverse.
          */
         if ( true == pDQTCntxt->bValidPicTag
               && pDQTCntxt->uiCurrentPicTag != pstPicCntxt->stPPB.pPPB->picture_tag
            )
         {
            bFoundEndOfGop = true;

            BXVD_Decoder_S_Printf("\n\tfound EOG picTagThis=%u  picTagNext=%u ",
                     pDQTCntxt->uiCurrentPicTag,
                     pstPicCntxt->stPPB.pPPB->picture_tag
                     );

            break;
         }

      }  /* end of if( !BXVD_Decoder_P_PictureSet_eDependent && pDQTCntxt->bDqtEnabled ) */


      /* Check for an error sequence.  If the picture sequence is fine,
       * generate the "BXVD_Interrupt_ePPBReceived" callback and
       * validate the static picture context.
       *
       * The error sequences are
       * - "base" followed by "single"
       * - "base" followed by "base"
       * - "single" followed by "dependent"
       * - "dependent" followed by "dependent"
       *
       * Note: the first two scenarios can span a call to this routine, the
       * last two cannot.
       * Note: the POC will be the same for all pictures in the set, is this of any use?
       */
      if ( BXVD_Decoder_P_PictureSet_eSingle == pstPicCntxt->eSetType
          || BXVD_Decoder_P_PictureSet_eBase == pstPicCntxt->eSetType
         )
      {
         /* Check for a bad picture sequence. A "base" picture can not be followed
          * by a "base" or "single" picture; this indicates a missing "dependent" picture.
          * Evaluation of picture sets won't span a call to this routine, don't perform
          * the check for the first picture, i.e. "NULL == pstPrevPicCntxt".
          */
         if ( NULL != pstPrevPicCntxt
               && BXVD_Decoder_P_PictureSet_eBase == pstPrevPicCntxt->eSetType
            )
         {
            uint32_t uiTempWriteOffset;

            BXVD_Decoder_S_Printf("bad picture sequence Base followed by %d\n", pstPicCntxt->eSetType );

            /* The picture in error is the previous one, backup the write pointer */

            uiTempWriteOffset = pstContextQueue->uiWriteOffset;

            BXVD_Decoder_S_Decrement_0BasedOffset(
                     &(uiTempWriteOffset),
                     1,
                     BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE
                     );

            pstPicCntxt = &pstContextQueue->astPictureContext[uiTempWriteOffset];

            if ( true == pDQTCntxt->bDqtEnabled )
            {
               /* The error picture will be handled by the DQT truncation logic. */
               bFoundEndOfGop = true;
               bDqtMvcError = true;
               break;
            }
            else
            {
               /* Mark the picture to be dropped. By "continue-ing", the error picture
                * will be dealt with at the top of this loop.
                * Drop the picture "in line".
                * Mark it as dropped so that the uni validation logic ignores it.
                */
               pstPicCntxt->bDropPicture = true;
               break;
            }
         }

         if ( BXVD_Decoder_P_PictureSet_eBase == pstPicCntxt->eSetType )
         {
            /* If a Base picture, check for an incomplete set of pictures at the
             * tail of the delivery queue.  Wait for all the pictures before
             * processing this set.
             */
            if ( uiNumPicturesToProcess  < BXVD_DECODER_S_PICTURES_PER_SET /*pstPicCntxt->uiSetCount*/ )
            {
               BXVD_Decoder_S_Printf("break at tail, pics on queue %d\n", uiDeliveryQ_PicturesToProcess );
               break;
            }
         }
#if 0
         /* Generate the PPBReceived callback */
         BXVD_Decoder_S_PPBReceived( hXvdCh, &(pstPicCntxt->stPPB.pPPB) );
#endif
      }
      else
      {
         /* Check for a bad picture sequence. A "dependent" picture can not follow either a
          * a "single" or a "dependent" picture; this would indicate a missing "base" picture.
          * Evaluation of picture sets won't span a call to this routine, if the first picture evaluated
          * is a "dependent" picture, the "base" picture is missing.
          */
         if ( NULL == pstPrevPicCntxt
              || BXVD_Decoder_P_PictureSet_eDependent == pstPrevPicCntxt->eSetType
              || BXVD_Decoder_P_PictureSet_eSingle == pstPrevPicCntxt->eSetType
            )
         {
            if ( NULL == pstPrevPicCntxt )
            {
               BXVD_Decoder_S_Printf("bad picture sequence NULL followed by Dependent\n", NULL );
            }
            else
            {
               BXVD_Decoder_S_Printf("bad picture sequence %d followed by Dependent\n", pstPrevPicCntxt->eSetType );
            }

            if ( true == pDQTCntxt->bDqtEnabled )
            {
               /* The error picture will be handled by the DQT truncation logic. */
               bFoundEndOfGop = true;
               bDqtMvcError = true;
               break;
            }
            else
            {
               /* The picture in error is the current one, mark it to be dropped. */

               pstPicCntxt->bDropPicture = true;

               /* Increment PictureContextQueue write offset */
               BXVD_Decoder_S_Increment_0BasedOffset(
                        &(pstContextQueue->uiWriteOffset),
                        1,
                        BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );

               break;
            }
         }

         /* An extended picture, link it to the preceding base picture. "pstPrevPicCntxt" should never be
          * NULL when processing complete sets. The preceding error logic ensures this.
          */
         if ( NULL != pstPrevPicCntxt )
         {
            /* Link this picture to the base. */
            pstPrevPicCntxt->pDependentPicture = pstPicCntxt;

            /* Line this picture to the base. */
            pstPicCntxt->pBasePicture = pstPrevPicCntxt;
         }
         else
         {
            BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_Update:: failed to link dependent picture to base.", NULL));
         }
      }

      /* Increment PictureContextQueue write offset */
      BXVD_Decoder_S_Increment_0BasedOffset(
               &(pstContextQueue->uiWriteOffset),
               1,
               BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );

      /* Decrement the count of pictures needing to be processed. */
      uiNumPicturesToProcess--;

      /* Save the pointer to the context in order to link base and dependent pictures. */
      pstPrevPicCntxt = pstPicCntxt;

   }  /* end of while ( uiDeliveryQWriteOffset0Based != pstContextQueue->uiWriteOffset ) */


   BXVD_Decoder_S_Printf("\nend vetting:: DQrd:%d DQwr:%d CQwr:%d",
            uiDeliveryQReadOffset0Based,
            uiDeliveryQWriteOffset0Based,
            pstContextQueue->uiWriteOffset
            );

   if ( true  == pDQTCntxt->bDqtEnabled )
   {
      BXVD_Decoder_S_Printf("   search:%d  found:%d  deadlock:%d",
            bSearchForStartOfNextGop,
            bFoundEndOfGop,
            bDeadlock
            );
   }

   BXVD_Decoder_S_Printf("\n", NULL );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_Vet );

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_DQT );

   /* Phase 2:
    * Now that done vetting the delivery queue, deal with DQT clean up.
    * If the end of the GOP was found on a previous call to this routine, determining that the queue
    * has data is fairly straight forward. If still searching for the start of the next GOP,
    * it gets more involved.
    * If the start of the next GOP has NOT been found, the picture queue is effectively empty.
    */
   if ( true == pDQTCntxt->bDqtEnabled
         && true == bSearchForStartOfNextGop
         && true == bFoundEndOfGop )
   {
      /* If a deadlock situation, then the GOP needs to be truncated. */
      if ( bDeadlock )
      {
         pDQTCntxt->bTruncatingGop = true;
         BXVD_Decoder_S_Printf("\n\tDeadLock:: CQwr=%d ", pstContextQueue->uiWriteOffset );
      }

      /* Need to acquire the picture tag of the next GOP. */
      pDQTCntxt->bValidPicTag = false;

      /* "pstContextQueue->uiWriteOffset" points to the next picture on the queue.
       * It could be the start of the next GOP or with MVC, an error picture.
       * In a deadlock scenario, it simply points just beyond the last picture received.
       * Load the following offsets with this value.
       */
      pDQTCntxt->uiEndOfGopOffset = pstContextQueue->uiWriteOffset;
      pDQTCntxt->uiReverseReadOffset = pstContextQueue->uiWriteOffset;

      /* Backup to the last picture of this GOP.
       * Still in picture context queue "domain", use "0" based routines.
       */
      BXVD_Decoder_S_Decrement_0BasedOffset( &(pDQTCntxt->uiReverseReadOffset), 1, BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );

      /* Get the context of this last picture. */
      pstPicCntxt = &pstContextQueue->astPictureContext[pDQTCntxt->uiReverseReadOffset];

      /* Is there only one picture in the GOP? */

      if ( pDQTCntxt->uiReverseReadOffset == uiDeliveryQReadOffset0Based )
      {
         /* Only one picture for this GOP.
          * If it is a "single" picture, all is good.
          * If it is from an incomplete set, an error condition;
          * will needed to trigger the DQT truncation logic.
          * This error condition will not be detected in the main "while" loop.
          */
         if ( BXVD_Decoder_P_PictureSet_eSingle == pstPicCntxt->eSetType )
         {
            pDQTCntxt->bConvertGopToUniPics = true;
            bSearchForStartOfNextGop = false;
            /*pDQTCntxt->bTruncatingGop = false;*/     /* SW7425-1261: only clear bTruncatingGop in ..._ReleaseGopTail */

            BXVD_Decoder_S_Printf("\n\tSinglePic(good):: rr=%d  eog=%d ",
                           pDQTCntxt->uiReverseReadOffset,
                           pDQTCntxt->uiEndOfGopOffset
                           );
         }
         else
         {
            pDQTCntxt->bConvertGopToUniPics = false;
            pDQTCntxt->bTruncatingGop = true;

            /* Need to backup the EOG offset to error picture */
            BXVD_Decoder_S_Decrement_0BasedOffset( &(pDQTCntxt->uiEndOfGopOffset), 1, BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );

            BXVD_Decoder_S_Printf("\n\tSinglePic(bad):: rr=%d  eog=%d ",
                           pDQTCntxt->uiReverseReadOffset,
                           pDQTCntxt->uiEndOfGopOffset
                           );

         }
      }
      else
      {
         /* More than one picture in the GOP. */

         if ( true == bDqtMvcError )
         {
            pDQTCntxt->bTruncatingGop = true;
            BXVD_Decoder_S_Printf("MVC error ");
         }

         /* If a deadlock and a multipicture protocol, we may have left off
          * on a base picture.  Backup the pointers to the next picture.
          * If at the first picture of the GOP, the code block above will
          * be executed.
          */
         if ( BXVD_Decoder_P_PictureSet_eBase == pstPicCntxt->eSetType )
         {
            BXVD_Decoder_S_Decrement_0BasedOffset( &(pDQTCntxt->uiReverseReadOffset), 1, BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );
            BXVD_Decoder_S_Decrement_0BasedOffset( &(pDQTCntxt->uiEndOfGopOffset), 1, BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );
         }

         /* If a multipicture protocol (like MVC) backup until either
          * - the first picture of a complete set is found
          * - OR the head of the queue is reached.
          */
         while ( pDQTCntxt->uiReverseReadOffset != uiDeliveryQReadOffset0Based
                  && BXVD_Decoder_P_PictureSet_eDependent == pstPicCntxt->eSetType
               )
         {
            BXVD_Decoder_S_Decrement_0BasedOffset( &(pDQTCntxt->uiReverseReadOffset), 1, BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE );
            pstPicCntxt = &pstContextQueue->astPictureContext[pDQTCntxt->uiReverseReadOffset];
         }

         pDQTCntxt->bConvertGopToUniPics = ( pDQTCntxt->uiEndOfGopOffset != uiDeliveryQReadOffset0Based );
         bSearchForStartOfNextGop = ( false == pDQTCntxt->bConvertGopToUniPics );

         BXVD_Decoder_S_Printf("\n\tEndGop:: rr=%d  eog=%d\n",
                        pDQTCntxt->uiReverseReadOffset,
                        pDQTCntxt->uiEndOfGopOffset
                        );

      }  /* end of else ( uiReverseReadOffset != uiReadOffset ) */


      /* Convert back to "2-based" offsets. */
      pDQTCntxt->uiEndOfGopOffset += BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      pDQTCntxt->uiReverseReadOffset += BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

   }  /* end of if ( true == pDQTCntxt->bDqtEnabled && .... )  */

   BXVD_Decoder_S_Printf("start validation:: Tail:%d Head:%d ",
         pstUnifiedQueue->pstContextTail->uiIndex,
         pstUnifiedQueue->pstContextHead->uiIndex
         );

   if ( true  == pDQTCntxt->bDqtEnabled )
   {
      BXVD_Decoder_S_Printf("   search:%d  found:%d  deadlock:%d ",
            bSearchForStartOfNextGop,
            bFoundEndOfGop,
            bDeadlock
            );
   }

   BXVD_Decoder_S_Printf("\n", NULL );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh,  BXVD_DecoderTimer_P_eFuncUQ_DQT );

ProcessGop:

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_Val );

   /* Phase 3: update the Unified Picture Queue, i.e. add pictures (or sets of pictures)
    * to the Unified Queue in display order.
    */
   while ( false == bSearchForStartOfNextGop
            && true == BXVD_Decoder_S_ContextQ_PeekAtNextElement( hXvdCh, &pstPicCntxt )
         )
   {
      /* Check that the Unified Q has enough room.
       */
      uiUniQFreeElements = BXVD_Decoder_S_UnifiedQ_GetFreeCount( hXvdCh );

      if ( pstPicCntxt->uiSetCount > uiUniQFreeElements )
      {
         BXVD_DBG_ERR(hXvdCh, ("BXVD_Decoder_S_UnifiedQ_Update Unified Q does not have enough room."));
         break;
      }

      /* During the vetting of the delivery queue, mismatched sets of pictures
       * will be marked for dropping. The actual drop occurs here.
       */
      if ( true == pstPicCntxt->bDropPicture )
      {
         BXVD_Decoder_P_UnifiedPictureContext  stUnifiedContext;

         /* Initialize "stUnifiedContext", in particular need to be certain that 
          * "pstDependentUniContext" is NULL. For the normal code flow, this reset 
          * is handled in "BXVD_Decoder_S_UnifiedQ_InvalidateElement.
          */
         BKNI_Memset( &stUnifiedContext, 0, sizeof( BXVD_Decoder_P_UnifiedPictureContext ) );


         BXVD_Decoder_S_Printf("edrop ", NULL );


         BXVD_Decoder_S_UnifiedQ_ValidatePicture( hXvdCh, pstPicCntxt, &stUnifiedContext, true );

         /* Remove the picture from the delivery queue. This could have been done in the
          * preceeding loop, but it is done here to keep the manipulation of the deliver
          * queue in as few places as possible.
          */
         BXVD_Decoder_S_DeliveryQ_UpdateReadOffset( hXvdCh, pstLocalState, 1 );

         /* Add the PPB to the release queue. */

         BXVD_Decoder_S_ReleaseQ_AddPPB( hXvdCh, pstPicCntxt->stPPB.pPPBPhysical );

      }
      else
      {
         /* Note: will only get complete sets of pictures at this point.
          * The following call adds the complete set of pictures to the Unified Queue.
          */
         BXVD_Decoder_S_UnifiedQ_AddPicture( hXvdCh, pstPicCntxt, true );

         /* Remove the picture(s) from the delivery queue. */

         BXVD_Decoder_S_DeliveryQ_UpdateReadOffset( hXvdCh, pstLocalState, pstPicCntxt->uiSetCount );

      }


   }  /* end of while ( pictures on the context queue ) */


   BXVD_Decoder_S_Printf("\t end validation:: Tail:%d Head:%d\n",
         pstUnifiedQueue->pstContextTail->uiIndex,
         pstUnifiedQueue->pstContextHead->uiIndex
         );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncUQ_Val );

   return;

}  /* end of BXVD_Decoder_S_UnifiedQ_Update() */



static void BXVD_Decoder_S_DeliveryQ_UpdateReadOffset(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState* pstLocalState,
   uint32_t uiDeltaChange
   )
{
   uint32_t uiReadOffset;

   BXVD_Decoder_P_DQTContext * pDQTCntxt = &( hXvdCh->stDecoderContext.stDqtCntxt );

   BSTD_UNUSED( pstLocalState );

   BDBG_ENTER( BXVD_Decoder_S_DeliveryQ_UpdateReadOffset );

   BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiReadOffset );

   /* If reading backwards from the display queue, update the "uiReverseReadOffset"
    * Othewise just update the standard "read offset".
   */
   if ( true == pDQTCntxt->bDqtEnabled )
   {

         BXVD_Decoder_S_Printf("\nUpRdOff:: Rr=%d Qr=%d revPlay %d :: ",
                  pDQTCntxt->uiReverseReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                  uiReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                  pDQTCntxt->bConvertGopToUniPics
                  );

      if ( true == pDQTCntxt->bConvertGopToUniPics )
      {
         if ( pDQTCntxt->uiReverseReadOffset == uiReadOffset )
         {
            /* If "uiReverseReadOffset" is equal to "*puiReadOffset", we've processed the
             * entire "head" of the GOP.
             *
             * Flag that we are done and jump the read offsets ahead to the "end" of
             * the GOP.
             *
             * Note: we could still be cleaning up pictures at the "tail" of this GOP.
            */
            pDQTCntxt->bConvertGopToUniPics = false;

            uiReadOffset = pDQTCntxt->uiEndOfGopOffset;
            BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( hXvdCh, uiReadOffset );
            
            pDQTCntxt->uiReverseReadOffset = pDQTCntxt->uiEndOfGopOffset;

            /* For debug. */
            hXvdCh->stDecoderContext.stLogData.uiPicturesFromAvd += uiDeltaChange;

            BXVD_Decoder_S_Printf("( done Rr=%d Qr=%d ) ",
                  pDQTCntxt->uiReverseReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                  uiReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE
                  );
         }
         else
         {
            /* Still processing this GOP, simply decrement the read ahead offset. */

            BXVD_Decoder_S_Decrement_2BasedOffset(
                     &(pDQTCntxt->uiReverseReadOffset),
                     uiDeltaChange,
                     BXVD_P_DISP_FIFO_DEPTH
                     );

            /* For debug. */
            hXvdCh->stDecoderContext.stLogData.uiPicturesFromAvd += uiDeltaChange;

            BXVD_Decoder_S_Printf("( just inc Rr=%d ) ",
                  pDQTCntxt->uiReverseReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE
                  );
         }

      }
      else
      {
         /* If "bDqtEnabled" is true and "bConvertGopToUniPics" is false,
          * the entire GOP has not been delivered to the display queue.
          * Other logic should prevent this case from being reached.
         */
         BXVD_Decoder_S_Printf("\nUpRdOff:: not supposed to be here:: Rr=%d  Qr=%d inRev = %d  ", 
                                    pDQTCntxt->uiReverseReadOffset, 
                                    uiReadOffset,  
                                    pDQTCntxt->bConvertGopToUniPics
                                    );
      }


   }       /* end if ( bDqtEnabled )*/
   else
   {
      /* Normal play, simply increment the "read offset" */

      BXVD_Decoder_S_Printf("\nUpRdOff:: %d(%d) ", uiReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE, uiDeltaChange );

      BXVD_P_INCREMENT_2BASED_OFFSET( uiReadOffset, uiDeltaChange );

      BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( hXvdCh, uiReadOffset );

      /* For debug. */
      hXvdCh->stDecoderContext.stLogData.uiPicturesFromAvd += uiDeltaChange;

      /* Keep these values in lock step to avoid issues when transistioning into DQT. */
      pDQTCntxt->uiEndOfGopOffset = uiReadOffset;
      pDQTCntxt->uiReverseReadOffset = uiReadOffset;

   }

   BDBG_LEAVE( BXVD_Decoder_S_DeliveryQ_UpdateReadOffset );

   return;

}

static void BXVD_Decoder_S_ReleaseQ_AddPPB(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_PPB *pPPBPhysical
   )
{
   /* SW7125-1285: To ensure that release queue and uiReleaseWriteOffset are updated in the 
    * correct order, declare the following two variables as volatile.  
    * When optimization was turned on, the compiler was reordering the writes to the release queue
    * and the shadow write offset.  There was a brief interval when the release queue 
    * contained a stale value.  This stale value made it impossible for AVD to properly manage
    * the picture buffers and PPB's were lost (leaked).
    */
   volatile uint32_t uiReleaseWriteOffset;
   volatile BXVD_P_PictureReleaseQueue * pReleaseQue;

   BDBG_ENTER( BXVD_Decoder_S_ReleaseQ_AddPPB );

   BDBG_ASSERT(pPPBPhysical);

   /*  Initialize the local variables. */
   BXVD_P_RELEASE_QUEUE_GET_ADDR( hXvdCh, pReleaseQue );
   BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset )

   /* Verify that the offset in in range */

   if ( ( uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE ) >= BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE  )
   {
      BXVD_DBG_ERR(hXvdCh,("BXVD_Decoder_S_ReleaseQ_AddPPB: out of range offset %d", uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE));
      goto Done;
   }

   /* "pPPBPhysical" should always be valid. */

   if ( ( NULL != pPPBPhysical ) )
   {

      BXVD_Decoder_S_Printf("relOffset:%d 0x%x \n",
            uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
            pPPBPhysical
            );

      /* Add the the element  to the release queue.  */

      pReleaseQue->display_elements[ uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE ] = pPPBPhysical;

#if BXVD_P_FW_HIM_API
      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)pReleaseQue, sizeof(BXVD_P_PictureReleaseQueue)));
#endif

      /* SW7125-1285: read the value back to ensure that is had been written to memory.
       * Needed for HIM; the PPB is being written to DRAM and the shadow write
       * offset to a register.
       */
      hXvdCh->stDecoderContext.stLogData.pLastPPBReleased = pReleaseQue->display_elements[ uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE ];

      /* Increment the release queue write offset and write it back out. */

      BXVD_P_INCREMENT_2BASED_OFFSET( uiReleaseWriteOffset, 1 );
      BXVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset )

      /* SW7125-1285: read the value back to ensure that is had been written to memory.
       * Needed for HIM; the PPB is being written to DRAM and the shadow write
       * offset to a register.
       */
      BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( 
                     hXvdCh, 
                     hXvdCh->stDecoderContext.stLogData.uiShadowIndexAfterLastRelease 
                     )

      /* For debug. */
      hXvdCh->stDecoderContext.stLogData.uiPicturesToAvd++;

   }

Done:

   BDBG_LEAVE( BXVD_Decoder_S_ReleaseQ_AddPPB );

   return;

}  /* End of BXVD_Decoder_S_ReleaseQ_AddPPB() */


void BXVD_Decoder_S_DqtInit( BXVD_ChannelHandle hXvdCh )
{
   BDBG_ASSERT(hXvdCh);

   /*
    * initialize the DQTM parameters
    */
   hXvdCh->stDecoderContext.stDqtCntxt.bConvertGopToUniPics = false;
   hXvdCh->stDecoderContext.stDqtCntxt.uiPreviousWrOffset = 0;
   hXvdCh->stDecoderContext.stDqtCntxt.uiTimesOffsetRepeated = 0;

   hXvdCh->stDecoderContext.stDqtCntxt.bDqtEnabled =  hXvdCh->stDecoderContext.bReversePlayback;

   hXvdCh->stDecoderContext.stDqtCntxt.uiDeadlockThreshold = BXVD_DECODER_DQT_DEADLOCK_THRESHOLD;
   hXvdCh->stDecoderContext.stDqtCntxt.bTruncatingGop = false;
   hXvdCh->stDecoderContext.stDqtCntxt.uiCurrentPicTag = -1;
   hXvdCh->stDecoderContext.stDqtCntxt.bValidPicTag = false;

   BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET(
                  hXvdCh,
                  hXvdCh->stDecoderContext.stDqtCntxt.uiReverseReadOffset
                  );
   return;

}


static bool  BXVD_Decoder_S_DeliveryQ_CheckForDeadlock(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiNumPicturesToProcess
   )
{
   bool     bDeadLock = false;
   uint32_t uiPicturesOnUniQue;

   BXVD_Decoder_P_DQTContext * pDQTCntxt = &( hXvdCh->stDecoderContext.stDqtCntxt );

   uiPicturesOnUniQue = BXVD_Decoder_S_UnifiedQ_GetPictureCount( hXvdCh );

   /*
    * There are two potential deadlock conditions.
    *
    * One is at the end of a stream.  Since we only mark the beginning of
    * GOPs and not their end, the DQT code will not push out the last
    * GOP since it doesn't see the start of the next one.
    *
    * The other could occur when the firmware is waiting for a PPB to be freed
    * and the DQT code is waiting for the start of the next GOP.
    *
    * The system is not deadlocked if there are more pictures to process as
    * indicated by "uiNumPicturesToProcess".  The system is not deadlocked 
    * if XDM is still working on the previous GOP as indicated by "uiPicturesOnUniQue".
    * If "uiPicturesOnUniQue" is equal to 1, the picture could be from the previous
    * decode (i.e. hold last picture).
    * This is a crude approach, but we declare a  deadlock situation if the Display Queue
    * write pointer has not advanced in "uiDeadlockThreshold" vsync's.
    */
   if (( 0 != uiNumPicturesToProcess ) || ( uiPicturesOnUniQue > 1 ))
   {
      pDQTCntxt->uiTimesOffsetRepeated = 0;
   }
   else
   {
      pDQTCntxt->uiTimesOffsetRepeated++;

      if ( pDQTCntxt->uiTimesOffsetRepeated > pDQTCntxt->uiDeadlockThreshold )
      {
         bDeadLock = true;

         BXVD_Decoder_S_Printf("\ndeadlock!!!\n", NULL );
      }
   }

   return bDeadLock;

}

static void BXVD_Decoder_S_DeliveryQ_ReleaseGopTail(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_P_LocalState * pstLocalState
   )
{
   bool                    bDoneProcessingGop;
   bool                    bNonDependentPicture;
   BXVD_P_DisplayElement   stDisplayElement;
   BXVD_Decoder_P_DQTContext *  pDQTCntxt;
   bool bPicturesToDrop;

   uint32_t uiDqReadOffset;
   uint32_t uiWriteOffset;
   uint32_t uiTempOffset;

   BXVD_P_PictureDeliveryQueue *          pstDeliveryQueue;

   BXVD_Decoder_P_PictureContextQueue *   pstContextQueue = &(hXvdCh->stDecoderContext.stPictureContextQueue);

   BXVD_Decoder_P_PictureContext *        pstTempPicCntxt;

   BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiDqReadOffset );

   BXVD_P_DELIVERY_QUEUE_GET_ADDR( hXvdCh, pstDeliveryQueue );

#if BXVD_P_FW_HIM_API
   BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)pstDeliveryQueue, sizeof(BXVD_P_PictureReleaseQueue)));
#endif

   uiWriteOffset = pstLocalState->uiDeliveryQueueWriteOffset;

   /*
    * Initialize the local variables.
    */
   pDQTCntxt = &( hXvdCh->stDecoderContext.stDqtCntxt );


   /* This routine is called when a GOP is being truncated.
    * While the pictures at the head of the GOP are being converted to 
    * unified pictures, this routine releases the pictures at the tail.
    */

   /* Are we done processing this GOP?
    * If the offset used to read the queue in reverse (uiReverseReadOffset) is
    * equal to the Delivery Queue read offset (uiDqReadOffset), all the PPB's
    * have been converted to Unified Pictures.
    * The check of "bConvertGopToUniPics" was needed for the implementation
    * prior to XDM.  It should be redundant at this time, but I want to do some
    * more testing before I remove it.
    */
   bDoneProcessingGop = ( pDQTCntxt->uiReverseReadOffset == uiDqReadOffset );
   bDoneProcessingGop &= ( false == pDQTCntxt->bConvertGopToUniPics );

   bPicturesToDrop = ( pDQTCntxt->uiEndOfGopOffset != uiWriteOffset );

   if ( true == bPicturesToDrop ) 
   {
      BXVD_Decoder_S_Printf("\nrelTail enter: Rr=%d DQ:%d/%d CQwr=%d Eog=%d done=%d\n",
                        pDQTCntxt->uiReverseReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        uiDqReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        uiWriteOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        pstContextQueue->uiWriteOffset,
                        pDQTCntxt->uiEndOfGopOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        bDoneProcessingGop
                        );
   }

   /*
   ** There are still pictures to throw away if "uiEndOfGopOffset" hasn't caught up to
   ** the delivery queue write offset (i.e. "ui32WrPtr")
   */
   while( pDQTCntxt->uiEndOfGopOffset != uiWriteOffset )
   {

      /* Get the address of the PPB */
      stDisplayElement.pPPBPhysical = (BXVD_P_PPB *)pstDeliveryQueue->display_elements[ pDQTCntxt->uiEndOfGopOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE];

      BMEM_ConvertOffsetToAddress(
            hXvdCh->hSystemHeap,
            (uint32_t)( stDisplayElement.pPPBPhysical ),
            (void *)&( stDisplayElement.pPPB )
            );

      stDisplayElement.pPPB = (BXVD_P_PPB *)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(stDisplayElement.pPPB, hXvdCh);

      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, stDisplayElement.pPPB, sizeof( BXVD_P_PPB)));

      /* Determine the picture type; only look at the picture tag for
       * "non-dependent" pictures.
       */
      if ( !( stDisplayElement.pPPB->flags & BXVD_P_PPB_MULTIVIEW_FIELD_MASK )
            || ( stDisplayElement.pPPB->flags & BXVD_P_PPB_MULTIVIEW_BASE_FLAG )
         )
      {
         bNonDependentPicture = true;
      }
      else
      {
         bNonDependentPicture = false;
      }

      /*
      ** If "picture_tag" doesn't match the saved value, we've reached the start of
      ** the next GOP. We're done cleaning up this GOP, update the appropriate offsets.
      ** If a "dependent" picture, simply drop it.  GOPs always start on a "single" or base picture.
      */
      if ( pDQTCntxt->uiCurrentPicTag != stDisplayElement.pPPB->picture_tag
            && true == bNonDependentPicture
         )
      {
         pDQTCntxt->bTruncatingGop = false;

         BXVD_Decoder_S_Printf("\t old tag = %u  new tag =%u\n", pDQTCntxt->uiCurrentPicTag, stDisplayElement.pPPB->picture_tag );

         if ( bDoneProcessingGop )
         {
            /*
            ** We'll hit this block only if the entire "head" of the GOP has been processed.
            ** If the "head" is still being processed, the logic in "BXVD_Decoder_S_DqtReadOffsetUpdate()"
            ** will update the read offsets.
            */
            uiDqReadOffset = pDQTCntxt->uiEndOfGopOffset;
            BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( hXvdCh, uiDqReadOffset );
            pDQTCntxt->uiReverseReadOffset = pDQTCntxt->uiEndOfGopOffset;
         }

         break;
      }

      /* Release the picture and invalidate the entry in the picture context queue. */

      BXVD_Decoder_S_Printf("\t releaseTail:: eog %d ", pDQTCntxt->uiEndOfGopOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE );

      BXVD_Decoder_S_ReleaseQ_AddPPB( hXvdCh, stDisplayElement.pPPBPhysical );

      pstTempPicCntxt = &(pstContextQueue->astPictureContext[ pDQTCntxt->uiEndOfGopOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE ]);

      /*
       * Increment the "uiEndOfGopOffset" offset.
       */
      BXVD_P_INCREMENT_2BASED_OFFSET( pDQTCntxt->uiEndOfGopOffset, 1 );

      /* For debug. */
      hXvdCh->stDecoderContext.stLogData.uiPicturesFromAvd++;

      /*
       * Jump the Picture Context Queue write pointer ahead if it is lagging.
       * This is the normal mode of operation once "uiEndOfGopOffset" has caught
       * up to "pstUnifiedQueue->uiWriteOffset".
       * It can also happen if a PPB is added to the delivery queue between the call
       * to "BXVD_Decoder_P_UpdatePictureContextQueue() and this routine.
       */
      uiTempOffset = pstContextQueue->uiWriteOffset + BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

      if ( pDQTCntxt->uiEndOfGopOffset <= uiWriteOffset
            && ( uiTempOffset < pDQTCntxt->uiEndOfGopOffset
                  || uiTempOffset > uiWriteOffset )
         )
      {
         pstContextQueue->uiWriteOffset = pDQTCntxt->uiEndOfGopOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      }
      else if ( uiTempOffset < pDQTCntxt->uiEndOfGopOffset && uiTempOffset > uiWriteOffset )
      {
         pstContextQueue->uiWriteOffset = pDQTCntxt->uiEndOfGopOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      }

   }


   if ( bDoneProcessingGop && ( pDQTCntxt->uiEndOfGopOffset == uiWriteOffset  ))
   {
      /*
      ** We'll hit this block if
      ** - the entire "head" of the GOP has been processed
      ** - the "tail" is being cleaned up
      ** - the first picture of the next GOP has NOT been found
      **
      ** The preceding can happen if
      ** - the video decoder is really slow delivering the pictures in a GOP
      ** - or we've made it to the beginning of the stream and need to restart at the end
      **
      ** If the "head" is still being processed, the logic in "BXVD_Decoder_S_DqtReadOffsetUpdate()"
      ** will update the read offsets.
      */
      uiDqReadOffset = pDQTCntxt->uiEndOfGopOffset;
      BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( hXvdCh, uiDqReadOffset );

      pDQTCntxt->uiReverseReadOffset = pDQTCntxt->uiEndOfGopOffset;
   }

   if ( true == bPicturesToDrop )   
   {
      BXVD_Decoder_S_Printf("relTail exit: Rr=%d DQ:%d/%d CQwr=%d Eog=%d playRev=%d\n",
                        pDQTCntxt->uiReverseReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        uiDqReadOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        uiWriteOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        pstContextQueue->uiWriteOffset,
                        pDQTCntxt->uiEndOfGopOffset-BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE,
                        pDQTCntxt->bConvertGopToUniPics
                        );
   }


   return;

}

/***************************************************************************
 *
 *
 *
 ***************************************************************************/



/***************************************************************************
 *
 *   Public Routines
 *
 ***************************************************************************/

/*
 * Returns the number of pictures on the unified picture queue.  For MVC,
 * this will be the number of picture "sets", i.e. the dependent pictures
 * are not included in the count.
 */
BERR_Code BXVD_Decoder_GetPictureCount_isr(
   BXVD_ChannelHandle hXvdCh,
   uint32_t *puiPictureCount
   )
{
   BDBG_ENTER( BXVD_Decoder_GetPictureCount_isr );

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncGetPicCount );


   if ( true == hXvdCh->stDecoderContext.bDecoderHasBeenInitialized )
   {
      *puiPictureCount = BXVD_Decoder_S_UnifiedQ_GetPictureCount( hXvdCh );
   }
   else
   {
      *puiPictureCount = 0;
   }

   BXVD_Decoder_S_Printf("_GetPictureCount_isr:: count = %d\n", *puiPictureCount );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncGetPicCount );

   BDBG_LEAVE( BXVD_Decoder_GetPictureCount_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_GetPictureCount_isr */


/*
 *
 */
BERR_Code BXVD_Decoder_PeekAtPicture_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t uiIndex,
         BXDM_Picture **pUnifiedPicture
         )
{
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;

   BDBG_ENTER( BXVD_Decoder_PeekAtPicture_isr );

   /* TODO: add code to use the index */
   BSTD_UNUSED( uiIndex );

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncPeekAtPic );

   BXVD_Decoder_S_UnifiedQ_UnifiedContextGet( hXvdCh, &pstUnifiedContext, false );

   *pUnifiedPicture = &(pstUnifiedContext->stUnifiedPicture);

   BDBG_LEAVE( BXVD_Decoder_PeekAtPicture_isr );

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncPeekAtPic );

   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_PeekAtPicture_isr */

/*
 *
 */
BERR_Code BXVD_Decoder_GetNextPicture_isr(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Picture **pUnifiedPicture
         )
{
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;

   BDBG_ENTER( BXVD_Decoder_GetNextPicture_isr );

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncGetNextPic );

   BXVD_Decoder_S_Printf("_GetNextPicture_isr:: ", NULL );

   BXVD_Decoder_S_UnifiedQ_UnifiedContextGet( hXvdCh, &pstUnifiedContext, true );

   *pUnifiedPicture = &(pstUnifiedContext->stUnifiedPicture);

   /*
    * Now that the PPB is being "processed", enqueue any user data.
    */
   if( pstUnifiedContext->pPPB->user_data )
   {
      BXVD_DecoderTimer_P_SnapshotCallbackStartTime( hXvdCh, BXVD_DecoderTimer_P_eCbUserData );

      /*
       * uiSerialNumber is passed to userdata where it is stored as uiDecoderPictureId.
       * See Jira SW7425-1780 for an explanation.
       */
      BXVD_P_Userdata_EnqueueDataPointer(
               hXvdCh,
               pstUnifiedContext->pPPB->protocol,
               (unsigned long)pstUnifiedContext->pPPB->user_data ,
               pstUnifiedContext->pPPB->flags,
               pstUnifiedContext->pPPB->pulldown,
               pstUnifiedContext->pPPB->pts,
               pstUnifiedContext->stUnifiedPicture.uiSerialNumber);

      BXVD_DecoderTimer_P_SnapshotCallbackEndTime( hXvdCh, BXVD_DecoderTimer_P_eCbUserData );
   }

   /* For debug. */
   hXvdCh->stDecoderContext.stLogData.uiPicturesToPP++;

#if BXVD_DM_ENABLE_YUV_GRAB_MODE

   if( true == hXvdCh->stDecoderContext.bGrabPpb )
      BXVD_Decoder_S_Dbg_DumpPPB( pstUnifiedContext->pPPB );

   if( true == hXvdCh->stDecoderContext.bGrabYuv )
      BXVD_Decoder_S_Dbg_DumpYUV( hXvdCh, pstUnifiedContext->pPPB );

   if( true == hXvdCh->stDecoderContext.bGrabRvc )
      BXVD_Decoder_S_Dbg_DumpRVC( hXvdCh );

#endif

   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncGetNextPic );


   BDBG_LEAVE( BXVD_Decoder_GetNextPicture_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_GetNextPicture_isr */

/*
 *
 */
BERR_Code BXVD_Decoder_ReleasePicture_isr(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Picture *pUnifiedPicture,
         const BXDM_Decoder_ReleasePictureInfo *pReleasePictureInfo
         )
{
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;

   BDBG_ENTER( BXVD_Decoder_ReleasePicture_isr );

   BSTD_UNUSED(pReleasePictureInfo);

   BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncReleasePic );

   BXVD_Decoder_S_Printf("_ReleasePicture_isr:: ", NULL );

   pstUnifiedContext = pUnifiedPicture->pPrivateContext;

   BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease( hXvdCh, pstUnifiedContext );

   /* For debug. */
   hXvdCh->stDecoderContext.stLogData.uiPicturesFromPP++;


   BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncReleasePic );

   BDBG_LEAVE( BXVD_Decoder_ReleasePicture_isr );

   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_ReleasePicture_isr */


/*
 *
 */
BERR_Code
BXVD_Decoder_GetPictureDropPendingCount_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t *puiPictureDropPendingCount
         )
{
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;

   uint32_t uiNumToBeDropped;

   BXVD_P_GET_DROP_COUNT( hXvdCh, uiNumToBeDropped );

   BDBG_ENTER( BXVD_Decoder_GetPictureDropPendingCount_isr );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( puiPictureDropPendingCount );

   /* The "pending drop count" is difference between the number of pictures
    * that the Picture Provider has requested to be dropped and the number
    * that AVD has actually dropped.
    * When this value transistions from non-zero to zero, the Picture Provider
    * will transistion from VSYNC to TSM mode.  At this point, the PP will need
    * a valid coded PTS.  For this reason, "uiDropCount" is a sampled version of
    * "n_drop" which is only updated when a PPB contains a coded PTS.
    *
    * Never allow the pending drop count to be less than '0'.  If more pictures have been
    * dropped than was requested, return a pending drop of '0'.
    *
    * SW7405-3929/-3925: Check that there are pictures in the queue, before asking for the context
    * (contents of the returned context are invalid if no pictures available)
    * If this is not done, pending drops can "ping-pong" between current value
    * and zero, causing selection mode to ping-pong between vTSM and TSM mode 
    */
   if ( BXVD_Decoder_S_UnifiedQ_GetPictureCount(hXvdCh) )
   {
      BXVD_Decoder_S_UnifiedQ_UnifiedContextGet( hXvdCh, &pstUnifiedContext, false );

      if ( uiNumToBeDropped < pstUnifiedContext->uiDropCount )
      {
         *puiPictureDropPendingCount = 0;
      }
      else
      {
         *puiPictureDropPendingCount = uiNumToBeDropped - pstUnifiedContext->uiDropCount;
      }
   }
   else
   {
      /* No pictures available in the unified Q, so we need to use previous data to supply the drop count ... */
      if ( uiNumToBeDropped < hXvdCh->stDecoderContext.uiDropCountSnapshot)
      {
         *puiPictureDropPendingCount = 0;
      }
      else
      {
         *puiPictureDropPendingCount = uiNumToBeDropped - hXvdCh->stDecoderContext.uiDropCountSnapshot;
      }
   }

   BDBG_LEAVE( BXVD_Decoder_GetPictureDropPendingCount_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_GetPictureDropPendingCount_isr */

/*
 *
 */
BERR_Code
BXVD_Decoder_RequestPictureDrop_isr(
         BXVD_ChannelHandle hXvdCh,
         uint32_t *puiPictureDropRequestCount
         )
{
   uint32_t uiDropsRequested = 0;

   uint32_t uiDropCount;

   BXVD_P_GET_DROP_COUNT( hXvdCh, uiDropCount );

   BDBG_ENTER( BXVD_Decoder_RequestPictureDrop_isr );

   BDBG_ASSERT( hXvdCh );

   /* We only request more drops if the decoder has caught up with the previous drop request.
    */
   if ( uiDropCount <= hXvdCh->stDecoderContext.uiDropCountSnapshot )
   {
      if ( *puiPictureDropRequestCount > BXVD_DECODER_MAX_FIELDS_TO_DROP )
      {
         /* PR47521: We need to cap the max requested drop count to prevent scenarious where a
          * large drop@decode request would occur that would normally recover on it's own.
          * (e.g. a unmarked stream discontinuity or stream wrap.).
          * We really only want to do a drop@decode if the stream is muxed poorly and/or we're
          * trying to do STC fast forward trick modes.  In such scenarioes, if we still need
          * more drops to catch up, the Picture Provider can always request more after the
          * initial drop has occurred.
          */
         uiDropsRequested = BXVD_DECODER_MAX_FIELDS_TO_DROP;

      }
      else
      {
         /* The decoder drops 2 fields at a time; ensure that "uiPictureDropRequestCount"
          * is an even value.
          */
         uiDropsRequested = *puiPictureDropRequestCount & ~0x01;
      }
      
      uiDropCount += uiDropsRequested;
      BXVD_P_SET_DROP_COUNT( hXvdCh, uiDropCount );

   }

   /* SE7405-3925: Update the actual number of drops requested from the decoder, to allow the caller
      to accurately update the drop request count status ... */
   *puiPictureDropRequestCount = uiDropsRequested;

   BDBG_LEAVE( BXVD_Decoder_RequestPictureDrop_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_RequestPictureDrop_isr */


/* To help debug queue issues. The previous delivery queue write offset should always be
 * equal to or a little behind the current write offset.  A big jump might indicate
 * an issue with the offsets.
 */
#define BXVD_DECODER_S_TYPICAL_NUM_NEW_PICS  5

/*
 *
 */
BERR_Code
BXVD_Decoder_DisplayInterruptEvent_isr(
         BXVD_ChannelHandle hXvdCh
         )
{
   BXVD_Decoder_P_LocalState stLocalDecoderState;
   BXVD_Decoder_P_LocalState * pstLocalState;

   uint32_t uiWriteOffsetDelta;

   BDBG_ENTER( BXVD_Decoder_DisplayInterruptEvent_isr );

   BDBG_ASSERT(hXvdCh);

   /* Hold off processing until the queues have been intialized. */

   if ( true == hXvdCh->stDecoderContext.bDecoderHasBeenInitialized )
   {
      BXVD_DecoderTimer_P_SnapshotFunctionStartTime( hXvdCh, BXVD_DecoderTimer_P_eFuncDisplayIsr );

      /* Initialize the local variable(s). */

      pstLocalState = &(stLocalDecoderState);

      hXvdCh->stDecoderContext.stCounters.uiVsyncCount++;

      /* Snapshot the delivery queue write offset so that it
       * is constant during the execution of the decoder ISR.
       */
      BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( hXvdCh, pstLocalState->uiDeliveryQueueWriteOffset );

      /* To help debug queue issues. uiPreviousDeliveryQWriteOffset should always be
       * equal to or a little behind the current write offset.  A big jump might indicate
       * an issue with the offsets.
       */
      BXVD_P_GET_QUEUE_DEPTH(
            hXvdCh->stDecoderContext.uiPreviousDeliveryQWriteOffset, 
            pstLocalState->uiDeliveryQueueWriteOffset, 
            uiWriteOffsetDelta 
            );

      if ( uiWriteOffsetDelta > BXVD_DECODER_S_TYPICAL_NUM_NEW_PICS )
      {
         uint32_t uiInstanceId = hXvdCh->ulChannelNum & 0xF;
         uiInstanceId |= ( hXvdCh->pXvd->uDecoderInstance & 0xF ) << 4 ;

         BDBG_MODULE_MSG( BXVD_QDBG, ("%03x:[%02x.xxx] %d pictures on delivery Q, WR offsets: previous::%d current:%d",
                                 hXvdCh->stDecoderContext.stCounters.uiVsyncCount & 0xFFF,
                                 uiInstanceId,
                                 uiWriteOffsetDelta,
                                 hXvdCh->stDecoderContext.uiPreviousDeliveryQWriteOffset, 
                                 pstLocalState->uiDeliveryQueueWriteOffset
                                 ));
      }

      hXvdCh->stDecoderContext.uiPreviousDeliveryQWriteOffset = pstLocalState->uiDeliveryQueueWriteOffset;

      /* process the pictures on the delivery queue. */
      BXVD_Decoder_S_UnifiedQ_Update( hXvdCh, pstLocalState );

      BXVD_Decoder_S_Log_Print( hXvdCh, pstLocalState, false );

      BXVD_DecoderTimer_P_SnapshotFunctionEndTime( hXvdCh, BXVD_DecoderTimer_P_eFuncDisplayIsr );
      BXVD_DecoderTimer_P_PrintResults( hXvdCh );

   }

   BDBG_LEAVE( BXVD_Decoder_DisplayInterruptEvent_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_DisplayInterruptEvent_isr */

/* SW7425-1064: called at open channel time to initialize the queues.  Previously the 
 * initialization occurred  when BXVD_Decoder_GetDMInterface was called.  However ..._GetDMInterface
 * gets called multiple times when installing/uninstalling the XMO; resetting the queues can
 * not occur at that time.
 */
BERR_Code BXVD_Decoder_OpenChannel(
   BXVD_ChannelHandle hXvdCh
   )
{
   BDBG_ENTER( BXVD_Decoder_OpenChannel );

   hXvdCh->stDecoderContext.bHostSparseMode = false;

   /* Initialize the Unified Picture Queue */
   BXVD_Decoder_S_UnifiedQ_Initialize( hXvdCh );

   BDBG_LEAVE( BXVD_Decoder_OpenChannel );

   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_OpenChannel */



/*
 *
 */
BERR_Code
BXVD_Decoder_GetDMInterface(
         BXVD_ChannelHandle hXvdCh,
         BXDM_Decoder_Interface *pstDecoderInterface,
         void **pPrivateContext
         )
{
   BDBG_ENTER( BXVD_Decoder_GetDMInterface );

   BDBG_ASSERT( pstDecoderInterface );
   BDBG_ASSERT( pPrivateContext );

   BKNI_Memset( pstDecoderInterface, 0, sizeof( BXDM_Decoder_Interface ) );

   pstDecoderInterface->getPictureCount_isr = (BXDM_Decoder_GetPictureCount_isr) BXVD_Decoder_GetPictureCount_isr;
   pstDecoderInterface->peekAtPicture_isr = (BXDM_Decoder_PeekAtPicture_isr) BXVD_Decoder_PeekAtPicture_isr;
   pstDecoderInterface->getNextPicture_isr = (BXDM_Decoder_GetNextPicture_isr) BXVD_Decoder_GetNextPicture_isr;
   pstDecoderInterface->releasePicture_isr = (BXDM_Decoder_ReleasePicture_isr) BXVD_Decoder_ReleasePicture_isr;
   pstDecoderInterface->getPictureDropPendingCount_isr = (BXDM_Decoder_GetPictureDropPendingCount_isr) BXVD_Decoder_GetPictureDropPendingCount_isr;
   pstDecoderInterface->requestPictureDrop_isr = (BXDM_Decoder_RequestPictureDrop_isr) BXVD_Decoder_RequestPictureDrop_isr;
   pstDecoderInterface->displayInterruptEvent_isr  = (BXDM_Decoder_DisplayInterruptEvent_isr) BXVD_Decoder_DisplayInterruptEvent_isr;
   pstDecoderInterface->displayInterruptStatus_isr = NULL;

   *pPrivateContext = hXvdCh;

   BDBG_LEAVE( BXVD_Decoder_GetDMInterface );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_GetDMInterface */

/*
 *
 */
BERR_Code BXVD_Decoder_GetPPBParameterQueueInfo_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PPBParameterInfo* apstPPBParameterInfo[],
   uint32_t uiPPBParameterInfoCount,
   uint32_t *puiValidPPBParameterInfoCount
   )
{
   uint32_t i;
   BXVD_Decoder_P_UnifiedPictureQueue * pstUnifiedQueue = &hXvdCh->stDecoderContext.stUnifiedPictureQueue;
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;

   BDBG_ENTER( BXVD_Decoder_GetPPBParameterQueueInfo_isr );

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT(apstPPBParameterInfo);
   BDBG_ASSERT(puiValidPPBParameterInfoCount);
   BDBG_ASSERT(uiPPBParameterInfoCount > 0);

   *puiValidPPBParameterInfoCount = 0;

   pstUnifiedContext = pstUnifiedQueue->pstContextTail;

   for (i = 0; i < uiPPBParameterInfoCount; i++)
   {
      /* Note: when DQT is enabled, the pictures are returned in display order, which
       * is reverse.
       */

      if ( NULL != pstUnifiedContext
         && true == pstUnifiedContext->bInUse )
      {
         apstPPBParameterInfo[i] = &(pstUnifiedContext->stUnifiedPicture);
         (*puiValidPPBParameterInfoCount)++;

         /*Note: this returns the dependent pictures as well. */
         pstUnifiedContext = pstUnifiedContext->pstNextUniContext;
      }
      else
      {
         apstPPBParameterInfo[i] = NULL;
      }
   }

   BDBG_LEAVE( BXVD_Decoder_GetPPBParameterQueueInfo_isr );
   return BERR_TRACE( BERR_SUCCESS );

}  /* end of BXVD_Decoder_GetPPBParameterQueueInfo_isr */

/*
 *
 */
BERR_Code
BXVD_Decoder_SetCRCMode_isr(
         BXVD_ChannelHandle hXvdCh,
         bool bEnable)
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_Decoder_SetCRCMode_isr);

   BDBG_ASSERT( hXvdCh );

   hXvdCh->stDecoderContext.bCRCMode = bEnable;

   BXVD_Decoder_S_Printf("BXVD_Decoder_SetCRCMode_isr(%d)\n", bEnable);

   BDBG_LEAVE(BXVD_Decoder_SetCRCMode_isr);

   return BERR_TRACE( rc );
}

/*
 *
 */
BERR_Code
BXVD_Decoder_GetCRCMode_isr(
         BXVD_ChannelHandle hXvdCh,
         bool *pbEnable)
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_Decoder_GetCRCMode_isr);

   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pbEnable );

   *pbEnable = hXvdCh->stDecoderContext.bCRCMode;

   BDBG_LEAVE(BXVD_Decoder_GetCRCMode_isr);

   return BERR_TRACE( rc );
}

/*
 *
 */
BERR_Code BXVD_Decoder_StartDecode_isr(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_Decoder_StartDecode_isr);

   BDBG_ASSERT( hXvdCh );
   BSTD_UNUSED(hXvdCh);

   BXVD_Decoder_S_Printf("BXVD_Decoder_StartDecode_isr\n", NULL );

   /* Nothing to do for the Unified Picture Queue, it is intialized
    * in "BXVD_Decoder_OpenChannel".  Also keep in mind that XDM
    * could still be holding onto pictures from a previous stream.
    * If the queue was reset, the decoder would not be able to return
    * the PPB's to the decoder.
    */

   /* TODO: should a snapshot be taken at start decode time instead of changing
    * on the fly?
    */
   /*hXvdCh->stDecoderContext.bCRCMode = hXvdCh->sDecodeSettings.bCrcMode;*/

#if 0 /* From DM V2. */
   if (!hXvdCh->stDMState.stChannel.bDMVersionDisplayed)
   {
      BXVD_DBG_WRN(hXvdCh,("Display Manager: [v%02d.%02d.%04d]",
                                BXVD_DM_P_VERSION_MAJOR,
                                BXVD_DM_P_VERSION_MINOR,
                                BXVD_DM_P_VERSION_SUBMINOR));
      hXvdCh->stDMState.stChannel.bDMVersionDisplayed = true;
   }

   BXVD_DMDBG_P_PrintNodes(hXvdCh);


   /* TODO: NA - We should probably remove the bPointerInitDone
    * variable altogether.  Right now, BXVD_INTR checks it before
    * calling into DM, so we'll leave it in to be compatible with the
    * old DM */
   hXvdCh->stDMStatus.bPointerInitDone = true;

#endif

   /* Initialize the DQT logic */

   BXVD_Decoder_S_DqtInit( hXvdCh );


   /* Initialize the picture context queue. */

   BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET(
            hXvdCh,
            hXvdCh->stDecoderContext.stPictureContextQueue.uiWriteOffset
            );

   hXvdCh->stDecoderContext.stPictureContextQueue.uiWriteOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

   /* FWAVD-289: Use the queue index to detect when a picture is being evaluated for the first time. */
   hXvdCh->stDecoderContext.uiPreviousWriteOffset = -1;

   /* Need to initialize in case the first PPB(s) don't contain a coded PTS. */
   hXvdCh->stDecoderContext.uiDropCountSnapshot = 0;

   /* Reset drop request count */
   BXVD_P_SET_DROP_COUNT( hXvdCh, 0 );

   /* Used to hold off processing in "BXVD_Decoder_DisplayInterruptEvent_isr"
    * until this routine has been called.  Implicit at this point of execution
    * is that the AVD queues have been initialized as well.  This is
    * essentially the old "eVideoState = BXVD_P_VideoState_DecodeStarted".
    */
   hXvdCh->stDecoderContext.bDecoderHasBeenInitialized = true;


   /* Conditionally initialize the decoder counter(s) */
   if ( false == hXvdCh->bPreserveCounters )
   {
      BKNI_Memset( &(hXvdCh->stDecoderContext.stCounters), 0, sizeof( BXVD_Decoder_Counters ) );
   }

   /* SW7425-3358: reset the chunk id. */
   hXvdCh->stDecoderContext.uiCurrentChunkId = 0;

   /* For debug. */
   BXVD_Decoder_S_Log_Init( hXvdCh );

   BKNI_Memset( &(hXvdCh->stDecoderContext.stLogData.stTimerData), 0, sizeof(BXVD_DecoderTimer_P_Data) );


#if BXVD_DM_ENABLE_YUV_GRAB_MODE

   hXvdCh->stDecoderContext.bGrabPpb = ( NULL == getenv("grabppb")) ? false : true;
   hXvdCh->stDecoderContext.bGrabYuv = ( NULL == getenv("grabyuv")) ? false : true;
   hXvdCh->stDecoderContext.bGrabRvc = ( NULL == getenv("grabrvc")) ? false : true;

   if ( true == hXvdCh->stDecoderContext.bGrabRvc )
   {
      BXVD_Reg_Write32_isr (hXvdCh->pXvd, hXvdCh->pXvd->stPlatformInfo.stReg.uiDecode_RVCCtl, 0x01);
   }
#endif

   BDBG_LEAVE(BXVD_Decoder_StartDecode_isr);

   return BERR_TRACE( rc );

}

/*
 *
 */
BERR_Code BXVD_Decoder_StopDecode_isr(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BXVD_Decoder_P_UnifiedPictureContext * pstUnifiedContext;
   BXVD_Decoder_P_DQTContext * pDQTCntxt = &( hXvdCh->stDecoderContext.stDqtCntxt );


   BDBG_ENTER(BXVD_Decoder_StopDecode_isr);

   BDBG_ASSERT( hXvdCh );

   BXVD_Decoder_S_Printf("BXVD_Decoder_StopDecode_isr:: ", NULL );

   /* Hold off processing in "BXVD_Decoder_DisplayInterruptEvent_isr"
    * until the queues have been reinitialized.
    */
   hXvdCh->stDecoderContext.bDecoderHasBeenInitialized = false;

   /*
    * Release any pictures on the Unified Picture Queue.
    * XDM could still be holding onto pictures, don't reset the queue.
    */
   while ( BXVD_Decoder_S_UnifiedQ_GetPictureCount( hXvdCh ) )
   {
      BXVD_Decoder_S_UnifiedQ_UnifiedContextGet( hXvdCh, &pstUnifiedContext, true );
      BXVD_Decoder_S_UnifiedQ_UnifiedContextRelease( hXvdCh, pstUnifiedContext );
   }

   /*
    * Conditionally clear the PVR state.
    */
   if ( false == hXvdCh->bPreserveState )
   {
      /* Disable the reverse playback of GOPs */
      hXvdCh->stDecoderContext.bReversePlayback = false;

      hXvdCh->stDecoderContext.bHostSparseMode = false;
   }

   /* In the event the Display Manager was cleaning up a "truncated" GOP,
    * clear the flags now.  Any pictures that remain at the end of the GOP
    * (in the delivery queue) will be cleaned up by the decoder.
    */
   pDQTCntxt->bTruncatingGop = false;
   pDQTCntxt->bValidPicTag = false;

   {
      /* Conditionally dump the state of the queues. */
      BXVD_Decoder_P_LocalState stLocalDecoderState;
      BXVD_Decoder_P_LocalState * pstLocalState;

      pstLocalState = &(stLocalDecoderState);

      BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( hXvdCh, pstLocalState->uiDeliveryQueueWriteOffset );

      BXVD_Decoder_S_Log_Print( hXvdCh, pstLocalState, true );
   }

   BDBG_LEAVE(BXVD_Decoder_StopDecode_isr);

   return BERR_TRACE( rc );
}

/*
 *
 */
BERR_Code BXVD_Decoder_WatchdogReset_isr(
   BXVD_ChannelHandle hXvdCh
   )
{
   BDBG_ENTER(BXVD_Decoder_WatchdogReset_isr);

   BDBG_ASSERT( hXvdCh );

   BXVD_Decoder_S_Printf("BXVD_Decoder_WatchdogReset_isr:: ", NULL );

   /* Hold off processing in "BXVD_Decoder_DisplayInterruptEvent_isr"
    * until the queues have been reinitialized.
    */
   hXvdCh->stDecoderContext.bDecoderHasBeenInitialized = false;

   /*
    * Reset the Unified Picture Queue.
    */
   BXVD_Decoder_S_UnifiedQ_Initialize( hXvdCh );

   BDBG_LEAVE(BXVD_Decoder_WatchdogReset_isr);

   return BERR_SUCCESS;
}

/*
 *
 */
BERR_Code BXVD_Decoder_GetCounters_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Decoder_Counters * pstCounters
   )
{
   BDBG_ENTER(BXVD_Decoder_GetCounters_isr);

   BDBG_ASSERT( hXvdCh );

   *pstCounters = hXvdCh->stDecoderContext.stCounters;

   BDBG_LEAVE(BXVD_Decoder_GetCounters_isr);

   return BERR_SUCCESS;
}

/*
 * Routines to support the "grab" debug mode.
 */
#if BXVD_DM_ENABLE_YUV_GRAB_MODE

/*
 * BXVD_Decoder_S_Dbg_SwapLong
 */
static uint32_t BXVD_Decoder_S_Dbg_SwapLong (
   uint32_t a
   )
{
   return ((a >> 24) & 0x000000FF)
            | ((a >>  8) & 0x0000FF00)
            | ((a <<  8) & 0x00FF0000)
            | ((a << 24) & 0xFF000000);
}

/*
 * BXVD_Decoder_S_Dbg_DestripeSection
 */
static int32_t BXVD_Decoder_S_Dbg_DestripeSection (
   unsigned char *from,
   unsigned char *to,
   int32_t image_wid,
   int32_t image_hgt,
   int32_t stripe_wid,
   int32_t stripe_hgt,
   uint32_t ulRangeRemapping
   )
{
   int32_t stripe_size = stripe_wid * stripe_hgt;
   int32_t pixels      = image_wid  * image_hgt;

   int32_t i;
   int32_t scol;
   int32_t icol;
   int32_t lpel;
   unsigned char *ptr, pel;

   /* memset (to, 0, pixels); */
   scol = 0;
   icol = 0;
   ptr  = from;

   for (i = 0; i < pixels; i++)
   {
      /* apply range mapping equation */

      pel = ptr [scol];
      if (ulRangeRemapping != 8)
      {
         lpel = (uint32_t) pel;

         lpel = ((((lpel - 128) *  (int32_t) ulRangeRemapping) + 4) >> 3) + 128;
         if (lpel > 255) pel = 255;
         else if (lpel < 0) pel = 0;
         else pel = (unsigned char) lpel;
      }

      *to++ = pel;

      /* If we reach the end of a stripe, move down to the
       * next strip to continue this row.
       */
      if (++scol == stripe_wid)
      {
         scol = 0;
         ptr += stripe_size;
      }

      /* If we reach the end of a row, go back to the 1st
       * stripe, but move down a row.  Reset everything.
       */
      if (++icol == image_wid)
      {
         int32_t edge_pixel, cnt;

         /* need to handle non four pixel align case */

         edge_pixel = (icol & 3);

         if ( edge_pixel )
         {
            to   -= edge_pixel;
            scol -= edge_pixel;

            for (cnt = 0; cnt < edge_pixel; cnt++)
            {
               pel = ptr [scol+3-cnt];

               if (ulRangeRemapping != 8)
               {
                  lpel = (uint32_t) pel;

                  lpel = ((((lpel - 128) *  (int32_t) ulRangeRemapping) + 4) >> 3) + 128;
                  if (lpel > 255) pel = 255;
                  else if (lpel < 0) pel = 0;
                  else pel = (unsigned char) lpel;
               }

               *to++ = pel;
            }
         }

         from += stripe_wid;
         ptr   = from;

         icol  = 0;
         scol  = 0;
      }
   }

   return 0;

}     /* end of BXVD_Decoder_S_Dbg_DestripeSection() */

/*
 * BXVD_Decoder_S_Dbg_DumpYUV
 */
void BXVD_Decoder_S_Dbg_DumpYUV(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_PPB *ppb
   )
{
   unsigned char *pYData;
   unsigned char *pUVData;
   uint32_t ulLumaRangeRemapping, ulChromaRangeRemapping;
   int32_t stripe_size = BXVD_P_StripeWidthLUT[hXvdCh->pXvd->uiDecode_StripeWidth];
   int32_t height;
   int32_t width;
   int32_t i, j;

   static FILE *fpYUV = NULL;
   static unsigned char pBuf[1920*1088], *pSwap;

   if (fpYUV== NULL)
   {
      if ((fpYUV=fopen("/Work/output.yuv", "wb"))==NULL)
      {
         BDBG_WRN(("Could not open output.yuv"));
         return;
      }
   }

   BDBG_WRN(("*** grabbing pic with width %d height %d, stripe_size: %d stripe height: %d",
				ppb->video_width, ppb->video_height, stripe_size, ppb->luma_stripe_height));

   height = ppb->video_height;
   width = ppb->video_width;

   /* Support range remapping during YUV grab */
   ulLumaRangeRemapping   = 0x08;
   ulChromaRangeRemapping = 0x08;

   if (ppb->protocol == BXVD_P_PPB_Protocol_eVC1)
   {
      ulLumaRangeRemapping   = ppb->other.vc1.range_remapping_ratio >> 16;
      ulChromaRangeRemapping = ppb->other.vc1.range_remapping_ratio & 0xffff;
   }

   BKNI_Printf("%d %d \n", ulLumaRangeRemapping, ulChromaRangeRemapping);

   /* Convert the two video buffers to logical addresses */
   BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, (uint32_t)ppb->luma_video_address, (void *)&pYData);
   BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, (uint32_t)ppb->chroma_video_address, (void *)&pUVData);

   BMEM_Heap_ConvertAddressToCached(hXvdCh->hPictureHeap, pYData, &pYData);
   BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hPictureHeap, pYData, (width*height)));

   BMEM_Heap_ConvertAddressToCached(hXvdCh->hPictureHeap, pUVData, &pUVData);
   BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hPictureHeap, pUVData, (width*height/2)));

   BXVD_Decoder_S_Dbg_DestripeSection(pYData, pBuf, width, height, stripe_size, ppb->luma_stripe_height, ulLumaRangeRemapping);

   /* byte swap here */
   /* if (bByteSwap) */
#if 0
   {
      for (i = 0; i < width * height; i+=4)
      *((uint32_t *)(pBuf + i)) = BXVD_Decoder_S_Dbg_SwapLong(*((uint32_t *)(pBuf + i)));
   }
#endif
    /* need to handle non four pixel align case
     */
   {
      pSwap = pBuf;
      for (i = 0; i < height; i++)
      {
         for (j = 0; j < (width & 0xfffc); j+=4)
         {
            *((uint32_t *)pSwap) = BXVD_Decoder_S_Dbg_SwapLong(*((uint32_t *)pSwap));
            pSwap+=4;
         }
         pSwap += (width & 0x3);
      }
   }

    /* Write the Y data */
    fwrite (pBuf, 1, width * height, fpYUV);

    BXVD_Decoder_S_Dbg_DestripeSection(pUVData, pBuf, width, height / 2, stripe_size, ppb->chroma_stripe_height, ulChromaRangeRemapping);

    /* byte swap here */
#if 0
    /* if (bByteSwap) */
    {
        for (i = 0; i < width * height; i+=4)
            *((uint32_t *)(pBuf + i)) = BXVD_Decoder_S_Dbg_SwapLong(*((uint32_t *)(pBuf + i)));
    }
#endif

   /* need to handle non four pixel align case
    */
   {
      pSwap = pBuf;
      for (i = 0; i < height/2; i++)
      {
         for (j = 0; j < (width & 0xfffc); j+=4)
         {
            *((uint32_t *)pSwap) = BXVD_Decoder_S_Dbg_SwapLong(*((uint32_t *)pSwap));
            pSwap+=4;
         }
         pSwap += (width & 0x3);
      }
   }

   /* Write the UV data (this also deinterleaves the UV  writing first U than V */
   for (i = 0; i < (width * height) / 2; i+=2)
      fwrite(pBuf + i, 1, 1, fpYUV);
   for (i = 1; i < (width * height) / 2; i+=2)
      fwrite(pBuf + i, 1, 1, fpYUV);
   fflush(fpYUV);

}     /* end of BXVD_Decoder_S_Dbg_DumpYUV() */

/*
 * BXVD_Decoder_S_Dbg_DumpRVC
 */
void BXVD_Decoder_S_Dbg_DumpRVC(
   BXVD_ChannelHandle hXvdCh
   )
{
   uint32_t size, start, end;
   uint32_t  currptr, buf_start, buf_end, buf_size;
   int32_t         contig;
   uint32_t      written;
   static unsigned char *pRVCdata = 0;
   static uint32_t lastrvcptr = 0xffffffff;
   static FILE *fpRVC= NULL;

   currptr    = BXVD_Reg_Read32_isr(hXvdCh->pXvd, hXvdCh->pXvd->stPlatformInfo.stReg.uiDecode_RVCPut);
   buf_start = BXVD_Reg_Read32_isr(hXvdCh->pXvd, hXvdCh->pXvd->stPlatformInfo.stReg.uiDecode_RVCBase);
   buf_end  = BXVD_Reg_Read32_isr(hXvdCh->pXvd, hXvdCh->pXvd->stPlatformInfo.stReg.uiDecode_RVCEnd);
   buf_end += 1;

   /* BDBG_WRN(("rvc registers (curr, start, end): 0x%x %x %x", currptr, buf_start, buf_end ));    */

   buf_size = buf_end - buf_start;
   if (buf_size <= 1)
      return;

   /*  for the initial case we set it to the start */
   if (0xffffffff == lastrvcptr)
      lastrvcptr = buf_start;

   /* check pointer */
   if ((currptr > buf_end) || (currptr < buf_start))
      return;

   /* calculate size to read */
   start = lastrvcptr;
   end   = currptr;
   lastrvcptr = currptr;
   if (end >= start)
   {
      contig  = 1;
      size    = end - start;
   }
   else
   {
      contig  = 0;
      size    = (buf_end - start) + (end - buf_start);
   }

   if (0 == size)
      return;

   if (fpRVC == NULL)
   {
      if ((fpRVC=fopen("/Work/rvc.bin", "wb"))==NULL)
      {
         return;
      }
   }

   /* pRVCdata contains a virtual pointer to the data */
   BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, start, (void *)&pRVCdata);
   
   BMEM_Heap_ConvertAddressToCached(hXvdCh->hPictureHeap, pRVCdata, &pRVCdata);
         
   BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hPictureHeap, pRVCdata, size));

   if (contig)
   {
      /* BCM_MBDL_MemRead_ToBuffer (start, size, buf, 1, D011, boardnum); */
      written = fwrite(pRVCdata, 1, size, fpRVC);
   }
   else
   {
      /*  get the first chunk */
      written = fwrite(pRVCdata, 1, buf_end - start, fpRVC);

      /* get the second chunk */
      /* the seond chunks starts at the buffer beginning - get its address */
      BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, buf_start, (void *)&pRVCdata);

      BMEM_Heap_ConvertAddressToCached(hXvdCh->hPictureHeap, pRVCdata, &pRVCdata);

      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hPictureHeap, pRVCdata, (end - buf_start)));

      written += fwrite(pRVCdata, 1, end - buf_start, fpRVC);
   }

   /* update the Get Pointer */
   BXVD_Reg_Write32_isr(hXvdCh->pXvd, hXvdCh->pXvd->stPlatformInfo.stReg.uiDecode_RVCGet, end);

   if (size != written)
   {
      BDBG_WRN (("GrabRvcFrame - error writing file size = %d written %d", size, written));
   }
   else
   {
      BDBG_WRN (("GrabRvcFrame - write %d bytes", size));
   }

   return;

}  /* end of BXVD_Decoder_S_Dbg_DumpRVC() */

/*
 * BXVD_Decoder_S_Dbg_DumpPPB
 */
void BXVD_Decoder_S_Dbg_DumpPPB(
   BXVD_P_PPB *ppb
   )
{
   static FILE *fpPPB = NULL;

   if (fpPPB == NULL)
   {
      if ((fpPPB=fopen("/Work/ppb_capture.bin", "wb"))==NULL)
      {
         return;
      }
   }

   fwrite(ppb, sizeof(BXVD_P_PPB), 1, fpPPB);

}  /* end of BXVD_Decoder_S_Dbg_DumpPPB() */

#endif /* End of BXVD_DM_ENABLE_YUV_GRAB_MODE */

