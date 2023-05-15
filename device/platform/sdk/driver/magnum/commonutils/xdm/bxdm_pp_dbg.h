/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_dbg.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 11/2/12 11:28a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_dbg.h $
 * 
 * Hydra_Software_Devel/16   11/2/12 11:28a btosi
 * SW7405-4736: added BXDM_PPDBC message
 * 
 * Hydra_Software_Devel/15   8/2/12 3:43p btosi
 * SW7405-4736: change the XDM uiInstanceId format to
 * uDecoderInstance.uiVDCRectangleNum
 * 
 * Hydra_Software_Devel/14   7/6/12 10:40a btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/13   6/27/12 9:35a btosi
 * SW7405-4736: added back in printing of the drop count
 * 
 * Hydra_Software_Devel/12   5/4/12 3:35p btosi
 * SW7425-2915: fixed issue with 2nd slot delay at startup and added
 * additional debug support for NRT mode
 * 
 * Hydra_Software_Devel/11   1/13/12 3:01p btosi
 * SW7405-4736: added debug support for MFD.bLast and picture-less
 * pictures
 * 
 * Hydra_Software_Devel/10   11/28/11 3:21p btosi
 * SW7405-4736: winnowed down the BXDM_PPQM and BXDM_PPDBG messages
 * 
 * Hydra_Software_Devel/9   5/10/11 1:41p btosi
 * SW7405-4736: added range checking and debug messages for the input
 * parameters
 * 
 * Hydra_Software_Devel/8   5/2/11 9:14a btosi
 * SW7405-4736: moved location of BXDM_PPDBG_P_PrintMFD
 * 
 * Hydra_Software_Devel/7   4/21/11 1:53p btosi
 * SW7405-4736: moved BXDM_PPQM_P_PrintUnifiedPicture to bxdm_pp_dbg.c,
 * tweaked the BXDM_PPQM message
 * 
 * Hydra_Software_Devel/6   4/12/11 2:19p btosi
 * SW7405-4736: added support for MFD debug messages
 * 
 * Hydra_Software_Devel/5   4/5/11 2:26p btosi
 * SW7405-4736: removed BXDM_PP_ENABLE_YUV_GRAB_MODE, grab is implemented
 * in bxvd_decoder.c
 * 
 * Hydra_Software_Devel/4   7/12/10 8:51a delkert
 * SW7335-781: removed temporary (broken) comment
 *
 * Hydra_Software_Devel/3   7/12/10 8:05a delkert
 * SW7335-781: Output warning when forcing picture selection override to
 * allow easier detection of conditions such as drop@decode and pcr
 * discontinuity
 *
 * Hydra_Software_Devel/2   3/9/10 3:12p btosi
 * SW7405-3245: build strings locally instead of calling BKNI_Vsnprintf,
 * shaved ~140 usec off the ISR execution time
 *
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_DBG_H__
#define bxdm_pp_DBG_H__

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bavc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

#define BXDM_PPDBG_FORMAT_INSTANCE_ID( _hXdmPP_ )   \
   _hXdmPP_->stDMConfig.uiInstanceID & 0xFF        \


#define BXDM_PPDBG_P_MAX_VSYNC_DEPTH 12
#define BXDM_PPDBG_P_MAX_DBG_LENGTH 1024

typedef enum BXDM_PPDBG_Selection
{
   BXDM_PPDBG_Selection_ePPBNotFound,
   BXDM_PPDBG_Selection_ePPBFound,
   BXDM_PPDBG_Selection_ePass,
   BXDM_PPDBG_Selection_eForce,
   BXDM_PPDBG_Selection_eLate,
   BXDM_PPDBG_Selection_eWait,
   BXDM_PPDBG_Selection_eFreeze,
   BXDM_PPDBG_Selection_eTooEarly,
   BXDM_PPDBG_Selection_eDrop,
   BXDM_PPDBG_Selection_eDelay,
   BXDM_PPDBG_Selection_eDependentPicture,

   BXDM_PPDBG_Selection_PolarityOverride_eBothField,
   BXDM_PPDBG_Selection_PolarityOverride_eProgressive,
   BXDM_PPDBG_Selection_PolarityOverride_e1stSlotRepeat,
   BXDM_PPDBG_Selection_PolarityOverride_e2ndSlotNextElement,
   BXDM_PPDBG_Selection_PolarityOverride_e2ndSlotNextPPB,
   BXDM_PPDBG_Selection_PolarityOverride_e2ndSlotInversion,
   BXDM_PPDBG_Selection_PolarityOverride_eSelectPrevious,
   BXDM_PPDBG_Selection_PolarityOverride_eRepeatPrevious,
   BXDM_PPDBG_Selection_PolarityOverride_eFICReset,

   /* Add new enums above this line */
   BXDM_PPDBG_Selection_eMax
} BXDM_PPDBG_Selection;

/*
 * Defines and macro(s) for building the debug messages.
 */
