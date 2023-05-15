/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_priv.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 9/11/12 1:05p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid_priv.c $
* 
* Hydra_Software_Devel/8   9/11/12 1:05p fbasso
* SW7344-366: fixed segmentation fault
* 
* Hydra_Software_Devel/7   8/24/12 5:02p fbasso
* SW7425-3620: replaced un-cached memory access with cached memory
* access.
* 
* Hydra_Software_Devel/6   4/2/12 10:26a fbasso
* SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* code in place to decode both images contained in the JPEG-B frame,
* only the first image is decoded for now and sent to XDM. As soon as
* interlaced mode is added in the SID->XDM->VDC path, this limitation
* will be removed and both images will be decoded and sent to display.
*
* Hydra_Software_Devel/5   3/19/12 4:02p fbasso
* SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* down and up sequence.
*
* Hydra_Software_Devel/4   3/15/12 6:41a fbasso
* SW7435-26: fixed deallocation of channel memory.
*
* Hydra_Software_Devel/3   2/21/12 2:20p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
*
* Hydra_Software_Devel/SW7425-2112/4   2/16/12 4:51p fbasso
* SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* still channels and motion channel; added support for channel change
* output mode (hold last picture)
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
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/6   8/22/11 4:29p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/5   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid_priv.c $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/8   9/11/12 1:05p fbasso
* SW7425-674: Added SW7344-366: fixed segmentation fault
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/7   8/24/12 5:02p fbasso
* SW7425-674: Added SW7425-3620: replaced un-cached memory access with cached memory
* SW7425-674: Added access.
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/6   4/2/12 10:26a fbasso
* SW7425-674: Added SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* SW7425-674: Added code in place to decode both images contained in the JPEG-B frame,
* SW7425-674: Added only the first image is decoded for now and sent to XDM. As soon as
* SW7425-674: Added interlaced mode is added in the SID->XDM->VDC path, this limitation
* SW7425-674: Added will be removed and both images will be decoded and sent to display.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/5   3/19/12 4:02p fbasso
* SW7425-674: Added SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* SW7425-674: Added down and up sequence.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/4   3/15/12 6:41a fbasso
* SW7425-674: Added SW7435-26: fixed deallocation of channel memory.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/3   2/21/12 2:20p fbasso
* SW7425-674: Added SW7425-2112: added support for MJPEG video decode. Merging new feature
* SW7425-674: Added to Hydra_Software_Devel.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/4   2/16/12 4:51p fbasso
* SW7425-674: Added SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* SW7425-674: Added still channels and motion channel; added support for channel change
* SW7425-674: Added output mode (hold last picture)
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
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-674: Added SW7425-1619: add queue based interface and multi channel support
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/6   8/22/11 4:29p fbasso
* SW7425-674: Added SWDTV-8264: returning Transparency RGB color for PNG decode when
* SW7425-674: Added present.
*
* Hydra_Software_Devel/1   5/03/06 09:19a fbasso
* - Created this file
******************************************************************************/

#include "bstd.h"
#include "berr.h"
#include "bkni.h"
#include "bkni_event_group.h"
#include "bpxl.h"

#include "bsid.h"
#include "bsid_priv.h"

BDBG_MODULE(BSID_PRIV);

