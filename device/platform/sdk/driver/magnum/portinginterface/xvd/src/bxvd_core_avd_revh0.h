/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_revh0.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 6/27/11 4:17p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_core_avd_revh0.h $
 * 
 * Hydra_Software_Devel/12   6/27/11 4:17p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/9/11 5:20p davidp
 * SW7425-542: Fix compile warning.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/9/11 3:56p davidp
 * SW7425-542: Remove unnecessary chip specific macro.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/11   6/8/11 1:03p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/10   7/13/10 4:29p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/9   4/1/10 2:59p davidp
 * SW7405-4160: Fix PM clock register field names.
 * 
 * Hydra_Software_Devel/8   10/19/09 3:56p davidp
 * SW3548-2458: Use new reset procedure for AVD h0 rev chips.
 * 
 * Hydra_Software_Devel/7   6/1/09 6:37p davidp
 * PR55198: Add AVD power management register pointers and masks to
 * register table.
 * 
 * Hydra_Software_Devel/6   10/17/08 5:32p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/5   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/4   6/10/08 4:36p davidp
 * PR38955: Added uiBvnf_Intr2_3_AvdStatus to platform register structure
 * to send to AVD FW.
 * 
 * Hydra_Software_Devel/3   5/22/08 4:57p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/2   5/14/08 12:34p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/1   1/29/08 5:27p davidp
 * PR39098: Initial version to separate AVD core dependent register usage.
 * 
 ***************************************************************************/
#ifndef _BXVD_CORE_AVD_REVH0_H_
#define _BXVD_CORE_AVD_REVH0_H_

#include "bxvd_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_MEM_SIZE 0

#define BXVD_P_WRITE_FWCMD_TO_MBX(hXvd, uInstance, value)  \
   BXVD_Reg_Write32(hXvd,                                  \
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox, \
		    value);

#define BXVD_P_WRITE_FWRSP_MBX(hXvd, uInstance, value)       \
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, value);

#define BXVD_P_GET_PIC_BUF_PHYS_ADDR(hXvdCh, lumaPhysAddr, chromaPhysAddr)  \
	lumaPhysAddr = p_display_element->pPPB->luma_video_address;         \
	chromaPhysAddr = p_display_element->pPPB->chroma_video_address;       



#define BXVD_P_READ_DISPLAY_INFO(hXvdDipCh, stDisplayInfo)                                   \
   stDisplayInfo.vsync_parity = hXvdDipCh->stChannelSettings.pstDisplayInfo->vsync_parity;   \
   stDisplayInfo.stc_snapshot = hXvdDipCh->stChannelSettings.pstDisplayInfo->stc_snapshot;   \
   stDisplayInfo.stc1_snapshot = hXvdDipCh->stChannelSettings.pstDisplayInfo->stc1_snapshot; \


#define BXVD_P_IS_DISPLAY_INFO_EQUAL(stDisplayInfo, stDisplayInfo1)   \
   (( stDisplayInfo.vsync_parity == stDisplayInfo1.vsync_parity ) &&  \
    ( stDisplayInfo.stc_snapshot == stDisplayInfo1.stc_snapshot ) &&  \
    ( stDisplayInfo.stc1_snapshot == stDisplayInfo1.stc1_snapshot ))


#define BXVD_P_SAVE_DIP_INFO_STC(hXvdDispCh, stDisplayInfo)                              \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[0].uiValue = stDisplayInfo.stc_snapshot;  \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[0].bValid = true;                         \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[1].uiValue = stDisplayInfo.stc1_snapshot; \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[1].bValid = true;


#define BXVD_P_DBG_MSG_DISP_INFO_OFFSET(hXvd, pInitRsp)                                     \
      BXVD_DBG_MSG(hXvd, (" dms_delivery_address0: %#x", pInitRsp->dms_delivery_address0)); \
      BXVD_DBG_MSG(hXvd, (" dms_delivery_address1: %#x", pInitRsp->dms_delivery_address1));


#define BXVD_P_SAVE_DISPLAY_INFO(hXvd, pInitRsp)                              \
      BMEM_ConvertOffsetToAddress(hXvd->hCodeHeap,                            \
                                  (uint32_t)pInitRsp->dms_delivery_address0,  \
                                  (void *)&hXvd->pDisplayInfo0);              \
      BMEM_ConvertOffsetToAddress(hXvd->hCodeHeap,                            \
                                  (uint32_t)pInitRsp->dms_delivery_address1,  \
                                  (void *)&hXvd->pDisplayInfo1);


#define BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_0(hXvdCh, hXvd)                         \
       hXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pDisplayInfo = hXvd->pDisplayInfo0;


#define BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_1(hXvdCh, hXvd)                         \
       hXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pDisplayInfo = hXvd->pDisplayInfo1;


#define BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_0(stXvdDipChSettings, hXvd) \
       stXvdDipChSettings.pstDisplayInfo = pXvd->pDisplayInfo0; 


