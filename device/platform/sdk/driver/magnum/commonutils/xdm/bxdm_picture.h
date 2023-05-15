/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_picture.h $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 7/27/12 11:22a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_picture.h $
 * 
 * Hydra_Software_Devel/17   7/27/12 11:22a btosi
 * SW7425-1064: added hChannel to BXDM_Picture
 * 
 * Hydra_Software_Devel/16   7/2/12 9:53a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/15   4/12/12 1:02p btosi
 * SW7125-1281: Add more range checking in bxvd_decoder.c.
 * 
 * Hydra_Software_Devel/14   3/16/12 11:56a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/13   2/10/12 11:17a btosi
 * SW7425-2247: add support for bar data
 * 
 * Hydra_Software_Devel/12   2/9/12 1:19p btosi
 * SW7425-2180: added support for the MFD fields ePxlFmt and ulRowStride
 * 
 * Hydra_Software_Devel/11   1/10/12 1:32p btosi
 * SW7425-1001: added support for bLast flag
 * 
 * Hydra_Software_Devel/10   2/17/11 2:01p btosi
 * SW7405-5135: if the SEI arrangement_cancel flag is set, set  both the
 * Unified and MFD picture orientations to 2D
 * 
 * Hydra_Software_Devel/9   10/29/10 10:44a btosi
 * SW7422-72: merging 3D changes to mainline
 * 
 * Hydra_Software_Devel/SW7422-72/2   10/29/10 10:33a btosi
 * SW7422-72: added uiFrameNum to BXDM_Picture_3D
 * 
 * Hydra_Software_Devel/SW7422-72/1   10/19/10 9:08a btosi
 * SW7422-72: expanded support for 3D
 * 
 * Hydra_Software_Devel/7   10/1/10 2:49p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 * 
 * Hydra_Software_Devel/6   7/29/10 11:22a btosi
 * SW7405-4560: added support for the SEI "new message" flag.
 * 
 * Hydra_Software_Devel/5   7/1/10 3:54p btosi
 * SW7405-4573: changed bool's to uint32_t's in the unified picture
 * structure
 * 
 * Hydra_Software_Devel/4   5/7/10 3:48p btosi
 * SW7405-3996: added BXDM_Picture_ExtensionType_eNone
 * 
 * Hydra_Software_Devel/3   5/4/10 10:46a btosi
 * SW7405-3996: added support for new SEI message format
 * 
 * Hydra_Software_Devel/SW7405-3996/1   4/1/10 4:01p btosi
 * SW7405-3996: first cut at generating picture extension callback from
 * SEI data
 * 
 * Hydra_Software_Devel/2   2/17/10 4:59p nilesh
 * SW7405-2993: Initial Documentation
 * 
 * Hydra_Software_Devel/1   2/16/10 10:50a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef BXDM_PICTURE_H_
#define BXDM_PICTURE_H_

#include "bavc.h"
#include "bfmt.h"
#include "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* BXDM_Picture_PullDown -
 *
 * BXDM_Picture_PullDown indicates how the picture is intended to be displayed.
 *
 * Interlaced pulldowns (TB, BT, BTB, TBT) explicitly indicate the expected display
 * order of the fields.
 *
 * Dangling pulldown (T,B) indicate that there's only one valid field on the picture.
 *
 * Progressive pulldown (Fx1,Fx2,Fx3,Fx4) indicate that the content is progressive
 * but is intended to be treated as 1,2,3 or 4 frames of video respectively.  I.e.
 * An Fx2 picture is treated similar to having two Fx1 pictures in a row.
 *
 * The pulldown combined with the frame rate is used for PTS interpolation of pictures
 * that do not have coded PTS values.  E.g. suppose the content is progressive and has
 * a coded frame rate = 30 frames/sec.  This corresponds to a deltaPTS between *frames*
 * of 1500 ticks (on a 45 Khz timebase).  The interpolated PTS values would be different
 * if between pulldown=Fx1 and pulldown=Fx2
 *
 *          |           | Pulldown=Fx1 | Pulldown=Fx2 |
 *          | Coded PTS |  Interp PTS  |  Interp PTS  |
 *          +-----------+--------------+--------------+
 *   UPB[0] |         0 |           -- |           -- |
 *   UPB[1] |        -- |         1500 |         3000 |
 *   UPB[2] |        -- |         3000 |         6000 |
 *   UPB[3] |        -- |         4500 |         9000 |
 *
 */
