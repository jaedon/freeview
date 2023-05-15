/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_reve1.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 6/8/11 1:03p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_core_avd_reve1.h $
 * 
 * Hydra_Software_Devel/16   6/8/11 1:03p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/15   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/14   6/10/08 4:36p davidp
 * PR38955: Added uiBvnf_Intr2_3_AvdStatus to platform register structure
 * to send to AVD FW.
 * 
 * Hydra_Software_Devel/13   5/22/08 4:57p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/12   5/14/08 12:35p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/11   10/31/07 11:07a btosi
 * PR36545: added uiDecode_CabacBinDepth to  BXVD_P_PlatformReg_RevE1
 * 
 * Hydra_Software_Devel/10   8/21/07 4:22p davidp
 * PR34052: Access PCROffset_STC register thru platformInfo.stReg
 * structure.
 * 
 * Hydra_Software_Devel/9   8/21/07 3:05p davidp
 * PR34052: Move register address pointers to platform_info.stReg
 * structure.
 * 
 * Hydra_Software_Devel/8   8/13/07 4:07p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   6/14/07 1:59p davidp
 * PR29915: Add support for 2nd display device interrupt, remove inner
 * loop watchdog interrupt.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   5/29/07 5:30p nilesh
 * PR29915: Updated still picture compatibility mode to match discussed
 * design
 * 
 * Hydra_Software_Devel/7   3/2/07 1:35p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 * 
 * Hydra_Software_Devel/PR26188/1   2/27/07 4:05p nilesh
 * PR26188: 7400B0 Bring-Up
 * 
 * Hydra_Software_Devel/6   1/9/07 10:48a nilesh
 * PR23113: Fixed macro to retrieve still picture Y/UV buffer addresses
 * 
 * Hydra_Software_Devel/5   12/14/06 2:23p nilesh
 * PR25443: Updated to support simplified FW API
 * 
 * Hydra_Software_Devel/4   11/17/06 4:52p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:55p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/3   11/13/06 2:02p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 1:27p nilesh
 * PR25365: Added BIMG support
 *
 ***************************************************************************/
#ifndef _BXVD_CORE_AVD_REVE1_H_
#define _BXVD_CORE_AVD_REVE1_H_

#include "bxvd_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_MEM_SIZE 0

#define BXVD_P_ENA_DECODER_UART(hxvd, uInstance)           \
   BXVD_Reg_Write32(hXvd,                                  \
		    hXvd->stPlatformInfo.stReg.uiSun_TestPortControl,      \
		    hXvd->stPlatformInfo.stReg.uiSun_TestPortControlAVDValue);

#define BXVD_P_WRITE_FWCMD_TO_MBX(hXvd, uInstance, value)  \
   BXVD_Reg_Write32(hXvd,                                  \
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox, \
		    value);

#define BXVD_P_WRITE_FWRSP_MBX(hXvd, uInstance, value)       \
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, value);

#define BXVD_P_GET_PIC_BUF_PHYS_ADDR(hXvdCh, lumaPhysAddr, chromaPhysAddr)  \
	lumaPhysAddr = p_display_element->pPPB->luma_video_address;              \
	chromaPhysAddr = p_display_element->pPPB->chroma_video_address;       

/* Firmware addresses and offsets */
#define  BAVC_BASE                   (0x80000)
#define  BAVC_CMD_OFFSET             (0x100)
#define  BAVC_CMD_BUF                (BAVC_BASE + BAVC_CMD_OFFSET)

/* Core Firmware */
   typedef struct {
         BXVD_IMAGE_FirmwareID outerELFFirmwareID;
         BXVD_IMAGE_FirmwareID innerELFFirmwareID;
         BXVD_IMAGE_FirmwareID authenticatedFirmwareID;
   } BXVD_P_CoreFirmware_RevE1;

/* Platform Register Maps */
   typedef struct {
         uint32_t uiSun_TestPortControl;	
         uint32_t uiSun_TestPortControlAVDValue;	
         uint32_t uiSun_SWReset;
         uint32_t uiSun_SWResetAVDMask;

         uint32_t uiAvd_CPUL2InterruptClear;
         uint32_t uiAvd_CPUL2InterruptMailboxMask;

         uint32_t uiBvnf_Intr2_3_AvdStatus;
         uint32_t uiBvnf_Intr2_3_AvdClear;
         uint32_t uiBvnf_Intr2_3_AvdMaskClear;

         uint32_t uiXPT_PCROffset_STC;

         uint32_t uiInterrupt_PicDataRdy;
         uint32_t uiInterrupt_PicDataRdy1;
         uint32_t uiInterrupt_Mailbox;
         uint32_t uiInterrupt_SeqHeader;
         uint32_t uiInterrupt_StillPictureRdy;
         uint32_t uiInterrupt_OuterWatchdog;
         uint32_t uiInterrupt_VICReg;
         uint32_t uiInterrupt_VICSCBWr;
         uint32_t uiInterrupt_VICInstrRd;
	
         uint32_t uiDecode_InnerInstructionBase;
         uint32_t uiDecode_InnerEndOfCode;
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
         uint32_t uiDecode_CabacBinDepth;
         uint32_t uiSunGisbArb_ReqMask;
         uint32_t uiSunGisbArb_ReqMaskAVDMask;         
	
         /* Rev E1 Specific Registers */
         uint32_t uiDecode_InnerGlobalIOBase;

         uint32_t uiDecode_RVCCtl;
         uint32_t uiDecode_RVCPut;
         uint32_t uiDecode_RVCGet;
         uint32_t uiDecode_RVCBase;
         uint32_t uiDecode_RVCEnd;
   } BXVD_P_PlatformReg_RevE1;

   typedef struct {
         BXVD_P_CoreFirmware_RevE1 stFirmware;
         BXVD_P_PlatformReg_RevE1 stReg;
   } BXVD_P_PlatformInfo_RevE1;

#define BXVD_P_CONTEXT_PLATFORM BXVD_P_PlatformInfo_RevE1 stPlatformInfo;

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


#endif /* _BXVD_CORE_AVD_REVE1_H_ */
