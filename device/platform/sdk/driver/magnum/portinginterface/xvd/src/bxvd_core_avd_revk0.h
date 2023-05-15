/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_core_avd_revk0.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 2/2/12 3:06p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_core_avd_revk0.h $
 * 
 * Hydra_Software_Devel/8   2/2/12 3:06p davidp
 * SW7425-2024: Define platform specific supported decode protocols mask.
 * 
 * Hydra_Software_Devel/7   6/27/11 4:17p davidp
 * SW7425-542: Merge devel branch to mainline.
 *
 * Hydra_Software_Devel/SW7425-542/6   6/22/11 2:04p davidp
 * SW7425-542: Use _isr version of BXVD_Reg_Read/Write in HIM access
 * macros.
 * 
 * Hydra_Software_Devel/SW7425-542/5   6/9/11 3:56p davidp
 * SW7425-542: Remove unnecessary chip specific macro.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/9/11 1:19p btosi
 * SW7425-542: added in support for the Host Memory Interface
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/7/11 5:44p davidp
 * SW7425-542: Add macros and support routines to handle old and new HIM
 * FW API.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:06p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/6   6/8/11 1:05p btosi
 * SW7425-542: moved the release/delivery queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/5   11/30/10 5:58p davidp
 * SW7422-112: Enable all AVD core clocks before resetting AVD cores.
 * 
 * Hydra_Software_Devel/4   10/7/10 3:32p davidp
 * SW7422-22: Add new reset procedure with BLD support.
 * 
 * Hydra_Software_Devel/3   9/29/10 2:48p davidp
 * SW7422-22: Add BLD specific registers to register table.
 * 
 * Hydra_Software_Devel/2   9/22/10 3:41p davidp
 * SW7358-11: Add new reset/shutdown registers to table.
 * 
 * Hydra_Software_Devel/1   9/10/10 5:46p davidp
 * SW7358-11: Add support for AVD Rev K core.
 * 
 ***************************************************************************/
#ifndef _BXVD_CORE_AVD_REVK0_H_
#define _BXVD_CORE_AVD_REVK0_H_

#include "bxvd_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BXVD_P_WRITE_FWCMD_TO_MBX(hXvd, uInstance, value)  \
   BXVD_Reg_Write32(hXvd,                                  \
		    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox, \
		    value);


#define BXVD_P_WRITE_FWRSP_MBX(hXvd, uInstance, value)       \
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, value);


#define BXVD_P_GET_PIC_BUF_PHYS_ADDR(hXvdCh, lumaPhysAddr, chromaPhysAddr)  \
	lumaPhysAddr = p_display_element->pPPB->luma_video_address;         \
	chromaPhysAddr = p_display_element->pPPB->chroma_video_address;       


#define BXVD_P_READ_DISPLAY_INFO(hXvdDipCh, stDisplayInfo)                          \
   BXVD_P_ReadDisplayInfo_HIM_API(hXvdDipCh->stChannelSettings.hXvd,                \
                                  hXvdDipCh->stChannelSettings.uiDisplayInfoOffset, \
                                  &stDisplayInfo);    


#define BXVD_P_READ_HIM(hXvd, wordOffset, value)                                  \
   BXVD_Reg_Write32_isr(hXvd,                                                     \
                    hXvd->stPlatformInfo.stReg.uiDecode_HST_SCR_RAM_ADDR,         \
                    wordOffset);                                                  \
   value = BXVD_Reg_Read32_isr(hXvd,                                              \
                           hXvd->stPlatformInfo.stReg.uiDecode_HST_SCR_RAM_DATA); 
   
      
#define BXVD_P_WRITE_HIM(hXvd, wordOffset, value)                         \
   BXVD_Reg_Write32_isr(hXvd,                                             \
                    hXvd->stPlatformInfo.stReg.uiDecode_HST_SCR_RAM_ADDR, \
                    wordOffset);                                          \
   BXVD_Reg_Write32_isr(hXvd,                                             \
                    hXvd->stPlatformInfo.stReg.uiDecode_HST_SCR_RAM_DATA, \
                    value);                                               


#define BXVD_P_SAVE_DIP_INFO_STC(hXvdDispCh, stDisplayInfo)                                \
   for (i=0; i< BXVD_P_STC_MAX; i++)                                                       \
   {                                                                                       \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[i].uiValue = stDisplayInfo.stc_snapshot[i]; \
      hXvdDipCh->stDisplayInterruptInfo.astSTC[i].bValid = true;                           \
   }

