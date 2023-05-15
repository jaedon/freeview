/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvee_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/4/12 7:09p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vee/7346/bvee_priv.h $
 * 
 * Hydra_Software_Devel/7   5/4/12 7:09p venkatr
 * SW7429-143 : [7429] Added macros for Luma, chroma buffers for HD
 * resolution
 * 
 * Hydra_Software_Devel/6   4/26/12 3:31p venkatr
 * SW7346-569 : [7346] Removing C bitfields for ITB entries.
 * 
 * Hydra_Software_Devel/5   3/7/12 10:11a venkatr
 * SW7231-585:[7231] Add structure to hold the external interrupt to DSP
 * 
 * Hydra_Software_Devel/4   12/2/11 6:37p venkatr
 * SW7346-543:[7346] H264 encoder output on chip of
 * T2V0040266_Blue_boarding_640x480p.yuv has bit miss-match with
 * simulator
 * 
 * Hydra_Software_Devel/3   11/8/11 3:52p venkatr
 * SW7346-515:[7346] VEE PI code for allocating reference buffers, and
 * modified for setting task user config
 * 
 * Hydra_Software_Devel/2   11/4/11 6:56p venkatr
 * SW7346-515:[7346] Removing malloc from isr context, hence static
 * allocation for picture parameter buffer during open
 * 
 * Hydra_Software_Devel/1   10/19/11 8:31p venkatr
 * SW7346-515 : [7346] VEE PI code initial check in
 * 
 *
 ***************************************************************************/
#ifndef BVEE_PRIV_H_
#define BVEE_PRIV_H_

#include "bstd.h"
#include "bkni.h"
#include "bvee.h"
#include "bvee_channel.h"
#include "bdsp.h"

/* Debug objects */
BDBG_OBJECT_ID_DECLARE(BVEE_Device);
BDBG_OBJECT_ID_DECLARE(BVEE_Channel);

#define BVEE_MAX_CHANNELS  (1) /* 1 encoder */
#define BVEE_MAX_DSP_TASKS (1) /* 1 encoder */
#define BVEE_MAX_PICTURE_BUFFERS (16) /* capture buffers*/
#define BVEE_MAX_VIDEODESCRIPTORS 32
#define BVEE_MAX_ITBDESCRIPTORS 32
#define BVEE_MAX_METADATADESCRIPTORS 1
#define BVEE_DEFAULT_PICT_WIDTH 440
#define BVEE_DEFAULT_PICT_HEIGHT 224
#define BVEE_DEFAULT_STRIPE_WIDTH 128

#define BVEE_ITB_ENTRY_TYPE_BASE_ADDRESS (0x20)
#define BVEE_ITB_ENTRY_TYPE_PTS_DTS      (0x21)
#define BVEE_ITB_ENTRY_TYPE_MUX_ESCR     (0x61)
#define BVEE_ITB_ENTRY_TYPE_ALGO_INFO    (0x62)

/* (CEILING((PicWidth+(Padding*2))/StripeWidth,1)* StripeWidth)*(CEILING((PicHeight +( Padding *2))/32,1)*32)) */
#define BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE			487424
/* (=(CEILING((PicWidth+(Padding*2))/StripeWidth,1)* StripeWidth)*(CEILING(((PicHeight/2) +( Padding *2))/32,1)*32)) */
#define BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE		286720

#define BVEE_H264_ENCODE_REF_LUMASTRIPE_HEIGHT      544  /*672*/
#define BVEE_H264_ENCODE_REF_CHROMASTRIPE_HEIGHT    304  /*336*/

/* LUMA FRAME BUF SIZE for HD resolution */
#define BVEE_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE_HD			1126400
/* CHROMA FRAME BUF SIZE for HD resolution */
#define BVEE_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE_HD		630784

#define BVEE_H264_ENCODE_REF_LUMASTRIPE_HEIGHT_HD      784
#define BVEE_H264_ENCODE_REF_CHROMASTRIPE_HEIGHT_HD    432
/***************************************************************************
Summary:
Channel State
***************************************************************************/
typedef enum BVEE_Channel_State
{
    BVEE_ChannelState_eStopped,             /* Not running */
    BVEE_ChannelState_eStarted,             /* Running in normal operation */
    BVEE_ChannelState_ePaused,              /* Running, paused */
    BVEE_ChannelState_eMax
} BVEE_Channel_State;
#define BVEE_MAX_CHANNELS  (1) /* 1 decoder */
#define BVEE_MAX_DSP_TASKS (1) /* 1 decoder */


