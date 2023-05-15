/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_priv.h $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 9/6/12 8:59p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid_priv.h $
* 
* Hydra_Software_Devel/7   9/6/12 8:59p fbasso
* SW7425-3898: removed symbolic links from directory.
*
* Hydra_Software_Devel/6   8/24/12 5:02p fbasso
* SW7425-3620: replaced un-cached memory access with cached memory
* access.
*
* Hydra_Software_Devel/5   4/2/12 10:26a fbasso
* SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* code in place to decode both images contained in the JPEG-B frame,
* only the first image is decoded for now and sent to XDM. As soon as
* interlaced mode is added in the SID->XDM->VDC path, this limitation
* will be removed and both images will be decoded and sent to display.
*
* Hydra_Software_Devel/4   3/19/12 3:52p fbasso
* SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* down and up sequence.
*
* Hydra_Software_Devel/3   2/23/12 5:12p fbasso
* SW7425-2112: fixed function used to calculate distance between ITB read
* and valid pointers. Fixed raced condition in fw when accessing CDB
* wrap pointer around wrap time.
*
* Hydra_Software_Devel/2   2/21/12 2:21p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
*
* Hydra_Software_Devel/SW7425-2112/7   2/20/12 6:43p fbasso
* SW7425-2112: disabled SID ARC uart.
*
* Hydra_Software_Devel/SW7425-2112/6   2/20/12 3:51p fbasso
* SW7425-2112: remove run time option to configure channel change output
* behavior; rather than sid, the behavior upon channel change is decided
* by xdm configuration; sid will simply follow xdm.
*
* Hydra_Software_Devel/SW7425-2112/5   2/16/12 4:51p fbasso
* SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* still channels and motion channel; added support for channel change
* output mode (hold last picture)
*
* Hydra_Software_Devel/SW7425-2112/4   2/15/12 11:32a fbasso
* SW7425-2112: configure ePulldown variable in unified picture structure
* to frame progressive to resolve incorrect cropping at display time.
* Introduced new function BSID_GetRaveItbCbdConfigInfo to retrieve
* default RAVE configuration to use during video decode.
*
* Hydra_Software_Devel/SW7425-2112/3   2/10/12 4:16p fbasso
* SW7425-2112: update SID to XDM path to use latest BAVC_MFD_Picture
* format protocol to send buffer to VDC.
*
* Hydra_Software_Devel/SW7425-2112/2   2/10/12 10:54a fbasso
* SW7425-2112: added support for motion jpeg decode using XPT RAVE to
* provide input data to SID fw/hw and Xdm interface to supply SID output
* pictures to VDC.
*
* Hydra_Software_Devel/SW7425-2112/1   1/26/12 5:46p fbasso
* SW7425-2112: rave support first draft
*
* Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
*
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: modified handling ofinput buffer in streaming mode.
* Extended to support up to sixteen channels
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/7   8/22/11 4:29p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/6   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid_priv.h $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/7   9/6/12 8:59p fbasso
* SW7425-674: Added SW7425-3898: removed symbolic links from directory.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/6   8/24/12 5:02p fbasso
* SW7425-674: Added SW7425-3620: replaced un-cached memory access with cached memory
* SW7425-674: Added access.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/5   4/2/12 10:26a fbasso
* SW7425-674: Added SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* SW7425-674: Added code in place to decode both images contained in the JPEG-B frame,
* SW7425-674: Added only the first image is decoded for now and sent to XDM. As soon as
* SW7425-674: Added interlaced mode is added in the SID->XDM->VDC path, this limitation
* SW7425-674: Added will be removed and both images will be decoded and sent to display.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/4   3/19/12 3:52p fbasso
* SW7425-674: Added SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* SW7425-674: Added down and up sequence.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/3   2/23/12 5:12p fbasso
* SW7425-674: Added SW7425-2112: fixed function used to calculate distance between ITB read
* SW7425-674: Added and valid pointers. Fixed raced condition in fw when accessing CDB
* SW7425-674: Added wrap pointer around wrap time.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/2   2/21/12 2:21p fbasso
* SW7425-674: Added SW7425-2112: added support for MJPEG video decode. Merging new feature
* SW7425-674: Added to Hydra_Software_Devel.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/7   2/20/12 6:43p fbasso
* SW7425-674: Added SW7425-2112: disabled SID ARC uart.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/6   2/20/12 3:51p fbasso
* SW7425-674: Added SW7425-2112: remove run time option to configure channel change output
* SW7425-674: Added behavior; rather than sid, the behavior upon channel change is decided
* SW7425-674: Added by xdm configuration; sid will simply follow xdm.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/5   2/16/12 4:51p fbasso
* SW7425-674: Added SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* SW7425-674: Added still channels and motion channel; added support for channel change
* SW7425-674: Added output mode (hold last picture)
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/4   2/15/12 11:32a fbasso
* SW7425-674: Added SW7425-2112: configure ePulldown variable in unified picture structure
* SW7425-674: Added to frame progressive to resolve incorrect cropping at display time.
* SW7425-674: Added Introduced new function BSID_GetRaveItbCbdConfigInfo to retrieve
* SW7425-674: Added default RAVE configuration to use during video decode.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/3   2/10/12 4:16p fbasso
* SW7425-674: Added SW7425-2112: update SID to XDM path to use latest BAVC_MFD_Picture
* SW7425-674: Added format protocol to send buffer to VDC.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/2   2/10/12 10:54a fbasso
* SW7425-674: Added SW7425-2112: added support for motion jpeg decode using XPT RAVE to
* SW7425-674: Added provide input data to SID fw/hw and Xdm interface to supply SID output
* SW7425-674: Added pictures to VDC.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/1   1/26/12 5:46p fbasso
* SW7425-674: Added SW7425-2112: rave support first draft
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-674: Added SW7425-1619: release SID multi channel interface to
* SW7425-674: Added Hydra_Software_Devel
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: Added SW7425-674: modified handling ofinput buffer in streaming mode.
* SW7425-674: Added Extended to support up to sixteen channels
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-674: Added SW7425-1619: add queue based interface and multi channel support
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/7   8/22/11 4:29p fbasso
* SW7425-674: Added SWDTV-8264: returning Transparency RGB color for PNG decode when
* SW7425-674: Added present.
*
* Hydra_Software_Devel/1   5/03/06 09:19a fbasso
* - Created this file
******************************************************************************/


