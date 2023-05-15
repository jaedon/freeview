/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform.h $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 3/6/13 5:54p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform.h $
 * 
 * Hydra_Software_Devel/22   3/6/13 5:54p nilesh
 * SW7425-4615: Merge refactor of platform vs core parameters
 * 
 * Hydra_Software_Devel/SW7425-4615/1   3/6/13 5:42p nilesh
 * SW7425-4615: Refactor platform vs core specific parameters
 * 
 * Hydra_Software_Devel/21   3/5/13 11:46a nilesh
 * SW7445-181: Added support for larger secure buffer size for VICE2 v2.1
 * 
 * Hydra_Software_Devel/20   3/1/13 9:37a nilesh
 * SW7145-21: Added 7145 support
 * 
 * Hydra_Software_Devel/19   12/3/12 5:38p nilesh
 * SW7445-39: FW v4.0.7.0
 * 
 * Hydra_Software_Devel/SW7445-39/2   11/20/12 3:16p nilesh
 * SW7445-39: Removed MPEG4 support from 7445
 * 
 * Hydra_Software_Devel/SW7445-39/1   11/16/12 10:55a nilesh
 * SW7445-39: Added support for multiple slices
 * 
 * Hydra_Software_Devel/18   11/14/12 2:17p nilesh
 * SW7445-39: Set VICE ARCSS DATA ADDR OFFSET
 * 
 * Hydra_Software_Devel/17   9/18/12 10:24a nilesh
 * SW7445-39: Initial 7445 support
 * 
 * Hydra_Software_Devel/16   2/14/12 10:05a nilesh
 * SW7425-2220: Cleaned up state when StopEncode(eImmediate) is used
 * 
 * Hydra_Software_Devel/15   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 * 
 * Hydra_Software_Devel/14   9/23/11 10:23a nilesh
 * SW7425-1224: BVCE_Power_[Standby/Resume] now behave the same regardless
 * of whether power management support is enabled
 * 
 * Hydra_Software_Devel/13   9/15/11 4:17p nilesh
 * SW7425-891: Implemented clean shutdown of MAU before SW INIT
 * 
 * Hydra_Software_Devel/12   9/14/11 2:55p nilesh
 * SW7425-890: Added support for 2 separate cabac data ready interrupts
 * for 7425 B0
 * 
 * Hydra_Software_Devel/11   9/8/11 1:58p nilesh
 * SW7425-891: Add B0 and Dual Encode Support
 * 
 * Hydra_Software_Devel/SW7425-891/1   9/7/11 3:52p nilesh
 * SW7425-891: Added dual channel support for 7425 B0
 * 
 * Hydra_Software_Devel/10   8/11/11 11:29a nilesh
 * SW7425-1006: VCE FW API v2.0
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/3   8/10/11 4:49p nilesh
 * SW7425-448: Rebase against mainline
 * 
 * Hydra_Software_Devel/9   8/10/11 4:22p nilesh
 * SW7425-448: Added Dynamic Power Management
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/2   8/2/11 9:32p nilesh
 * SW7425-448: Refactored power management logic.
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/1   7/22/11 11:15p nilesh
 * SW7425-1: Additional CDB debug logs
 * 
 * Hydra_Software_Devel/8   4/5/11 4:37p nilesh
 * SW7425-172,SW7425-173: Added device (watchdog) and channel (event)
 * callback suppport
 * 
 * Hydra_Software_Devel/7   12/6/10 11:52a nilesh
 * SW7425-1: Moved mailbox register config to platform config
 * 
 * Hydra_Software_Devel/6   11/18/10 12:18p nilesh
 * SW7425-1: Removed some ifdefs to more more modular on other platforms
 * 
 * Hydra_Software_Devel/5   11/18/10 12:09p nilesh
 * SW7425-1: Fix watchdog
 * 
 * Hydra_Software_Devel/4   11/17/10 1:23p nilesh
 * SW7425-1: Moved MIPs debug register definitions to platform specific
 * file
 * 
 * Hydra_Software_Devel/3   11/17/10 12:41p nilesh
 * SW7425-1: Moved platform specific code
 * 
 * Hydra_Software_Devel/2   10/20/10 4:56p nilesh
 * SW7425-1: Support only one encode channel.  Reduced FW size to 512k per
 * ARC
 * 
 * Hydra_Software_Devel/1   10/12/10 1:35p nilesh
 * SW7425-1: Platform specific defines
 *
 ***************************************************************************/

#ifndef BVCE_PLATFORM_H_
#define BVCE_PLATFORM_H_

#include "bint.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* PLATFORM FLAGS */
#if (BCHP_CHIP == 7425)
#include "bvce_platform_7425.h"
#elif (BCHP_CHIP == 7435)
#include "bvce_platform_7435.h"
#elif (BCHP_CHIP == 7445)
#include "bvce_platform_7445.h"
#elif (BCHP_CHIP == 7145)
#include "bvce_platform_7145.h"
#else
#warning "No Platform Config Specified"
#endif

#ifndef BVCE_PLATFORM_P_NUM_ARC_CORES
#define BVCE_PLATFORM_P_NUM_ARC_CORES 2
#endif

#ifndef BVCE_PLATFORM_P_FW_CODE_SIZE
#define BVCE_PLATFORM_P_FW_CODE_SIZE (512*1024)
#endif

#ifndef BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS
#error BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS not defined
#endif

#ifndef BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS
#error BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS not defined
#endif

#ifndef BVCE_PLATFORM_P_ITB_ALIGNMENT
#error BVCE_PLATFORM_P_ITB_ALIGNMENT not defined
#endif

#ifndef BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES
#error BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES not defined
#endif

