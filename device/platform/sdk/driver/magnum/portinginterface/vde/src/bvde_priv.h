/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvde_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 12/6/11 11:14a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vde/7425/bvde_priv.h $
 * 
 * Hydra_Software_Devel/6   12/6/11 11:14a venkatr
 * SW7425-1903:[7425] vde build broken
 * 
 * Hydra_Software_Devel/5   7/14/11 1:32p speter
 * SW7358-63: [7425] Storing a local copy of the XptContextMap structure
 * 
 * Hydra_Software_Devel/4   6/16/11 10:35a speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/3   6/10/11 7:53p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/2   6/2/11 3:55p speter
 * SW7425-409: [7425] Modifying the frame buffer allocation part
 * 
 * Hydra_Software_Devel/1   5/31/11 4:45p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 ***************************************************************************/

#ifndef BVDE_PRIV_H_
#define BVDE_PRIV_H_

#include "bstd.h"
#include "bkni.h"
#include "bvde.h"
#include "bvde_channel.h"
#include "bdsp.h"
#include "blst_slist.h"
#include "blst_squeue.h"

/* Debug objects */
BDBG_OBJECT_ID_DECLARE(BVDE_Device);
BDBG_OBJECT_ID_DECLARE(BVDE_BufferNode);
BDBG_OBJECT_ID_DECLARE(BVDE_Channel);

/***************************************************************************
Summary:
Channel State
***************************************************************************/
typedef enum BVDE_ChannelState
{
    BVDE_ChannelState_eStopped,             /* Not running */
    BVDE_ChannelState_eStarted,             /* Running in normal operation */
    BVDE_ChannelState_ePaused,              /* Running, paused */
    BVDE_ChannelState_eDisabled,            /* Prepared for flush from started state */
    BVDE_ChannelState_eDisabledPaused,      /* Prepared for flush from paused state */
    BVDE_ChannelState_eMax
} BVDE_ChannelState;


#define BVDE_MAX_CHANNELS  (1) /* 1 decoder */
#define BVDE_MAX_DSP_TASKS (1) /* 1 decoder */

#define BVDE_TOTAL_HORZIZONTAL_PADDING 96     
#define BVDE_TOTAL_VERTICAL_CHROMA_PADDING 48
#define BVDE_TOTAL_VERTICAL_LUMA_PADDING 96

/***************************************************************************
Summary:
Device Handle
***************************************************************************/
typedef struct BVDE_Device
{
    BDBG_OBJECT(BVDE_Device)

    /* Open Parameters */
    BCHP_Handle chpHandle;
    BREG_Handle regHandle;
    BMEM_Handle memHandle;
    BINT_Handle intHandle;
    BTMR_Handle tmrHandle;
    BDSP_Handle dspHandle;
    BVDE_Settings settings;
    
    /* Software resource allocation */
#if BVDE_MAX_CHANNELS > 0
    BDSP_ContextHandle  dspContext;
    BVDE_ChannelHandle  channels[BVDE_MAX_CHANNELS];
    struct 
    {
        BVDE_ChannelState state;
        BVDE_ChannelStartSettings startSettings;
    } channelWatchdogInfo[BVDE_MAX_CHANNELS];
#endif

    /* Interrupts */
    BVDE_InterruptHandlers interrupts;

} BVDE_Device;

/***************************************************************************
Summary:
Buffer Type
***************************************************************************/
typedef enum BVDE_BufferType
{
    BVDE_BufferType_eFrameBuffer,
    BVDE_BufferType_eMax
} BVDE_BufferType;

/***************************************************************************
Summary:
Buffer Node
***************************************************************************/
typedef struct BVDE_BufferNode
{
    BDBG_OBJECT(BVDE_BufferNode)
    BLST_S_ENTRY(BVDE_BufferNode) node;
    void *pMemory;
    uint32_t offset;
    unsigned bufferSize;
    BVDE_BufferType type;
} BVDE_BufferNode;


/***************************************************************************
Summary:
Allocate buffers from the resource pool
***************************************************************************/
BVDE_BufferNode *BVDE_P_AllocateBuffer(
    BVDE_Handle handle,
    BVDE_BufferType type
    );

/***************************************************************************
Summary:
Release buffers to the resource pool
***************************************************************************/
void BVDE_P_FreeBuffer(
    BVDE_Handle handle,
    BVDE_BufferNode *pNode
    );


/***************************************************************************
Summary:
Microsequencer FW version
***************************************************************************/
extern const uint32_t g_BVDE_MS_FirmwareVersion[4];

/***************************************************************************
Summary:
Video Frame Buffer related Memory structure
***************************************************************************/
typedef struct BVDE_FrameBufferMemory
{   
    unsigned lumasize;
    unsigned chromasize;    
    unsigned lumarefsize;
    unsigned chromarefsize;    
    void     *pFrameBufferMemory;
    BXDM_Picture *pUpbMemory; 
    unsigned totalFrames;
    unsigned totalRefFrames;
} BVDE_FrameBufferMemory;


/***************************************************************************
Summary:
Decoder Handle
***************************************************************************/
typedef struct BVDE_Channel
{
    BDBG_OBJECT(BVDE_Channel)
    BVDE_Handle deviceHandle;
    unsigned index;
    unsigned mode;
    char name[10]; /* Channel %u */

    /* Basic State Information */
    BVDE_ChannelState state;
    BVDE_ChannelOpenSettings settings;
    BVDE_ChannelStartSettings startSettings;
    BAVC_XptContextMap contextMap;

    /* DSP Task Information */
    BDSP_TaskHandle task;
    BDSP_TaskCreateSettings taskCreateSettings;
    BDSP_CIT_P_FwBranchInfo primaryBranch;
    BVDE_FrameBufferMemory videoMemory;
} BVDE_Channel;

BERR_Code BVDE_Channel_P_ApplyCodecSettings(BVDE_ChannelHandle handle);
BERR_Code BVDE_Channel_P_AllocateFrameBuffer(BVDE_ChannelHandle handle);
void BVDE_Channel_P_DeAllocateFrameBuffer(BVDE_ChannelHandle handle);
unsigned BVDE_Channel_P_GetResolutionWidth (BVDE_Resolution resolution);
unsigned BVDE_Channel_P_GetResolutionHeight (BVDE_Resolution resolution);


#endif /* #ifndef BVDE_PRIV_H_ */