#define BXDM_PPDBG_P_STR_PREFIX     5     /* All the strings start with a five character prefix. */
#define BXDM_PPDBG_P_STR_SUFFIX     1     /* Always want a NULL terminator at the end of the string. */
#define BXDM_PPDBG_P_STR_PER_VSYNC  15    /* Except for the "stTSMString" string, 8 characters will be appended on each vsync.
                                           * Typically 5 or 7 characters will be appended to "stTSMString" on each vsync, but it
                                           * could be more than that if pictures are dropped.  A value of 15 will give us a little
                                           * wiggle room. */

#define BXDM_PPDBG_P_STR_LENGTH  BXDM_PPDBG_P_STR_PREFIX                                           \
                                 + ( BXDM_PPDBG_P_STR_PER_VSYNC *  BXDM_PPDBG_P_MAX_VSYNC_DEPTH )  \
                                 + BXDM_PPDBG_P_STR_SUFFIX                                         \


#define BXDM_PPDBG_P_APPEND_CHAR( _pStr, _char )                  \
   if ( _pStr->uiDebugStrOffset < BXDM_PPDBG_P_STR_LENGTH - 2 )   \
   {                                                              \
      _pStr->szDebugStr[ _pStr->uiDebugStrOffset++ ] = _char;     \
   }                                                              \


typedef struct BXDM_PPDBG_P_String
{
   char szDebugStr[BXDM_PPDBG_P_MAX_DBG_LENGTH];
   uint32_t uiDebugStrOffset;
} BXDM_PPDBG_P_String;

typedef struct BXDM_PPDBG_P_Info
{
   uint32_t uiVsyncCount;

   BXDM_PPDBG_P_String stHeaderString;
   BXDM_PPDBG_P_String stTSMString;
   BXDM_PPDBG_P_String stPendingDropString;
   BXDM_PPDBG_P_String stInterruptString;
   BXDM_PPDBG_P_String stSourcePolarityOverrideString;
   BXDM_PPDBG_P_String stCallbackString;
   BXDM_PPDBG_P_String stStateString;
   BXDM_PPDBG_P_String stState2String;
   BXDM_PPDBG_P_String stStcDeltaString;

   bool bPrintSPO;
   bool bPrintCallbacks;
   bool bPrintState2;
   bool bPrintDropCount;
   
   uint32_t abSelectionLogHeader[BXDM_PPDBG_P_MAX_VSYNC_DEPTH];
} BXDM_PPDBG_P_Info;

#define BXDM_PPDBG_Output_SPO_UNUSED_0       0x0001
#define BXDM_PPDBG_Output_SPO_UNUSED_1       0x0002
#define BXDM_PPDBG_Output_SPO_UNUSED_2       0x0004
#define BXDM_PPDBG_Output_SPO_UNUSED_4       0x0008

#define BXDM_PPDBG_Output_SPO_TopField       0x0010
#define BXDM_PPDBG_Output_SPO_BottomField    0x0020
#define BXDM_PPDBG_Output_SPO_SingleField    0x0040

#define BXDM_PPDBG_Output_SPO_Interlaced     0x0100
#define BXDM_PPDBG_Output_SPO_Progressive    0x0200
#define BXDM_PPDBG_Output_SPO_pToi           0x0400
#define BXDM_PPDBG_Output_SPO_iTop           0x0800

#define BXDM_PPDBG_Output_SPO_ProgBothField  0x1000
#define BXDM_PPDBG_Output_SPO_UNUSED_5       0x2000
#define BXDM_PPDBG_Output_SPO_UNUSED_6       0x4000
#define BXDM_PPDBG_Output_SPO_ProgRepeat     0x8000

#define BXDM_PPDBG_Callback_FirstPtsReady      0x00001
#define BXDM_PPDBG_Callback_FirstCodedPtsReady 0x00002
#define BXDM_PPDBG_Callback_FirstPtsPassed     0x00004
#define BXDM_PPDBG_Callback_PtsError           0x00008

#define BXDM_PPDBG_Callback_PtsStcOffset       0x00010
#define BXDM_PPDBG_Callback_IFrame             0x00020
#define BXDM_PPDBG_Callback_PictureParameters  0x00040
#define BXDM_PPDBG_Callback_TsmPassInASTMMode  0x00080

#define BXDM_PPDBG_Callback_RequestSTC         0x00100
#define BXDM_PPDBG_Callback_ClipStart          0x00200
#define BXDM_PPDBG_Callback_ClipStop           0x00400
#define BXDM_PPDBG_Callback_PictureMarker      0x00800

#define BXDM_PPDBG_Callback_PPBReceived        0x01000
#define BXDM_PPDBG_Callback_PictureUnderEvaluation      0x02000
#define BXDM_PPDBG_Callback_DecodeError        0x04000
#define BXDM_PPDBG_Callback_TSMResult          0x08000

#define BXDM_PPDBG_Callback_ExtensionData      0x10000
#define BXDM_PPDBG_Callback_ChunkDone          0x20000

