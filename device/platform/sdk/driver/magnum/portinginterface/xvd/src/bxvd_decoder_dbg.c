/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_decoder_dbg.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 1/12/12 3:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_decoder_dbg.c $
 * 
 * Hydra_Software_Devel/10   1/12/12 3:23p btosi
 * SW7405-4736: added debug support for new PPB field flags_ext0
 * 
 * Hydra_Software_Devel/9   11/30/11 3:06p btosi
 * SW7405-4736: shortened the BXVD_UP message
 * 
 * Hydra_Software_Devel/8   7/28/11 11:38a btosi
 * SW7405-4736: added the PPB flags to the BXVD_UP message
 * 
 * Hydra_Software_Devel/7   7/27/11 2:02p btosi
 * SWDTV-7795: added new protocols to the debug messages
 * 
 * Hydra_Software_Devel/6   7/20/11 3:20p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/5   4/15/11 10:50a btosi
 * SW7405-4736: added support to print the SEI messages
 * 
 * Hydra_Software_Devel/4   4/12/11 2:19p btosi
 * SW7405-4736: added support for MFD debug messages
 * 
 * Hydra_Software_Devel/3   4/4/11 2:32p btosi
 * SW7405-4763: added newline at end of file
 * 
 * Hydra_Software_Devel/2   4/4/11 2:21p btosi
 * SW7405-4736: added BDBG_DEBUG_BUILD
 * 
 * Hydra_Software_Devel/1   4/4/11 11:26a btosi
 * SW7405-4736: initial check in
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"

#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_decoder.h"
#include "bxvd_decoder_timer.h"

#include "bxvd_vdec_info.h"

BDBG_MODULE(BXVD_DECODER_DBG);
BDBG_FILE_MODULE(BXVD_UP);
BDBG_FILE_MODULE(BXVD_SEI);

#if BDBG_DEBUG_BUILD

/*
 * Lookup tables for mapping variables to strings.
 */
static const char * s_aPictureCodingToStrLUT[BXDM_Picture_Coding_eMax] = 
{
   "u",  /* BXDM_Picture_Coding_eUnknown */
   "I",  /* BXDM_Picture_Coding_eI */
   "P",  /* BXDM_Picture_Coding_eP */
   "B"   /* BXDM_Picture_Coding_eB */
};

static const char * s_aPulldownToStrLUT[BXDM_Picture_PullDown_eMax] =
{
   "err",   /* invalid */
   "T  ",   /* BXDM_Picture_PullDown_eTop = 1 */
   "B  ",   /* BXDM_Picture_PullDown_eBottom = 2 */
   "TB ",   /* BXDM_Picture_PullDown_eTopBottom = 3 */
   "BT ",   /* BXDM_Picture_PullDown_eBottomTop = 4 */
   "TBT",   /* BXDM_Picture_PullDown_eTopBottomTop = 5 */
   "BTB",   /* BXDM_Picture_PullDown_eBottomTopBottom = 6 */
   "X2 ",   /* BXDM_Picture_PullDown_eFrameX2 = 7 */
   "X3 ",   /* BXDM_Picture_PullDown_eFrameX3 = 8 */
   "X1 ",   /* BXDM_Picture_PullDown_eFrameX1 = 9 */
   "X4 "    /* BXDM_Picture_PullDown_eFrameX4 = 10 */
};

#define BXVD_DECODER_S_MAX_VIDEO_PROTOCOL 19

static const char * s_aProtocolToStrLUT[BXVD_DECODER_S_MAX_VIDEO_PROTOCOL] =
{
   "H264 ",          /* H.264 */
   "MPEG2",          /* MPEG-2 */
   "H261 ",          /* H.261 */
   "H263 ",          /* H.263 */
   "VC1  ",          /* VC1 Advanced profile */
   "MPEG1",          /* MPEG-1 */
   "MPEG2DTV",       /* MPEG-2 DirecTV DSS ES */
   "VC1SimpleMain",  /* VC1 Simple & Main profile */
   "MPEG4Part2",     /* MPEG 4, Part 2. */
   "AVS ",           /* AVS Jinzhun profile. */
   "MPEG2_DSS_PES",  /* MPEG-2 DirecTV DSS PES */
   "SVC ",           /* Scalable Video Codec */
   "SVC_BL",         /* Scalable Video Codec Base Layer */
   "MVC ",           /* MVC Multi View Coding */
   "VP6 ",           /* VP6 */
   "VP7 ",           /* VP7 */
   "VP8 ",           /* VP8 */
   "RV9 ",           /* Real Video 9 */
   "SPARK"           /* Sorenson Spark */
};

static const char * s_aOrientationToStrLUT[BXDM_Picture_Orientation_eMax] =
{
   "2D ",   /* BXDM_Picture_Orientation_e2D */
   "Chk",   /* BXDM_Picture_Orientation_e3D_Checker */
   "Col",   /* BXDM_Picture_Orientation_e3D_Column */
   "Row",   /* BXDM_Picture_Orientation_e3D_Row */
   "SbS",   /* BXDM_Picture_Orientation_e3D_SideBySide */
   "ToB",   /* BXDM_Picture_Orientation_e3D_TopBottom */
   "Ful"    /* BXDM_Picture_Orientation_e3D_FullFrame */
};

static const char * s_aFrameRelationShipLUT[BXDM_Picture_FrameRelationship_eMax] =
{
   "-ukn",     /* BXDM_Picture_FrameRelationship_eUnknown */
   "-LR ",     /* BXDM_Picture_FrameRelationship_eFrame0Left */
   "-RL!"      /* BXDM_Picture_FrameRelationship_eFrame0Right */
};