#ifndef MIN_PICTURE_BUFFER_SIZE_IN_BYTES
#error MIN_PICTURE_BUFFER_SIZE_IN_BYTES not defined
#endif

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#error MIN_SECURE_BUFFER_SIZE_IN_BYTES not defined
#endif

#define BVCE_P_VIDEOCOMPRESSIONSTD_MAX BAVC_VideoCompressionStd_eMVC
#define BVCE_P_VIDEOCOMPRESSIONSTD_UNSUPPORTED 0xFFFFFFFF

extern const uint32_t BVCE_P_ProtocolLUT[];

typedef struct BVCE_Platform_P_RegisterSetting
{
      char *szDescription;
      uint32_t uiAddress;
      uint32_t uiValue;
      uint32_t uiMask;
} BVCE_Platform_P_RegisterSetting;

typedef struct BVCE_Platform_P_RegisterSettingsArray
{
      const BVCE_Platform_P_RegisterSetting *astRegisterSettings;
      uint32_t uiRegisterCount;
} BVCE_Platform_P_RegisterSettingsArray;

typedef struct BVCE_Platform_P_CoreRegisters
{
      uint32_t uiInstructionStartPhysicalAddress;
      uint32_t uiDataSpaceStartRelativeOffset;
      uint32_t uiDataSpaceStartSystemOffset;
      uint32_t uiDCCMBase;
} BVCE_Platform_P_CoreRegisters;

typedef struct BVCE_Platform_P_OutputContext
{
      uint32_t uiReadPointer;
      uint32_t uiBasePointer;
      uint32_t uiValidPointer;
      uint32_t uiWritePointer;
      uint32_t uiEndPointer;
} BVCE_Platform_P_OutputContext;

typedef struct BVCE_Platform_P_OutputRegisters
{
      BVCE_Platform_P_OutputContext stCDB;
      BVCE_Platform_P_OutputContext stITB;
} BVCE_Platform_P_OutputRegisters;

typedef struct BVCE_Platform_P_DebugRegisters
{
      uint32_t uiCMEPictureIndex;
      uint32_t uiPicArcPC;
      uint32_t uiPicArcStatus32;
      uint32_t uiCDBDepth[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];
      uint32_t uiSTC[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];
} BVCE_Platform_P_DebugRegisters;

typedef struct BVCE_Platform_P_MailboxRegisters
{
      uint32_t uiHost2ViceMailboxAddress;
      uint32_t uiVice2HostMailboxAddress;
      uint32_t uiBvn2ViceMailboxAddress;
} BVCE_Platform_P_MailboxRegisters;

typedef struct BVCE_Platform_P_InterruptMasks
{
      uint32_t uiWatchdog[BVCE_PLATFORM_P_NUM_ARC_CORES];
      uint32_t uiDataReady[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];
      uint32_t uiError;
      uint32_t uiMailbox;
} BVCE_Platform_P_InterruptMasks;

typedef struct BVCE_Platform_P_InterruptSettings
{
      uint32_t uiInterruptStatusRegister;
      uint32_t uiInterruptClearRegister;
      BVCE_Platform_P_InterruptMasks stMask;

      BINT_Id idWatchdog[BVCE_PLATFORM_P_NUM_ARC_CORES];
      BINT_Id idDataReady[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];
      BINT_Id idEvent;
      BINT_Id idMailbox;
} BVCE_Platform_P_InterruptSettings;

typedef enum BVCE_Power_Type
{
   BVCE_Power_Type_ePower,
   BVCE_Power_Type_eClock,

   BVCE_Power_Type_eMax
} BVCE_Power_Type;

typedef struct BVCE_Power_P_Resource
{
#ifdef BCHP_PWR_SUPPORT
   BCHP_PWR_ResourceId id;
#else
   unsigned id;
#endif
   unsigned uiRefCount;
} BVCE_Power_P_Resource;

typedef struct BVCE_Platform_P_PowerSettings
{
   BVCE_Power_P_Resource astResource[BVCE_Power_Type_eMax];
} BVCE_Platform_P_PowerSettings;

typedef struct BVCE_Platform_P_Config
{
      BVCE_Platform_P_RegisterSettingsArray stViceReset;
      BVCE_Platform_P_RegisterSettingsArray stViceInterruptEnable;
      BVCE_Platform_P_RegisterSettingsArray stViceInterruptDisable;
      BVCE_Platform_P_RegisterSettingsArray stViceBoot;
      BVCE_Platform_P_RegisterSettingsArray stViceWatchdogEnable;
      BVCE_Platform_P_RegisterSettingsArray stViceWatchdogDisable;

      BVCE_Platform_P_CoreRegisters stCore[BVCE_PLATFORM_P_NUM_ARC_CORES];
      BVCE_Platform_P_OutputRegisters stOutput[BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS];
      BVCE_Platform_P_InterruptSettings stInterrupt;
      BVCE_Platform_P_DebugRegisters stDebug;
      BVCE_Platform_P_MailboxRegisters stMailbox;
      BVCE_Platform_P_PowerSettings stPower;
} BVCE_Platform_P_Config;

BERR_Code
BVCE_Platform_P_GetConfig(
         unsigned uiInstance,
         BVCE_Platform_P_Config *pstPlatformConfig
         );

BERR_Code
BVCE_Platform_P_WriteRegisterList(
         BREG_Handle hReg,
         const BVCE_Platform_P_RegisterSetting *astRegisterList,
         uint32_t uiCount
         );

BERR_Code
BVCE_Platform_P_PreReset(
   unsigned uiInstance,
   BREG_Handle hReg
   );

#if BVCE_P_ENABLE_UART
BERR_Code
BVCE_Platform_P_EnableUART(
   BREG_Handle hReg
   );
#endif

#ifdef __cplusplus
}
#endif

#endif /* BVCE_PLATFORM_H_ */