/******************************************************************************
* Function name: BSID_SetHwConfig
*
* Comments:
*
******************************************************************************/
BERR_Code  BSID_P_SetFwHwDefault(
    BSID_Handle hSid,
    BSID_Settings ps_DefSettings
)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_P_SetFwHwDefault);

    hSid->sFwHwConfig.eSidArcStatus            = BSID_ResourceStatus_eIdle;
    hSid->sFwHwConfig.ui16_JPEGHorizAndVerFilt = ps_DefSettings.ui16_JPEGHorizAndVerFilt;
    hSid->sFwHwConfig.ui8_AlphaValue           = ps_DefSettings.ui8_AlphaValue;
    hSid->sFwHwConfig.b_EndianessSwap          = ps_DefSettings.b_EndianessSwap;
    hSid->sFwHwConfig.bSelfTest                = ps_DefSettings.b_SelfTest;

    /* allocate memory buffer for sid arc code and auxiliary data structures */
    {
        BSID_LinearBuffer *sMemory = &hSid->sFwHwConfig.sMemory;
        sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSid->hMem, BSID_TOT_MEM_SIZE, BSID_ARC_CODE_ALIGN, 0);
        if (sMemory->pui32_VirtAddr == NULL)
        {
            BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_AllocAligned failed"));
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }

        sMemory->ui32_Size = BSID_TOT_MEM_SIZE;
        retCode = BMEM_ConvertAddressToOffset(hSid->hMem, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_ConvertAddressToOffset returned with error 0x%x", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

        /* sanity check since arc code must start at 1k boundary */
        if (((uint32_t)sMemory->pui32_VirtAddr & (BSID_ARC_CODE_ALIGN-1)) || (sMemory->ui32_PhisAddr & (BSID_ARC_CODE_ALIGN-1)))
        {
            BDBG_ERR(("BSID_P_SetFwHwDefault: wrong alignment vAddr 0x%x pAddr 0x%x", sMemory->pui32_VirtAddr, sMemory->ui32_PhisAddr));
            return BERR_TRACE(BERR_UNKNOWN);
        }

#if 0
        BDBG_MSG(("BSID MEMORY ALLOCATED startv 0x%x startp 0x%x size 0x%x\n",
        sMemory->pui32_VirtAddr,
        sMemory->ui32_PhisAddr,
        sMemory->ui32_Size));
#endif
    }

    /* split memory across arc code memory, mailbox memory and general purpose memory */
    {
        BSID_LinearBuffer *sMemory       = &hSid->sFwHwConfig.sMemory;
        BSID_LinearBuffer *sCodeMemory   = &hSid->sFwHwConfig.sCodeMemory;
        BSID_LinearBuffer *sMbxMemory    = &hSid->sFwHwConfig.sMbxMemory;
        BSID_LinearBuffer *sInpDmaMemory = &hSid->sFwHwConfig.sInpDmaMemory;
        BSID_LinearBuffer *sDataMemory   = NULL;

        hSid->sFwHwConfig.eMemoryMode   = BSID_MemoryMode_eUnifiedMemory;

        /* code memory block */
        sCodeMemory->pui32_VirtAddr = sMemory->pui32_VirtAddr;
        sCodeMemory->ui32_PhisAddr = sMemory->ui32_PhisAddr;
        sCodeMemory->ui32_Size     = BSID_ARC_CODE_SIZE;

#if 0
        BDBG_MSG(("CODE MEMORY ALLOCATED startv 0x%x startp 0x%x size 0x%x\n",
        sCodeMemory->pui32_VirtAddr,
        sCodeMemory->ui32_PhisAddr,
        sCodeMemory->ui32_Size));
#endif
#ifdef BSID_USE_CACHED_MEMORY
        retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sCodeMemory->pui32_VirtAddr, &sCodeMemory->pv_CachedAddr);
        if (retCode != BERR_SUCCESS)
        {
           BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
           return BERR_TRACE(BERR_UNKNOWN);
        }