#ifndef BSID_PRIV_H__
#define BSID_PRIV_H__

#include "bsid.h"
#include "bsid_msg.h"

#include "bxdm_picture.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
#define BSID_TOT_MEM_SIZE                                             700 * 1024 /* byte */
#define BSID_ARC_CODE_SIZE                                            512 * 1024 /* byte */
#define BSID_ARC_CODE_ALIGN                                                   10 /* bit */
#define BSID_MBX_MEMORY_SIZE                                                1024 /* byte */
#define BSID_DATA_MEMORY_SIZE                                         100 * 1024 /* byte */
#define BSID_INPUT_DMA_MEMORY_SIZE                                          1024 /* byte */


/*********************************************************************************
/////////////////////// Defines, Typedef, Structs ////////////////////////////////
*********************************************************************************/
#define BSID_RLELEGACY_MAXWIDTH                                      720 /* pixel */
#define BSID_RLEHDDVD_MAXWIDTH                                      1920 /* pixel */
#define BSID_RLEBD_MAXWIDTH                                       8*1024 /* pixel */
#define BSID_JPEG_MAXWIDTH                                        8*1024 /* pixel */
#define BSID_PNGPALETTE_MAXWIDTH                                  8*1024 /* pixel */
#define BSID_PNGRGBA_MAXWIDTH                                     4*1024 /* pixel */
#define BSID_PNG16BPP_MAXWIDTH                                    2*1024 /* pixel */
#define BSID_GIF_MAXWIDTH                                         8*1024 /* pixel */
#define BSID_MEM_ALIGN                                                 3 /* bit */

#define BSID_INCR_INDEX(index, maxValue) {  \
    if ((index + 1) >= maxValue) {          \
        index = 0;                          \
    }                                       \
    else {                                  \
        index ++;                           \
    }                                       \
}

/********************************************************************************/
typedef enum BSID_ArcImageFormat {
    SID_RLE_SVCD   = 0 | (3 << 4),
    SID_RLE_DVD    = 0 | (2 << 4),
    SID_RLE_BLURAY = 0 | (1 << 4),
    SID_RLE_HDDVD  = 0 | (0 << 4),
    SID_PNG        = 1,
    SID_GIF        = 2,
    SID_JPEG       = 3,
    SID_MPEG_IFRAME = 5
} BSID_ArcImageFormat;