#define BXDM_PPDBG_State_SelectionMode_TSM       0x00001
#define BXDM_PPDBG_State_SelectionMode_VSYNC     0x00002
#define BXDM_PPDBG_State_DisplayMode_TSM         0x00004
#define BXDM_PPDBG_State_DisplayMode_VSYNC       0x00008

#define BXDM_PPDBG_State_IgnoreCadenceMatch      0x00010
#define BXDM_PPDBG_State_PCRDiscontinuity        0x00020
#define BXDM_PPDBG_State_PCRPresent              0x00040
#define BXDM_PPDBG_State_STCInvalid              0x00080

#define BXDM_PPDBG_State_ProgressiveFramePulldown       0x00100
#define BXDM_PPDBG_State_ProgressiveStreamPulldown      0x00200
#define BXDM_PPDBG_State_ProgressiveSourceFormat        0x00400
#define BXDM_PPDBG_State_ProgressiveSequence            0x00800

#define BXDM_PPDBG_State_UseHwPcrOffset            0x01000
#define BXDM_PPDBG_State_480pTo480i                0x02000
#define BXDM_PPDBG_State_1080pTo1080i              0x04000
#define BXDM_PPDBG_State_60iTo60p                  0x08000
#define BXDM_PPDBG_State_24iTo24p                  0x10000
#define BXDM_PPDBG_State_240iTo240p                0x20000

/* Bit definitions for the second state word. */

/*#define BXDM_PPDBG_State2_StcInvalidAtStart     0x00001 */
#define BXDM_PPDBG_State2_StcStalled            0x00001
#define BXDM_PPDBG_State2_RequestedStcStall     0x00002
#define BXDM_PPDBG_State2_IgnorePicture         0x00004

#if BDBG_DEBUG_BUILD
BERR_Code BXDM_PPDBG_P_OutputLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_P_LocalState *pLocalState,
   const BAVC_MFD_Picture *pMFDPicture
   );

BERR_Code BXDM_PPDBG_P_Print(
   const BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_P_LocalState* pLocalState
   );

BERR_Code BXDM_PPDBG_P_SelectionLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PPDBG_Selection eSelectionInfo
   );

BERR_Code BXDM_PPDBG_P_OutputSPOLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiOverrideBits
   );

BERR_Code BXDM_PPDBG_P_CallbackTriggeredLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiCallbackTriggeredBits
   );

BERR_Code BXDM_PPDBG_P_StateLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiStateBits
   );

BERR_Code BXDM_PPDBG_P_State2Log(
   const BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiStateBits
   );

BERR_Code BXDM_PPDBG_P_StcDeltaLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PictureProvider_P_LocalState* pLocalState
   );

BERR_Code BXDM_PPDBG_P_DecoderDropLog(
   const BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiPendingDrop
   );

void BXDM_PPDBG_P_PrintNodes(
   const BXDM_PictureProvider_Handle hXdmPP
   );

void BXDM_PPDBG_P_PrintSelectionModeOverride(
   char *pMsg,
   const BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context *pPicture);

void BXDM_PPDBG_P_PrintEndSelectionModeOverride(
   const BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context *pPicture);

void BXDM_PPDBG_P_PrintMFD(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BAVC_MFD_Picture* pMFDPicture
   );

void BXDM_PPDBG_P_PrintUnifiedPicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicture
   );

void BXDM_PPDBG_P_PrintDMConfig(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   bool bLastCall
   );

#else
/* Non-DEBUG build */

#define BXDM_PPDBG_P_OutputLog( hXdmPP, pLocalState, pMFDPicture )
#define BXDM_PPDBG_P_Print( hXdmPP, pLocalState )
#define BXDM_PPDBG_P_SelectionLog( hXdmPP, eSelectionInfo )
#define BXDM_PPDBG_P_OutputSPOLog( hXdmPP, uiOverrideBits )
#define BXDM_PPDBG_P_CallbackTriggeredLog( hXdmPP, uiCallbackTriggeredBits )
#define BXDM_PPDBG_P_StateLog( hXdmPP, uiStateBits )
#define BXDM_PPDBG_P_State2Log( hXdmPP, uiStateBits )
#define BXDM_PPDBG_P_StcDeltaLog( hXdmPP, pLocalState )
#define BXDM_PPDBG_P_DecoderDropLog( hXdmPP, uiPendingDrop )
#define BXDM_PPDBG_P_PrintNodes( hXdmPP )
#define BXDM_PPDBG_P_PrintSelectionModeOverride( pMsg, hXdmPP, pPicture )
#define BXDM_PPDBG_P_PrintEndSelectionModeOverride( hXdmPP, pPicture )
#define BXDM_PPDBG_P_PrintMFD( hXdmPP, pLocalState, pMFDPicture )
#define BXDM_PPDBG_P_PrintUnifiedPicture( hXdmPP, pLocalState, pstPicture )
#define BXDM_PPDBG_P_PrintDMConfig( hXdmPP, pLocalState, bLastCall )

#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_DBG_H__ */