#endif
        /* mbx memory block */
        sMbxMemory->pui32_VirtAddr = (uint32_t *)((((uint32_t)sCodeMemory->pui32_VirtAddr + sCodeMemory->ui32_Size) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN);
        sMbxMemory->ui32_PhisAddr = ((sCodeMemory->ui32_PhisAddr + sCodeMemory->ui32_Size) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN;
        sMbxMemory->ui32_Size     = BSID_MBX_MEMORY_SIZE;
#if 0
        BDBG_MSG(("SMBX MEMORY ALLOCATED startv 0x%x startp 0x%x size 0x%x\n",
        sMbxMemory->pui32_VirtAddr,
        sMbxMemory->ui32_PhisAddr,
        sMbxMemory->ui32_Size));
#endif

#ifdef BSID_USE_CACHED_MEMORY
        retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sMbxMemory->pui32_VirtAddr, &sMbxMemory->pv_CachedAddr);
        if (retCode != BERR_SUCCESS)
        {
           BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
           return BERR_TRACE(BERR_UNKNOWN);
        }
#endif
        /* input dma memory block */
        sInpDmaMemory->pui32_VirtAddr = (uint32_t *)((((uint32_t)sMbxMemory->pui32_VirtAddr + sMbxMemory->ui32_Size) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN);
        sInpDmaMemory->ui32_PhisAddr = ((sMbxMemory->ui32_PhisAddr + sMbxMemory->ui32_Size) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN;
        sInpDmaMemory->ui32_Size     = BSID_INPUT_DMA_MEMORY_SIZE;
#if 0
        BDBG_MSG(("IDMA MEMORY ALLOCATED startv 0x%x startp 0x%x size 0x%x\n",
        sInpDmaMemory->pui32_VirtAddr,
        sInpDmaMemory->ui32_PhisAddr,
        sInpDmaMemory->ui32_Size));
#endif
#ifdef BSID_USE_CACHED_MEMORY
        retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sInpDmaMemory->pui32_VirtAddr, &sInpDmaMemory->pv_CachedAddr);
        if (retCode != BERR_SUCCESS)
        {
           BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
           return BERR_TRACE(BERR_UNKNOWN);
        }
#endif

        /* data structure memory */
        {
            uint32_t ui32_ChannelNumber = 0;

            for (ui32_ChannelNumber = 0; ui32_ChannelNumber < BSID_MAX_CHANNELS; ui32_ChannelNumber++)
            {
                sDataMemory = &hSid->sFwHwConfig.sDataMemory[ui32_ChannelNumber];

                /* data memory block */
                sDataMemory->pui32_VirtAddr = (uint32_t *)((((uint32_t)sInpDmaMemory->pui32_VirtAddr + ((1 + ui32_ChannelNumber) * sInpDmaMemory->ui32_Size)) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN);
                sDataMemory->ui32_PhisAddr = ((sInpDmaMemory->ui32_PhisAddr + ((1 + ui32_ChannelNumber) * sInpDmaMemory->ui32_Size)) + BSID_MEM_ALIGN) & ~BSID_MEM_ALIGN;
                sDataMemory->ui32_Size     = BSID_DATA_MEMORY_SIZE;
#if 0
                BDBG_MSG(("DATA MEMORY ALLOCATED channel %d startv 0x%x startp 0x%x size 0x%x\n",
                ui32_ChannelNumber,
                sDataMemory->pui32_VirtAddr,
                sDataMemory->ui32_PhisAddr,
                sDataMemory->ui32_Size));
#endif
#ifdef BSID_USE_CACHED_MEMORY
                retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sDataMemory->pui32_VirtAddr, &sDataMemory->pv_CachedAddr);
                if (retCode != BERR_SUCCESS)
                {
                   BDBG_ERR(("BSID_P_SetFwHwDefault: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
                   return BERR_TRACE(BERR_UNKNOWN);
                }
#endif
            }
        }
    }

    /* image format conversion table for image format between sid pi and sid arc */
    {
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eRLE_Legacy_DVD] = SID_RLE_DVD;
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eRLE_HD_DVD] = SID_RLE_HDDVD;
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eRLE_BD] = SID_RLE_BLURAY;
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eJPEG] = SID_JPEG;
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_ePNG] = SID_PNG;
        hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eGIF] = SID_GIF;
		hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eMPEG_IFRAME] = SID_MPEG_IFRAME;
    }

    BDBG_LEAVE(BSID_P_SetFwHwDefault);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_GetOperationStatus
*
* Comments:
*
******************************************************************************/
BSID_ResourceStatus BSID_P_GetOperationStatus(BSID_Handle hSid)
{
    return hSid->sFwHwConfig.eSidArcStatus;
}

/******************************************************************************
* Function name: BSID_P_GetOperationStatus
*
* Comments:
*
******************************************************************************/
BSID_ResourceStatus BSID_P_GetOperationStatus_Isr(BSID_Handle hSid)
{
    return hSid->sFwHwConfig.eSidArcStatus;
}

/******************************************************************************
* Function name: BSID_P_CreateChannelQueue
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_CreateChannelMemory(BSID_ChannelHandle hSidCh)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_Handle hSid = hSidCh->hSid;
    BSID_LinearBuffer *sMemory = NULL;
    BSID_CommandQueueHeader *psQueueHeader = NULL;

    /* allocate channel request queue */
    sMemory = &hSidCh->sReqQueue.sBuf;
    sMemory->ui32_Size = sizeof(BSID_CommandQueueHeader) + (hSidCh->ui32_QueueTrueDepth * sizeof(BSID_ReqQueueEntry));
    sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSid->hMem, sMemory->ui32_Size, BSID_QUEUE_ALIGNMENT, 0);
    if (sMemory->pui32_VirtAddr == NULL)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for request queue failed\n"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* obtain physical address */
    retCode = BMEM_ConvertAddressToOffset(hSid->hMem, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
        return BERR_TRACE(BERR_UNKNOWN);
    }

#ifdef BSID_USE_CACHED_MEMORY
    retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
       return BERR_TRACE(BERR_UNKNOWN);
    }
#endif

    /* reset queue indexes */
#ifdef BSID_USE_CACHED_MEMORY
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pv_CachedAddr;
#else
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pui32_VirtAddr;
#endif
    psQueueHeader->ui32_ReadIndex = psQueueHeader->ui32_WriteIndex = 0;
#ifdef BSID_USE_CACHED_MEMORY
    BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif

#if 0
    BDBG_MSG(("reqQueue Qsize %d vaddr 0x%x paddr 0x%x depth %d\n",
    sMemory->ui32_Size,
    sMemory->pui32_VirtAddr,
    sMemory->ui32_PhisAddr,
    hSidCh->ui32_QueueTrueDepth));