typedef enum BXDM_Picture_PullDown
{
   BXDM_Picture_PullDown_eTop = 1,
   BXDM_Picture_PullDown_eBottom = 2,
   BXDM_Picture_PullDown_eTopBottom = 3,
   BXDM_Picture_PullDown_eBottomTop = 4,
   BXDM_Picture_PullDown_eTopBottomTop = 5,
   BXDM_Picture_PullDown_eBottomTopBottom = 6,
   BXDM_Picture_PullDown_eFrameX2 = 7,
   BXDM_Picture_PullDown_eFrameX3 = 8,
   BXDM_Picture_PullDown_eFrameX1 = 9,
   BXDM_Picture_PullDown_eFrameX4 = 10,

   BXDM_Picture_PullDown_eMax
} BXDM_Picture_PullDown;

/* BXDM_Picture_SourceFormat -
 *
 * BXDM_Picture_SourceFormat indicates if the original source content was *captured*
 * in an interlaced or progressive manner.  The SourceFormat matters when the content
 * is interlaced coded and there is more than 1 field in the picture
 * (e.g. pulldown = TB,BT,BTB,TBT).  If the SourceFormat=eProgressive, then XDM can
 * handle picture repeats with better quality than if the SourceFormat=eInterlaced.
 *
 * Content with progressive pulldown (Fx1,Fx2,Fx3,Fx4) should always have SourceFormat=eProgressive
 *
 * Interlaced coded 3:2 film content should have SourceFormat=eProgressive
 *
 * SourceFormat=eUnknown should be used if the nature of the source content is unknown.
 */
typedef enum BXDM_Picture_SourceFormat
{
   BXDM_Picture_SourceFormat_eUnknown,
   BXDM_Picture_SourceFormat_eInterlaced,
   BXDM_Picture_SourceFormat_eProgressive,

   BXDM_Picture_SourceFormat_eMax
} BXDM_Picture_SourceFormat;

typedef struct BXDM_Picture_Dimension
{
   uint32_t uiWidth;
   uint32_t uiHeight;

   uint32_t bValid;
} BXDM_Picture_Dimension;

typedef struct BXDM_Picture_ChromaLocation
{
   uint32_t bValid;
   BAVC_ChromaLocation eMpegType;
} BXDM_Picture_ChromaLocation;

typedef struct BXDM_Picture_QualifiedInt32
{
      uint32_t bValid;
      int32_t iValue;
} BXDM_Picture_QualifiedInt32;

/* For  SW7425-2181 */
typedef struct BXDM_Pixture_PixelFormat
{
   uint32_t     bValid;
   BPXL_Format  ePixelFormat;
}  BXDM_Pixture_PixelFormat;

#define BXDM_PICTURE_MAX_CHROMA_LOCATIONS 3

typedef struct BXDM_Picture_BufferInfo
{
   BMEM_Handle hHeap;                  /* See BAVC_MFD_Picture.hHeap in bavc.h */
   void *pLumaBufferVirtualAddress;    /* See BAVC_MFD_Picture.pLuminanceFrameBufferAddress in bavc.h */
   void *pChromaBufferVirtualAddress;  /* See BAVC_MFD_Picture.pChrominanceFrameBufferAddress in bavc.h */

   BXDM_Picture_Dimension stSource;    /* This contains the coded dimensions of the picture */

   BXDM_Picture_Dimension stDisplay;   /* OPTIONAL: This contains the intended display dimensions of the picture
                                        * If specified, XDM/Display will scale the source to the specified display
                                        * dimensions before applying any other (e.g. aspect ratio) scaling.  This is
                                        * used for content whose coded dimensions can change on-the-fly (e.g. to meet
                                        * bandwidth constraints) but the original source dimensions are fixed.
                                        */

   BAVC_StripeWidth eStripeWidth;      /* See BAVC_MFD_Picture.eStripeWidth in bavc.h */
   uint32_t uiLumaStripeHeight;        /* This is the stripe height of the luma buffer.
                                        * The luma stripe height will be greater or equal to the
                                        * source height
                                        */
   uint32_t uiChromaStripeHeight;      /* This is the stripe height of the chroma buffer.
                                        * The chroma stripe height will be greater or equal to the
                                        * source height/2
                                        */

   /* See BAVC_MFD_Picture.eMpegType in bavc.h The chroma location can be specified independently
    * for the field that is being displayed.  If the chroma location is the same for all fields, 
    * then all 3 entries should be set to the same value. Index is BAVC_Polarity enum.
    */
   BXDM_Picture_ChromaLocation stChromaLocation[ BXDM_PICTURE_MAX_CHROMA_LOCATIONS ];

   BAVC_YCbCrType eYCbCrType;                       /* See BAVC_MFD_Picture.eYCbCrType in bavc.h */

   BXDM_Picture_PullDown ePulldown;                 /* See BXDM_Picture_PullDown documentation */
   BXDM_Picture_SourceFormat eSourceFormat;         /* See BXDM_Picture_SourceFormat documentation */

   BXDM_Pixture_PixelFormat   stPixelFormat;       /* SW7425-2181: see bpxl.h for defintions */
   uint32_t                   uiRowStride;         /* SW7425-2181: stride of surface when ePixelFormat is a 422 format */

   uint32_t bValid;                                 /* False for a picture that doesn't contain actual picture buffers
                                                     * E.g. dummy picture used to forward a marker from the decoder
                                                     */
} BXDM_Picture_BufferInfo;


