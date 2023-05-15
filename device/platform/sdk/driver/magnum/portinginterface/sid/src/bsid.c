/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 9/17/12 2:59p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid.c $
* 
* Hydra_Software_Devel/12   9/17/12 2:59p fbasso
* SW7445-34: added support for 7445 to sid.
*
* Hydra_Software_Devel/11   8/24/12 5:02p fbasso
* SW7425-3620: replaced un-cached memory access with cached memory
* access.
*
* Hydra_Software_Devel/10   7/31/12 5:02p jtna
* SW7425-2449: fix BSID power management
*
* Hydra_Software_Devel/9   6/13/12 3:06p fbasso
* SW7425-3239: fixed calculation of memory region for SID boot and text
* segments to be authenticated during secure boot
*
* Hydra_Software_Devel/8   5/2/12 5:22p fbasso
* SW7425-2449: added missing call to acquire and release power resources
* from bchp_pwr base modules.
*
* Hydra_Software_Devel/7   5/1/12 1:08p fbasso
* SW7584-18: added support for 7584 to SID
*
* Hydra_Software_Devel/6   3/19/12 3:52p fbasso
* SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* down and up sequence.
*
* Hydra_Software_Devel/5   3/5/12 10:45a fbasso
* SW7435-26: added support for 7435 to SID PI.
*
* Hydra_Software_Devel/4   2/21/12 2:20p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
*
* Hydra_Software_Devel/SW7425-2112/6   2/20/12 3:50p fbasso
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
* Hydra_Software_Devel/1   1/20/12 2:04p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
*
* Hydra_Software_Devel/sid_channel_interface_devel/3   1/3/12 4:53p fbasso
* SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* for 7346
*
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: modified handling ofinput buffer in streaming mode.
* Extended to support up to sixteen channels
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/12   8/22/11 4:29p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/11   6/21/11 11:12a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid.c $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/12   9/17/12 2:59p fbasso
* SW7425-674: Added SW7445-34: added support for 7445 to sid.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/11   8/24/12 5:02p fbasso
* SW7425-674: Added SW7425-3620: replaced un-cached memory access with cached memory
* SW7425-674: Added access.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/10   7/31/12 5:02p jtna
* SW7425-674: Added SW7425-2449: fix BSID power management
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/9   6/13/12 3:06p fbasso
* SW7425-674: Added SW7425-3239: fixed calculation of memory region for SID boot and text
* SW7425-674: Added segments to be authenticated during secure boot
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/8   5/2/12 5:22p fbasso
* SW7425-674: Added SW7425-2449: added missing call to acquire and release power resources
* SW7425-674: Added from bchp_pwr base modules.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/7   5/1/12 1:08p fbasso
* SW7425-674: Added SW7584-18: added support for 7584 to SID
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/6   3/19/12 3:52p fbasso
* SW7425-674: Added SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* SW7425-674: Added down and up sequence.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/5   3/5/12 10:45a fbasso
* SW7425-674: Added SW7435-26: added support for 7435 to SID PI.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/4   2/21/12 2:20p fbasso
* SW7425-674: Added SW7425-2112: added support for MJPEG video decode. Merging new feature
* SW7425-674: Added to Hydra_Software_Devel.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/6   2/20/12 3:50p fbasso
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
* SW7425-674: Added Hydra_Software_Devel/1   1/20/12 2:04p fbasso
* SW7425-674: Added SW7425-1619: release SID multi channel interface to
* SW7425-674: Added Hydra_Software_Devel
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/3   1/3/12 4:53p fbasso
* SW7425-674: Added SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* SW7425-674: Added for 7346
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: Added SW7425-674: modified handling ofinput buffer in streaming mode.
* SW7425-674: Added Extended to support up to sixteen channels
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-674: Added SW7425-1619: add queue based interface and multi channel support
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/12   8/22/11 4:29p fbasso
* SW7425-674: Added SWDTV-8264: returning Transparency RGB color for PNG decode when
* SW7425-674: Added present.
*
* Hydra_Software_Devel/1   5/03/06 09:19a fbasso
* - Created this file
******************************************************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#include "bsid.h"
#include "bsid_priv.h"
#include "bsid_img.h"
#include "bsid_msg.h"
#if ((BCHP_CHIP==7435) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==7584) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==7445) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_sid_l2.h"
#else
#include "bchp_int_id_gfx_l2.h"
#endif
#include "bchp_sid_arc_core.h"
#include "bchp_sid_arc_dbg.h"
#include "bchp_sid_arc.h"
#include "bchp_sid_strm.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(BSID);

#define BSID_PI_VERSION         0x00001000
#define BSID_FW_VERSION         0x00001000
#if ((BCHP_CHIP==7435) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==7584) && (BCHP_VER >= BCHP_VER_A0)) || \
    ((BCHP_CHIP==7445) && (BCHP_VER >= BCHP_VER_A0))
#define BCHP_INT_ID_SID_INTR  BCHP_INT_ID_CREATE(BCHP_SID_L2_CPU_STATUS, BCHP_SID_L2_CPU_STATUS_SID_INTR_SHIFT)
#endif

/*****************************************************************************/
static const BSID_Settings stMyDefSettings =
{
    0x0,                      /* SID default instance number */
    0x0000,                   /* horizontal and vertical filter selection for JPEG upsampling */
    0xFF,                     /* transparency value (alpha) that will be attached in case of pixel format RGB or YUV444 */
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
    true,                     /* Endianess selection: true for big endian */
#else
    false,                    /* Endianess selection: false for little endian */
#endif
    true,                     /* When uploading the SID ARC code to DRAM perfom a sanity test */
    NULL,                     /* Boot callback function for SID. This is needed for firmware authentication */
    NULL,                     /* App structure ptr that needs to be passed into the callback */
};