typedef struct BVEE_Device
{
    BDBG_OBJECT(BVEE_Device)

    /* Open Parameters */
    BCHP_Handle chpHandle;
    BREG_Handle regHandle;
    BMEM_Handle memHandle;
    BINT_Handle intHandle;
    BTMR_Handle tmrHandle;
    BDSP_Handle dspHandle;
    BVEE_OpenSettings opensettings;
    
    /* Software resource allocation */
#if 1 /*BVEE_MAX_CHANNELS > 0*/
    BDSP_ContextHandle  dspContext;
    BVEE_ChannelHandle  channels[BVEE_MAX_CHANNELS];
    struct 
    {
        BVEE_Channel_State state;
        BVEE_ChannelStartSettings startsettings;
    } channelWatchdogInfo[BVEE_MAX_CHANNELS];
#endif

    /* Interrupts */
    BVEE_InterruptHandlers interrupts;

} BVEE_Device;

/***************************************************************************
Summary:
Allocate frame buffer for capturing raw video data
***************************************************************************/
 BERR_Code BVEE_Channel_P_AllocatePictParamBuffer(
    BVEE_ChannelHandle handle
    );

/***************************************************************************
Summary:
Release the memory allocated for capture buffer
***************************************************************************/
 BERR_Code BVEE_Channel_P_DeAllocatePictParamBuffer(
    BVEE_ChannelHandle handle
    );
/***************************************************************************
Summary:
Device Level Interrupts
***************************************************************************/
typedef enum BVEE_CHROMA_SAMPLING
{
    BVEE_eCHROMA_SAMPLING_420,
    BVEE_eCHROMA_SAMPLING_422,
    BVEE_eCHROMA_SAMPLING_444,
    BVEE_eCHROMA_SAMPLING_LAST,
    BVEE_eCHROMA_SAMPLING_INVALID
}BVEE_CHROMA_SAMPLING;

/***************************************************************************
Summary:
Device Level Interrupts
***************************************************************************/
typedef struct BVEE_CapBufferMemory
{   
    unsigned numpictures;
    BVENC_VF_sPicParamBuff *pPpBuffer;/*To store picture parameter sent to bdsp*/
    void *pPDesc; /*To store picture desc sent by application/nexus*/
    bool bValid;
}BVEE_CapBufferMemory;

typedef struct BVEE_P_ITBData
{
	uint32_t words[4];
}BVEE_P_ITBData;
typedef union BVEE_P_ITBEntry
{
   uint32_t data[16];
   struct {
   BVEE_P_ITBData baseAddress;
   BVEE_P_ITBData ptsDts;
   BVEE_P_ITBData bitRate;
   BVEE_P_ITBData escrMetadata;
   }fields;
}BVEE_P_ITBEntry;

/* Get Field Routines for ITB fields */
#define BVEE_ITB_WORD(Entry,Field) BVEE_ITB_##Entry##_##Field##_WORD
#define BVEE_ITB_MASK(Entry,Field) BVEE_ITB_##Entry##_##Field##_MASK
#define BVEE_ITB_SHIFT(Entry,Field) BVEE_ITB_##Entry##_##Field##_SHIFT


#define BVEE_ITB_GET_FIELD(Memory,Entry,Field)\
	((((Memory)->words[BVEE_ITB_WORD(Entry,Field)] & \
       BVEE_ITB_MASK(Entry,Field)) >> \
	  BVEE_ITB_SHIFT(Entry,Field)))

/* General Fields */
#define BVEE_ITB_GENERIC_ENTRY_TYPE_WORD      (0)
#define BVEE_ITB_GENERIC_ENTRY_TYPE_MASK      (0xFF000000)
#define BVEE_ITB_GENERIC_ENTRY_TYPE_SHIFT     (24)