#define BXDM_PICTURE_MAX_PICTURE_ORDER_COUNT 3

typedef struct BXDM_Picture_PictureOrderCount
{
   uint32_t uiPictureId;

   /* BAVC_Polarity is used as index */
   BXDM_Picture_QualifiedInt32 stPictureOrderCount[BXDM_PICTURE_MAX_PICTURE_ORDER_COUNT];

   uint32_t bValid;
} BXDM_Picture_PictureOrderCount;

typedef struct BXDM_Picture_PTS
{
   uint32_t uiValue; /* For non-DirecTV protocols, in 45 Khz clock ticks.
                      * For DirecTV protocols, in 27 Mhz clock ticks.
                      */
   uint32_t bValid;
} BXDM_Picture_PTS;

typedef struct BXDM_Picture_Tag
{
   uint32_t uiValue;      /* Flag that arrives in ITB associated with BTP command 0xD.
                           * This flag is STICKY in subsequent pictures */
   uint32_t bValid;
   uint32_t bNewTagAvailable; /* True if the uiValue indicates a new picture tag ITB entry */
} BXDM_Picture_Tag;

typedef struct BXDM_Picture_Marker
{
   uint32_t uiValue;      /* Flag that arrives in ITB associated with BTP command 0xC.
                           * A change in value indicates a new marker has arrived.
                           * This flag is STICKY in subsequent pictures
                           */
   uint32_t bValid;
} BXDM_Picture_Marker;

/* BXDM_Picture_Coding -
 *
 * BXDM_Picture_Coding indicates the coding of the picture.
 */
typedef enum BXDM_Picture_Coding
{
   BXDM_Picture_Coding_eUnknown,
   BXDM_Picture_Coding_eI,
   BXDM_Picture_Coding_eP,
   BXDM_Picture_Coding_eB,

   BXDM_Picture_Coding_eMax
} BXDM_Picture_Coding;

/* BXDM_Picture_Sequence -
 *
 * BXDM_Picture_Sequence indicates the interlaced/progressive
 * nature of a sequence of pictures.
 */
typedef enum BXDM_Picture_Sequence
{
      BXDM_Picture_Sequence_eUnknown,
      BXDM_Picture_Sequence_eInterlaced,
      BXDM_Picture_Sequence_eProgressive,

      BXDM_Picture_Sequence_eMax
} BXDM_Picture_Sequence;

typedef struct BXDM_Picture_Type
{
   BXDM_Picture_Coding eCoding;
   BXDM_Picture_Sequence eSequence;
   uint32_t bReference;         /* True if this picture is a reference picture */
   uint32_t bRandomAccessPoint; /* True if this picture is a Random Access Point */
   uint32_t bLowDelay;          /* True if this picture is from a low delay stream (See MPEG-2 Sequence Extension) */

   uint32_t bLastPicture;       /* SW7425-1001: effectively an EOS flag.  Currently defined to 
                                 * only be delivered with a "picture-less" PPB. */
} BXDM_Picture_Type;

typedef struct BXDM_Picture_Error
{
   uint32_t bThisPicture;    /* This picture contains an error */
   uint32_t bPreviousRefPic; /* this picture was derived from a picture with an error
                          * it may or may not contain an error
                          */
} BXDM_Picture_Error;

typedef struct BXDM_Picture_Overscan
{
   uint32_t bOverscanAppropriate; /* True if horizontal overscan region should be
                                   * cropped.  False if horizontal overscan should
                                   * NOT be cropped
                                   */
                                   
   uint32_t bValid;               /* True if PPB specified overscan appropriate flag */
} BXDM_Picture_Overscan;

typedef struct BXDM_Picture_PCROffset
{
   uint32_t uiValue;          /* Value of PCR offset.  Always relative to the current STC value. */
   uint32_t bValid;           /* True if coded PCR offset exists */
   uint32_t bDiscontinuity;   /* True if unmarked discontinuity was detected */
} BXDM_Picture_PCROffset;