#define BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_1(stXvdDipChSettings, hXvd) \
       stXvdDipChSettings.pstDisplayInfo = pXvd->pDisplayInfo1; 


/* Firmware addresses and offsets */
#define  BAVC_BASE                   (0x80000)
#define  BAVC_CMD_OFFSET             (0x100)
#define  BAVC_CMD_BUF                (BAVC_BASE + BAVC_CMD_OFFSET)

/* Core Firmware */
typedef struct {
	BXVD_IMAGE_FirmwareID outerELFFirmwareID;
	BXVD_IMAGE_FirmwareID innerELFFirmwareID;
	BXVD_IMAGE_FirmwareID authenticatedFirmwareID;
} BXVD_P_CoreFirmware_RevH0;

/* Platform Register Maps */
typedef struct {
      uint32_t uiSun_SWReset;
      uint32_t uiSun_SWResetAVDMask;
      
      uint32_t uiAvd_CPUL2InterruptSet;
      uint32_t uiAvd_CPUL2InterruptClear;
      uint32_t uiAvd_CPUL2InterruptMailboxMask;
      
      uint32_t uiBvnf_Intr2_3_AvdStatus;
      uint32_t uiBvnf_Intr2_3_AvdClear;
      uint32_t uiBvnf_Intr2_3_AvdMaskClear;
      
      uint32_t uiXPT_PCROffset_STC;
      
      uint32_t uiInterrupt_PicDataRdy;
      uint32_t uiInterrupt_PicDataRdy1;
      uint32_t uiInterrupt_Mailbox;
      uint32_t uiInterrupt_StereoSeqError;
      uint32_t uiInterrupt_StillPictureRdy;
      uint32_t uiInterrupt_OuterWatchdog;
      uint32_t uiInterrupt_VICReg;
      uint32_t uiInterrupt_VICSCBWr;
      uint32_t uiInterrupt_VICInstrRd;

      uint32_t uiDecode_InnerInstructionBase;
      uint32_t uiDecode_InnerEndOfCode;
      uint32_t uiDecode_InnerGlobalIOBase;
      uint32_t uiDecode_InnerCPUDebug;
      uint32_t uiDecode_InnerCPUAux;

      uint32_t uiDecode_OuterInstructionBase;
      uint32_t uiDecode_OuterEndOfCode;
      uint32_t uiDecode_OuterGlobalIOBase;
      uint32_t uiDecode_OuterCPUDebug;
      uint32_t uiDecode_OuterCPUAux;
      uint32_t uiDecode_OuterInterruptMask;
      uint32_t uiDecode_OuterInterruptClear;
      uint32_t uiDecode_OuterWatchdogTimer;
      uint32_t uiDecode_OuterCPU2HostMailbox;
      uint32_t uiDecode_OuterHost2CPUMailbox;
      uint32_t uiDecode_OuterCPU2HostStatus;

      uint32_t uiDecode_CabacBinDepth;

      uint32_t uiDecode_CabacBinCtl;
      uint32_t uiDecode_CabacBinCtl_ResetMask;
      uint32_t uiDecode_SintStrmSts;
      uint32_t uiDecode_SintStrmSts_ResetMask;
      uint32_t uiDecode_OLSintStrmSts;
      uint32_t uiDecode_OLSintStrmSts_ResetMask;
      uint32_t uiDecode_MainCtl;
      uint32_t uiDecode_MainCtl_ResetMask;

      uint32_t uiSunGisbArb_ReqMask;
      uint32_t uiSunGisbArb_ReqMaskAVDMask;

      uint32_t uiDecode_StripeWidth;
      uint32_t uiDecode_StripeWidthMask;
      uint32_t uiDecode_StripeWidthShift;
      uint32_t uiDecode_PFRI;
      uint32_t uiDecode_PFRIBankHeightMask;
      uint32_t uiDecode_PFRIBankHeightShift;

      uint32_t uiDecode_RVCCtl;
      uint32_t uiDecode_RVCPut;
      uint32_t uiDecode_RVCGet;
      uint32_t uiDecode_RVCBase;
      uint32_t uiDecode_RVCEnd;

      uint32_t uiVCXO_AVDCtrl;
      uint32_t uiVCXO_AVDCtrl_PwrDnMask;
      uint32_t uiClkGen_CoreClkCtrl;
      uint32_t uiClkGen_CoreClkCtrl_PwrDnMask;
      uint32_t uiClkGen_SCBClkCtrl;
      uint32_t uiClkGen_SCBClkCtrl_PwrDnMask;
      uint32_t uiClkGen_GISBClkCtrl;
      uint32_t uiClkGen_GISBClkCtrl_PwrDnMask;
} BXVD_P_PlatformReg_RevH0;