/********************************************************************************/
typedef enum BSID_MemoryMode {
    BSID_MemoryMode_eNotUnifiedMemory = 0x0,
    BSID_MemoryMode_eUnifiedMemory    = 0x1,
    BSID_MemoryMode_eMemoryModeLast
} BSID_MemoryMode;

/********************************************************************************/
typedef struct BSID_LinearBuffer {
    uint32_t *pui32_VirtAddr;
    uint32_t  ui32_PhisAddr;
    uint32_t  ui32_Size;
#ifdef BSID_USE_CACHED_MEMORY
    void     *pv_CachedAddr;
#endif
} BSID_LinearBuffer;

/********************************************************************************/
#define BSID_IMGFMT_NUMBER                                 BSID_ImageFormat_eLast
typedef struct BSID_FwHwConfig {
    bool                bSelfTest;
    uint16_t            ui16_JPEGHorizAndVerFilt;
    uint8_t             ui8_AlphaValue;
    bool                b_EndianessSwap;
    BSID_MemoryMode     eMemoryMode;
    BSID_LinearBuffer   sMemory;
    BSID_LinearBuffer   sCodeMemory;
    BSID_LinearBuffer   sMbxMemory;
    BSID_LinearBuffer   sInpDmaMemory;
    BSID_LinearBuffer   sDataMemory[BSID_MAX_CHANNELS];
    BSID_ResourceStatus eSidArcStatus;
    BSID_ArcImageFormat ae_ImgFmtPi2Arc[BSID_IMGFMT_NUMBER];
} BSID_FwHwConfig;

/********************************************************************************/
typedef struct BSID_DataMapHeader {
    uint32_t ui32_InpBufferAddr;
    uint32_t ui32_InpBufferSize;
    uint32_t ui32_NumSegments;
} BSID_DataMapHeader;

/********************************************************************************/
typedef struct BSID_DataMapSegment {
    uint32_t ui32_SegmentOffset;
    uint32_t ui32_SegmentSize;
} BSID_DataMapSegment;

/********************************************************************************/
typedef struct BSID_DataMap {
    BSID_DataMapHeader   sDataMapHeader;
    BSID_DataMapSegment  sFirstSegment;
} BSID_DataMap;

/********************************************************************************/
typedef struct BSID_P_Handle {
    uint32_t            ui32_SidInstance;     /* instance number. */
    BCHP_Handle         hChp;                 /* handle to chip module. */
    BREG_Handle         hReg;                 /* handle to register module. */
    BMEM_Handle         hMem;                 /* handle to memory module. */
    BINT_Handle         hInt;                 /* handle to interrupt module. */
    BSID_FwHwConfig     sFwHwConfig;          /* setup for SID ARC firmware and Hardware */
    BSID_MailboxInfo    sMailbox;             /* mailbox related registers/variables. */
    BINT_CallbackHandle hServiceIsr;          /* isr callback associated with sid irq */
    BKNI_EventHandle    hOperationEvent;      /* event used to implement sync/async logic */
    BERR_Code           ui32_OperationResult; /* syncronous operation result */
    BSID_BootInfo       sBootInfo;
    BERR_Code           (*pExternalBootCallback)(void *, const BSID_BootInfo *);
    void                *pExternalBootCallbackData;
    BSID_ChannelHandle  ahChannel[BSID_MAX_CHANNELS];
    uint32_t            ui32_TotalChannels;
} BSID_P_Handle;

/********************************************************************************/
typedef enum {
    BSID_Operation_eDecodeImage,
    BSID_Operation_eGetStreamInfo,
    BSID_Operation_eDecodeImageSegment,
    BSID_Operation_eSetDmaChunk,
    BSID_Operation_eFlushChannel,
    BSID_Operation_eStartDecode,
    BSID_Operation_eStopDecode,
    BSID_Operation_eLast
} BSID_Operation;