#define BXVD_P_DBG_MSG_DISP_INFO_OFFSET(hXvd, pInitRsp)                                    \
   BXVD_DBG_MSG(hXvd, (" display_info_0_offset: %#x", pInitRsp->display_info_0_offset));   \
   BXVD_DBG_MSG(hXvd, (" display_info_1_offset: %#x", pInitRsp->display_info_1_offset));

#define BXVD_P_SAVE_DISPLAY_INFO(hXvd, pInitRsp)                     \
   hXvd->uiDisplayInfo0_Offset = pInitRsp->display_info_0_offset/4;  \
   hXvd->uiDisplayInfo1_Offset = pInitRsp->display_info_1_offset/4;


#define BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_0(hXvdCh, hXvd)  hXvdCh=hXvdCh


#define BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_1(hXvdCh, hXvd)  hXvdCh=hXvdCh


#define BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_0(stXvdDipChSettings, hXvd)  \
   stXvdDipChSettings.uiDisplayInfoOffset = hXvd->uiDisplayInfo0_Offset;


#define BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_1(stXvdDipChSettings, hXvd)  \
   stXvdDipChSettings.uiDisplayInfoOffset = hXvd->uiDisplayInfo1_Offset;

/* Core Firmware */
typedef struct {
	BXVD_IMAGE_FirmwareID outerELFFirmwareID;
	BXVD_IMAGE_FirmwareID innerELFFirmwareID;
	BXVD_IMAGE_FirmwareID authenticatedFirmwareID;
} BXVD_P_CoreFirmware_RevK0;

/* Platform Register Maps */
typedef struct {
      uint32_t uiSun_SWInitSet;
      uint32_t uiSun_SWInitSetAvdMask;
      uint32_t uiSun_SWInitClear;
      uint32_t uiSun_SWInitClearAvdMask;

      uint32_t uiAvd_SoftShutdownCtrl;
      uint32_t uiAvd_SoftShutdownCtrlMask;

      uint32_t uiDecode_IPShim_AvdClkGate; 
      uint32_t uiDecode_IPShim_OtpCtl;
      uint32_t uiDecode_IPShim_OtpCtl_DisableRV9;
      uint32_t uiDecode_CPUId;
      uint32_t uiDecode_CPUId_BldIdMask;

      uint32_t uiAvd_CPUL2InterruptSet;
      uint32_t uiAvd_CPUL2InterruptClear;
      uint32_t uiAvd_CPUL2InterruptMailboxMask;
      
      uint32_t uiBvnf_Intr2_3_AvdStatus;
      uint32_t uiBvnf_Intr2_3_AvdClear;
      uint32_t uiBvnf_Intr2_3_AvdMaskClear;
      
      uint32_t uiInterrupt_PicDataRdy;
      uint32_t uiInterrupt_PicDataRdy1;
      uint32_t uiInterrupt_Mailbox;
      uint32_t uiInterrupt_StereoSeqError;
      uint32_t uiInterrupt_StillPictureRdy;
      uint32_t uiInterrupt_OuterWatchdog;
      uint32_t uiInterrupt_VICReg;
      uint32_t uiInterrupt_VICSCBWr;
      uint32_t uiInterrupt_VICInstrRd;
      uint32_t uiInterrupt_VICILInstrRd;
      uint32_t uiInterrupt_VICBLInstrRd;
      
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

      uint32_t uiDecode_BaseInstructionBase;
      uint32_t uiDecode_BaseEndOfCode;
      uint32_t uiDecode_BaseGlobalIOBase;
      uint32_t uiDecode_BaseCPUDebug;
      uint32_t uiDecode_BaseCPUAux;

      uint32_t uiDecode_CabacBinDepth;

      uint32_t uiDecode_CabacBinCtl;
      uint32_t uiDecode_CabacBinCtl_ResetMask;
      uint32_t uiDecode_SintStrmSts;
      uint32_t uiDecode_SintStrmSts_ResetMask;
      uint32_t uiDecode_OLSintStrmSts;
      uint32_t uiDecode_OLSintStrmSts_ResetMask;
      uint32_t uiDecode_MainCtl;
      uint32_t uiDecode_MainCtl_ResetMask;
      uint32_t uiDecode_SWReset;
      uint32_t uiDecode_SWReset_ILSIResetMask;

      uint32_t uiDecode_BLD_SintStrmSts;
      uint32_t uiDecode_BLD_SintStrmSts_ResetMask;
      uint32_t uiDecode_BLD_ILSScale;
      uint32_t uiDecode_BLD_ILSScale_ResetMask;
      uint32_t uiDecode_BLD_MainCtl;
      uint32_t uiDecode_BLD_MainCtl_ResetMask;
      uint32_t uiDecode_BLD_SWReset;
      uint32_t uiDecode_BLD_SWReset_ILSIResetMask;

      uint32_t uiSunGisbArb_ReqMask;
      uint32_t uiSunGisbArb_ReqMaskAVDMask;

      uint32_t uiDecode_StripeWidth;
      uint32_t uiDecode_StripeWidthShift;
      uint32_t uiDecode_PFRIBankHeightShift;

      uint32_t uiDecode_PFRIData;
      uint32_t uiDecode_PFRIDataDDR3ModeShift;
      uint32_t uiDecode_PFRIDataBusWidthShift;
      uint32_t uiDecode_PFRIDataSourceShift;
      uint32_t uiDecode_PFRIDataSourceMask;
      uint32_t uiDecode_PFRIData2SourceShift;

      uint32_t uiDecode_BLStripeWidth;
      uint32_t uiDecode_BLPFRIData;

      uint32_t uiAVD_PcacheMode;
      uint32_t uiAVD_PcacheModeYGranMask;
      uint32_t uiAVD_PcacheModeYGranShift;
      uint32_t uiAVD_PcacheModeXGranMask;
      uint32_t uiAVD_PcacheModeXGranShift;

      uint32_t uiDecode_RVCCtl;
      uint32_t uiDecode_RVCPut;
      uint32_t uiDecode_RVCGet;
      uint32_t uiDecode_RVCBase;
      uint32_t uiDecode_RVCEnd;

      uint32_t uiDecode_HST_SCR_RAM_ADDR;
      uint32_t uiDecode_HST_SCR_RAM_DATA;
} BXVD_P_PlatformReg_RevK0;