typedef enum BXDM_Picture_Protocol_Level
{
   BXDM_Picture_Protocol_Level_eUnknown = 0,
   BXDM_Picture_Protocol_Level_e00,
   BXDM_Picture_Protocol_Level_e10,
   BXDM_Picture_Protocol_Level_e1B,
   BXDM_Picture_Protocol_Level_e11,
   BXDM_Picture_Protocol_Level_e12,
   BXDM_Picture_Protocol_Level_e13,
   BXDM_Picture_Protocol_Level_e20,
   BXDM_Picture_Protocol_Level_e21,
   BXDM_Picture_Protocol_Level_e22,
   BXDM_Picture_Protocol_Level_e30,
   BXDM_Picture_Protocol_Level_e31,
   BXDM_Picture_Protocol_Level_e32,
   BXDM_Picture_Protocol_Level_e40,
   BXDM_Picture_Protocol_Level_e41,
   BXDM_Picture_Protocol_Level_e42,
   BXDM_Picture_Protocol_Level_e50,
   BXDM_Picture_Protocol_Level_e51,
   BXDM_Picture_Protocol_Level_e60,
   BXDM_Picture_Protocol_Level_e62,
   BXDM_Picture_Protocol_Level_eLow,
   BXDM_Picture_Protocol_Level_eMain,
   BXDM_Picture_Protocol_Level_eHigh,
   BXDM_Picture_Protocol_Level_eHigh1440,

   BXDM_Picture_Protocol_Level_eMaxLevel
} BXDM_Picture_Protocol_Level;

typedef enum BXDM_Picture_Profile
{
   BXDM_Picture_Profile_eUnknown = 0,
   BXDM_Picture_Profile_eSimple,
   BXDM_Picture_Profile_eMain,
   BXDM_Picture_Profile_eHigh,
   BXDM_Picture_Profile_eAdavance,
   BXDM_Picture_Profile_eJizhun,
   BXDM_Picture_Profile_eSnrScalable,
   BXDM_Picture_Profile_eSpatiallyScalable,
   BXDM_Picture_Profile_eAdavanceSimple,
   BXDM_Picture_Profile_eBaseline,

   BXDM_Picture_Profile_eMaxProfile
} BXDM_Picture_Profile;

typedef struct BXDM_Picture_Protocol
{
   BAVC_VideoCompressionStd eProtocol; /* video protocol */
   BXDM_Picture_Protocol_Level eLevel; /* video protocol level */
   BXDM_Picture_Profile eProfile;      /* video protocol profile */
} BXDM_Picture_Protocol;

/* BXDM_Picture_VideoFormat -
 *
 * BXDM_Picture_VideoFormat is based on MPEG2 Sequence Display Extension video_format field
 */
typedef enum BXDM_Picture_VideoFormat
{
      BXDM_Picture_VideoFormat_eComponent,
      BXDM_Picture_VideoFormat_ePAL,
      BXDM_Picture_VideoFormat_eNTSC,
      BXDM_Picture_VideoFormat_eSECAM,
      BXDM_Picture_VideoFormat_eMAC,
      BXDM_Picture_VideoFormat_eUnknown,
      BXDM_Picture_VideoFormat_eReserved6,
      BXDM_Picture_VideoFormat_eReserved7
} BXDM_Picture_VideoFormat;

/* BXDM_Picture_DisplayInfo -
 *
 * BXDM_Picture_DisplayInfo is based on the corresponding MPEG2 Sequence Display Extension fields
 */
typedef struct BXDM_Picture_DisplayInfo
{
   BAVC_MatrixCoefficients eMatrixCoefficients;
   BAVC_ColorPrimaries eColorPrimaries;
   BAVC_TransferCharacteristics eTransferCharacteristics;
   BXDM_Picture_VideoFormat eVideoFormat;

   uint32_t bValid;
} BXDM_Picture_DisplayInfo;

/* BXDM_Picture_AspectRatio -
 *
 * BXDM_Picture_AspectRatio specifies the Display Aspect Ratio (DAR) or
 * the Sample Aspect Ratio (SAR).
 */
typedef struct BXDM_Picture_AspectRatio
{
   BFMT_AspectRatio eAspectRatio;
   uint32_t uiSampleAspectRatioX; /* Valid only when eAspectRatio=eSar */
   uint32_t uiSampleAspectRatioY; /* Valid only when eAspectRatio=eSar */

   uint32_t bValid;
} BXDM_Picture_AspectRatio;

