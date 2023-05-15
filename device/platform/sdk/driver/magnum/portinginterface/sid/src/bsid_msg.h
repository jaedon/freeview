/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_msg.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 9/6/12 8:59p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid_msg.h $
* 
* Hydra_Software_Devel/4   9/6/12 8:59p fbasso
* SW7425-3898: removed symbolic links from directory.
*
* Hydra_Software_Devel/3   8/24/12 5:02p fbasso
* SW7425-3620: replaced un-cached memory access with cached memory
* access.
*
* Hydra_Software_Devel/2   2/21/12 2:20p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
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
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/3/12 4:53p fbasso
* SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* for 7346
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/7   8/22/11 4:29p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/6   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid_msg.h $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/4   9/6/12 8:59p fbasso
* SW7425-674: Added SW7425-3898: removed symbolic links from directory.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/3   8/24/12 5:02p fbasso
* SW7425-674: Added SW7425-3620: replaced un-cached memory access with cached memory
* SW7425-674: Added access.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/2   2/21/12 2:20p fbasso
* SW7425-674: Added SW7425-2112: added support for MJPEG video decode. Merging new feature
* SW7425-674: Added to Hydra_Software_Devel.
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
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/2   1/3/12 4:53p fbasso
* SW7425-674: Added SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* SW7425-674: Added for 7346
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


#ifndef BSID_MSG_H__
#define BSID_MSG_H__

#include "bkni.h"
#include "bsid.h"
#include "bchp_sid_arc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************
/////////////////////// Defines, Typedef, Structs ////////////////////////////////
*********************************************************************************/

/********************************************************************************/
#define BSID_MSG_BASE                                                 (0x00700000)
typedef enum {
  BSID_MSG_HDR_REV                                      = (BSID_MSG_BASE + 0x0050),
  BSID_MSG_HDR_TEST                                     = (BSID_MSG_BASE + 0x0051),
  BSID_MSG_HDR_INIT                                     = (BSID_MSG_BASE + 0x0052),
  BSID_MSG_HDR_DECODE                                   = (BSID_MSG_BASE + 0x0053),
  BSID_MSG_HDR_DECODESEGMENT                            = (BSID_MSG_BASE + 0x0054),
  BSID_MSG_HDR_GETSTREAMINFO                            = (BSID_MSG_BASE + 0x0055),
  BSID_MSG_HDR_OPENCHANNEL                              = (BSID_MSG_BASE + 0x0057),
  BSID_MSG_HDR_CLOSECHANNEL                             = (BSID_MSG_BASE + 0x0059),
  BSID_MSG_HDR_FLUSHCHANNEL                             = (BSID_MSG_BASE + 0x0060),
  BSID_MSG_HDR_SYNCCHANNEL                              = (BSID_MSG_BASE + 0x0061),
  BSID_MSG_HDR_STARTDECODE                              = (BSID_MSG_BASE + 0x0062),
  BSID_MSG_HDR_STOPDECODE                               = (BSID_MSG_BASE + 0x0063)
} BSID_MSG_HDR;


/********************************************************************************/
typedef struct BSID_Cmd_Hdr {
    uint32_t header;
    uint32_t sequence;
    uint32_t needAck;
} BSID_Cmd_Hdr;


/********************************************************************************/
typedef struct BSID_Rsp_Hdr {
    uint32_t header;
    uint32_t sequence;
    uint32_t errCode;
} BSID_Rsp_Hdr;


/********************************************************************************/
typedef struct BSID_Msg_Hdr {
    uint32_t header;
    uint32_t sequence;
    uint32_t errCode;
} BSID_Msg_Hdr;


/********************************************************************************/
#define BSID_P_CMD_MAXSIZE                                                      64
typedef struct BSID_Cmd {
    BSID_Cmd_Hdr cmdHdr;
    uint32_t     cmdBody[ BSID_P_CMD_MAXSIZE - (sizeof(BSID_Cmd_Hdr)/sizeof(uint32_t)) ];
} BSID_Cmd;


/********************************************************************************/
#define BSID_P_RSP_MAXSIZE                                                      64
typedef struct BSID_Rsp {
    BSID_Rsp_Hdr rspHdr;
    uint32_t     rspBody[ BSID_P_RSP_MAXSIZE - (sizeof(BSID_Rsp_Hdr)/sizeof(uint32_t)) ];
} BSID_Rsp;


/********************************************************************************/
#define BSID_P_MSG_MAXSIZE                                                      64
typedef struct BSID_Msg {
    BSID_Msg_Hdr msgHdr;
    uint32_t     msgBody[ BSID_P_MSG_MAXSIZE - (sizeof(BSID_Msg_Hdr)/sizeof(uint32_t)) ];
} BSID_Msg;


/********************************************************************************/
/* Message Command: INIT */
typedef struct BSID_Cmd_Init {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     frequency;
   uint32_t     baudRate;
   uint32_t     mbxCmdBufAddr;
   uint32_t     mbxRspBufAddr;
   uint32_t     mbxCmdRspBufSize;
   uint32_t     memMode;
   uint32_t     memBase;
   uint32_t     memSize;
   uint32_t     msBufBase;
   uint32_t     msBufSize;
   uint32_t     dmaInfoBufAddr;
   uint32_t     dmaInfoBufSize;
   uint32_t     endianessSwap;
   uint32_t     jpegFiltMode;
   uint32_t     alphaValue;
   uint32_t     dbgMsgOn;
} BSID_Cmd_Init;