/* Base Address Fields */
#define BVEE_ITB_BASE_ADDRESS_ERROR_WORD            (0)
#define BVEE_ITB_BASE_ADDRESS_ERROR_MASK            (0x00800000)
#define BVEE_ITB_BASE_ADDRESS_ERROR_SHIFT           (23)
#define BVEE_ITB_BASE_ADDRESS_CDB_ADDRESS_WORD      (1)
#define BVEE_ITB_BASE_ADDRESS_CDB_ADDRESS_MASK      (0xFFFFFFFF)
#define BVEE_ITB_BASE_ADDRESS_CDB_ADDRESS_SHIFT     (0)
#define BVEE_ITB_BASE_ADDRESS_FRAME_VALID_WORD      (2)
#define BVEE_ITB_BASE_ADDRESS_FRAME_VALID_MASK      (0x80000000)
#define BVEE_ITB_BASE_ADDRESS_FRAME_VALID_SHIFT     (31)
#define BVEE_ITB_BASE_ADDRESS_FRAME_LENGTH_WORD     (2)
#define BVEE_ITB_BASE_ADDRESS_FRAME_LENGTH_MASK     (0x0000FFFF)
#define BVEE_ITB_BASE_ADDRESS_FRAME_LENGTH_SHIFT    (0)

/* PTS_DTS Fields */
#define BVEE_ITB_PTS_DTS_DTS_VALID_WORD     (0)
#define BVEE_ITB_PTS_DTS_DTS_VALID_MASK     (0x00008000)
#define BVEE_ITB_PTS_DTS_DTS_VALID_SHIFT    (15)
#define BVEE_ITB_PTS_DTS_PTS_32_WORD        (0)
#define BVEE_ITB_PTS_DTS_PTS_32_MASK        (0x00000002)
#define BVEE_ITB_PTS_DTS_PTS_32_SHIFT       (1)
#define BVEE_ITB_PTS_DTS_DTS_32_WORD        (0)
#define BVEE_ITB_PTS_DTS_DTS_32_MASK        (0x00000001)
#define BVEE_ITB_PTS_DTS_DTS_32_SHIFT       (0)
#define BVEE_ITB_PTS_DTS_PTS_WORD           (1)
#define BVEE_ITB_PTS_DTS_PTS_MASK           (0xFFFFFFFF)
#define BVEE_ITB_PTS_DTS_PTS_SHIFT          (0)
#define BVEE_ITB_PTS_DTS_STC_UPPER_WORD     (2)
#define BVEE_ITB_PTS_DTS_STC_UPPER_MASK     (0xFFFFFFFF)
#define BVEE_ITB_PTS_DTS_STC_UPPER_SHIFT    (0)
#define BVEE_ITB_PTS_DTS_STC_LOWER_WORD     (3)
#define BVEE_ITB_PTS_DTS_STC_LOWER_MASK     (0xFFFFFFFF)
#define BVEE_ITB_PTS_DTS_STC_LOWER_SHIFT    (0)

/* BIT_RATE fields */
#define BVEE_ITB_BIT_RATE_SHR_WORD               (1)
#define BVEE_ITB_BIT_RATE_SHR_MASK               (0xFFFF0000)
#define BVEE_ITB_BIT_RATE_SHR_SHIFT              (16)
#define BVEE_ITB_BIT_RATE_TICKS_PER_BIT_WORD     (1)
#define BVEE_ITB_BIT_RATE_TICKS_PER_BIT_MASK     (0x0000FFFF)
#define BVEE_ITB_BIT_RATE_TICKS_PER_BIT_SHIFT    (0)
#define BVEE_ITB_BIT_RATE_SAMPLE_RATE_WORD       (2)
#define BVEE_ITB_BIT_RATE_SAMPLE_RATE_MASK       (0xFFFFFFFF)
#define BVEE_ITB_BIT_RATE_SAMPLE_RATE_SHIFT      (0)

/* ESCR_METADATA fields */
#define BVEE_ITB_ESCR_METADATA_ESCR_WORD         (1)
#define BVEE_ITB_ESCR_METADATA_ESCR_MASK         (0xFFFFFFFF)
#define BVEE_ITB_ESCR_METADATA_ESCR_SHIFT        (0)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_WORD    (2)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_MASK    (0xFFFFFFFF)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_SHIFT   (0)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_WORD    (3)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_MASK    (0x80000000)
#define BVEE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_SHIFT   (31)