/* BXDM_Picture_PanScanVectorType -
 *
 * BXDM_Picture_PanScanVectorType indicates how the pan scan window is specified.
 *
 *    |----------- SW ------------|
 *
 *  - +---------------------------+
 *  | |      -                    |
 *  | |      |                    |
 *  | |      V                    |
 *    |      O                    |
 *  S |      |                    |
 *  H |      - |- W -|            |
 *    ||- HO -|+-----+ -          |
 *  | |        |xxxxx| |          |
 *  | |        |xxxxx| H          |
 *  | |        |xxxxx| |          |
 *  | |        +-----+ -          |
 *  - +---------------------------+
 *
 * **********************************************
 *  BXDM_Picture_PanScanVectorType_eSourceWindow
 * **********************************************
 *
 *  SW = Source Width (post-clip, post-overscan crop)
 *  SH = Source Height (post-clip, post-overscan crop)
 *  HO = BXDM_Picture_PanScanVector.iHertical >> 4
 *  VO = BXDM_Picture_PanScanVector.iVertical >> 4
 *  W  = BXDM_Picture_PanScanVector.uiWidth
 *  H  = BXDM_Picture_PanScanVector.uiHeight
 *  x  = Pan Scan Window
 *
 * ********************************************
 *  BXDM_Picture_PanScanVectorType_eSourceCrop
 * ********************************************
 *
 *  SW = Source Width (post-clip, post-overscan crop)
 *  SH = Source Height (post-clip, post-overscan crop)
 *  HO = BXDM_Picture_PanScanVector.iHertical >> 4
 *  VO = BXDM_Picture_PanScanVector.iVertical >> 4
 *  W  = (SW - BXDM_Picture_PanScanVector.uiWidth)
 *  H  = (SH - BXDM_Picture_PanScanVector.uiHeight)
 *  x  = Pan Scan Window
 *
 * ***********************************************
 *  BXDM_Picture_PanScanVectorType_eDisplayWindow
 * ***********************************************
 *
 *  SW = Source Width (post-clip, post-overscan crop)
 *  SH = Source Height (post-clip, post-overscan crop)
 *  DW = Display Width (See BXDM_Picture.BXDM_Picture_BufferInfo.stDisplay.uiWidth)
 *  DH = Display Height (See BXDM_Picture.BXDM_Picture_BufferInfo.stDisplay.uiHeight)
 *  HO = BXDM_Picture_PanScanVector.iHertical >> 4 * (SW/DW)
 *  VO = BXDM_Picture_PanScanVector.iVertical >> 4 * (SH/DH)
 *  W  = BXDM_Picture_PanScanVector.uiWidth * (SW/DW)
 *  H  = BXDM_Picture_PanScanVector.uiHeight * (SH/DH)
 *  x  = Pan Scan Window
 *
 */
typedef enum BXDM_Picture_PanScanVectorType
{
      BXDM_Picture_PanScanVectorType_eSourceWindow,  /* Values indicate pan-scan window size/location relative to source size */
      BXDM_Picture_PanScanVectorType_eSourceCrop,    /* Values indicate horizontal/vertical crop relative to source size */
      BXDM_Picture_PanScanVectorType_eDisplayWindow, /* Values indicate pan-scan window size/location relative to display size */

      BXDM_Picture_PanScanVectorType_eMax
} BXDM_Picture_PanScanVectorType;

typedef struct BXDM_Picture_PanScanVector
{
      int32_t iHorizontal; /* 1/16th pixel */
      int32_t iVertical; /* 1/16th pixel */
      uint32_t uiWidth;
      uint32_t uiHeight;
      BXDM_Picture_PanScanVectorType eType;
} BXDM_Picture_PanScanVector;

#define BXDM_PICTURE_MAX_PAN_SCAN_VECTOR 3

typedef struct BXDM_Picture_PanScan
{
   /* Indexed by displayed element based on pulldown.
    *   | max vectors | pulldown |
    *   |      1      |  T,B,F   |
    *   |      2      |  TB,BT   |
    *   |      3      |  TBT,BTB |
    */
   BXDM_Picture_PanScanVector stVector[BXDM_PICTURE_MAX_PAN_SCAN_VECTOR];

   /* 0 indicates no pan-scan data present */
   uint32_t uiCount;
} BXDM_Picture_PanScan;

typedef struct BXDM_Picture_ActiveFormatDescription
{
   uint32_t uiValue;
   uint32_t bValid;
} BXDM_Picture_ActiveFormatDescription;

/* SW7425-2247: support for bar data. */
typedef struct BXDM_Picture_BarData
{
   BAVC_BarDataType  eBarDataType;        /* specify top/bottom or left/right data */
   uint32_t          uiTopLeftBarValue;   /* either the top or left bar data value */
   uint32_t          uiBotRightBarValue;  /* either the bottom or right bar data value */
}  BXDM_Picture_BarData;

