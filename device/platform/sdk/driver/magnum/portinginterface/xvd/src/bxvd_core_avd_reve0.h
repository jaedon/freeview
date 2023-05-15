/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_reve0.h $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 6/27/11 4:17p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_core_avd_reve0.h $
 * 
 * Hydra_Software_Devel/31   6/27/11 4:17p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/5   6/14/11 3:12p davidp
 * SW7425-542: Merge from mainline to branch.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/9/11 5:20p davidp
 * SW7425-542: Fix compile warning.
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/9/11 3:56p davidp
 * SW7425-542: Remove unnecessary chip specific macro.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/7/11 5:44p davidp
 * SW7425-542: Add macros and support routines to handle old and new HIM
 * FW API.
 * 
 * Hydra_Software_Devel/30   6/8/11 1:03p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/29   7/13/10 4:26p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/28   4/1/10 2:59p davidp
 * SW7405-4160: Fix PM clock register field names.
 * 
 * Hydra_Software_Devel/27   10/16/09 4:07p davidp
 * SW7401-4332: Implement new AVD reset procedure.
 * 
 * Hydra_Software_Devel/26   6/10/09 5:18p davidp
 * PR55198: Add power management support to 7400 platform.
 * 
 * Hydra_Software_Devel/25   10/17/08 5:32p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/24   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/23   6/10/08 4:36p davidp
 * PR38955: Added uiBvnf_Intr2_3_AvdStatus to platform register structure
 * to send to AVD FW.
 * 
 * Hydra_Software_Devel/22   5/22/08 4:57p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/21   5/14/08 12:32p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/20   1/29/08 6:00p davidp
 * PR39098: Remove unused register items from revE0 register structure.
 * 
 * Hydra_Software_Devel/19   1/25/08 7:01p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/18   11/6/07 5:55p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/17   10/31/07 11:07a btosi
 * PR36545: added uiDecode_CabacBinDepth to  BXVD_P_PlatformReg_RevE0
 * 
 * Hydra_Software_Devel/16   8/21/07 4:22p davidp
 * PR34052: Access PCROffset_STC register thru platformInfo.stReg
 * structure.
 * 
 * Hydra_Software_Devel/15   8/21/07 3:05p davidp
 * PR34052: Move register address pointers to platform_info.stReg
 * structure.
 * 
 * Hydra_Software_Devel/14   8/13/07 4:07p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   6/14/07 1:01p davidp
 * PR29915: Add 2nd display device picture data ready interrupt for each
 * decoder
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   5/29/07 5:30p nilesh
 * PR29915: Updated still picture compatibility mode to match discussed
 * design
 * 
 * Hydra_Software_Devel/13   3/2/07 1:35p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 * 
 * Hydra_Software_Devel/PR26188/1   2/27/07 4:05p nilesh
 * PR26188: 7400B0 Bring-Up
 * 
 * Hydra_Software_Devel/12   12/12/06 2:46p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/11   11/17/06 4:51p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:54p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/10   11/13/06 2:02p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 11:06a nilesh
 * PR25365: Added BIMG support
 * 
 * Hydra_Software_Devel/9   9/22/06 2:22p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 * 
 * Hydra_Software_Devel/8   8/21/06 1:52p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/7   8/17/06 12:03p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 2:01p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/6   8/7/06 1:57p nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/5   8/7/06 11:47a nilesh
 * PR22673: Renamed incorrect rev G to rev E0
 * 
 * Hydra_Software_Devel/4   7/31/06 7:51p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 * 
 * Hydra_Software_Devel/3   7/31/06 9:12a pblanco
 * PR22673: Moved 7401/7118 specific BAVC definitions here.
 * 
 * Hydra_Software_Devel/2   7/27/06 8:06p davidp
 * PR22673: Fix typeo
 *
 ***************************************************************************/
#ifndef _BXVD_CORE_AVD_REVE0_H_
#define _BXVD_CORE_AVD_REVE0_H_

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
} BXVD_P_CoreFirmware_RevE0;

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
      uint32_t uiDecode_OuterWatchdogTimerLimit;
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

      uint32_t uiDecode_RVCCtl;
      uint32_t uiDecode_RVCPut;
      uint32_t uiDecode_RVCGet;
      uint32_t uiDecode_RVCBase;
      uint32_t uiDecode_RVCEnd;

#if BXVD_P_POWER_MANAGEMENT
      uint32_t uiVCXO_AVDCtrl;
      uint32_t uiVCXO_AVDCtrl_PwrDnMask;
      uint32_t uiClkGen_CoreClkCtrl;
      uint32_t uiClkGen_CoreClkCtrl_PwrDnMask;
      uint32_t uiClkGen_SCBClkCtrl;
      uint32_t uiClkGen_SCBClkCtrl_PwrDnMask;
      uint32_t uiClkGen_GISBClkCtrl;
      uint32_t uiClkGen_GISBClkCtrl_PwrDnMask;
#endif
} BXVD_P_PlatformReg_RevE0;

typedef struct {
	BXVD_P_CoreFirmware_RevE0 stFirmware;
	BXVD_P_PlatformReg_RevE0 stReg;
} BXVD_P_PlatformInfo_RevE0;

#define BXVD_P_CONTEXT_PLATFORM BXVD_P_PlatformInfo_RevE0 stPlatformInfo;

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


#endif /* _BXVD_CORE_AVD_REVE0_H_ */