typedef struct BVEE_ChannelOutputDescriptorInfo
{   
    uint32_t uiITBBufferShadowReadOffset; /* Points to the ITB entry that needs to be parsed next */
    uint32_t uiCDBBufferShadowReadOffset; /* Points to the CDB location that needs to be muxed next */
    
    uint32_t uiCDBBufferShadowValidOffset;
      
    BAVC_VideoBufferDescriptor *pstDescriptorsCached;
    BAVC_VideoBufferDescriptor *pstDescriptorsUncached;
    BAVC_VideoMetadataDescriptor *pstMetadataCached;
    BAVC_VideoMetadataDescriptor *pstMetadataUncached;

    uint32_t uiDescriptorWriteOffset;
    uint32_t uiDescriptorReadOffset;

    uint32_t uiMetadataDescriptorWriteOffset;
    uint32_t uiMetadataDescriptorReadOffset;
    /* ITB Parsing Info */
    struct
    {
        BVEE_P_ITBEntry current, next;
    } itb;
    bool bFrameStart;
    bool bMetadataSent;   
    unsigned uiPendingDescriptors;
    unsigned uiConsumedDescriptors;
    
    void *pCdbBaseCached;
}BVEE_ChannelOutputDescriptorInfo;    
/***************************************************************************
Summary:
VEE channel
***************************************************************************/
typedef struct BVEE_Channel
{
    BDBG_OBJECT(BVEE_Channel)
    BVEE_Handle devicehandle;
    unsigned index;
    unsigned mode;
    char name[10]; /* Channel %u */

    /* Basic State Information */
    BVEE_Channel_State state;
    BVEE_ChannelOpenSettings opensettings;
    BVEE_ChannelStartSettings startsettings;
    BVEE_CapBufferMemory *capturepicture; /*VEE Source*/
    BAVC_XptContextMap contextmap;    /*VEE Destination*/
    BVEE_ChannelOutputDescriptorInfo veeoutput;
    
    /* DSP Task Information */
    BDSP_TaskHandle task;
    BDSP_TaskCreateSettings taskcreatesettings;
    BDSP_CIT_P_FwBranchInfo primarybranch;
    uint32_t ui32RefFrameBaseAddr;
} BVEE_Channel;
/***************************************************************************
Summary:
Struct for External interrupt to DSP
***************************************************************************/
typedef struct BVEE_ExtInterrupt
{
	BDSP_Handle	hDsp;
	void * pExtInterruptHandle;
}BVEE_ExtInterrupt;

#define BVEE_P_ITBENTRY_ERROR_OFFSET 0
#define BVEE_P_ITBENTRY_ERROR_SHIFT 23
#define BVEE_P_ITBENTRY_ERROR_MASK 0x00000001

#define BVEE_P_ITBENTRY_CDBADDRESS_OFFSET 1
#define BVEE_P_ITBENTRY_CDBADDRESS_SHIFT 0
#define BVEE_P_ITBENTRY_CDBADDRESS_MASK 0xFFFFFFFF

#define BVEE_P_ITBENTRY_DTSVALID_OFFSET 4
#define BVEE_P_ITBENTRY_DTSVALID_SHIFT 15
#define BVEE_P_ITBENTRY_DTSVALID_MASK 0x00000001

#define BVEE_P_ITBENTRY_PTS32_OFFSET 4
#define BVEE_P_ITBENTRY_PTS32_SHIFT 1
#define BVEE_P_ITBENTRY_PTS32_MASK 0x00000001

#define BVEE_P_ITBENTRY_DTS32_OFFSET 4
#define BVEE_P_ITBENTRY_DTS32_SHIFT 0
#define BVEE_P_ITBENTRY_DTS32_MASK 0x00000001

#define BVEE_P_ITBENTRY_PTS_OFFSET 5
#define BVEE_P_ITBENTRY_PTS_SHIFT 0
#define BVEE_P_ITBENTRY_PTS_MASK 0xFFFFFFFF

#define BVEE_P_ITBENTRY_DTS_OFFSET 6
#define BVEE_P_ITBENTRY_DTS_SHIFT 0
#define BVEE_P_ITBENTRY_DTS_MASK 0xFFFFFFFF