typedef struct BXDM_Picture_GopTimeCode
{
     uint32_t uiHours;    /* The hours field */
     uint32_t uiMinutes;  /* The minutes field */
     uint32_t uiSeconds;  /* The seconds field */
     uint32_t uiPictures; /* The pictures (frames) field */
     uint32_t bValid;     /* Valid timecode in input stream */
} BXDM_Picture_GopTimeCode;

typedef struct BXDM_Picture_Rate
{
      uint16_t uiNumerator;   /* 0 indicates unknown rate */
      uint16_t uiDenominator; /* 0 indicates unknown rate */
} BXDM_Picture_Rate;

typedef enum BXDM_Picture_FrameRateType
{
      BXDM_Picture_FrameRateType_eUnknown,
      BXDM_Picture_FrameRateType_eFixed,
      BXDM_Picture_FrameRateType_eVariable,

      BXDM_Picture_FrameRateType_eMax
} BXDM_Picture_FrameRateType;

typedef struct BXDM_Picture_FrameRate
{
      BXDM_Picture_Rate stRate;         /* Coded frame rate. */
      BXDM_Picture_FrameRateType eType;
      BXDM_Picture_Rate stExtension;    /* Multiplier for coded frame rate. 0 indicates no multiplier. */

      uint32_t bValid;
} BXDM_Picture_FrameRate;

/* SWDEPRECATED-1003: */
typedef struct BXDM_Picture_FrameRateOverride
{
   uint32_t bValid;
   BXDM_Picture_Rate stRate;         /* Coded frame rate. */
   uint32_t bTreatAsSingleElement;  /* assume FrameX1 for all pictures when calculating the predicted PTS 
                                   value of the next picture. */
} BXDM_Picture_FrameRateOverride;

typedef struct BXDM_Picture_DigitalNoiseReduction
{
   uint32_t uiAdjustedQuantizationParameter;
   uint32_t bValid;
} BXDM_Picture_DigitalNoiseReduction;

typedef struct BXDM_Picture_RangeRemapping
{
   uint32_t uiLuma;
   uint32_t uiChroma;
   uint32_t bValid;
} BXDM_Picture_RangeRemapping;

/* BXDM_Picture_Clipping -
 *
 * BXDM_Picture_Clipping specifies the portion of the coded source buffer
 * that needs to be cropped before displaying.
 */
typedef struct BXDM_Picture_Clipping
{
      uint32_t uiTop;
      uint32_t uiBottom;
      uint32_t uiLeft;
      uint32_t uiRight;

      uint32_t bValid;
} BXDM_Picture_Clipping;

/* Support for picture extension data; 
 * MVC graphics offset, SEI messages....
 */


/* 
 * MVC graphics offset.
 */
typedef struct BXDM_Picture_Extension_BluRay3DGraphicsOffset
{
   uint32_t uiCount;
   uint8_t * puiOffsetData;

} BXDM_Picture_Extension_BluRay3DGraphicsOffset;


/* 
 * SEI Frame Packing.
 */

/* 
 * Bit definitions for the 'uiFlags' element in BXDM_Picture_Extension_SEIFramePacking structure.
 *
 * "arrangement_cancel":
 * The H264 specification was not completely rigorous in the definition of this bit.  We are assuming 
 * that if this bit is set, none of the other fields in the message are valid.   This then implies 
 * that the picture is a 2D picture.  The orientation will be set to "2D" in both the Unified and
 * MFD picture structures. (SW7405-5135)
 */ 
#define BXDM_PICTURE_SEI_FRAMEPACK_ARRANGEMENT_CANCEL_FLAG           (0x00000001)      /* if set, implies 2D */
#define BXDM_PICTURE_SEI_FRAMEPACK_QUINCUNX_SAMPLING_FLAG            (0x00000002)
#define BXDM_PICTURE_SEI_FRAMEPACK_SPATIAL_FLIPPING_FLAG             (0x00000004)
#define BXDM_PICTURE_SEI_FRAMEPACK_FRAME0_FLIPPED_FLAG               (0x00000008)
#define BXDM_PICTURE_SEI_FRAMEPACK_FIELD_VIEWS_FLAG                  (0x00000010)
#define BXDM_PICTURE_SEI_FRAMEPACK_CURRENT_FRAME_IS_FRAME0_FLAG      (0x00000020)
#define BXDM_PICTURE_SEI_FRAMEPACK_FRAME0_SELF_CONTAINED_FLAG        (0x00000040)
#define BXDM_PICTURE_SEI_FRAMEPACK_FRAME1_SELF_CONTAINED_FLAG        (0x00000080)
#define BXDM_PICTURE_SEI_FRAMEPACK_ARRANGEMENT_EXTENSION_FLAG        (0x00000100)