#endif

    /* allocate channel release queue */
    sMemory = &hSidCh->sRelQueue.sBuf;
    sMemory->ui32_Size = sizeof(BSID_CommandQueueHeader) + (hSidCh->ui32_QueueTrueDepth * sizeof(BSID_RelQueueEntry));
    sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSid->hMem, sMemory->ui32_Size, BSID_QUEUE_ALIGNMENT, 0);
    if (sMemory->pui32_VirtAddr == NULL)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for release queue failed\n"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* obtain physical address */
    retCode = BMEM_ConvertAddressToOffset(hSid->hMem, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
        return BERR_TRACE(BERR_UNKNOWN);
    }

#ifdef BSID_USE_CACHED_MEMORY
    retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
       return BERR_TRACE(BERR_UNKNOWN);
    }
#endif

    /* reset queue indexes */
#ifdef BSID_USE_CACHED_MEMORY
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pv_CachedAddr;
#else
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pui32_VirtAddr;
#endif
    psQueueHeader->ui32_ReadIndex = psQueueHeader->ui32_WriteIndex = 0;
#ifdef BSID_USE_CACHED_MEMORY
    BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif

#if 0
    BDBG_MSG(("relQueue Qsize %d vaddr 0x%x paddr 0x%x depth %d\n",
    sMemory->ui32_Size,
    sMemory->pui32_VirtAddr,
    sMemory->ui32_PhisAddr,
    hSidCh->ui32_QueueTrueDepth));
#endif

    /* allocate channel data queue */
    sMemory = &hSidCh->sDataQueue.sBuf;
    sMemory->ui32_Size = sizeof(BSID_CommandQueueHeader) + (hSidCh->ui32_QueueTrueDepth * sizeof(BSID_DataQueueEntry));
    sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSid->hMem, sMemory->ui32_Size, BSID_QUEUE_ALIGNMENT, 0);
    if (sMemory->pui32_VirtAddr == NULL)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for data queue failed\n"));
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* obtain physical address */
    retCode = BMEM_ConvertAddressToOffset(hSid->hMem, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
        return BERR_TRACE(BERR_UNKNOWN);
    }

#ifdef BSID_USE_CACHED_MEMORY
    retCode = BMEM_ConvertAddressToCached(hSid->hMem, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
       return BERR_TRACE(BERR_UNKNOWN);
    }
#endif

    /* reset queue indexes */
#ifdef BSID_USE_CACHED_MEMORY
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pv_CachedAddr;
#else
    psQueueHeader = (BSID_CommandQueueHeader *)sMemory->pui32_VirtAddr;
#endif
    psQueueHeader->ui32_ReadIndex = psQueueHeader->ui32_WriteIndex = 0;
#ifdef BSID_USE_CACHED_MEMORY
    BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif

#if 0
    BDBG_MSG(("dataQueue Qsize %d vaddr 0x%x paddr 0x%x depth %d\n",
    sMemory->ui32_Size,
    sMemory->pui32_VirtAddr,
    sMemory->ui32_PhisAddr,
    hSidCh->ui32_QueueTrueDepth));
#endif

    /* check if we are dealing with a motion channel */
    if (hSidCh->e_ChannelType == BSID_ChannelType_eMotion)
    {
        unsigned int buffer_index = 0x0;
        BSID_RaveStatusReport *pMemoryReport;
        BSID_PlaybackQueueHeader *psPbQueueHeader;
        BSID_PlaybackQueueEntry *psPbQueueEntry;
        unsigned int pbQueueSize;
        BXDM_Picture *pUniPicture;

        /* allocate rave report memory and playback buffer queue. */

        /* allocate rave report memory */
        sMemory = &hSidCh->sRaveReport;
        sMemory->ui32_Size = sizeof(BSID_RaveStatusReport);
        sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSidCh->hChHeap, sMemory->ui32_Size, BSID_QUEUE_ALIGNMENT, 0);
        if (sMemory->pui32_VirtAddr == NULL)
        {
            BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for memory report\n"));
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }

        /* obtain physical address */
        retCode = BMEM_ConvertAddressToOffset(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

#ifdef BSID_USE_CACHED_MEMORY
        retCode = BMEM_ConvertAddressToCached(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
        if (retCode != BERR_SUCCESS)
        {
           BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
           return BERR_TRACE(BERR_UNKNOWN);
        }
#endif

#ifdef BSID_USE_CACHED_MEMORY
        pMemoryReport = (BSID_RaveStatusReport *)sMemory->pv_CachedAddr;
#else
        pMemoryReport = (BSID_RaveStatusReport *)sMemory->pui32_VirtAddr;
#endif

        BKNI_Memset((void *)pMemoryReport, 0x0, sizeof(BSID_RaveStatusReport));
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)pMemoryReport, sizeof(BSID_RaveStatusReport));
#endif