/*****************************************************************************/
static const BSID_RevisionInfo stMyRevisionInfo =
{
    BSID_PI_VERSION,          /* PI version */
    BSID_FW_VERSION,          /* FW version */
    BSID_RLELEGACY_MAXWIDTH,  /* image width max for legacy RLE */
    BSID_RLEHDDVD_MAXWIDTH,   /* image width max for HD-DVD RLE */
    BSID_RLEBD_MAXWIDTH,      /* image width max for BD RLE */
    BSID_JPEG_MAXWIDTH,       /* image width max for JPEG */
    BSID_PNGPALETTE_MAXWIDTH, /* image width max for PNG Palletized */
    BSID_PNGRGBA_MAXWIDTH,    /* image width max for PNG RGBA */
    BSID_PNG16BPP_MAXWIDTH,   /* image width max for PNG 16 BitPerPixel */
    BSID_GIF_MAXWIDTH,        /* image width max for GIF */
};


/******************************************************************************
* Function name: BSID_Open
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_Open(
    BSID_Handle         *phSid,
    BCHP_Handle          hChp,
    BREG_Handle          hReg,
    BMEM_Handle          hMem,
    BINT_Handle          hInt,
    const BSID_Settings *ps_DefSettings)
{
    BERR_Code     retCode = BERR_SUCCESS;
    BSID_Handle   hSid    = NULL;

    BDBG_ENTER(BSID_Open);

    /* sanity check */
    BDBG_ASSERT(phSid);
    BDBG_ASSERT(hChp);
    BDBG_ASSERT(hReg);
    BDBG_ASSERT(hMem);
    BDBG_ASSERT(hInt);
    BDBG_ASSERT(ps_DefSettings);

    /* make sure instance number is valid */
    if (ps_DefSettings->ui32_SidInstance >= BSID_NUM_DEVICES)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* allocate device handle */
    hSid = (BSID_Handle)BKNI_Malloc(sizeof (BSID_P_Handle));
    if (hSid == NULL)
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* init device handle structure to default value */
    BKNI_Memset((void *)hSid, 0x00, sizeof (BSID_P_Handle));

    /* save instance number */
    hSid->ui32_SidInstance = ps_DefSettings->ui32_SidInstance;

    /* save base modules handles for future use */
    hSid->hChp = hChp;
    hSid->hReg = hReg;
    hSid->hInt = hInt;
    hSid->hMem = hMem;

    /* extern boot book keeping */
    hSid->pExternalBootCallbackData = ps_DefSettings->pv_ExternalBootCallbackData;
    hSid->pExternalBootCallback = ps_DefSettings->p_ExternalBootCallback;

    /* set firmware/hardware configuration and buffers */
    retCode = BSID_P_SetFwHwDefault(hSid, *ps_DefSettings);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BSID_P_SetFwHwDefault returned with error 0x%x", retCode));
        return BERR_TRACE(retCode);
    }

    /* acquire power resources */
#ifdef BCHP_PWR_RESOURCE_SID
    BCHP_PWR_AcquireResource(hSid->hChp, BCHP_PWR_RESOURCE_SID);
#endif
#ifdef BCHP_PWR_RESOURCE_SID_SRAM
    BCHP_PWR_AcquireResource(hSid->hChp, BCHP_PWR_RESOURCE_SID_SRAM);
#endif

    /* reset sid arc, download sid arc firmware, boot sid arc */
    retCode = BSID_P_BootArc(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BSID_P_BootArc failed"));
        return BERR_TRACE(retCode);
    }

    /* set mailbox information */
    retCode = BSID_P_SetMailboxInfo(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BSID_P_SetMailboxInfo failed"));
        return BERR_TRACE(retCode);
    }

    /* create and associate dispatch callback for sid general purpose interrupt */
    retCode = BINT_CreateCallback(
        &hSid->hServiceIsr,
        hSid->hInt,
        BCHP_INT_ID_SID_INTR,
        BSID_P_Dispatch_Irq,
        hSid,
        0);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_Open: BINT_CreateCallback failed"));
       return BERR_TRACE(retCode);
    }

    /* enable sid callback */
    retCode = BINT_EnableCallback(hSid->hServiceIsr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BINT_EnableCallback failed"));
        return BERR_TRACE(retCode);
    }

    /* create event for maibox handling */
    retCode = BKNI_CreateEvent(&hSid->sMailbox.hMailboxEvent);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_Open: BKNI_CreateEvent failed"));
       return BERR_TRACE(retCode);
    }

    /* reset mailbox event */
    BKNI_ResetEvent(hSid->sMailbox.hMailboxEvent);

    /* create event for maibox handling */
    retCode = BKNI_CreateEvent(&hSid->hOperationEvent);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_Open: BKNI_CreateEvent failed"));
       return BERR_TRACE(retCode);
    }

    /* reset operation event */
    BKNI_ResetEvent(hSid->hOperationEvent);

    /* init sid arc */
    retCode = BSID_P_SendCmdInit(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BSID_P_SendCmdInit failed"));
        return BERR_TRACE(retCode);
    }

    /* prepare device handle */
    *phSid = hSid;

    BDBG_LEAVE(BSID_Open);
    return BERR_TRACE(retCode);
}