/* 
 * The SEI message as delivered by AVD. 
 */
typedef struct BXDM_Picture_Extension_SEIFramePacking
{
   uint32_t uiFlags;
   uint32_t uiFramePackingArrangementId;
   uint32_t uiFramePackingArrangementType;
   uint32_t uiContentInterpretationType;
   uint32_t uiFrame0GridPositionX;
   uint32_t uiFrame0GridPositionY;
   uint32_t uiFrame1GridPositionX;
   uint32_t uiFrame1GridPositionY;
   uint32_t uiFramePackingArrangementReservedByte;
   uint32_t uiFramePackingArrangementRepetitionPeriod;

} BXDM_Picture_Extension_SEIFramePacking;

/* A wrapper around the preceding structure to allow additional information to be 
 * passed back to the application.
 */
typedef struct BXDM_Picture_Extension_SEIMsg_FramePacking
{
   uint32_t uiFlags;
   BXDM_Picture_Extension_SEIFramePacking * pstSeiData;

} BXDM_Picture_Extension_SEIMsg_FramePacking;

/* Bit definitions for the 'uiFlags' element in the preceding
 * BXDM_Picture_Extension_SEIMsg_FramePacking structure
 */

/* SW7405-4560: Set when the SEI data is delivered for the first time. 
 * Cleared when the message is repeated with the same data. 
 */
#define BXDM_PICTURE_SEIMSG_FRAMEPACK_NEW_MESSAGE_FLAG    0x00000001

/*
 * Message types.
 */
typedef enum BXDM_Picture_ExtensionType
{
   BXDM_Picture_ExtensionType_eNone=0,
   BXDM_Picture_ExtensionType_eBluRay3DGraphicsOffset,
   BXDM_Picture_ExtensionType_eSEIFramePacking,          /* DEPRECATED: SW7405-4560: use BXDM_Picture_ExtensionType_eSEIMsg_FramePacking */
   BXDM_Picture_ExtensionType_eSEIMsg_FramePacking,

   /* New extension types should be added ABOVE this line */
   BXDM_Picture_ExtensionType_eMax

} BXDM_Picture_ExtensionType;

/*
 * Wrapper for data passed in the extension data callback.
 */
typedef struct BXDM_Picture_ExtensionData
{
   BXDM_Picture_ExtensionType eType;

   union
   {
      BXDM_Picture_Extension_BluRay3DGraphicsOffset   stBluRay3DGraphicsOffset;
      BXDM_Picture_Extension_SEIFramePacking *        pstSEIFramePacking; /* DEPRECATED: SW7405-4560: use stSEIFramePacking */
      BXDM_Picture_Extension_SEIMsg_FramePacking      stSEIFramePacking;

      /* Add new extension data structures here */
   } data;

} BXDM_Picture_ExtensionData;


/* 
 * BXDM_Picture_ExtensionInfo, basically a per picture array of messages.
 */
#define BXDM_MAX_PICTURE_EXTENSION_INFO   4

typedef struct BXDM_Picture_ExtensionInfo
{
   /* Number of entries in the following array. */
   uint32_t uiCount;

   BXDM_Picture_ExtensionData  astExtensionData[BXDM_MAX_PICTURE_EXTENSION_INFO];

   /* When dealing with multi buffer content (MVC and other full frame 3D) will point
    * to the extension data of the second buffer.
    */
   struct BXDM_Picture_ExtensionInfo *pNext;

} BXDM_Picture_ExtensionInfo;


/* 
 * Picture stats.
 */
typedef struct BXDM_Picture_Stats_MacroBlockCount
{
      uint32_t uiTotal;
      uint32_t uiIntraCoded;
      uint32_t uiInterCoded;

      uint32_t bValid;
} BXDM_Picture_Stats_MacroBlockCount;

typedef struct BXDM_Picture_Stats
{
      BXDM_Picture_Stats_MacroBlockCount stMacroBlockCount;
      uint32_t uiBitRate;           /* in units of 400 bits/second (Based on MPEG2 Sequence Header) */
      uint32_t uiDeltaPicturesSeen; /* Indicates the total number of pictures seen by the decoder since the previous picture.  Usually this is 1.  Values larger than 1 indicate the decoder may have dropped pictures in the middle (e.g. I only mode).  0 indicates unknown. */
} BXDM_Picture_Stats;

/*
 * 3D support
 */

#define BXDM_PICTURE_MAX_3D_BUFFERS 2

/* BXDM_Picture_Orientation -
 *
 * indicates if this is a 2D or 3D picture.  In addition if 3D,
 * the frame packing and orientation.
 */