typedef struct {
	BXVD_P_CoreFirmware_RevK0 stFirmware;
	BXVD_P_PlatformReg_RevK0 stReg;
} BXVD_P_PlatformInfo_RevK0;

#define BXVD_P_CONTEXT_PLATFORM BXVD_P_PlatformInfo_RevK0 stPlatformInfo;

/*
 * Macros for reading/writing words to/from the Host Interface Memory.
 */

#define BXVD_P_HOSTINTERFACEMEMORY_DUMP_OFFSETS( _str_, _stOffsets_ )               \
{                                                                                   \
   BKNI_Printf("%s: base: %08x word: %08x shift: %08x mask: %08x inverse %08x\n",   \
                     _str_,                                                         \
                     _stOffsets_.ulByteOffset,                                      \
                     _stOffsets_.ulWordOffset,                                      \
                     _stOffsets_.ulByteShift,                                       \
                     _stOffsets_.ulByteMask,                                        \
                     _stOffsets_.ulInverseMask                                      \
                     );                                                             \
}


#define BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( _stOffsets_, _ulByteCount_ ) \
{                                                                             \
   /* Size of the data element. */                                            \
   _stOffsets_.ulBytesPerValue = _ulByteCount_;                               \
                                                                              \
   /* Create a mask based on the size of the data element. */                 \
   switch( _ulByteCount_ )                                                    \
   {                                                                          \
      case 1:  _stOffsets_.ulByteMask = 0xFF ;        break;                  \
      case 2:  _stOffsets_.ulByteMask = 0xFFFF ;      break;                  \
      case 3:  _stOffsets_.ulByteMask = 0xFFFFFF ;    break;                  \
      case 4:  _stOffsets_.ulByteMask = 0xFFFFFFFF ;  break;                  \
      default:  _stOffsets_.ulByteMask = 0xFF ;       break;                  \
   }                                                                          \
                                                                              \
   /* Divide by 4 to turn the HIM byte offset into a word offset. */          \
   _stOffsets_.ulWordOffset = ( _stOffsets_.ulByteOffset >> 2 );              \
                                                                              \
   /* Extract the two low order bits and multiply                             \
    * by 8 to get the byte shift */                                                       \
   _stOffsets_.ulByteShift = (( _stOffsets_.ulByteOffset & 0x3 ) *  8 );                  \
                                                                                          \
   /* Create the inverse of the mask for read-modify-write operations */                  \
   _stOffsets_.ulInverseMask = ~( _stOffsets_.ulByteMask << _stOffsets_.ulByteShift  );   \
}

#define BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _stOffset_, _pulValueFromMemory_ )    \
{                                                                                            \
   unsigned long  _ulTempValueFromMemory_ ;                                                  \
   BXVD_Handle _hXvd_ = _hXvdCh_->pXvd;                                                      \
                                                                                             \
   /* Read a word from the Host Interface Memory. */                                         \
   BXVD_P_READ_HIM( _hXvd_, _stOffset_.ulWordOffset, _ulTempValueFromMemory_ );              \
                                                                                             \
   /* Shift the value to get the appropriate byte. */                                        \
   _ulTempValueFromMemory_ >>= _stOffset_.ulByteShift ;                                      \
                                                                                             \
   /* Mask off any higher bits. */                                                           \
   _ulTempValueFromMemory_ &= _stOffset_.ulByteMask ;                                        \
                                                                                             \
   *_pulValueFromMemory_ = (uint32_t)_ulTempValueFromMemory_ ;                               \
}