/******************************************************************************
* Function name: BSID_Close
*
* Comments:
*
******************************************************************************/
void BSID_Close(
    BSID_Handle hSid
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_Open);

    /* wait for the idle state of the device before close it */
    do {
      BKNI_Sleep(10);
    } while (hSid->sFwHwConfig.eSidArcStatus != BSID_ResourceStatus_eIdle);


    BREG_Write32(hSid->hReg, BCHP_SID_ARC_DBG_CPU_DBG, 1);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_CORE_CPU_PC, 0x02000000);

    /* destroy sid interrupt callback */
    retCode = BINT_DestroyCallback(hSid->hServiceIsr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Close: BINT_DestroyCallback returned with error 0x%x", retCode));
        return;
    }

    /* destroy mailbox event */
    BKNI_DestroyEvent(hSid->sMailbox.hMailboxEvent);

    /* destroy operation event */
    BKNI_DestroyEvent(hSid->hOperationEvent);

    /* free sid memory buffer */
    BMEM_Free(hSid->hMem, hSid->sFwHwConfig.sMemory.pui32_VirtAddr);

    /* release power resources */
#ifdef BCHP_PWR_RESOURCE_SID_SRAM
    BCHP_PWR_ReleaseResource(hSid->hChp, BCHP_PWR_RESOURCE_SID_SRAM);
#endif
#ifdef BCHP_PWR_RESOURCE_SID
    BCHP_PWR_ReleaseResource(hSid->hChp, BCHP_PWR_RESOURCE_SID);
#endif

    /* free device handle memory */
    BKNI_Free((void *)hSid);

    BDBG_LEAVE(BSID_Close);
    return;
}

/******************************************************************************
* Function name: BSID_GetDefaultSettings
*
* Comments:
*
******************************************************************************/
void BSID_GetDefaultSettings(
    BSID_Settings *pst_SidDefSettings
    )
{
    BDBG_ENTER(BSID_GetDefaultSettings);

    BDBG_ASSERT(pst_SidDefSettings);

    BKNI_Memcpy((void *)pst_SidDefSettings,
        (void *)&stMyDefSettings,
        sizeof(BSID_Settings));

    BDBG_LEAVE(BSID_GetDefaultSettings);
    return;
}