#if 0
        BDBG_MSG(("MemoryReport size %d vaddr 0x%x paddr 0x%x\n",
        sMemory->ui32_Size,
        sMemory->pui32_VirtAddr,
        sMemory->ui32_PhisAddr));
#endif

        for (buffer_index = 0x0; buffer_index < hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber; buffer_index++)
        {
            /* allocate output buffer */
            sMemory = &hSidCh->a_OutputBuffer[buffer_index];
            sMemory->ui32_Size = (hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputMaxWidth *
                                 hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputMaxHeight *
                                 (BPXL_BITS_PER_PIXEL(BPXL_eA8_Y8_Cb8_Cr8)>>3));
            sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSidCh->hChHeap, sMemory->ui32_Size, BSID_OUTPUT_BUFFER_ALIGNMENT, 0);
            if (sMemory->pui32_VirtAddr == NULL)
            {
                BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for output buffer allocation failed %d\n",
                hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber));
                return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            }

            /* obtain physical address */
            retCode = BMEM_ConvertAddressToOffset(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
                return BERR_TRACE(BERR_UNKNOWN);
            }

#ifdef BSID_USE_CACHED_MEMORY
            retCode = BMEM_ConvertAddressToCached(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
            if (retCode != BERR_SUCCESS)
            {
               BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
               return BERR_TRACE(BERR_UNKNOWN);
            }
#endif
            /* this structure will be used to pass data downstream */
            hSidCh->a_OutputBuffer[buffer_index].ui32_PhisAddr = sMemory->ui32_PhisAddr;

#if 0
            BDBG_MSG(("OutputBuffer[%d] size %d vaddr 0x%x paddr 0x%x\n",
            buffer_index,
            sMemory->ui32_Size,
            sMemory->pui32_VirtAddr,
            sMemory->ui32_PhisAddr));
#endif
        }

        pbQueueSize = hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber;

        /* allocate playback queue */
        sMemory = &hSidCh->sPlaybackQueue.sBuf;
        sMemory->ui32_Size = sizeof(BSID_PlaybackQueueHeader) + (pbQueueSize * sizeof(BSID_PlaybackQueueEntry));
        sMemory->pui32_VirtAddr = (uint32_t *)BMEM_AllocAligned(hSidCh->hChHeap, sMemory->ui32_Size, BSID_QUEUE_ALIGNMENT, 0);
        if (sMemory->pui32_VirtAddr == NULL)
        {
            BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_AllocAligned for request queue failed\n"));
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }

        /* obtain physical address */
        retCode = BMEM_ConvertAddressToOffset(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, (uint32_t *)&sMemory->ui32_PhisAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToOffset returned with error 0x%x\n", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

#ifdef BSID_USE_CACHED_MEMORY
        retCode = BMEM_ConvertAddressToCached(hSidCh->hChHeap, (void *)sMemory->pui32_VirtAddr, &sMemory->pv_CachedAddr);
        if (retCode != BERR_SUCCESS)
        {
           BDBG_ERR(("BSID_P_CreateChannelMemory: BMEM_ConvertAddressToCached returned with error 0x%x\n", retCode));
           return BERR_TRACE(BERR_UNKNOWN);
        }
#endif

        /* reset queue indexes */
#ifdef BSID_USE_CACHED_MEMORY
        psPbQueueHeader = (BSID_PlaybackQueueHeader *)sMemory->pv_CachedAddr;
#else
        psPbQueueHeader = (BSID_PlaybackQueueHeader *)sMemory->pui32_VirtAddr;
#endif
        psPbQueueHeader->ui32_QueueSize = pbQueueSize;
        psPbQueueHeader->ui32_NewPictWriteIndex = 0x0;
        psPbQueueHeader->ui32_ChannelState = BSID_ChannelState_eReady;
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif

#if 0
        BDBG_MSG(("Playback Qsize %d vaddr 0x%x paddr 0x%x numElem %d\n",
        sMemory->ui32_Size,
        sMemory->pui32_VirtAddr,
        sMemory->ui32_PhisAddr,
        psPbQueueHeader->ui32_QueueSize));
#endif

        for (buffer_index = 0x0; buffer_index < (psPbQueueHeader->ui32_QueueSize); buffer_index++)
        {
#if 0
            /* create hSurface */
            {
               pUniPicture = &hSidCh->a_DisplayBuffer[buffer_index];

               retCode = BSUR_Surface_Create(
                            hSidCh->hChHeap,
                            1280,
                            720,
                            1280*2,
                            hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr,
                            BPXL_eY18_Cr8_Y08_Cb8,
                            NULL,
                            BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED,
                            NULL,
                            &pUniPicture->stBufferInfo.hSurface);
                if (retCode != BERR_SUCCESS)
                {
                   BDBG_ERR(("BSID_P_CreateChannelMemory: BSUR_Surface_Create failed 0x%x\n", retCode));
                   return BERR_TRACE(BERR_UNKNOWN);
                }

                BKNI_Printf("hSurface[%d]=%p, outputBuffer 0x%x\n",
                buffer_index,
                pUniPicture->stBufferInfo.hSurface,
                hSidCh->a_OutputBuffer[buffer_index].ui32_PhisAddr);

                pUniPicture->stBufferInfo.hHeap = hSidCh->hChHeap;
                pUniPicture->stBufferInfo.bValid = true;
                pUniPicture->stPTS.uiValue = 0x0;
                pUniPicture->stPTS.bValid = true;
                pUniPicture->stFrameRate.eType = BXDM_Picture_FrameRateType_eUnknown;

                if (buffer_index == 0)
                    BKNI_Memset(hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr, 0x80, 1280*2*720);

                if (buffer_index == 1)
                    BKNI_Memset(hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr, 0x20, 1280*2*720);

                if (buffer_index == 2)
                    BKNI_Memset(hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr, 0xE0, 1280*2*720);
            }
#else
            /* init some unified picture fields that will not change during playback */
            {
                pUniPicture = &hSidCh->a_DisplayBuffer[buffer_index];

                pUniPicture->stBufferInfo.hHeap = hSidCh->hChHeap;
                pUniPicture->stBufferInfo.bValid = true;
                pUniPicture->stPTS.uiValue = 0x0;
                pUniPicture->stPTS.bValid = true;
                pUniPicture->stFrameRate.eType = BXDM_Picture_FrameRateType_eUnknown;
            }
#endif

            psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                            (psPbQueueHeader->ui32_NewPictWriteIndex * sizeof(BSID_PlaybackQueueEntry)));

#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif

            BKNI_Memset((void *)psPbQueueEntry, 0x00, sizeof(BSID_PlaybackQueueEntry));
            psPbQueueEntry->ui32_OutputBufferAddress = hSidCh->a_OutputBuffer[buffer_index].ui32_PhisAddr;
            psPbQueueEntry->ps_UnifiedPicture = &hSidCh->a_DisplayBuffer[buffer_index];
            psPbQueueEntry->ui32_OutputState = BSID_OutputBufferState_eIdle;
            psPbQueueEntry->ui32_ChannelState = BSID_ChannelState_eReady;

#if 0
            BKNI_Printf("psQueueEntry[%d]=%p off 0x%x\n",
            buffer_index, psPbQueueEntry, psPbQueueEntry->ui32_Address);
#endif

            BSID_INCR_INDEX(psPbQueueHeader->ui32_NewPictWriteIndex, psPbQueueHeader->ui32_QueueSize);
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
        }