/********************************************************************************/
#define BSID_QUEUE_NUMBER                                   2 * BSID_MAX_CHANNELS
#define BSID_QUEUE_ENTRY_ELEM            sizeof(BSID_Cmd_Decode)/sizeof(uint32_t)
#define BSID_QUEUE_ALIGNMENT                                                    2
#define BSID_DATA_QUEUE_NUM_ELEM         sizeof(BSID_ImageHeader)/sizeof(uint32_t)
typedef struct BSID_ReqQueueEntry {
   BSID_Cmd_Hdr cmdHeader;
   uint32_t     entry[BSID_QUEUE_ENTRY_ELEM];
} BSID_ReqQueueEntry;

typedef struct BSID_RelQueueEntry {
   BSID_Rsp_Hdr rspHeader;
   uint32_t     sidMemStreamInfoBufVirtAddr;
   uint32_t     streamInfoBufVirtAddr;
   uint32_t     checkSumValue;
   uint32_t     operationCompleted;
} BSID_RelQueueEntry;

typedef struct BSID_DataQueueEntry {
   uint32_t     entry[BSID_DATA_QUEUE_NUM_ELEM];
} BSID_DataQueueEntry;

typedef struct BSID_CommandQueueHeader {
    uint32_t ui32_ReadIndex;
    uint32_t ui32_WriteIndex;
    uint32_t ui32_WaitForData;
} BSID_CommandQueueHeader;

typedef struct BSID_ChannelQueue {
    BSID_LinearBuffer sBuf;
    BSID_CommandQueueHeader  sHdr;
} BSID_ChannelQueue;

/********************************************************************************/
#define BSID_MAX_OUTPUT_BUFFER                                                 10
#define BSID_OUTPUT_BUFFER_ALIGNMENT                                            2
#define BSID_RAVE_CDB_LENGHT                                      (3*1024*1024/2)
#define BSID_RAVE_CDB_ALIGNMENT                                                 4
#define BSID_RAVE_ITB_LENGHT                                           (512*1024)
#define BSID_RAVE_ITB_ALIGNMENT                                                 4
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
#define BSID_RAVE_CDB_ENDIAN                                                false
#define BSID_RAVE_ITB_ENDIAN                                                 true
#else
#define BSID_RAVE_CDB_ENDIAN                                                 true
#define BSID_RAVE_ITB_ENDIAN                                                false
#endif

typedef enum BSID_ChannelState {
    /* channel not been fully opened or just closed. */
    BSID_ChannelState_eClose   = 0x0,
    /* channel has been opened, still channels were flushed, playback can begin/resume. */
    BSID_ChannelState_eReady   = 0x1,
    /* channel available for playback */
    BSID_ChannelState_eDecode  = 0x2,
    /* decode was suspended. This were still channels go.  */
    BSID_ChannelState_eSuspend = 0x3,

    BSID_ChannelState_eStop = 0x4
} BSID_ChannelState;

typedef enum BSID_OutputBufferState {
    BSID_OutputBufferState_eIdle        = 0x0, /* no decode, no display */
    BSID_OutputBufferState_eBooked      = 0x1, /* booked for decoding */
    BSID_OutputBufferState_eDecoded     = 0x2, /* decoded, no display */
    BSID_OutputBufferState_eXdmed       = 0x3  /* no decode, display */
} BSID_OutputBufferState;

typedef enum BSID_ChannelChangeOutputMode
{
  BSID_ChannelChangeOutputMode_eMute,
  BSID_ChannelChangeOutputMode_eLastFramePreviousChannel,
  BSID_ChannelChangeOutputMode_eLast
} BSID_ChannelChangeOutputMode;

typedef struct BSID_RaveStatusReport {

    /* start and end of cdb buffer: don't change during playback */
    uint32_t CDB_Base;
    uint32_t CDB_End;

    /* used by pi to update fw */
    struct from_pi {
        uint32_t CDB_Read;
        uint32_t CDB_Valid;
        uint32_t CDB_Wrap;
    } from_pi;

    /* used by fw to indicate where the last decode was executed at */
    struct from_fw {
        uint32_t CDB_Read;
        uint32_t CDB_Wrap;
    } from_fw;

} BSID_RaveStatusReport;