/******************************************************************************
* Function name: BSID_OpenChannel
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_OpenChannel(
    BSID_Handle                 hSid,
    BSID_ChannelHandle         *phSidChannel,
    uint32_t                    ui32_ChannelId,
    const BSID_OpenChannelSettings *ps_OpenChannelSettings
)
{
    BERR_Code           retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = NULL;

    /**
     ** Fix ui32_ChannelId to channel number conversion
     **/

    BDBG_ENTER(BSID_OpenChannel);

    BDBG_ASSERT(hSid);
    BDBG_ASSERT(phSidChannel);
    BDBG_ASSERT(ps_OpenChannelSettings);

    /* make sure the specifyed channel is valid */
    if (hSid->ui32_TotalChannels >= BSID_MAX_CHANNELS)
    {
        BDBG_ERR(("BSID_OpenChannel: exceeded maximum number of supported channels\n"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* allocate device channel handle */
    hSidCh = (BSID_ChannelHandle)BKNI_Malloc(sizeof(BSID_P_Channel));
    if (hSidCh == NULL)
    {
        BDBG_ERR(("BSID_OpenChannel: channel handle allocation failed\n"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* init device channel handle structure to default value */
    BKNI_Memset((void *)hSidCh, 0x00, sizeof(BSID_P_Channel));

    /* register some device channel information */
    BKNI_Memcpy((void *)&hSidCh->sChSettings, (void *)ps_OpenChannelSettings, sizeof(BSID_OpenChannelSettings));
    hSidCh->ui32_ChannelNum     = hSid->ui32_TotalChannels;
    hSidCh->hSid                = hSid;
    hSidCh->ui32_ChannelID      = ui32_ChannelId;
    hSidCh->ui32_SequenceNumber = 0x0;
    hSidCh->b_FlushPending      = false;
    hSidCh->e_ChannelType       = hSidCh->sChSettings.e_ChannelType;
    hSidCh->e_ChannelState      = BSID_ChannelState_eClose;
    hSidCh->e_ChannelChangeOutputMode = BSID_ChannelChangeOutputMode_eLastFramePreviousChannel;

    if (hSidCh->e_ChannelType == BSID_ChannelType_eStill)
    {
        hSidCh->ui32_QueueTrueDepth = (1 + hSidCh->sChSettings.u_ChannelSpecific.still.ui32_QueueDepth);
    }
    else
    {
        BDBG_ASSERT(hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber);

        hSidCh->ui32_QueueTrueDepth = (1 + hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber);
        hSidCh->hChHeap = (hSidCh->sChSettings.u_ChannelSpecific.motion.hOutputBuffersHeap == NULL) ? \
                           hSid->hMem : hSidCh->sChSettings.u_ChannelSpecific.motion.hOutputBuffersHeap;
    }

    /* register channel to device handle */
    hSid->ahChannel[hSidCh->ui32_ChannelNum] = hSidCh;
    hSid->ui32_TotalChannels++;

    /* create sync event */
    retCode = BKNI_CreateEvent(&hSidCh->hSyncEvent);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_OpenChannel: BKNI_CreateEvent failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    BKNI_ResetEvent(hSidCh->hSyncEvent);

    /* create device channel queue */
    retCode = BSID_P_CreateChannelMemory(hSidCh);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_OpenChannel: BSID_P_CreateChannelQueue failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    /* send open channel command to sid arc */
    retCode = BSID_P_SendCmdOpenChannel(hSidCh);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_OpenChannel: BSID_P_SendCmdOpenChannel failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    /* channel is in open state */
    hSidCh->e_ChannelState = BSID_ChannelState_eReady;

    /* return device handle */
    *phSidChannel = hSidCh;

    BDBG_LEAVE(BSID_OpenChannel);
    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_CloseChannel
*
* Comments:
*
******************************************************************************/
void BSID_CloseChannel(
    BSID_ChannelHandle hSidCh
)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_Handle hSid = hSidCh->hSid;

    BDBG_ASSERT(hSidCh);

    BDBG_ENTER(BSID_CloseChannel);

    /* make sure the specifyed channel is valid */
    if (hSid->ui32_TotalChannels == 0)
    {
        BDBG_ERR(("BSID_OpenChannel: all the channel were already released\n"));
        return;
    }

    /* send close channel command to sid arc */
    retCode = BSID_P_SendCmdCloseChannel(hSidCh);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_CloseChannel: BSID_P_SendCmdCloseChannel failed with error 0x%x\n", retCode));
        return;
    }

    /* destroy device channel queue */
    retCode = BSID_P_DestroyChannelMemory(hSidCh);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_CloseChannel: BSID_P_DestroyChannelQueue failed with error 0x%x\n", retCode));
        return;
    }

    hSidCh->e_ChannelState = BSID_ChannelState_eReady;

    /* destroy sync event */
    if (hSidCh->hSyncEvent != NULL)
    {
        BKNI_DestroyEvent(hSidCh->hSyncEvent);
        hSidCh->hSyncEvent = NULL;
    }

    /* invalidate the device channel from the main device handle */
    hSid->ahChannel[hSidCh->ui32_ChannelNum] = NULL;
    hSid->ui32_TotalChannels --;

    /* free device channel handle */
    BKNI_Free(hSidCh);
    hSidCh = NULL;

    BDBG_LEAVE(BSID_CloseChannel);
    return;
}

/******************************************************************************
* Function name: BSID_GetDefaultChannelSettings
*
* Comments:
*
******************************************************************************/
void BSID_GetOpenChannelSettings(
    BSID_ChannelType e_ChannelType,
    BSID_OpenChannelSettings *ps_OpenChannelSettings
    )
{
    BDBG_ENTER(BSID_GetOpenChannelSettings);

    BDBG_ASSERT(ps_OpenChannelSettings);

    ps_OpenChannelSettings->ui32_ChannelPriority    = 0x0;

    switch (e_ChannelType)
    {
        case BSID_ChannelType_eStill:
        {
            ps_OpenChannelSettings->e_ChannelType = BSID_ChannelType_eStill;
            ps_OpenChannelSettings->u_ChannelSpecific.still.ui32_QueueDepth              = 1;
            ps_OpenChannelSettings->u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset   = 0x0;
            ps_OpenChannelSettings->u_ChannelSpecific.still.s_MemoryBounds.ui32_Size     = 0x0;
            ps_OpenChannelSettings->u_ChannelSpecific.still.p_CallbackFunc_Isr          = NULL;
            ps_OpenChannelSettings->u_ChannelSpecific.still.ui32_CallbackEvents     = 0x0;
            ps_OpenChannelSettings->u_ChannelSpecific.still.pv_CallbackData         = NULL;
        }
        break;
        case BSID_ChannelType_eMotion:
        {
            ps_OpenChannelSettings->e_ChannelType           = BSID_ChannelType_eStill;
            ps_OpenChannelSettings->ui32_ChannelPriority    = 0x0;
            ps_OpenChannelSettings->u_ChannelSpecific.motion.ui32_OutputBuffersNumber    = 3;
            ps_OpenChannelSettings->u_ChannelSpecific.motion.ui32_OutputMaxWidth         = 1920;
            ps_OpenChannelSettings->u_ChannelSpecific.motion.ui32_OutputMaxWidth         = 1080;
            ps_OpenChannelSettings->u_ChannelSpecific.motion.hOutputBuffersHeap          = NULL;
        }
        break;
        default:
        break;
    }

    BDBG_LEAVE(BSID_GetOpenChannelSettings);
    return;
}

/******************************************************************************
* Function name: BSID_GetChannelSettings
*
* Comments:
*
******************************************************************************/
void BSID_GetChannelSettings(
    BSID_ChannelHandle hSidChannel,
    BSID_ChannelSettings *ps_ChannelSettings
    )
{
    BDBG_ENTER(BSID_GetChannelSettings);

    BDBG_ASSERT(hSidChannel);
    BDBG_ASSERT(ps_ChannelSettings);

    switch (hSidChannel->e_ChannelType)
    {
        case BSID_ChannelType_eStill:
            ps_ChannelSettings->ui32_ChannelPriority = hSidChannel->sChSettings.ui32_ChannelPriority;
            ps_ChannelSettings->u_ChannelSpecific.still.p_CallbackFunc_Isr = hSidChannel->sChSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr;
            ps_ChannelSettings->u_ChannelSpecific.still.ui32_CallbackEvents = hSidChannel->sChSettings.u_ChannelSpecific.still.ui32_CallbackEvents;
            ps_ChannelSettings->u_ChannelSpecific.still.pv_CallbackData = hSidChannel->sChSettings.u_ChannelSpecific.still.pv_CallbackData;
        break;

        case BSID_ChannelType_eMotion:
            ps_ChannelSettings->ui32_ChannelPriority = hSidChannel->sChSettings.ui32_ChannelPriority;
        break;

        default:
        break;
    }

    BDBG_LEAVE(BSID_GetChannelSettings);
    return;
}

/******************************************************************************
* Function name: BSID_SetChannelSettings
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_SetChannelSettings(
    BSID_ChannelHandle  hSidChannel,
    const BSID_ChannelSettings *ps_ChannelSettings
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_SetChannelSettings );

    BDBG_ASSERT(hSidChannel);
    BDBG_ASSERT(ps_ChannelSettings);

    switch (hSidChannel->e_ChannelType)
    {
        case BSID_ChannelType_eStill:
            hSidChannel->sChSettings.ui32_ChannelPriority = ps_ChannelSettings->ui32_ChannelPriority;
            hSidChannel->sChSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr       = ps_ChannelSettings->u_ChannelSpecific.still.p_CallbackFunc_Isr;
            hSidChannel->sChSettings.u_ChannelSpecific.still.ui32_CallbackEvents  = ps_ChannelSettings->u_ChannelSpecific.still.ui32_CallbackEvents;
            hSidChannel->sChSettings.u_ChannelSpecific.still.pv_CallbackData      = ps_ChannelSettings->u_ChannelSpecific.still.pv_CallbackData;
        break;

        case BSID_ChannelType_eMotion:
            hSidChannel->sChSettings.ui32_ChannelPriority = ps_ChannelSettings->ui32_ChannelPriority;
        break;

        default:
            retCode = BERR_UNKNOWN;
        break;
    }

    BDBG_LEAVE( BSID_SetChannelSettings );

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_CheckParams
*
* Comments:
*
******************************************************************************/
static BERR_Code BSID_P_CheckParams(
    BSID_ChannelHandle hSidChannel,
    BSID_Operation Operation,
    void *pv_OpParam)
{
    if (hSidChannel == NULL)
    {
        BDBG_ERR(("BSID_P_CheckParams: hSidChannel is null. Please open a channel"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch (Operation)
    {
        case BSID_Operation_eDecodeImage:
            {
                BSID_DecodeImage   *ps_DecodeImage  = (BSID_DecodeImage *)pv_OpParam;
                BSID_ImageBuffer   *ps_ImageBuffer  = &ps_DecodeImage->s_ImageBuffer;

                if (hSidChannel->e_ChannelType != BSID_ChannelType_eStill)
                {
                    BDBG_ERR(("BSID_P_CheckParams: api not supported on Motion Channel"));
                    return BERR_TRACE(BERR_UNKNOWN);
                }

                if ((ps_DecodeImage == NULL) ||
                    (ps_ImageBuffer == NULL))
                {
                    BDBG_ERR(("BSID_P_CheckParams: null pointers in BSID_DecodeImage"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_DecodeImage->e_ImageFormat >= BSID_ImageFormat_eLast)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, invalid image format"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_ImageBuffer->ui32_Offset == 0)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, null surface buffer pointer"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_ImageBuffer->ui32_Offset & BSID_MEM_ALIGN)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, surface buffer is not 4 byte aligned"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_ImageBuffer->ui32_Pitch == 0)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, surface pitch is 0"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_ImageBuffer->ui32_Pitch & BSID_MEM_ALIGN)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, pitch is not 4 bytes aligned"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
#if 0
                if ((ps_ImageBuffer->ui32_Height * ps_ImageBuffer->ui32_Pitch) > hSid->sFwHwConfig.ui32_MultiScanBufSize)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, surface size exceed maximum image size decodable"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
#endif

                if (ps_ImageBuffer->ui32_Width == 0)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, surface width is 0"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_ImageBuffer->ui32_Height == 0)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, surface height is 0"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset != 0x0)
                {
                    uint32_t ui32_InpDataMinAddr   = ((BSID_LinDataMap *)ps_DecodeImage->pv_InputDataMap)->ui32_Offset;
                    uint32_t ui32_InpDataMaxAddr   = ui32_InpDataMinAddr + (((BSID_LinDataMap *)ps_DecodeImage->pv_InputDataMap)->ui32_Size);
                    uint32_t ui32_ImageMinAddr     = ps_ImageBuffer->ui32_Offset;
                    uint32_t ui32_ImageMaxAddr     = ui32_ImageMinAddr + (ps_ImageBuffer->ui32_Height * ps_ImageBuffer->ui32_Pitch);
                    uint32_t ui32_MemBoundsMinAddr = hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset;
                    uint32_t ui32_MemBoundsMaxAddr = ui32_MemBoundsMinAddr + hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Size;

                    if ((ui32_InpDataMinAddr < ui32_MemBoundsMinAddr) ||
                        (ui32_InpDataMaxAddr > ui32_MemBoundsMaxAddr) ||
                        (ui32_ImageMinAddr < ui32_MemBoundsMinAddr) ||
                        (ui32_ImageMaxAddr > ui32_MemBoundsMaxAddr))
                    {
                        BDBG_ERR(("BSID_P_CheckParams: memory out of bounds"));
                        return BERR_TRACE(BERR_INVALID_PARAMETER);
                    }
                }
            }
            break;

        case BSID_Operation_eGetStreamInfo:
            {
                BSID_StreamInfo *ps_StreamInfo = (BSID_StreamInfo *)pv_OpParam;

                if (hSidChannel->e_ChannelType != BSID_ChannelType_eStill)
                {
                    BDBG_ERR(("BSID_P_CheckParams: api not supported on Motion Channel"));
                    return BERR_TRACE(BERR_UNKNOWN);
                }

                if (ps_StreamInfo == NULL)
                {
                    BDBG_ERR(("BSID_P_CheckParams: null pointers in BSID_StreamInfo"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_StreamInfo->e_ImageFormat >= BSID_ImageFormat_eLast)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, invalid image format"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (ps_StreamInfo->ps_OutImageInfo == NULL)
                {
                    BDBG_ERR(("BSID_P_CheckParams: error, ps_OutImageInfo is null"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if (hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset != 0x0)
                {
                    uint32_t ui32_InpDataMinAddr   = ((BSID_LinDataMap *)ps_StreamInfo->pv_InputDataMap)->ui32_Offset;
                    uint32_t ui32_InpDataMaxAddr   = ui32_InpDataMinAddr + (((BSID_LinDataMap *)ps_StreamInfo->pv_InputDataMap)->ui32_Size);
                    uint32_t ui32_MemBoundsMinAddr = hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset;
                    uint32_t ui32_MemBoundsMaxAddr = ui32_MemBoundsMinAddr + hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Size;

                    if ((ui32_InpDataMinAddr < ui32_MemBoundsMinAddr) ||
                        (ui32_InpDataMaxAddr > ui32_MemBoundsMaxAddr))
                    {
                        BDBG_ERR(("BSID_P_CheckParams: memory out of bounds"));
                        return BERR_TRACE(BERR_INVALID_PARAMETER);
                    }
                }
            }
            break;
        case BSID_Operation_eSetDmaChunk:
            {
                BSID_DmaChunkInfo *ps_DmaChunkInfo = (BSID_DmaChunkInfo *)pv_OpParam;

                if (hSidChannel->e_ChannelType != BSID_ChannelType_eStill)
                {
                    BDBG_ERR(("BSID_P_CheckParams: api not supported on Motion Channel"));
                    return BERR_TRACE(BERR_UNKNOWN);
                }

                if (ps_DmaChunkInfo == NULL)
                {
                    BDBG_ERR(("BSID_P_CheckParams: null pointers in BSID_DmaChunkInfo"));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }

                if ((hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset != 0x0) && (ps_DmaChunkInfo->ui32_AbortDma == 0))
                {
                    uint32_t ui32_DmaMinAddr   = ps_DmaChunkInfo->ui32_Offset;
                    uint32_t ui32_DmaMaxAddr   = ui32_DmaMinAddr + ps_DmaChunkInfo->ui32_Size;
                    uint32_t ui32_MemBoundsMinAddr = hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset;
                    uint32_t ui32_MemBoundsMaxAddr = ui32_MemBoundsMinAddr + hSidChannel->sChSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Size;

                    if ((ui32_DmaMinAddr < ui32_MemBoundsMinAddr) ||
                        (ui32_DmaMaxAddr > ui32_MemBoundsMaxAddr))
                    {
                        BDBG_ERR(("BSID_P_CheckParams: memory out of bounds"));
                        return BERR_TRACE(BERR_INVALID_PARAMETER);
                    }
                }
            }
            break;
        default:
            break;
    }

    return BERR_TRACE(BERR_SUCCESS);
}

/******************************************************************************
* Function name: BSID_GetStreamInfo
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_GetStreamInfo(
    BSID_ChannelHandle  hSidChannel,
    BSID_StreamInfo     *ps_StreamInfo
)
{
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_GetStreamInfo);

    /* sanity check */
    BDBG_ASSERT(hSidChannel);
    BDBG_ASSERT(ps_StreamInfo);

    /*
     * This operation is not executed in multiple passes (like segmented decode).
     * If a motion jpeg channel was opened before this call we reject.
     */
    if (BSID_IsStillOperationAllowed(hSidChannel->hSid) == false)
    {
        BDBG_ERR(("BSID_GetStreamInfo can't be executed. Motion decode channel is opened!"));
        retCode = BERR_UNKNOWN;
        return BERR_TRACE(retCode);
    }

    BKNI_EnterCriticalSection();
    hSidChannel->e_ChannelState = BSID_ChannelState_eDecode;
    BKNI_LeaveCriticalSection();

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eGetStreamInfo, (void *)ps_StreamInfo);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_GetStreamInfo: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* is the sid ready to accept this request? */
    if (BSID_P_GetChannelQueueStatus(hSidChannel) == BSID_ChannelQueueStatus_NotFull)
    {
        /* post request to channel queue */
        retCode = BSID_P_SendCmdGetStreamInfo(hSidChannel, ps_StreamInfo);
        if (retCode != BERR_SUCCESS)
        {
            return BERR_TRACE(retCode);
        }
    }
    else
    {
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    BDBG_LEAVE(BSID_GetStreamInfo);
    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_DecodeStillImage
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_DecodeStillImage(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DecodeImage    *ps_DecodeImage
    )
{
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_DecodeStillImage);

    /* sanity check */
    BDBG_ASSERT(hSidChannel);
    BDBG_ASSERT(ps_DecodeImage);

    /*
     * This operation is not executed in multiple passes (like segmented decode)
     * If a motion jpeg channel was opened before this call we reject.
     */
    if (BSID_IsStillOperationAllowed(hSidChannel->hSid) == false)
    {
        BDBG_ERR(("BSID_DecodeStillImage can't be executed. Motion decode channel is opened!"));
        retCode = BERR_UNKNOWN;
        return BERR_TRACE(retCode);
    }

    BKNI_EnterCriticalSection();
    hSidChannel->e_ChannelState = BSID_ChannelState_eDecode;
    BKNI_LeaveCriticalSection();

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eDecodeImage, (void *)ps_DecodeImage);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_DecodeStillImage: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* is the sid ready to accept this request? */
    if (BSID_P_GetChannelQueueStatus(hSidChannel) == BSID_ChannelQueueStatus_NotFull)
    {
        /* prepare decode command and send it to sid arc */
        retCode = BSID_P_SendCmdDecodeStillImage(hSidChannel, ps_DecodeImage);
        if (retCode != BERR_SUCCESS)
        {
            /* un-lock status of sid arc: sid arc status is idle */
            return BERR_TRACE(retCode);
        }
    }
    else
    {
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    BDBG_LEAVE(BSID_DecodeStillImage);
    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_DecodeImageSegment
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_DecodeImageSegment(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DecodeSegment *ps_DecodeSegment
    )
{
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_DecodeImageSegment);

    /* sanity check */
    BDBG_ASSERT(hSidChannel);
    BDBG_ASSERT(ps_DecodeSegment);

    /*
     * This operation may be executed in multiple passes. Reject all the
     * new segmented decode operation. Allow the ones that are happening
     * for which b_FirstSegment is set to false.
     */
    if ((BSID_IsStillOperationAllowed(hSidChannel->hSid) == false) && (ps_DecodeSegment->b_FirstSegment == true))
    {
        BDBG_ERR(("BSID_DecodeImageSegment can't be executed. Motion decode channel is opened!"));
        retCode = BERR_UNKNOWN;
        return BERR_TRACE(retCode);
    }

    BKNI_EnterCriticalSection();
    hSidChannel->e_ChannelState = BSID_ChannelState_eDecode;
    BKNI_LeaveCriticalSection();

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eDecodeImageSegment, (void *)ps_DecodeSegment);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_DecodeImageSegment: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* is the sid ready to accept this request? */
    if (BSID_P_GetChannelQueueStatus(hSidChannel) == BSID_ChannelQueueStatus_NotFull)
    {
        /* prepare decode command and send it to sid arc */
        retCode = BSID_P_SendCmdDecodeSegment(hSidChannel, ps_DecodeSegment);
        if (retCode != BERR_SUCCESS)
        {
            return BERR_TRACE(retCode);
        }
    }
    else
    {
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    BDBG_LEAVE(BSID_DecodeImageSegment);
    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_SetDMAChunkInfo
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_SetDmaChunkInfo(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DmaChunkInfo   *ps_DmaChunkInfo
    )
{
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_SetDmaChunkInfo);

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eSetDmaChunk, (void *)ps_DmaChunkInfo);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SetDmaChunkInfo: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* prepare decode command and send it to sid arc */
    BSID_P_SetDmaChunkInfo(hSidChannel, ps_DmaChunkInfo);

    BDBG_LEAVE(BSID_SetDmaChunkInfo);
    return BERR_SUCCESS;
}

/******************************************************************************
* Function name: BSID_SetDMAChunkInfo
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_SetDmaChunkInfo_Isr(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DmaChunkInfo   *ps_DmaChunkInfo
    )
{
    BERR_Code   retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_SetDmaChunkInfo);

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eSetDmaChunk, (void *)ps_DmaChunkInfo);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SetDmaChunkInfo: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* prepare decode command and send it to sid arc */
    BSID_P_SetDmaChunkInfo_Isr(hSidChannel, ps_DmaChunkInfo);

    BDBG_LEAVE(BSID_SetDmaChunkInfo);
    return BERR_SUCCESS;
}

/******************************************************************************
* Function name: BSID_GetStatus
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_GetStatus(
    BSID_ChannelHandle  hSidChannel,
    BSID_Status         *ps_Status
    )
{
    BERR_Code retCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    BDBG_ENTER( BSID_GetStatus );
    BSTD_UNUSED(hSidChannel);
    BSTD_UNUSED(ps_Status);
    BDBG_LEAVE( BSID_GetStatus );
    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_GetStatus
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_GetRevisionInfo(
    BSID_Handle        hSid,
    BSID_RevisionInfo *pst_RevisionInfo
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_GetRevisionInfo );

    BSTD_UNUSED(hSid);

    BDBG_ASSERT(pst_RevisionInfo);

    BKNI_Memcpy((void *)pst_RevisionInfo,
        (void *)&stMyRevisionInfo,
        sizeof(BSID_RevisionInfo));

    BDBG_LEAVE( BSID_GetRevisionInfo );

    return BERR_TRACE(retCode);
}


/******************************************************************************
* Function name: BSID_DisableForFlush
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_DisableForFlush(
    BSID_ChannelHandle  hSidChannel
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_DisableForFlush );

    BDBG_ASSERT(hSidChannel);

    /* prepare decode command and send it to sid arc */
    retCode = BSID_P_DisableForFlush(hSidChannel);
    if (retCode != BERR_SUCCESS)
    {
        /* un-lock status of sid arc: sid arc status is idle */
        return BERR_TRACE(retCode);
    }

    BDBG_LEAVE( BSID_DisableForFlush );

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_FlushChannel
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_FlushChannel(
    BSID_ChannelHandle  hSidChannel,
    const BSID_FlushSettings *ps_FlushSettings
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_FlushChannel );

    BDBG_ASSERT(hSidChannel);

    if (hSidChannel->e_ChannelType == BSID_ChannelType_eStill)
    {
    /* prepare decode command and send it to sid arc */
    retCode = BSID_P_FlushChannel(hSidChannel, ps_FlushSettings);
    if (retCode != BERR_SUCCESS)
    {
        /* un-lock status of sid arc: sid arc status is idle */
        return BERR_TRACE(retCode);
    }
    }
    else
    {
        /* prepare decode command and send it to sid arc */
        retCode = BSID_P_FlushDecode(hSidChannel);
        if (retCode != BERR_SUCCESS)
        {
            /* un-lock status of sid arc: sid arc status is idle */
            return BERR_TRACE(retCode);
        }
    }

    BDBG_LEAVE( BSID_FlushChannel );

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_StartDecode
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_StartDecode(
    BSID_ChannelHandle  hSidChannel,
    const BSID_StartDecodeSettings *ps_StartDecodeSetting
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_StartDecode );

    BDBG_ASSERT(hSidChannel);

    /* check state: start decode can occur in ready state only */
    if (BSID_IsMotionOperationAllowed(hSidChannel->hSid) == false)
    {
        BDBG_ERR(("BSID_StartDecode can't be executed. Still decode operations in progress!"));
        retCode = BERR_UNKNOWN;
        return BERR_TRACE(retCode);
    }

    /* sanity check */
    retCode = BSID_P_CheckParams(hSidChannel, BSID_Operation_eStartDecode, (void *)ps_StartDecodeSetting);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_StartDecode: BSID_CheckParams failed"));
        return BERR_TRACE(retCode);
    }

    /* prepare decode command and send it to sid arc */
    retCode = BSID_P_StartDecode(hSidChannel, ps_StartDecodeSetting);
    if (retCode != BERR_SUCCESS)
    {
        /* un-lock status of sid arc: sid arc status is idle */
        return BERR_TRACE(retCode);
    }

    BDBG_LEAVE( BSID_StartDecode );

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_StopDecode
*
* Comments:
*
******************************************************************************/
void BSID_StopDecode(
    BSID_ChannelHandle  hSidChannel
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_StopDecode );

    BDBG_ASSERT(hSidChannel);

    retCode = BSID_P_DisableForFlush(hSidChannel);
    if (retCode != BERR_SUCCESS)
    {
        /* un-lock status of sid arc: sid arc status is idle */
        BDBG_ERR(("BSID_StopDecode: BSID_P_DisableForFlush failed retCode 0x%x", retCode));
        return;
    }

    hSidChannel->e_ChannelState = BSID_ChannelState_eReady;

    BDBG_LEAVE( BSID_StopDecode );

    return;
}

/******************************************************************************
* Function name: BSID_GetRaveItbCdbConfigInfo
*
* Comments:
*
******************************************************************************/
void BSID_GetRaveItbCdbConfigInfo(
    BAVC_CdbItbConfig  *ps_ConfigInfo
    )
{
    BDBG_ENTER( BSID_GetRaveItbCdbConfigInfo );

    ps_ConfigInfo->Cdb.Length           = BSID_RAVE_CDB_LENGHT;
    ps_ConfigInfo->Cdb.Alignment        = BSID_RAVE_CDB_ALIGNMENT;
    ps_ConfigInfo->Cdb.LittleEndian     = BSID_RAVE_CDB_ENDIAN;
    ps_ConfigInfo->Itb.Length           = BSID_RAVE_ITB_LENGHT;
    ps_ConfigInfo->Itb.Alignment        = BSID_RAVE_ITB_ALIGNMENT;
    ps_ConfigInfo->Itb.LittleEndian     = BSID_RAVE_ITB_ENDIAN;
    ps_ConfigInfo->UsePictureCounter = false;

    BDBG_LEAVE( BSID_GetRaveItbCdbConfigInfo );

    return;
}

/******************************************************************************
* Function name: BSID_GetXdmInterface
*
* Comments:
*
******************************************************************************/
void BSID_GetXdmInterface(
    BSID_ChannelHandle  hSidChannel,
    BXDM_Decoder_Interface *ps_XdmInterface,
    void **pCtx
    )
{
    BDBG_ENTER( BSID_GetXdmInterface );

    BKNI_Memset((void *)ps_XdmInterface, 0, sizeof(BXDM_Decoder_Interface));
    ps_XdmInterface->getPictureCount_isr              = BSID_P_GetPictureCount_isr;
    ps_XdmInterface->peekAtPicture_isr                = BSID_P_PeekAtPicture_isr;
    ps_XdmInterface->getNextPicture_isr               = BSID_P_GetNextPicture_isr;
    ps_XdmInterface->releasePicture_isr               = BSID_P_ReleasePicture_isr;
    ps_XdmInterface->getPictureDropPendingCount_isr   = NULL;
    ps_XdmInterface->requestPictureDrop_isr           = NULL;
    ps_XdmInterface->displayInterruptEvent_isr        = BSID_P_DisplayInterruptEvent_isr;

   *pCtx = hSidChannel;

   BDBG_LEAVE( BSID_GetXdmInterface );

   return;
}

/******************************************************************************
* Function name: BSID_Standby
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_Standby(
    BSID_Handle  hSid
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_Standby );

    retCode = BSID_P_SuspendChannels(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Standby: BSID_SuspendStillDecode failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    BREG_Write32(hSid->hReg, BCHP_SID_ARC_DBG_CPU_DBG, 1);
    BREG_Write32(hSid->hReg, BCHP_SID_ARC_CORE_CPU_PC, 0x02000000);

    /* callback must be disabled and re-enabled */
    retCode = BINT_DisableCallback(hSid->hServiceIsr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Standby: BINT_DisableCallback returned with error 0x%x", retCode));
        return BERR_TRACE(retCode);
    }

    /* release power resources */
#ifdef BCHP_PWR_RESOURCE_SID_SRAM
    BCHP_PWR_ReleaseResource(hSid->hChp, BCHP_PWR_RESOURCE_SID_SRAM);
#endif
#ifdef BCHP_PWR_RESOURCE_SID
    BCHP_PWR_ReleaseResource(hSid->hChp, BCHP_PWR_RESOURCE_SID);
#endif

    BDBG_LEAVE( BSID_Standby );

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_Resume
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_Resume(
    BSID_Handle  hSid
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER( BSID_Resume );

    /* acquire power resources */
#ifdef BCHP_PWR_RESOURCE_SID
    BCHP_PWR_AcquireResource(hSid->hChp, BCHP_PWR_RESOURCE_SID);
#endif
#ifdef BCHP_PWR_RESOURCE_SID_SRAM
    BCHP_PWR_AcquireResource(hSid->hChp, BCHP_PWR_RESOURCE_SID_SRAM);
#endif

    retCode = BSID_P_BootArc(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Resume: BSID_P_BootArc failed"));
        return BERR_TRACE(retCode);
    }

    retCode = BINT_EnableCallback(hSid->hServiceIsr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Resume: BINT_EnableCallback failed"));
        return BERR_TRACE(retCode);
    }

    retCode = BSID_P_SendCmdInit(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Open: BSID_P_SendCmdInit failed"));
        return BERR_TRACE(retCode);
    }

    retCode = BSID_P_ResumeChannels(hSid);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_Resume: BSID_P_ResumeChannels failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    BDBG_LEAVE( BSID_Resume );

    return BERR_TRACE(retCode);
}