#if 0
        BKNI_Printf("new picture index is %d %d\n",
        psPbQueueHeader->ui32_NewPictWriteIndex,
        psPbQueueHeader->ui32_QueueSize);
#endif
        psPbQueueHeader->ui32_NewPictWriteIndex = 0x0;
        psPbQueueHeader->ui32_DecodedReadIndex = 0x0;
        psPbQueueHeader->ui32_DisplayReadIndex = 0x0;
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
    }

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_DestroyChannelQueue
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_DestroyChannelMemory(BSID_ChannelHandle hSidCh)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_Handle hSid = hSidCh->hSid;

    /* free channel request queue */
    if (hSidCh->sReqQueue.sBuf.pui32_VirtAddr != NULL)
    {
        retCode = BMEM_Free(hSid->hMem, hSidCh->sReqQueue.sBuf.pui32_VirtAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for request queue returned with error 0x%x\n", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

        hSidCh->sReqQueue.sBuf.pui32_VirtAddr = NULL;
    }

    /* free channel request queue */
    if (hSidCh->sRelQueue.sBuf.pui32_VirtAddr != NULL)
    {
        retCode = BMEM_Free(hSid->hMem, hSidCh->sRelQueue.sBuf.pui32_VirtAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for release queue returned with error 0x%x\n", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

        hSidCh->sRelQueue.sBuf.pui32_VirtAddr = NULL;
    }

    /* free channel data queue */
    if (hSidCh->sDataQueue.sBuf.pui32_VirtAddr != NULL)
    {
        retCode = BMEM_Free(hSid->hMem, hSidCh->sDataQueue.sBuf.pui32_VirtAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for data queue returned with error 0x%x\n", retCode));
            return BERR_TRACE(BERR_UNKNOWN);
        }

        hSidCh->sDataQueue.sBuf.pui32_VirtAddr = NULL;
    }

    /* check if we are dealing with a motion channel */
    if (hSidCh->e_ChannelType == BSID_ChannelType_eMotion)
    {
        /* free output buffer */
        unsigned int buffer_index = 0x0;
        BXDM_Picture *pUniPicture;

        for (buffer_index = 0x0; buffer_index < hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber; buffer_index++)
        {
            if (hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr != NULL)
            {
                retCode = BMEM_Free(hSidCh->hChHeap, hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr);
                if (retCode != BERR_SUCCESS)
                {
                    BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for data output buffer returned with error 0x%x\n", retCode));
                    return BERR_TRACE(BERR_UNKNOWN);
                }

                hSidCh->a_OutputBuffer[buffer_index].pui32_VirtAddr = NULL;
            }
        }

        if (hSidCh->sRaveReport.pui32_VirtAddr != NULL)
        {
            retCode = BMEM_Free(hSidCh->hChHeap, hSidCh->sRaveReport.pui32_VirtAddr);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for memory info returned with error 0x%x\n", retCode));
                return BERR_TRACE(BERR_UNKNOWN);
            }

            hSidCh->sRaveReport.pui32_VirtAddr = NULL;
        }

        if (hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr != NULL)
        {
            retCode = BMEM_Free(hSidCh->hChHeap, hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for playback queue returned with error 0x%x\n", retCode));
                return BERR_TRACE(BERR_UNKNOWN);
            }

            hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr = NULL;
        }

        for (buffer_index = 0x0; buffer_index < hSidCh->sChSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber; buffer_index++)
        {
            pUniPicture = &hSidCh->a_DisplayBuffer[buffer_index];

#if 0
            if (pUniPicture->stBufferInfo.hSurface != NULL)
            {
                retCode = BSUR_Surface_Destroy(pUniPicture->stBufferInfo.hSurface);
                if (retCode != BERR_SUCCESS)
                {
                    BDBG_ERR(("BSID_P_DestroyChannelMemory: BMEM_Free for hSurface returned with error 0x%x\n", retCode));
                    return BERR_TRACE(BERR_UNKNOWN);
                }

                pUniPicture->stBufferInfo.hSurface = NULL;
            }
#endif
        }
    }

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_GetChannelQueueStatus
*
* Comments:
*
******************************************************************************/
BSID_ChannelQueueStatus BSID_P_GetChannelQueueStatus(
    BSID_ChannelHandle hSidCh)
{
    BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;

    /*
    BKNI_Printf("BSID_P_GetChannelQueueStatus reqQ wr %d rd %d size %d\n",
    psQueueHeader->ui32_WriteIndex,
    psQueueHeader->ui32_ReadIndex,
    hSidCh->ui32_QueueTrueDepth);
    */

    if (((psQueueHeader->ui32_WriteIndex + 1) % hSidCh->ui32_QueueTrueDepth) == psQueueHeader->ui32_ReadIndex)
    {
        /*BKNI_Printf("BSID_P_GetChannelQueueStatus full\n");*/
        return BSID_ChannelQueueStatus_Full;
        /*return BERR_TRACE(BSID_ChannelQueueStatus_Full);*/
    }
    else
    {
        /*BKNI_Printf("BSID_P_GetChannelQueueStatus not full\n");*/
        return BSID_ChannelQueueStatus_NotFull;
        /*return BERR_TRACE(BSID_ChannelQueueStatus_NotFull);*/
    }
}

/******************************************************************************
* Function name: BSID_SuspendStillDecode
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_SuspendChannels(BSID_Handle hSid)
{
    BERR_Code retCode = BERR_SUCCESS;
    BKNI_EventGroupHandle hEventGroup = NULL;
    BKNI_EventHandle *pTriggeredEvents = NULL;
    unsigned numTriggeredEvents;
    unsigned int channel_index = 0;
    bool bAllChannelClosed = true;

    retCode = BKNI_CreateEventGroup(&hEventGroup);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SuspendStillDecode: BKNI_CreateEventGroup failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    pTriggeredEvents = (BKNI_EventHandle *)BKNI_Malloc(sizeof(BKNI_EventHandle)*BSID_MAX_CHANNELS);
    if (pTriggeredEvents == NULL)
    {
        BDBG_ERR(("BSID_SuspendStillDecode: BKNI_Malloc failed\n"));
        return BERR_TRACE(retCode);
    }

    for (channel_index = 0; channel_index < BSID_MAX_CHANNELS; channel_index++)
    {
        if (hSid->ahChannel[channel_index] != NULL)
        {
            bAllChannelClosed = false;
            hSid->ahChannel[channel_index]->e_ChannelState = BSID_ChannelState_eSuspend;

            retCode = BKNI_AddEventGroup(hEventGroup, hSid->ahChannel[channel_index]->hSyncEvent);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_SuspendStillDecode: BKNI_AddEventGroup failed with error 0x%x\n", retCode));
                return BERR_TRACE(retCode);
            }

            retCode = BSID_P_SendSyncChannel(hSid->ahChannel[channel_index]);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_SuspendStillDecode: BSID_P_SendSyncChannel failed with error 0x%x\n", retCode));
                return BERR_TRACE(retCode);
            }
        }
    }

    if (bAllChannelClosed == true)
    {
        goto no_wait;
    }
    retCode = BKNI_WaitForGroup(hEventGroup, BKNI_INFINITE, pTriggeredEvents, BSID_MAX_CHANNELS, &numTriggeredEvents);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SuspendStillDecode: BKNI_WaitForGroup failed with error 0x%x\n", retCode));
        return BERR_TRACE(retCode);
    }

    for (channel_index = 0; channel_index < BSID_MAX_CHANNELS; channel_index++)
    {
        if (hSid->ahChannel[channel_index] != NULL)
        {
            retCode = BSID_P_SendCmdCloseChannel(hSid->ahChannel[channel_index]);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_CloseChannel: BSID_P_SendCmdCloseChannel failed with error 0x%x\n", retCode));
                return BERR_TRACE(retCode);
            }
        }
    }
#if 0
    BKNI_Printf("BKNI_WaitForGroup waited on %d events\n", numTriggeredEvents);
#endif

no_wait:
    if (hEventGroup != NULL)
    {
    BKNI_DestroyEventGroup(hEventGroup);
    }

    if (pTriggeredEvents != NULL)
    {
    BKNI_Free(pTriggeredEvents);
    }

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_ResumeStillDecode
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_ResumeChannels(BSID_Handle hSid)
{
    BERR_Code retCode = BERR_SUCCESS;

    unsigned int channel_index = 0;

    for (channel_index = 0; channel_index < BSID_MAX_CHANNELS; channel_index++)
    {
        if (hSid->ahChannel[channel_index] != NULL)
        {
            retCode = BSID_P_SendCmdOpenChannel(hSid->ahChannel[channel_index]);
            if (retCode != BERR_SUCCESS)
            {
                BDBG_ERR(("BSID_OpenChannel: BSID_P_SendCmdOpenChannel failed with error 0x%x\n", retCode));
                return BERR_TRACE(retCode);
            }
            hSid->ahChannel[channel_index]->e_ChannelState = BSID_ChannelState_eReady;

            BKNI_ResetEvent(hSid->ahChannel[channel_index]->hSyncEvent);
        }
    }

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_IsStillOperationAllowed
*
* Comments:
*
******************************************************************************/
bool BSID_IsStillOperationAllowed(BSID_Handle hSid)
{
    unsigned int channel_index = 0;
    bool still_allowed = true;

    for (channel_index = 0; channel_index < BSID_MAX_CHANNELS; channel_index++)
    {
        if (hSid->ahChannel[channel_index] != NULL)
        {
            if (hSid->ahChannel[channel_index]->e_ChannelType == BSID_ChannelType_eMotion)
            {
                still_allowed = false;
                break;
            }
        }
    }

    return still_allowed;
}

/******************************************************************************
* Function name: BSID_IsMotionOperationAllowed
*
* Comments:
*
******************************************************************************/
bool BSID_IsMotionOperationAllowed(BSID_Handle hSid)
{
    unsigned int channel_index = 0;
    bool motion_allowed = true;

    for (channel_index = 0; channel_index < BSID_MAX_CHANNELS; channel_index++)
    {
        if (hSid->ahChannel[channel_index] != NULL)
        {
            if ((hSid->ahChannel[channel_index]->e_ChannelType == BSID_ChannelType_eStill) &&
                (hSid->ahChannel[channel_index]->e_ChannelState == BSID_ChannelState_eDecode))
            {
                motion_allowed = false;
                break;
            }
        }
    }

    return motion_allowed;
}