typedef enum BXDM_Picture_Orientation
{
   BXDM_Picture_Orientation_e2D = 0,
   BXDM_Picture_Orientation_e3D_Checker,
   BXDM_Picture_Orientation_e3D_Column,
   BXDM_Picture_Orientation_e3D_Row,
   BXDM_Picture_Orientation_e3D_SideBySide,
   BXDM_Picture_Orientation_e3D_TopBottom,
   BXDM_Picture_Orientation_e3D_FullFrame,

   BXDM_Picture_Orientation_eMax
} BXDM_Picture_Orientation;

/* BXDM_Picture_FrameRelationship -
 *
 * when 3D, indicates if frame 0 is the left or right image
 */
typedef enum BXDM_Picture_FrameRelationship
{
   BXDM_Picture_FrameRelationship_eUnknown = 0,
   BXDM_Picture_FrameRelationship_eFrame0Left,
   BXDM_Picture_FrameRelationship_eFrame0Right,
   
   BXDM_Picture_FrameRelationship_eMax
} BXDM_Picture_FrameRelationship;



typedef struct BXDM_Picture_3D
{
   BXDM_Picture_Orientation         eOrientation;
   BXDM_Picture_FrameRelationship   eFrameRelationship;
   uint32_t                         uiFrameNum;          /* 0 for frame 0, 1 for frame 1*/

} BXDM_Picture_3D;



/*
 *  BXDM_Picture
 */
typedef struct BXDM_Picture
{
   /************/
   /* Required */
   /************/

   /* Buffer Parameters */
   BXDM_Picture_BufferInfo stBufferInfo;


   /************/
   /* Optional */
   /************/

   /* Source Parameters */
   BXDM_Picture_Clipping stClipping;
   BXDM_Picture_Protocol stProtocol;
   BXDM_Picture_Type stPictureType;
   BXDM_Picture_Error stError;

   /* Timing Parameters */
   BXDM_Picture_PictureOrderCount stPOC;
   BXDM_Picture_FrameRate stFrameRate;
   BXDM_Picture_PTS stPTS;
   BXDM_Picture_PCROffset stPCROffset;
   BXDM_Picture_GopTimeCode stGOPTimeCode;
   BXDM_Picture_Tag stPictureTag;
   BXDM_Picture_Marker stPictureMarker;
   uint32_t uiChunkId;  /*SW7425-3358:*/

   /* Display Parameters */
   BXDM_Picture_Overscan stOverscan;
   BXDM_Picture_DisplayInfo stDisplayInfo;
   BXDM_Picture_PanScan stPanScan;
   BXDM_Picture_AspectRatio stAspectRatio;
   BXDM_Picture_DigitalNoiseReduction stDigitalNoiseReduction;
   BXDM_Picture_RangeRemapping stRangeRemapping;
   BXDM_Picture_ActiveFormatDescription stActiveFormatDescription;

   BXDM_Picture_BarData stBarData;   /* SW7425-2247: support for bar data. */

   /* Extension Data */
   BXDM_Picture_ExtensionInfo stExtensionInfo;
   BXDM_Picture_Stats stStats;

   /* 3D */
   BXDM_Picture_3D st3D;

   uint32_t uiSerialNumber;

   void *pPrivateContext;  /* Private context for decoder use */
   void *hChannel;         /* SW7425-1064: handle of the channel that created the picture. */

   void *pNextPicture; /* Linked list of dependent unified pictures */
} BXDM_Picture;

/* 
 * SW7425-2536: added to support "drop at decode" for non-AVD decoders. 
 */
typedef struct BXDM_PictureProvider_PictureInfo 
{
   /* Indicates that the information is this structure is valid. That might 
    * not be the case if a picture has not been selected for display.
    */
   uint32_t bValid;

   /* STC on this vsync, includes the STC jitter correction and PCR offsets */
   uint32_t uiEffectiveStc;

   /* PTS of the selected element, includes the PTS jitter correction 
    * and PTS (display) offset. 
    */   
   uint32_t uiEffectivePts;

   /* Picture was selected in TSM mode (as opposed to vsync). For the purposes 
    * of calculating a drop count, uiEffectiveStc and uiEffectivePts are only
    * meaningful when in TSM mode.
    */   
   uint32_t bSelectionModeTsm;

   /* Frame rate used by XDM, could be coded, specified or calculated */
   BXDM_Picture_FrameRate stFrameRate;

   /* Serial number from the Unified Picture structure. */
   uint32_t uiSerialNumber;   
   
} BXDM_PictureProvider_PictureInfo;


#ifdef __cplusplus
}
#endif

#endif /* BXDM_PICTURE_H_ */