static const char * s_aPictureSetTypeToStrLUT[BXVD_Decoder_P_PictureSet_eMax] = 
{
   "S",  /* BXVD_Decoder_P_PictureSet_eSingle */
   "B",  /* BXVD_Decoder_P_PictureSet_eBase */
   "e"   /* BXVD_Decoder_P_PictureSet_eDependent */
};


/*
 * Print routines.
 */

void BXVD_DecoderDbg_P_PrintUnifiedPicture(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiPPBFlags,
   uint32_t uiPPBFlagsExt0,
   BXVD_Decoder_P_UnifiedPictureContext * pstContext,
   bool bDropped
   )
{
   BXDM_Picture * pstPicture;
   bool           bProtocolValid;

   BDBG_ENTER( BXVD_DecoderDbg_P_PrintUnifiedPicture );

   BDBG_ASSERT( pstContext );   

   pstPicture = &(pstContext->stUnifiedPicture);
   
   bProtocolValid = ( pstPicture->stProtocol.eProtocol < BXVD_DECODER_S_MAX_VIDEO_PROTOCOL ) ? true : false ;

   BDBG_ASSERT( pstPicture );   

   BDBG_MODULE_MSG( BXVD_UP, ("%c%03x:[%01x.%03x] pts:%08x(%d) flg:%08x ext0:%08x %s,%s %s %s %s%s %c",
                                 ( bDropped ) ? 'D' : ' ',
                                 hXvdCh->stDecoderContext.stCounters.uiVsyncCount & 0xFFF,
                                 hXvdCh->ulChannelNum & 0xF,
                                 pstPicture->uiSerialNumber & 0xFFF,
                                 pstPicture->stPTS.uiValue,
                                 pstPicture->stPTS.bValid,
                                 uiPPBFlags,
                                 uiPPBFlagsExt0,
                                 s_aPictureCodingToStrLUT[  pstPicture->stPictureType.eCoding ],
                                 s_aPictureSetTypeToStrLUT[ pstContext->eSetType ],
                                 s_aPulldownToStrLUT[ pstPicture->stBufferInfo.ePulldown ],
                                 ( bProtocolValid ) ? s_aProtocolToStrLUT[ pstPicture->stProtocol.eProtocol ] : "ukn" ,
                                 s_aOrientationToStrLUT[ pstPicture->st3D.eOrientation ],
                                 ( BXDM_Picture_Orientation_e2D == pstPicture->st3D.eOrientation ) ?
                                          "    " : s_aFrameRelationShipLUT[ pstPicture->st3D.eFrameRelationship ],
                                 ( pstPicture->stError.bThisPicture ) ? 'E' : ' '
                                 ));

   BDBG_LEAVE( BXVD_DecoderDbg_P_PrintUnifiedPicture );

   return;

}  /* end of BXVD_DecoderDbg_P_PrintUnifiedPicture() */



void BXVD_DecoderDbg_P_PrintSeiMessage(
   BXVD_ChannelHandle hXvdCh,
   BXVD_P_SEI_Message * pSEIMessage,
   uint32_t uiSerialNumber
   )
{
   BDBG_ENTER( BXVD_DecoderDbg_P_PrintSeiMessage );

   BDBG_ASSERT( hXvdCh );   
   BDBG_ASSERT( pSEIMessage );   

   switch( pSEIMessage->uiMsgType )
   {
      case BXVD_P_PPB_SEI_MSG_MVC_GRAPHICS_OFFSET:
      {
         BXVD_P_MVC_Offset_Meta * pMetaData = (BXVD_P_MVC_Offset_Meta *)&(pSEIMessage->data.stOffsetMeta);

         BDBG_MODULE_MSG( BXVD_SEI, (" %04x:[%01x.%03x] GraphicsOffset: num values %d: %02x %02x %02x %02x ...",
                                 hXvdCh->stDecoderContext.stCounters.uiVsyncCount & 0xFFFF,
                                 hXvdCh->ulChannelNum & 0xF,
                                 uiSerialNumber & 0xFFF,
                                 pMetaData->size,
                                 pMetaData->offset[0],
                                 pMetaData->offset[1],
                                 pMetaData->offset[2],
                                 pMetaData->offset[3]
                                 ));

         break;
      }

      case BXVD_P_PPB_SEI_MSG_FRAMEPACKING:
      {
         BXVD_P_SEI_FramePacking * pstAvdSEIData = (BXVD_P_SEI_FramePacking *)&(pSEIMessage->data.stSEIFramePacking);

         BDBG_MODULE_MSG( BXVD_SEI, (" %04x:[%01x.%03x] FramePacking: flags:%08x arrange:%08x interp:%08x",
                                 hXvdCh->stDecoderContext.stCounters.uiVsyncCount & 0xFFFF,
                                 hXvdCh->ulChannelNum & 0xF,
                                 uiSerialNumber & 0xFFF,
                                 pstAvdSEIData->flags,
                                 pstAvdSEIData->frame_packing_arrangement_type,
                                 pstAvdSEIData->content_interpretation_type
                                ));
         break;
      }

      default:
         BDBG_MODULE_MSG( BXVD_SEI, (" %04x:[%01x.%03x] invalid SEI uiMsgType: %d",
                                 hXvdCh->stDecoderContext.stCounters.uiVsyncCount & 0xFFFF,
                                 hXvdCh->ulChannelNum & 0xF,
                                 uiSerialNumber & 0xFFF,
                                 pSEIMessage->uiMsgType
                                 ));
         break;
   }


   BDBG_LEAVE( BXVD_DecoderDbg_P_PrintSeiMessage );

   return;

}  /* end of BXVD_DecoderDbg_P_PrintSeiMessage() */



#endif   /* if debug build */