#define BXVD_AVD_P_HOSTINTERFACEMEMORY_WRITE( _hXvdCh_, _stOffset_, _ulValueToMemory_ )      \
{                                                                                            \
   unsigned long  _ulTempValueToMemory_ ;                                                    \
   unsigned long  _ulCurrentValueInMemory_ ;                                                 \
   BXVD_Handle _hXvd_ = _hXvdCh_->pXvd ;                                                     \
                                                                                             \
   /* Read the current word from the Host Interface Memory. */                               \
   BXVD_P_READ_HIM( _hXvd_, _stOffset_.ulWordOffset, _ulCurrentValueInMemory_ );             \
                                                                                             \
   /* Mask off the byte about to be written */                                               \
   _ulCurrentValueInMemory_ &= _stOffset_.ulInverseMask ;                                    \
                                                                                             \
   /* Shift the value to get the appropriate byte. */                                        \
   _ulTempValueToMemory_ = _ulValueToMemory_  ;                                              \
   _ulTempValueToMemory_ <<= _stOffset_.ulByteShift ;                                        \
                                                                                             \
   /* Or the new byte with the other three current bytes. */                                 \
   _ulTempValueToMemory_ = ( _ulTempValueToMemory_ |  _ulCurrentValueInMemory_ ) ;           \
                                                                                             \
   /* Write a word to the Host Interface Memory. */                                          \
   BXVD_P_WRITE_HIM( _hXvd_, _stOffset_.ulWordOffset, _ulTempValueToMemory_ );               \
}

/*
 * Macros for managing the delivery queue.
 */
#define BXVD_AVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                                 \
{                                                                                                                 \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stDeliveryQueue.stReadIndex, &_uiReadOffset_ );   \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                                 \
{                                                                                                                 \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_WRITE( _hXvdCh_, _hXvdCh_->stDeliveryQueue.stReadIndex, _uiReadOffset_ );   \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                               \
{                                                                                                                 \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stDeliveryQueue.stWriteIndex, &_uiWriteOffset_ ); \
}

#define BXVD_AVD_P_DELIVERY_QUEUE_GET_ADDR( _hXvdCh_, _pDeliveryQue_ )                \
{                                                                                         \
    _pDeliveryQue_ = _hXvdCh_->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue;  \
}

/*
 * Macros for managing the release queue.
 */
#define BXVD_AVD_P_RELEASE_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                                   \
{                                                                                                              \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stReleaseQueue.stReadIndex, &_uiReadOffset_ );     \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                 \
{                                                                                                              \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stReleaseQueue.stWriteIndex, &_uiWriteOffset_ );   \
}

#define BXVD_AVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                 \
{                                                                                                              \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_WRITE( _hXvdCh_, _hXvdCh_->stReleaseQueue.stWriteIndex, _uiWriteOffset_ );   \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                \
{                                                                                                                    \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stReleaseQueue.stShadowWriteIndex, &_uiWriteOffset_ );   \
}

#define BXVD_AVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                                \
{                                                                                                                    \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_WRITE( _hXvdCh_, _hXvdCh_->stReleaseQueue.stShadowWriteIndex, _uiWriteOffset_ );   \
}

#define BXVD_AVD_P_RELEASE_QUEUE_GET_ADDR( _hXvdCh_, _pReleaseQue_ )               \
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
#define BXVD_AVD_P_SET_DROP_COUNT( _hXvdCh_, _count_ )                                    \
{                                                                                         \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_WRITE( _hXvdCh_, _hXvdCh_->stDropCountIndex, _count_ ); \
}

#define BXVD_AVD_P_GET_DROP_COUNT( _hXvdCh_, _count_ )                                     \
{                                                                                          \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stDropCountIndex, &_count_ );  \
}

/*
 * Macro for retrieving the AVD status block.
 */
#define BXVD_AVD_P_GET_VIDEO_DECODER_STATUS( _hXvdCh_, _ulStatus_ )                             \
{                                                                                               \
   BXVD_AVD_P_HOSTINTERFACEMEMORY_READ( _hXvdCh_, _hXvdCh_->stStatusBlockIndex, &_ulStatus_ );  \
}

#ifdef __cplusplus
}
#endif

#endif /* _BXVD_CORE_AVD_REVK0_H_ */