/* Message Response: INIT */
typedef struct BSID_Rsp_Init {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_Init;


/********************************************************************************/
/* Message Command: DECODE */
typedef struct BSID_Cmd_Decode {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     imgFormat;
   uint32_t     imgWidth;
   uint32_t     imgHeight;
   uint32_t     imgPitch;
   uint32_t     imgBpp;
   uint32_t     sidMemImgBufAddr;
   uint32_t     sidMemImgBufSize;
   uint32_t     sidMemInpBufAddr0;
   uint32_t     sidMemInpBufSize0;
   uint32_t     sidMemInpBufAddr1;
   uint32_t     sidMemInpBufSize1;
   uint32_t     segMaxHeight;
   uint32_t     firstSegment;
   uint32_t     dbgRleParseHeader;
   uint32_t     dbgMsgOn;
   uint32_t 	isIntraQuantTablePresent;
   uint32_t     lastDMAchunk;
   uint32_t     msBufBase;
   uint32_t     msBufSize;
   uint8_t   	intra_quant_table[BSID_MPEG2IFRAME_INTRA_QUANT_ENRIES];
   uint8_t      gamma_correction_table[BSID_GAMMA_CORRECTION_TABLE_ENTRIES];
   uint32_t     bypass;
   uint32_t     doCheckSum;
} BSID_Cmd_Decode;

/* Message Response: DECODE */
typedef struct BSID_Rsp_Decode {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_Decode;


/********************************************************************************/
/* Message Command: GETSTREAMINFO*/
typedef struct BSID_Cmd_GetStreamInfo {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     imgFormat;
   uint32_t     sidMemInpBufAddr0;
   uint32_t     sidMemInpBufSize0;
   uint32_t     sidMemInpBufAddr1;
   uint32_t     sidMemInpBufSize1;
   uint32_t     sidMemStreamInfoBufPhisAddr;
   uint32_t     sidMemStreamInfoBufVirtAddr;
   uint32_t     sidMemStreamInfoBufSize;
   uint32_t     streamInfoBufVirtAddr;
   uint32_t     bypass;
   uint32_t     lastDMAchunk;
   uint32_t     dbgMsgOn;
} BSID_Cmd_GetStreamInfo;


/* Message Response: GETSTREAMINFO */
typedef struct BSID_Rsp_GetStreamInfo {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_GetStreamInfo;

/********************************************************************************/
/* Message Command: DECODESEGMENT */
typedef struct BSID_Cmd_Decode BSID_Cmd_DecodeSegment;


/* Message Response: DECODESEGMENT */
typedef struct BSID_Rsp_Decode BSID_Rsp_DecodeSegment;

/********************************************************************************/
/* Message Command: OPENCHANNEL */
typedef struct BSID_Cmd_OpenChannel {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     channelID;
   uint32_t     channelNumber;
   uint32_t     priority;
   uint32_t     reqQueueAddr;
   uint32_t     relQueueAddr;
   uint32_t     queueDepth;
   uint32_t     dbgMsgOn;
} BSID_Cmd_OpenChannel;

/* Message Response: OPENCHANNEL */
typedef struct BSID_Rsp_OpenChannel {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_OpenChannel;

/********************************************************************************/
/* Message Command: CLOSECHANNEL */
typedef struct BSID_Cmd_CloseChannel {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     channelID;
   uint32_t     channelNumber;
   uint32_t     dbgMsgOn;
} BSID_Cmd_CloseChannel;

/* Message Response: CLOSECHANNEL */
typedef struct BSID_Rsp_CloseChannel {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_CloseChannel;

/********************************************************************************/
/* Message Command: FLUSHCHANNEL */
typedef struct BSID_Cmd_FlushChannel {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     channelID;
   uint32_t     channelNumber;
   uint32_t     dbgMsgOn;
} BSID_Cmd_FlushChannel;

/* Message Response: FLUSHCHANNEL */
typedef struct BSID_Rsp_FlushChannel {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_FlushChannel;

/********************************************************************************/
/* Message Command: STARTDECODE */
typedef struct BSID_Cmd_StartDecode {
   BSID_Cmd_Hdr cmdHdr;
    uint32_t    channelID;
    uint32_t    channelNumber;
    uint32_t    imgFormat;
    uint32_t    raveReportBufAddr;
    uint32_t    playbackQueueAddr;
    uint32_t    queueDepth;
    uint32_t    sidMemStreamInfoBufPhisAddr;
    uint32_t    sidMemStreamInfoBufVirtAddr;
    uint32_t    sidMemStreamInfoBufSize;
    uint32_t    numPicToDecode;
    uint32_t    endianessInput;
    uint32_t    endianessOutput;
    uint32_t    bypass;
    uint32_t    dbgMsgOn;
} BSID_Cmd_StartDecode;

/* Message Response: STARTDECODE */
typedef struct BSID_Rsp_StartDecode {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_StartDecode;

/********************************************************************************/
/* Message Command: STOPDECODE */
typedef struct BSID_Cmd_StopDecode {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     channelID;
   uint32_t     channelNumber;
   uint32_t     dbgMsgOn;
} BSID_Cmd_StopDecode;

/* Message Response: STOPDECODE */
typedef struct BSID_Rsp_StopDecode {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_StopDecode;

/********************************************************************************/
/* Message Command: SYNCCHANNEL */
typedef struct BSID_Cmd_SyncChannel {
   BSID_Cmd_Hdr cmdHdr;
   uint32_t     channelID;
   uint32_t     channelNumber;
   uint32_t     bypass;
   uint32_t     dbgMsgOn;
} BSID_Cmd_SyncChannel;

/* Message Response: SYNCCHANNEL */
typedef struct BSID_Rsp_SyncChannel {
   BSID_Rsp_Hdr rspHdr;
} BSID_Rsp_SyncChannel;


/********************************************************************************/
typedef struct BSID_MailboxInfo {
    uint32_t  ui32_CmdQPhisAddr;
    void     *pv_CmdQVirtAddr;
    uint32_t  ui32_RspQPhisAddr;
    void     *pv_RspQVirtAddr;
#ifdef BSID_USE_CACHED_MEMORY
    void     *pv_CmdQCachedAddr;
    void     *pv_RspQCachedAddr;
#endif
    BKNI_EventHandle hMailboxEvent;   /* mailbox event */
} BSID_MailboxInfo;

/*********************************************************************************
////////////////////// Function prototypes declaration ///////////////////////////
*********************************************************************************/
BERR_Code BSID_P_SetMailboxInfo(BSID_Handle hSid);
BERR_Code BSID_P_SendCmdInit(BSID_Handle hSid);
BERR_Code BSID_P_SendCmdDecodeStillImage(BSID_ChannelHandle hSidCh,
                                      const BSID_DecodeImage *ps_DecodeImage);
BERR_Code BSID_P_SendCmdGetStreamInfo(BSID_ChannelHandle hSidCh,
                                      BSID_StreamInfo *ps_StreamInfo);
BERR_Code BSID_P_SendCmdDecodeSegment(BSID_ChannelHandle hSidCh,
                                      const BSID_DecodeSegment *ps_DecodeSegment);
void BSID_P_Dispatch_Irq(void *pv_Param1, int iParam2);
void BSID_P_BuildGammaCorrectionTable(
    BSID_Handle hSid,
    unsigned char *gamma_correction_table
    );
BERR_Code BSID_P_SendCmdOpenChannel(BSID_ChannelHandle hSidCh);
BERR_Code BSID_P_SendCmdCloseChannel(BSID_ChannelHandle hSidCh);
void BSID_P_SetDmaChunkInfo(BSID_ChannelHandle  hSidCh,
                            const BSID_DmaChunkInfo *ps_DmaChunkInfo);
void BSID_P_SetDmaChunkInfo_Isr(BSID_ChannelHandle  hSidCh,
                            const BSID_DmaChunkInfo *ps_DmaChunkInfo);
BERR_Code BSID_P_DisableForFlush(BSID_ChannelHandle hSidCh);
BERR_Code BSID_P_FlushChannel(BSID_ChannelHandle  hSidCh,
                            const BSID_FlushSettings *ps_FlushSettings);
BERR_Code BSID_P_StartDecode(BSID_ChannelHandle  hSidCh,
                            const BSID_StartDecodeSettings *ps_StartDecodeSettings);
BERR_Code BSID_P_StopDecode(BSID_ChannelHandle  hSidCh);
BERR_Code BSID_P_FlushDecode(BSID_ChannelHandle hSidCh);
BERR_Code BSID_P_SendSyncChannel(BSID_ChannelHandle hSidCh);

BERR_Code BSID_P_GetPictureCount_isr(
    void *pvHandle, uint32_t *puiPictureCount);
BERR_Code BSID_P_PeekAtPicture_isr(
    void *pvHandle, uint32_t uiIndex, const BXDM_Picture **pUnifiedPicture);
BERR_Code BSID_P_GetNextPicture_isr(
    void *pvHandle, const BXDM_Picture **pUnifiedPicture);
BERR_Code BSID_P_ReleasePicture_isr(
    void *pvHandle, const BXDM_Picture *pUnifiedPicture,
    const BXDM_Decoder_ReleasePictureInfo *pReleasePictureInfo);
BERR_Code BSID_P_GetPictureDropPendingCount_isr(
    void *pvHandle, uint32_t *puiPictureDropPendingCount);
BERR_Code BSID_P_RequestPictureDrop_isr(
    void *pvHandle, uint32_t *puiPictureDropRequestCount);
BERR_Code BSID_P_DisplayInterruptEvent_isr(
    void *pvHandle);

/*********************************************************************************
//////////////////////////////////////////////////////////////////////////////////
*********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BSID_MSG_H__ */

/* end of file */