typedef struct {
	BXVD_P_CoreFirmware_RevH0 stFirmware;
	BXVD_P_PlatformReg_RevH0 stReg;
} BXVD_P_PlatformInfo_RevH0;

#define BXVD_P_CONTEXT_PLATFORM BXVD_P_PlatformInfo_RevH0 stPlatformInfo;

/*
 * Macros for managing the delivery queue.
 */
#define BXVD_AVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                               \
{                                                                                                           \
   _uiReadOffset_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue->queue_read_offset;  \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                               \
{                                                                                                           \
   _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue->queue_read_offset = _uiReadOffset_;  \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                \
{                                                                                                              \
   _uiWriteOffset_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue->queue_write_offset;   \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_GET_ADDR( _hXvdCh_, _pDeliveryQue_ )                    \
{                                                                                         \
    _pDeliveryQue_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue;  \
}

/*
 * Macros for managing the release queue.
 */
#define BXVD_AVD_P_RELEASE_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                                \
{                                                                                                           \
   _uiReadOffset_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue->queue_read_offset;   \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                 \
{                                                                                                              \
   _uiWriteOffset_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue->queue_write_offset;    \
}

#define BXVD_AVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                 \
{                                                                                                              \
   _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue->queue_write_offset = _uiWriteOffset_;    \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )  \
{                                                                                      \
   _uiWriteOffset_ = _hXvdCh_->stChBufferConfig.pAVD_PI2DMS_Buffer->pdq_write_offset;  \
}

#define BXVD_AVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )  \
{                                                                                      \
   _hXvdCh_->stChBufferConfig.pAVD_PI2DMS_Buffer->pdq_write_offset = _uiWriteOffset_;  \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_ADDR( _hXvdCh_, _pReleaseQue_ )                   \
{                                                                                      \
   _pReleaseQue_= _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue;   \
}

/*
 * Macro to get the number of pictures on the delivery or release queue.
 * The offsets should be '0' based.
 */
#define BXVD_AVD_P_GET_QUEUE_DEPTH( _uiReadOffset_, _uiWriteOffset_, _uiDepth_ )                \
{                                                                                               \
   if ( _uiReadOffset_ == _uiWriteOffset_ )                                                     \
   {                                                                                            \
      _uiDepth_ = 0;                                                                            \
   }                                                                                            \
   else if ( _uiReadOffset_ < _uiWriteOffset_ )                                                 \
   {                                                                                            \
      _uiDepth_ = _uiWriteOffset_ - _uiReadOffset_;                                             \
   }                                                                                            \
   else                                                                                         \
   {                                                                                            \
      _uiDepth_ = ( BXVD_P_MAX_ELEMENTS_IN_DISPLAY_QUEUE - _uiReadOffset_ ) + _uiWriteOffset_;  \
   }                                                                                            \
}

/*
 * Macro to increment the queue offset.
 */
#define BXVD_AVD_P_INCREMENT_2BASED_OFFSET( _uiOffset_, _uiCount_ )                                            \
{                                                                                                              \
    /* If the offset is about to go off the end of the queue,                                                  \
     * set it back to the beginning.  Otherwise just increment it.                                             \
     */                                                                                                        \
   if( _uiOffset_ + _uiCount_ >= BXVD_P_DISP_FIFO_DEPTH )                                                      \
   {                                                                                                           \
      _uiOffset_ = ( _uiOffset_ + _uiCount_ - BXVD_P_DISP_FIFO_DEPTH ) + BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE ; \
   }                                                                                                           \
   else                                                                                                        \
   {                                                                                                           \
      _uiOffset_ += _uiCount_;                                                                                 \
   }                                                                                                           \
}

/*
 * Macros for accessing the drop count.
 */
#define BXVD_AVD_P_SET_DROP_COUNT( _hXvdCh_, _count_ )                     \
{                                                                          \
   _hXvdCh_->stChBufferConfig.pAVD_PI2DMS_Buffer->drop_count = _count_;    \
}

#define BXVD_AVD_P_GET_DROP_COUNT( _hXvdCh_, _count_ )                     \
{                                                                          \
   _count_ = _hXvdCh_->stChBufferConfig.pAVD_PI2DMS_Buffer->drop_count;    \
}

/*
 * Macro for retrieving the AVD status block.
 */
#define BXVD_AVD_P_GET_VIDEO_DECODER_STATUS( _hXvdCh_, _ulStatus_ )     \
{                                                                       \
   if ( _hXvdCh_->ulAvdStatusBlock != 0 )                               \
   {                                                                    \
      _ulStatus_ = *(uint32_t *)( _hXvdCh_->ulAvdStatusBlock );         \
   }                                                                    \
   else                                                                 \
   {                                                                    \
      _ulStatus_ = 0;                                                   \
   }                                                                    \
}

#ifdef __cplusplus
}
#endif


#endif /* _BXVD_CORE_AVD_REVH0_H_ */