typedef struct BSID_PlaybackQueueEntry {
    uint32_t ui32_PictureIdx;
    uint32_t ui32_CdbAddress;
    uint32_t ui32_CdbSize;
    uint32_t ui32_LastCdbRead;
    uint32_t ui32_Pts;
    uint32_t ui32_OutputState;  /* BSID_OutputBufferState */
    uint32_t ui32_ChannelState; /* BSID_ChannelState */
    uint32_t ui32_OutputBufferAddress;
    uint32_t ui32_OutputBufferMaxSize;
    uint32_t ui32_Width;
    uint32_t ui32_Height;
    uint32_t ui32_Pitch;
    uint32_t ui32_PixelFormat;
    uint32_t ui32_BitPerPixel;
    uint32_t ui32_TrueWidth;
    uint32_t ui32_TrueHeight;
    uint32_t ui32_DecodeErrors;
    uint32_t ui32_setup_time;
    uint32_t ui32_decode_time;
    void * ps_UnifiedPicture;
    uint32_t ui32_UnifiedPictureReady;
    uint32_t ui32_WrapAroundOccurred;
    uint32_t ui32_JpegType;
} BSID_PlaybackQueueEntry;

typedef struct BSID_PlaybackQueueHeader {
    uint32_t ui32_NewPictWriteIndex;
    uint32_t ui32_DecodedReadIndex;
    uint32_t ui32_DisplayReadIndex;
    uint32_t ui32_QueueSize;
    uint32_t ui32_ChannelState;
    uint32_t ui32_InitPlayback;
} BSID_PlaybackQueueHeader;

typedef struct BSID_PlaybackQueue {
    BSID_LinearBuffer        sBuf;
    BSID_PlaybackQueueHeader sHdr;
} BSID_PlaybackQueue;


/********************************************************************************/
typedef struct BSID_P_Channel {
    /* generic */
    BSID_Handle          hSid;
    uint32_t             ui32_ChannelNum;
    uint32_t             ui32_ChannelID;
    uint32_t             ui32_QueueTrueDepth;
    uint32_t             ui32_SequenceNumber;
    BSID_OpenChannelSettings sChSettings;
    BSID_ChannelQueue    sReqQueue;  /* operations from mips to arc */
    BSID_ChannelQueue    sRelQueue;  /* results from arc to mips */
    BSID_ChannelQueue    sDataQueue; /* large data exchange between mips and arc */
    bool                 b_FlushPending;
    BKNI_EventHandle     hSyncEvent;


    /* motion channel only related */
    BSID_ChannelType     e_ChannelType;
    BSID_ChannelState    e_ChannelState;
    BMEM_Handle          hChHeap;
    BAVC_XptContextMap   sContextMap;
    BSID_LinearBuffer    a_OutputBuffer[BSID_MAX_OUTPUT_BUFFER];
    BSID_LinearBuffer    sRaveReport;
    BSID_PlaybackQueue   sPlaybackQueue;
    BXDM_Picture         a_DisplayBuffer[BSID_MAX_OUTPUT_BUFFER];
    unsigned int         last_ITB_Read;
    BSID_ChannelChangeOutputMode e_ChannelChangeOutputMode;
} BSID_P_Channel;

/********************************************************************************/
typedef enum BSID_ChannelQueueStatus {
    BSID_ChannelQueueStatus_NotFull,
    BSID_ChannelQueueStatus_Full
} BSID_ChannelQueueStatus;

/*********************************************************************************
////////////////////// Function prototypes declaration ///////////////////////////
*********************************************************************************/
BERR_Code BSID_P_SetFwHwDefault(BSID_Handle hSid, BSID_Settings ps_DefSettings);
BSID_ResourceStatus BSID_P_GetOperationStatus(BSID_Handle hSid);
BSID_ResourceStatus BSID_P_GetOperationStatus_Isr(BSID_Handle hSid);
BERR_Code BSID_P_CreateChannelMemory(BSID_ChannelHandle hSidCh);
BERR_Code BSID_P_DestroyChannelMemory(BSID_ChannelHandle hSidCh);
BSID_ChannelQueueStatus BSID_P_GetChannelQueueStatus(BSID_ChannelHandle hSidCh);
BERR_Code BSID_P_SuspendChannels(BSID_Handle hSid);
BERR_Code BSID_P_ResumeChannels(BSID_Handle hSid);
bool BSID_IsStillOperationAllowed(BSID_Handle hSid);
bool BSID_IsMotionOperationAllowed(BSID_Handle hSid);

/*********************************************************************************
//////////////////////////////////////////////////////////////////////////////////
*********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BSID_PRIV_H__ */

/* end of file */