#define BVEE_P_ITBENTRY_IFRAME_OFFSET 7
#define BVEE_P_ITBENTRY_IFRAME_SHIFT 8
#define BVEE_P_ITBENTRY_IFRAME_MASK 0x00000001

#define BVEE_P_ITBENTRY_SHR_OFFSET 9
#define BVEE_P_ITBENTRY_SHR_SHIFT 16
#define BVEE_P_ITBENTRY_SHR_MASK 0x0000FFFF

#define BVEE_P_ITBENTRY_TICKSPERBIT_OFFSET 9
#define BVEE_P_ITBENTRY_TICKSPERBIT_SHIFT 0
#define BVEE_P_ITBENTRY_TICKSPERBIT_MASK 0x0000FFFF

#define BVEE_P_ITBENTRY_ESCR_OFFSET 13
#define BVEE_P_ITBENTRY_ESCR_SHIFT 0
#define BVEE_P_ITBENTRY_ESCR_MASK 0xFFFFFFFF

#define BVEE_P_ITBENTRY_OPTS_OFFSET 14
#define BVEE_P_ITBENTRY_OPTS_SHIFT 0
#define BVEE_P_ITBENTRY_OPTS_MASK 0xFFFFFFFF

#define BVEE_P_ITBENTRY_METADATA_OFFSET 15
#define BVEE_P_ITBENTRY_METADATA_SHIFT 0
#define BVEE_P_ITBENTRY_METADATA_MASK 0xFFFFFFFF

#define BVEE_P_ITBEntry_Get(_pentry, _field) (((_pentry)->data[BVEE_P_ITBENTRY_##_field##_OFFSET] >> BVEE_P_ITBENTRY_##_field##_SHIFT ) & BVEE_P_ITBENTRY_##_field##_MASK )

#define BVEE_P_ITBEntry_GetCDBAddress(_pentry) BVEE_P_ITBEntry_Get(_pentry, CDBADDRESS)
#define BVEE_P_ITBEntry_GetError(_pentry) BVEE_P_ITBEntry_Get(_pentry, ERROR)
#define BVEE_P_ITBEntry_GetDTS(_pentry) ( ( ( (uint64_t) BVEE_P_ITBEntry_Get(_pentry, DTS32)) << 32 ) | BVEE_P_ITBEntry_Get(_pentry, DTS) )
#define BVEE_P_ITBEntry_GetDTSValid(_pentry) BVEE_P_ITBEntry_Get(_pentry, DTSVALID)
#define BVEE_P_ITBEntry_GetPTS(_pentry) ( ( ( (uint64_t) BVEE_P_ITBEntry_Get(_pentry, PTS32)) << 32 ) | BVEE_P_ITBEntry_Get(_pentry, PTS) )
#define BVEE_P_ITBEntry_GetIFrame(_pentry) BVEE_P_ITBEntry_Get(_pentry, IFRAME)
#define BVEE_P_ITBEntry_GetTicksPerBit(_pentry) ((uint16_t) BVEE_P_ITBEntry_Get(_pentry, TICKSPERBIT))
#define BVEE_P_ITBEntry_GetSHR(_pentry) ((int16_t) BVEE_P_ITBEntry_Get(_pentry, SHR))
#define BVEE_P_ITBEntry_GetESCR(_pentry) BVEE_P_ITBEntry_Get(_pentry, ESCR)
#define BVEE_P_ITBEntry_GetOriginalPTS(_pentry) BVEE_P_ITBEntry_Get(_pentry, OPTS)
#define BVEE_P_ITBEntry_GetMetadata(_pentry) BVEE_P_ITBEntry_Get(_pentry, METADATA)

#define BVEE_P_ITBEntry_IsEOS(_pentry) \
   ( ( 0 == BVEE_P_ITBEntry_GetSHR(_pentry) ) \
     && ( 0 == BVEE_P_ITBEntry_GetTicksPerBit(_pentry) ) \
     && ( 0 == BVEE_P_ITBEntry_GetPTS(_pentry) ) \
     && ( 0 == BVEE_P_ITBEntry_GetDTS(_pentry) ) \
     && ( 0 == BVEE_P_ITBEntry_GetOriginalPTS(_pentry) ) )
#endif
