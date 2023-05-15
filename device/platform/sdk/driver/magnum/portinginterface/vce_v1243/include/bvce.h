/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce.h $
 * $brcm_Revision: Hydra_Software_Devel/70 $
 * $brcm_Date: 3/28/13 1:58p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/include/bvce.h $
 * 
 * Hydra_Software_Devel/70   3/28/13 1:58p nilesh
 * SW7425-4659: Added BVCE_OpenSettings.bA2NPictureDrop
 * 
 * Hydra_Software_Devel/69   3/20/13 4:06p nilesh
 * SW7425-4612:BVCE_Channel_FlushEncode() now waits for EOS if the channel
 * is in the stopping state
 * 
 * Hydra_Software_Devel/68   3/19/13 6:34p nilesh
 * SW7435-688: Added BVCE_Channel_StopMode_eAbort support
 * 
 * Hydra_Software_Devel/67   3/19/13 1:57p nilesh
 * SW7435-688: Added BVCE_Channel_FlushEncode()
 * 
 * Hydra_Software_Devel/66   2/28/13 4:49p nilesh
 * SW7425-4355: Indicate uiEtsDtsOffset is in 27Mhz units
 * 
 * Hydra_Software_Devel/65   2/28/13 3:55p nilesh
 * SW7425-2568: Added drop due to HRD to status
 * 
 * Hydra_Software_Devel/64   2/28/13 10:47a nilesh
 * SW7425-4355: Populate uiEtsDtsOffset in status and video metadata
 * descriptor
 * 
 * Hydra_Software_Devel/63   1/30/13 4:06p nilesh
 * SW7425-4492: Added support for specifying the max number of channels
 * 
 * Hydra_Software_Devel/62   1/30/13 11:33a nilesh
 * SW7425-4489: Added VBR support
 * 
 * Hydra_Software_Devel/61   1/18/13 4:51p nilesh
 * SW7425-4356: Fixed typo
 * 
 * Hydra_Software_Devel/60   1/15/13 12:50p nilesh
 * SW7425-4356: Added
 * BVCE_Channel_StartEncodeSettings.uiNumParallelNRTEncodes
 * 
 * Hydra_Software_Devel/59   12/18/12 4:23p nilesh
 * SW7425-4344: Removed XUDlib dependency on VCE
 * 
 * Hydra_Software_Devel/58   12/18/12 3:58p nilesh
 * SW7435-562: Added BVCE_Output_OpenSettings.hDescriptorMem
 * 
 * Hydra_Software_Devel/57   12/5/12 3:20p nilesh
 * SW7425-4230: Added BVCE_GetVersionInfo()
 * 
 * Hydra_Software_Devel/56   12/3/12 5:39p nilesh
 * SW7445-39: Added support for multiple slices
 * 
 * Hydra_Software_Devel/SW7445-39/1   11/16/12 10:54a nilesh
 * SW7445-39: Added support for multiple slices
 * 
 * Hydra_Software_Devel/55   11/6/12 4:08p nilesh
 * SW7425-4186: Added EOS event status
 * 
 * Hydra_Software_Devel/54   7/18/12 4:53p nilesh
 * SW7425-3156: Updated BVCE_Channel_ClearStatus behavior to be more
 * intuitive
 * 
 * Hydra_Software_Devel/53   6/6/12 3:03p nilesh
 * SW7425-3167: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-3167/1   6/5/12 11:50a nilesh
 * SW7425-3167: Removed bTrackInput and
 * bAllowGOPRestartOnSceneChangeDetect from BVCE_GOPStructure
 * 
 * Hydra_Software_Devel/52   5/1/12 1:01p nilesh
 * SW7425-1221: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1221/1   4/23/12 4:50p nilesh
 * SW7425-2244: Added 42-bit STC support
 * 
 * Hydra_Software_Devel/51   2/8/12 4:25p nilesh
 * SW7425-2220,SW7425-2349: Renamed
 * BVCE_Channel_StartEncodeSettings.bFastChannelChangeMode to
 * BVCE_Channel_StartEncodeSettings.bAdaptiveLowDelayMode
 * 
 * Hydra_Software_Devel/50   1/26/12 11:22a nilesh
 * SW7425-2265: Use worst case GOP length in StartSettings instead of
 * EncodeSettings for calculating A2PDelay
 * 
 * Hydra_Software_Devel/49   1/25/12 4:50p nilesh
 * SW7425-2220,SW7425-997: Change BVCE_Channel_StopEncode() prototype to
 * support different stop modes
 * 
 * Hydra_Software_Devel/48   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 * 
 * Hydra_Software_Devel/47   12/19/11 2:51p nilesh
 * SW7425-1995: Prevent BCHP_PWR_[Acquire/Release]Resource from being
 * called in ISR functions. Instead, require the encoder channel to be
 * started when calling ISR routines.
 * 
 * Hydra_Software_Devel/46   12/9/11 10:40a nilesh
 * SW7425-1566: Moved MultiChannelMode to
 * BVCE_Channel_OpenSettings.eMultiChannelMode
 * 
 * Hydra_Software_Devel/45   12/5/11 3:35p nilesh
 * SW7425-1848: Added BVCE_OpenSettings.bVerificationMode (Default is:
 * false)
 * 
 * Hydra_Software_Devel/44   12/5/11 2:14p nilesh
 * SW7425-960: Added VCE user data support
 * 
 * Hydra_Software_Devel/43   11/17/11 3:35p nilesh
 * SW7425-1746: Added BVCE_Channel_BeginNewRAP()
 * 
 * Hydra_Software_Devel/42   11/16/11 4:16p nilesh
 * SW7425-1566: Change BVCE_MultiChannelMode enum to match FW enum
 * 
 * Hydra_Software_Devel/41   11/16/11 4:10p nilesh
 * SW7425-1566: Added BVCE_MultiChannelMode support
 * 
 * Hydra_Software_Devel/40   11/11/11 4:59p nilesh
 * SW7425-1663: Updated error flags
 * 
 * Hydra_Software_Devel/39   10/24/11 4:48p nilesh
 * SW7425-1580: GetStatus - removed I picture count, added dropped counts
 * 
 * Hydra_Software_Devel/38   10/18/11 5:18p nilesh
 * SW7425-890: Added new error bits
 * 
 * Hydra_Software_Devel/37   9/22/11 3:55p pblanco
 * SW7425-1337: Added uiDescriptorDepth member to the
 * BVCE_Output_OpenSettings structure.
 * 
 * Hydra_Software_Devel/36   8/22/11 10:31p nilesh
 * SW7425-1006: VCE FW API v2.0.0.2
 * 
 * Hydra_Software_Devel/35   8/15/11 5:28p nilesh
 * SW7425-448: Added Active Power Management
 * (BVCE_Power_Standby/BVCE_Power_Resume)
 * 
 * Hydra_Software_Devel/34   8/11/11 11:29a nilesh
 * SW7425-1006: VCE FW API v2.0
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/2   8/10/11 5:29p nilesh
 * SW7425-1006: Merge VCE_FW_API_v1_2 branch to VICE2_FW_Dev branch
 * 
 * Hydra_Software_Devel/VICE2_FW_Dev/1   7/25/11 5:00p nilesh
 * SW7425-1006: Added support for unsupported resolution error bit
 * 
 * Hydra_Software_Devel/33   6/6/11 5:37p nilesh
 * SW7425-608: Added support for unsupported BVN frame rate error
 * 
 * Hydra_Software_Devel/32   4/19/11 3:16p nilesh
 * SW7425-171: Removed
 * BVCE_Channel_GetDefaultEncodeSettings_OnInputChange() since it is
 * redundant with BVCE_Channel_GetEncodeSettings_OnInputChange()
 * 
 * Hydra_Software_Devel/31   4/16/11 1:03a nilesh
 * SW7425-171: Added support for pending settings
 * 
 * Hydra_Software_Devel/30   4/11/11 5:20p nilesh
 * SW7425-171: Added BVCE_Channel_SetEncodeSettings_OnInputChange() and
 * BVCE_Channel_GetDefaultEncodeSettings_OnInputChange()
 * 
 * Hydra_Software_Devel/29   4/11/11 4:39p nilesh
 * SW7425-171: Separated events from errors in
 * BVCE_Channel_EventCallbackSettings and BVCE_Channel_Status
 * 
 * Hydra_Software_Devel/28   4/5/11 4:37p nilesh
 * SW7425-172,SW7425-173: Added device (watchdog) and channel (event)
 * callback suppport
 * 
 * Hydra_Software_Devel/27   3/29/11 4:09p nilesh
 * SW7425-74: Added H.264 NALU detection
 * 
 * Hydra_Software_Devel/26   3/2/11 6:32p nilesh
 * SW7425-141: Changed "EndToEndDelay" to "A2PDelay"
 * 
 * Hydra_Software_Devel/25   3/2/11 4:53p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * Hydra_Software_Devel/24   2/23/11 2:21p nilesh
 * SW7425-1: Clean up
 * 
 * Hydra_Software_Devel/23   2/22/11 5:44p nilesh
 * SW7425-102: Cleaned up end-to-end delay API
 * 
 * Hydra_Software_Devel/22   2/22/11 3:27p nilesh
 * SW7425-1: Phase 1.0 FW RC
 * 
 * Hydra_Software_Devel/21   2/15/11 4:20p nilesh
 * SW7425-1: Input Change Callback moved to be part of channel callback
 * settings instead of device callback settings
 * 
 * Hydra_Software_Devel/20   2/15/11 2:48p nilesh
 * SW7425-1: Changed BVCE_Output_GetBufferDescriptors() prototype to be
 * more accurate
 * 
 * Hydra_Software_Devel/19   2/15/11 1:31p nilesh
 * SW7425-96: Cleaned up heap logic to minimize usage of mapped system
 * heap
 * 
 * Hydra_Software_Devel/18   2/8/11 12:36p nilesh
 * SW7425-1: Changed how protocol, profile, and level are specified
 * 
 * Hydra_Software_Devel/17   2/7/11 10:56a nilesh
 * SW7425-1: Removed unused end-to-end delay setting
 * 
 * Hydra_Software_Devel/16   2/3/11 4:49p nilesh
 * SW7425-1: Changed uint32_t to unsigned wherever applicable
 * 
 * Hydra_Software_Devel/15   2/2/11 12:46p nilesh
 * SW7425-1: Changed BVCE_Channel_AddUserDataBuffers() prototype
 * 
 * Hydra_Software_Devel/14   1/26/11 4:45p nilesh
 * SW7425-1: API Changes
 * 
 * Hydra_Software_Devel/13   1/25/11 2:18p vanessah
 * 
 * SW7425-32:
 * 
 * Add MPEG-2 API support for ViCE2
 * 
 * Hydra_Software_Devel/12   12/27/10 6:11p nilesh
 * SW7425-1: Add API to allow selection of STC that encoder should use
 * 
 * Hydra_Software_Devel/11   12/1/10 3:42p nilesh
 * SW7425-1: Updated delay mode API.  Added logic that verifies BVN
 * metadata address at compile-time and run-time.
 * 
 * Hydra_Software_Devel/10   11/18/10 11:30a nilesh
 * SW7425-1: Commented out helper functions until API is finalized
 * 
 * Hydra_Software_Devel/9   11/17/10 4:41p nilesh
 * SW7425-1: Separated encode mode (static) and delay mode (dynamic)
 * settings
 * 
 * Hydra_Software_Devel/8   10/21/10 4:57p nilesh
 * SW7425-1: Fixed BVN Metadata Register Address Reporting
 * 
 * Hydra_Software_Devel/7   10/21/10 11:03a nilesh
 * SW7425-1: Support query of BVN2VICE metadata offset via channel status
 * 
 * Hydra_Software_Devel/6   10/20/10 4:57p nilesh
 * SW7425-1: Added uiLevel and eInputType start encode parameters
 * 
 * Hydra_Software_Devel/5   10/13/10 12:08p nilesh
 * SW7425-1: Fixed typos
 * 
 * Hydra_Software_Devel/4   10/12/10 1:42p nilesh
 * SW7425-1: Removed VCE output context struct definition.  Added data
 * ready callback definition.
 * 
 * Hydra_Software_Devel/3   10/7/10 11:08a nilesh
 * SW7425-1: Fixed typos
 * 
 * Hydra_Software_Devel/2   10/1/10 1:02p nilesh
 * SW7425-1: Updated user data API to use array instead of linked list
 * 
 * Hydra_Software_Devel/1   9/20/10 4:42p nilesh
 * SW7425-1:
 * Added file element "bvce.h".
 * SW7425-1: Merged vce to mainline
 * 
 * Hydra_Software_Devel/SW7425-1/4   9/20/10 2:18p nilesh
 * SW7425-1: Added BVN Mailbox DCCM address to status.  Added
 * delta(originaPTS,newPTS) to status.
 * 
 * Hydra_Software_Devel/SW7425-1/3   9/14/10 12:29p nilesh
 * SW7425-1: Added additional BVCE_Output APIs for muxer support.
 * Commented out suspend/resume and power APIs until usage is determined.
 * 
 * Hydra_Software_Devel/SW7425-1/2   8/18/10 1:42p nilesh
 * SW7425-1: Added ITB/CDB management APIs (BVCE_Output)
 * 
 * Hydra_Software_Devel/SW7425-1/1   7/14/10 4:29p nilesh
 * SW7425-1: Created VCE PI
 *
 ***************************************************************************/

/**** Module Overview ****
 *
 * The VCE (Video Coding Engine) API is a library used to control the ViCE2 encoder on integrated chipsets.
 * The VCP PIs responsibilities include:
 *    - Loading and booting the ViCE2 FW
 *    - Managing memory allocation
 *    - Configuring channels
 *    - Providing error handling hooks
 *       - Watchdog and Error interrupt
 *       - Debug logging
 *       - Status Info
 *
 * The VCE PI is not involved in any real-time aspect of the video encoding.  It's role is to setup
 * and configure the encoder, and then the encoder FW takes care of reading the source pictures.
 * After encoding starts, the VCE PI is involved in error handling (Watchdog and Error interrupts)
 * and reconfiguring the encoder based on the app's requirements.
 *
 * Each VICE2 hardware device is represented by a VCE Handle (BVCE_Handle).
 * Each VICE2 hardware device supports one or more encode channels represnted by a VCE Channel (BVCE_Channel_Handle).
 *
 * -- FW Boot/Loading --
 * The VCE PI supports loading and booting the FW for all the ARCs in the VICE hardware.  The VCE PI also supports
 * loading of secure authenticated firmware from both an internal or external source.
 *
 * -- Memory Management --
 * To adhere to magnum PI coding conventions, all memory required for any of the encode channels is allocated up-front
 * in the BVCE_Open() call.  Subsequent BVCE_Channel_Open() calls sub-allocate memory as needed.
 *
 * There are 4 distinct memory regions that can be specified by the app:
 *    1) firmware memory - this is where the the ARC FW code/data is loaded
 *    2) context memory - this is where any data structures (e.g. user data) are allocated
 *    3) picture memory - this is where the FW picture buffers are allocated.  The separate memory is
 *       needed to support non-UMA systems where picture buffers could be in a separate physical memory.
 *    4) secure memory - this is where any data that needs to be secure (bin and Output buffers)
 *       would be allocated.
 *
 *  It is important to note that the heaps for the firmware, context, picture, and secure memory could all be the same (e.g. in
 *  a non-secure UMA system).
 *
 * -- Channel Configuration --
 * The VCE PI will enforce validity of settings such as combinations that are allowed (e.g. protocol + profile) and also what is
 * allowed to be changed dynamically (e.g. frame rate, bit rate) vs what is not allowed to be changed dynamicaly (e.g. protocol).
 * The VCE PI will remember the latest encoder configuration state to facilitate a clean restore during a watchdog reset and/or
 * context switch.
 *
 * -- User Data --
 * The VCE PI allows the app to queue user data packets to be inserted into the stream.
 *
 * -- Debug --
 * The VCE PI provides status and debug log information for the encoder.  Any of the ARCs' UART output can be
 * read by the host.  Also, debug commands can be sent to any of the ARCs.  The app can also query status and
 * error information from the encoders at anytime.
 *
 * -- Power Management --
 *  *** TODO ***
 */

#ifndef BVCE_H_
#define BVCE_H_

/* basemodules includes */
#include "bmem.h"
#include "bint.h"
#include "btmr.h"

/* commonutils includes */
#include "bavc.h"
#include "bafl.h" /* ARC firmware ELF Loader */
#include "budp_vce.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/******************************/
/* VCE Device Level Functions */
/******************************/

typedef struct BVCE_P_Context *BVCE_Handle;

/* BVCE_MemoryConfig - The memory configuration is determined by the application up-front based on the expected usage
 * modes and corresponding FW memory requirements.  The FW will provide a memory table.  This memory
 * is allocated during BVCE_Open() and is sub-allocated as needed in BVCE_Channel_Open().  The worst case memory requirements
 * needs to be determined here.
 *
 * The memory is allocated from the heaps as follows:
 *    - General Heap
 *       - FW Context Memory
 *    - Picture Heap
 *       - Uncompressed picture buffers
 *    - Secure Heap
 *       - BIN buffer
 *       - CDB/ITB
 *
 * Note: VCE PI will coalesce memory sizes as needed depending on the actual heaps that are provided.  For maximum
 * application flexibility, it is recommended that the app always specify separate general, picture, and secure heap
 * sizes and then specify the heaps that are required.  E.g. if only a system heap is specified (default), then the
 * total memory allocated from the system heap would be (system + picture + secure).
 */
typedef struct BVCE_MemoryConfig
{
      size_t uiGeneralMemSize; /* The total amount of general heap memory that should be allocated */
      size_t uiPictureMemSize; /* The total amount of picture heap memory that should be allocated */
      size_t uiSecureMemSize;  /* The total amount of secure heap memory that should be allocated */
} BVCE_MemoryConfig;

/* BVCE_ArcInstance - enum allowing selection of a specific ARC in the ViCE encoder core
 * E.g. for secure boot, debug log processing, debug commands, etc. */
typedef enum BVCE_ArcInstance
{
      BVCE_ArcInstance_ePicArc, /* The Picture ARC (the master) */
      BVCE_ArcInstance_eMBArc,  /* The MacroBlock ARC  (the slave) */

      BVCE_ArcInstance_eMax
} BVCE_ArcInstance;

/* BVCE_Debug_BufferingMode - controls how the debug buffer is written by the FW */
typedef enum BVCE_Debug_BufferingMode
{
      BVCE_Debug_BufferingMode_eOverwriteOldData, /* FW continually writes to debug buffer
                                                   * and stops when debug buffer is being
                                                   * read.  This mode is useful for normal
                                                   * operation where the application does not
                                                   * need to periodically debug buffer but the
                                                   * latest debug buffer output is available to
                                                   * read to debug errors.  E.g. During a watchdog.
                                                   * the app can read the debug log to see if there
                                                   * is any information that would help diagnose the
                                                   * cause of the watchdog.
                                                   */
      BVCE_Debug_BufferingMode_eDiscardNewData, /* FW stops writing debug buffer when full.
                                                 * When full, the debug buffer output is dropped.
                                                 * This mode is useful for implementing a virtual
                                                 * UART when the physical UARTs are not present.
                                                 * The app needs to periodically read the debug log
                                                 * buffer to prevent it from getting full.
                                                 */

      BVCE_Debug_BufferingMode_eMax
} BVCE_Debug_BufferingMode;

typedef struct BVCE_OpenSettings
{
      uint32_t uiSignature;

      unsigned uiInstance; /* AVD 0 or AVD 1 */

      /********************/
      /* Memory and Heaps */
      /********************/

      /* VCE Heap Accessibility Requirements
       *
       *                        [Heaps]
       * [Requirements]         Secure  Firmware Picture General System
       * MIPS Accessible         -       Y        -       -       Y
       * VCE FW Accessible       -       Y        -       Y       Y
       * VCE HW Accessible       Y       Y        Y       Y       Y
       *
       */

      /* VCE Buffer Allocation Heap Priorities
       *
       *                [Heaps]
       * [Buffers]      Secure  Firmware Picture General System
       * Debug Log       -       -        -       -       1
       * Firmware Code   -       1        -       -       2
       * HW Scratch      -       -        -       1       2
       * Pictures        -       -        1       2       3
       * CDB             1       -        -       2       3
       * ITB             -       -        -       -       1
       *
       */

      BMEM_Handle hFirmwareMem[BVCE_ArcInstance_eMax]; /* If NULL, system memory handle (hMem) used.
                                                        * If non-NULL, heap should be created with "system bookkeeping"
                                                        * to ensure firmware is loaded at the expected location.
                                                        * One firmware memory heap exists for each ARC.  This is needed to
                                                        * support loading of ARC FW to specific memory regions (e.g. for
                                                        * authenticated firmware support).
                                                        */

      BMEM_Handle hGeneralMem;  /* If NULL, system memory handle (hMem) used.
                                 * If non-NULL, all picture memory allocated from this heap
                                 */

      BMEM_Handle hPictureMem;  /* If NULL, general memory handle (hMem) used.
                                 * If non-NULL, all picture memory allocated from this heap
                                 */

      BMEM_Handle hSecureMem;   /* If NULL, system memory handle (hMem) used.
                                 * If non-NULL, all secure memory allocated from this heap
                                 */

      BVCE_MemoryConfig stMemoryConfig; /* The total amount of memory that needs to be allocated for *all* channels
                                         * E.g. Let's say the application intends to open 2 simultaneous encode
                                         * channels: 1 for SD MPEG2 and 1 for HD VC1.
                                         * The app would need to look up the memory requirements for each configuration
                                         * and combine them.
                                         *    - BVCE_MemoryConfig.uiGeneralMemSize = GeneralMemSize(SD MPEG2) + GeneralMemSize(HD VC1)
                                         *    - BVCE_MemoryConfig.uiPictureMemSize = PictureMemSize(SD MPEG2) + PictureMemSize(HD VC1)
                                         *    - BVCE_MemoryConfig.uiSecureMemSize = SecureMemSize(SD MPEG2) + SecureMemSize(HD VC1)
                                         */

      /*********************/
      /* Firmware and Boot */
      /*********************/

      /* Interface to access firmware image. This interface must be
       * implemented and the function pointers must be stored here.
       */
      const BIMG_Interface *pImgInterface;

      /* Context for the Image Interface. This is also provided by
       * the implementation of the Image interface
       */
      void **pImgContext;

      /* Add Authenticated image support */

      /* Pointer to the ARC boot callback function. If non-NULL,
       * PI will call this function after the firmware has been
       * loaded into memory instead of booting the ARC. If this
       * function is NULL, then the PI will boot the ARC
       * normally. This function should return BERR_SUCCESS if
       * successful.
       */
      BERR_Code (*pARCBootCallback)(void* pPrivateData,
                                    const BAFL_BootInfo *pstBootInfo);

      /* Pointer to ARC boot callback private data that is passed
       * back into the callback.  Can be used to store any
       * information necessary for the application to boot the
       * core.
       */
      void *pARCBootCallbackData;

      /*********/
      /* Debug */
      /*********/
      BTMR_Handle hTimer; /* If non-NULL, may be used to gather performance data */

      /* Size of debug logging buffer */
      size_t uiDebugLogBufferSize[BVCE_ArcInstance_eMax];
      BVCE_Debug_BufferingMode eDebugLogBufferMode[BVCE_ArcInstance_eMax];

      bool bVerificationMode;
      bool bA2NPictureDrop;
} BVCE_OpenSettings;

void
BVCE_GetDefaultOpenSettings(
         BVCE_OpenSettings  *pstOpenSettings /* [out] Default VCE settings */
         );

BERR_Code
BVCE_Open(
         BVCE_Handle *phVce, /* [out] VCE Device handle returned */
         BCHP_Handle hChp,   /* [in] Chip handle */
         BREG_Handle hReg,   /* [in] Register handle */
         BMEM_Handle hMem,   /* [in] System Memory handle */
         BINT_Handle hInt,   /* [in] Interrupt handle */
         const BVCE_OpenSettings *pOpenSettings /* [in] VCE Device Open settings */
         );

BERR_Code
BVCE_Close(
         BVCE_Handle hVce
         );

BERR_Code
BVCE_GetTotalChannels(
         BVCE_Handle hVce,
         unsigned *puiTotalChannels
         );

typedef struct BVCE_VersionInfo
{
   unsigned uiFirmwareVersion;
   unsigned uiFirmwareApiVersion;
   unsigned uiBvn2ViceApiVersion;
} BVCE_VersionInfo;

BERR_Code
BVCE_GetVersionInfo(
   BVCE_Handle hVce,
   BVCE_VersionInfo *pstVersionInfo
   );

/* BVCE_Debug_ReadBuffer - reads the debug buffer log for the specified ARC into the
 * specified buffer.  If the debug buffer is configured for continuous mode, the
 * logging is temporarily disabled while the log is read out.
 */
BERR_Code
BVCE_Debug_ReadBuffer(
         BVCE_Handle hVce,
         BVCE_ArcInstance eARCInstance,
         char *szBuffer,   /* [in] pointer to buffer where log is copied to */
         size_t uiBufferSize,  /* [in] maximum number of bytes to copy to buffer */
         size_t *puiBytesRead  /* [out] number of bytes copied from debug log */
         );

/* BVCE_Debug_SendCommand - sends the debug command to the specified ARC */
BERR_Code
BVCE_Debug_SendCommand(
         BVCE_Handle hVce,
         BVCE_ArcInstance eARCInstance,
         char        *szCommand  /* [in] pointer to a double null terminated command string of debug uart commands */
         );

/* BVCE_ProcessWatchdog - reinitializes the encoder ARCs and restarts any channels
 * that were previously active.
 */
BERR_Code
BVCE_ProcessWatchdog(
         BVCE_Handle hVce
         );

/*********************************/
/* VCE Device Callback Functions */
/*********************************/

/* Device Callback - Watchdog
 *
 * Generated when the VICE2 watchdog timer has timed out.
 * The app can call BVCE_ProcessWatchdog to reinit and resume any previous encodes.
 * Alternatively, the app can call BCE_Close/BVCE_Open to do it's own error handling.
 */
typedef struct BVCE_WatchdogCallbackInfo
{
      unsigned uiUnused;
      /* TODO: Do we need any info here? */
} BVCE_WatchdogCallbackInfo;

typedef void (*BVCE_WatchdogCallbackHandler_isr)(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BVCE_WatchdogCallbackInfo *pInfo );

typedef struct BVCE_WatchdogCallbackSettings
{
      bool bEnable;

      BVCE_WatchdogCallbackHandler_isr fCallback;
      void *pPrivateContext;
      int32_t iPrivateParam;
} BVCE_WatchdogCallbackSettings;

typedef struct BVCE_CallbackSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_GetDefaultCallbackSettings() */

      BVCE_WatchdogCallbackSettings stWatchdog;
} BVCE_CallbackSettings;

void
BVCE_GetDefaultCallbackSettings(
         BVCE_CallbackSettings* pstCallbackSettings
         );

BERR_Code
BVCE_SetCallbackSettings(
         BVCE_Handle hVce,
         const BVCE_CallbackSettings* pstCallbackSettings
         );

BERR_Code
BVCE_GetCallbackSettings(
         BVCE_Handle hVce,
         BVCE_CallbackSettings* pstCallbackSettings
         );

/************************/
/* VCE Output Functions */
/************************/

typedef struct BVCE_P_OutputBuffers *BVCE_OutputBuffers_Handle;

typedef struct BVCE_Output_AllocBuffersSettings
{
      uint32_t uiSignature;

      BAVC_CdbItbConfig stConfig;  /* [in] Size and alignment for ITB and CDB */
      BMEM_Handle hITBMem; /* [optional] If null, uses hMem from VCE Handle. */
      BMEM_Handle hCDBMem; /* [optional] If null, uses hSecureMem (if non-null) from VCE Handle. */
} BVCE_Output_AllocBuffersSettings;

void
BVCE_Output_GetDefaultAllocBuffersSettings(
         BVCE_Output_AllocBuffersSettings *pstOutputAllocBuffersSettings
   );

/* BVCE_Output_AllocBuffer - Allocates ITB/CDB data (in *addition* to any memory allocated in BVCE_Open) */
BERR_Code
BVCE_Output_AllocBuffers(
         BVCE_Handle hVce,
         BVCE_OutputBuffers_Handle *phVceOutputBuffers,
         const BVCE_Output_AllocBuffersSettings *pstOutputAllocBuffersSettings /* [in] VCE Output Alloc Buffer settings */
         );

/* Should only be called when the buffer is not attached to a Output context */
BERR_Code
BVCE_Output_FreeBuffers(
         BVCE_OutputBuffers_Handle hVceOutputBuffers
         );

typedef struct BVCE_Output_OpenSettings
{
      uint32_t uiSignature;

      unsigned uiInstance;

      bool bEnableDataUnitDetection; /* If TRUE: CDB will be parsed to detect data units and video buffer descriptors
                                      * returned by BVCE_Output_GetBufferDescriptors will indicate the location and type
                                      * of data unit.  E.g. For H.264, the start code for each NALU will generate a new
                                      * video buffer descriptor with the BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START
                                      * flag set.
                                      *
                                      * Note: If TRUE, it is required that VCE PI has full access to the CDB (e.g. it is not in
                                      * secure memory and not encrypted)
                                      *
                                      * Data Unit detection is needed for muxing certain protocols/container (e.g. H.264 in MP4)
                                      */
      uint32_t uiDescriptorQueueDepth; /* Default: 3192 Value of 0 reserved for later use */

      BMEM_Handle hDescriptorMem;     /* [optional] Specifies heap used to allocate buffer and metadata descriptors.
                                       * If null, uses hMem from VCE Handle.
                                       */
} BVCE_Output_OpenSettings;

typedef struct BVCE_P_Output_Context *BVCE_Output_Handle;

void
BVCE_Output_GetDefaultOpenSettings(
         BVCE_Output_OpenSettings *pstOutputOpenSettings
   );

BERR_Code
BVCE_Output_Open(
         BVCE_Handle hVce,
         BVCE_Output_Handle *phVceOutput,
         const BVCE_Output_OpenSettings *pstOutputOpenSettings /* [in] VCE Output settings */
         );

BERR_Code
BVCE_Output_Close(
         BVCE_Output_Handle hVceOutput
         );

/* BVCE_Output_SetBuffers -
 * Sets the output HW to use the specified output buffers.  The output HW pointers are initialized to point
 * to the beginning of the output buffers.
 * Cannot be called while actively used by an encode
 * After a stop/suspend, use caution when calling by ensuring the consumer of the output is finished
 */
BERR_Code
BVCE_Output_SetBuffers(
         BVCE_Output_Handle hVceOutput,
         const BVCE_OutputBuffers_Handle hVceOutputBuffers
         );

BERR_Code
BVCE_Output_Reset(
         BVCE_Output_Handle hVceOutput
         );

/* BVCE_Output_GetRegisters -
 * Returns the registers associated with the specified output hardware
 */
BERR_Code
BVCE_Output_GetRegisters(
         BVCE_Output_Handle hVceOutput,
         BAVC_VideoContextMap *pstVceOutputRegisters
         );

/* BVCE_Output_GetBufferDescriptors -
 * Returns video buffer descriptors for CDB content in the
 * BAVC_VideoBufferDescriptor array(s)
 */
BERR_Code
BVCE_Output_GetBufferDescriptors(
   BVCE_Output_Handle hVceOutput,
   const BAVC_VideoBufferDescriptor **astDescriptors0, /* Pointer to an array of descriptors. E.g. *astDescriptorsX[0] is the first descriptor. *astDescriptorsX may be set to NULL iff uiNumDescriptorsX=0. */
   size_t *puiNumDescriptors0,
   const BAVC_VideoBufferDescriptor **astDescriptors1, /* Needed to handle FIFO wrap */
   size_t *puiNumDescriptors1
   );

/* BVCE_Output_ConsumeBufferDescriptors -
 * Reclaims the specified number of video buffer descriptors
 * The CDB read pointer is updated accordingly
 */
BERR_Code
BVCE_Output_ConsumeBufferDescriptors(
   BVCE_Output_Handle hVceOutput,
   size_t uiNumBufferDescriptors
   );

/* BVCE_Output_GetBufferStatus -
 * Returns the output buffer status (e.g. the base virtual address)
 */
BERR_Code
BVCE_Output_GetBufferStatus(
   BVCE_Output_Handle hVceOutput,
   BAVC_VideoBufferStatus *pBufferStatus
   );

/*******************************/
/* VCE Channel Level Functions */
/*******************************/

typedef struct BVCE_P_Channel_Context *BVCE_Channel_Handle;

typedef enum BVCE_MultiChannelMode
{
   BVCE_MultiChannelMode_eMulti, /* Multiple streams allowed at a time
                                  * Combined Resolution of active encodes CANNOT exceed Max Resolution
                                  * No Low Delay Support: BVCE_Channel_StartEncodeSettings.bPipelineLowDelayMode MUST BE false
                                  * Increased A2PDelay for ALL streams
                                  * DEPRECATED: Use eCustom and BVCE_Channel_OpenSettings.uiMaxNumChannels
                                  */

   BVCE_MultiChannelMode_eSingle, /* Only single stream allowed at a time
                                   * Low Delay Supported: BVCE_Channel_StartEncodeSettings.bPipelineLowDelayMode CAN be true
                                   * Max Resolution is supported
                                   * DEPRECATED: Use eCustom and BVCE_Channel_OpenSettings.uiMaxNumChannels
                                   */

   BVCE_MultiChannelMode_eMultiNRTOnly, /* Multiple NRT streams allowed at a time
                                         * NRT mode ONLY: BVCE_Channel_StartEncodeSettings.bNonRealTimeEncodeMode MUST BE true
                                         * Max Resolution is supported on EACH stream
                                         * DEPRECATED: Use eCustom and BVCE_Channel_OpenSettings.uiMaxNumChannels
                                         */

   BVCE_MultiChannelMode_eCustom, /* When set, the max number of simultaneous channels is specified in
                                   * BVCE_Channel_OpenSettings.uiMaxNumChannels */

   BVCE_MultiChannelMode_eMax
} BVCE_MultiChannelMode;

typedef struct BVCE_Channel_OpenSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultOpenSettings() */

      unsigned uiInstance; /* Channel instance */

      BMEM_Handle hChannelSystemMem; /* [optional] System memory Mem handle for this channel */
      BMEM_Handle hChannelGeneralMem; /* [optional] General memory Mem handle for this channel */
      BMEM_Handle hChannelPictureMem; /* [optional] Picture memory Mem handle for this channel */
      BMEM_Handle hChannelSecureMem;  /* [optional] Output/Secure memory Mem handle for this channel */

      BVCE_MemoryConfig stMemoryConfig; /* The total amount of memory that needs to be sub-allocated for this channel */

      BVCE_MultiChannelMode eMultiChannelMode;  /* Specifies the expected usage for number and type of
                                                 * simultaneous channels on this device. */
      unsigned uiMaxNumChannels; /* If eMultiChannelMode = eCustom, specifies the maximum number of simultaneous channels
                                  * that will be operating simultaneously on this device.  A value of 0 indicates the max
                                  * channels supports by the device as determined by FW. */
} BVCE_Channel_OpenSettings;

void
BVCE_Channel_GetDefaultOpenSettings(
         BVCE_Channel_OpenSettings *pstChSettings
         );

BERR_Code
BVCE_Channel_Open(
         BVCE_Handle hVce,
         BVCE_Channel_Handle *phVceCh,
         const BVCE_Channel_OpenSettings *pstChOpenSettings /* [in] VCE Channel settings */
         );

BERR_Code
BVCE_Channel_Close(
         BVCE_Channel_Handle hVceCh
         );

/* Channel Callback - Error
 *
 * Generated if the channel channel has an error to report.
 */
typedef struct BVCE_Channel_EventCallbackInfo
{
      unsigned uiUnused;
      /* TODO: Do we need any info here? */
} BVCE_Channel_EventCallbackInfo;

typedef void (*BVCE_Channel_EventCallbackHandler_isr)(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BVCE_Channel_EventCallbackInfo *pInfo );

typedef struct BVCE_Channel_EventCallbackSettings
{
      bool bEnable;

      BVCE_Channel_EventCallbackHandler_isr fCallback;
      void *pPrivateContext;
      int32_t iPrivateParam;

      uint32_t uiErrorMask; /* 0 = all errors allowed.  See BVCE_CHANNEL_STATUS_FLAGS_ERROR_XXX */
      uint32_t uiEventMask; /* 0 = all events allowed (i.e. no events are masked).  See BVCE_CHANNEL_STATUS_FLAGS_EVENT_XXX  */
} BVCE_Channel_EventCallbackSettings;

typedef struct BVCE_Channel_CallbackSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultCallbackSettings() */

      BVCE_Channel_EventCallbackSettings stEvent;
} BVCE_Channel_CallbackSettings;

void
BVCE_Channel_GetDefaultCallbackSettings(
         BVCE_Channel_CallbackSettings* pstCallbackSettings
         );

BERR_Code
BVCE_Channel_SetCallbackSettings(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_CallbackSettings* pstCallbackSettings
         );

BERR_Code
BVCE_Channel_GetCallbackSettings(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_CallbackSettings* pstCallbackSettings
         );

/* GOP Parameters */
/* Note: GOPLength = 1 + uiNumberOfPFrames + uiNumberOfPFrames*uiNumberOfBFrames */
typedef struct BVCE_GOPStructure
{
      /* The following are only relevant if bTrackInput = false */
      unsigned uiNumberOfPFrames; /* number of P frames between I frames. 0xFFFFFFFF indicated IP infinite mode */

      /* The following are only relevant if uiNumberOfPFrames != 0xFFFFFFFF */
      unsigned uiNumberOfBFrames; /* number of B frames between I or P frames */

      /* The following are only relevant if uiNumberOfBFrames != 0 */
      bool bAllowOpenGOP;
} BVCE_GOPStructure;

typedef struct
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultStartEncodeSettings() */

      /* Encode Mode */
      bool bNonRealTimeEncodeMode;  /* If FALSE: Normal operation for encoding/transcoding a
                                     * video source in real time.
                                     *
                                     * If TRUE: aka As Fast As Possible (AFAP) Mode.
                                     * If VCE is not ready for a new picture
                                     * (e.g. encoder is busy, output CDB/ITB is full, etc),
                                     * it will put back pressure on BVN to prevent loss of
                                     * video frames */

      bool bPipelineLowDelayMode;   /* If FALSE: Output is written to CDB on frame boundaries.
                                      * The data for the first frame will not be written
                                     * to CDB until the entire frame has been encoded.
                                     *
                                     * NOTE: bNonRealTimeEncodeMode must be FALSE if this is TRUE
                                     *
                                     * If TRUE: Output is written to CDB on macroblock boundaries.
                                        * The data for the first frame will be written as soon
                                        * as the first macroblock has been encoded. When this
                                        * mode is enabled, the transmission of the encoded
                                        * frame can begin before the entire frame has finished
                                        * encoding.                                   *
                                        *
                                        * This mode is useful in cases where minimal latency is
                                        * needed (e.g. video conferencing, gaming, etc.).
                                        *
                                        * NOTE: I,P pictures only. B pictures are not allowed
                                        */

      bool bAdaptiveLowDelayMode;    /* If TRUE: Encoder will drop all incoming frames until the first
                                      * decoded frame from the new channel is seen.  The first frame will be
                                      * encoded with a low A2PDelay.  A2PDelay will automatically ramp to the
                                      * desired A2PDelay.
                                      */

      /* Protocol/Profile/Level */
      BAVC_VideoBufferInfo stProtocolInfo;

      BAVC_ScanType eInputType;
      /* TODO: Add resolution here? <-- I don't think we need this...since this is sent by BVN.
       * FW wants us to send expected resolution to them, but FW may be able to allocate on-the-fly
       * based on resolution sent in BVN metadata. */

      BVCE_Output_Handle hOutputHandle; /* Output Context Handle - HW + ITB/CDB to use for this encode */
      unsigned uiStcIndex; /* Which STC broadcast the VCE encoder should use */

      /* Encoder Bounds - If the bounds are known and specified, encoder latency can be improved.
       * These bounds are for a single encode session.  E.g. if the output frame rate is known to to be
       * fixed at 30fps, then the output frame rate min/max can be set to 30.  */
      struct
      {
         /* Output Frame Rate -
          *    BVCE_Channel_EncodeSettings.stFrameRate.eFrameRate must be within eMin and eMax during encode */
         struct
         {
            BAVC_FrameRateCode eMin;
            BAVC_FrameRateCode eMax;
         } stFrameRate;

         /* Input Frame Rate -
          *    The input frame rate (from bvn) cannot go lower than eMin during the encode */
         struct
         {
            BAVC_FrameRateCode eMin;
         } stInputFrameRate;

         /* Picture Dimension -
          *    The input picture dimensions (from bvn) cannot go above stMax */
         struct
         {
            /* Size (in pixels) */
            struct
            {
               unsigned uiWidth;
               unsigned uiHeight;
            } stMax;
         } stDimensions;

         /* GOP Structure -
          *    The worst case GOP structure.  Delay increases with number of B frames */
         BVCE_GOPStructure stGOPStructure;
      } stBounds;

      unsigned uiRateBufferDelay; /* in ms.  Higher values indicate better quality but more latency.  0 indicates use encoder's defaults */

      unsigned uiNumParallelNRTEncodes;  /* uiNumParallelNRTEncodes indicates how many
                                            parallel encodes are expected.  0 indicates VCE
                                            defaults.  1 means normal NRT. 2 or
                                            more indicates FNRT. */
} BVCE_Channel_StartEncodeSettings;


void
BVCE_Channel_GetDefaultStartEncodeSettings(
         BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings
         );

/* BVCE_Channel_StartEncode - Configures the encoder and starts the encode process.
 *
 * Note: BVCE_Channel_SetEncodeSettings() should be called before this to set up the initial encode parameters
 */
BERR_Code
BVCE_Channel_StartEncode(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings
         );

/* BVCE_Channel_StopEncode - Stops the encode process.
 */
typedef enum BVCE_Channel_StopMode
{
   BVCE_Channel_StopMode_eNormal,    /* Default: Encoder will stop the encode cleanly.
                                      * Existing pictures will be finished.
                                      * An EOS will be appended to the ES stream.
                                      * The app MUST continue running the muxer thread
                                      * until the EOS before stopping the thread.
                                      */
   BVCE_Channel_StopMode_eImmediate, /* Encoder will stop as fast as possible.
                                      * Current pictures in flight may be dropped.
                                      * An EOS will be appended to the ES stream.
                                      *
                                      * The app MUST continue running the muxer thread
                                      * until the EOS before stopping the thread.
                                      */

   BVCE_Channel_StopMode_eAbort,     /* Encoder will stop as fast as possible.
                                      * Current pictures in flight may be dropped.
                                      * An EOS will NOT be appended to the ES stream.
                                      *
                                      * The app MUST END the the muxer thread
                                      * PRIOR to calling with the eAbort flag
                                      */

   BVCE_Channel_StopMode_eMax
} BVCE_Channel_StopMode;

typedef struct BVCE_Channel_StopEncodeSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultStopEncodeSettings() */

      BVCE_Channel_StopMode eStopMode;
} BVCE_Channel_StopEncodeSettings;

void
BVCE_Channel_GetDefaultStopEncodeSettings(
         BVCE_Channel_StopEncodeSettings *pstChStopEncodeSettings
         );

BERR_Code
BVCE_Channel_StopEncode(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_StopEncodeSettings *pstChStopEncodeSettings
         );

/* BVCE_Channel_FlushEncode - consumes all pending encoder buffer descriptors
 * If the channel has been stopped, then the flush will occur continuously
 * until the EOS is seen
 * */
BERR_Code
BVCE_Channel_FlushEncode(
         BVCE_Channel_Handle hVceCh
         );

/* Channel Settings */

typedef struct BVCE_BitRate
{
      /* Bit Rate (in bits/sec) where 1Mbps = 1000000 */
      unsigned uiMax;
      unsigned uiTarget; /* If "0", means CBR, and uiTarget=uiMax.  If non-zero, then VBR. */
} BVCE_BitRate;

typedef struct BVCE_FrameRate
{
      BAVC_FrameRateCode eFrameRate;

      bool bVariableFrameRateMode; /* See http://twiki-01.broadcom.com/bin/view/Arch/TranscodingBvnIntegration#User_Version */
} BVCE_FrameRate;

typedef struct BVCE_Channel_EncodeSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultEncodeSettings() */

      /* Frame Rate (in frames/second) */
      BVCE_FrameRate stFrameRate;

      /* Rate Control Parameters */
      BVCE_BitRate stBitRate;

      unsigned uiA2PDelay; /* In 27Mhz clock ticks.  Desired "Arrival-to-Presentation" delay.  0 indicates use encoder's default. */

      /* GOP Parameters */
      BVCE_GOPStructure stGOPStructure;

      /* ITFP Mode */
      bool bITFPEnable;

      /* Number of Slices Per Picture */
      unsigned uiNumSlicesPerPic; /* 0 indicates use encoder's default. 16 is max. */
} BVCE_Channel_EncodeSettings;

void
BVCE_Channel_GetDefaultEncodeSettings(
         BVCE_Channel_EncodeSettings *pstChEncodeSettings
         );

BERR_Code
BVCE_Channel_SetEncodeSettings(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_EncodeSettings *pstChEncodeSettings
         );

BERR_Code
BVCE_Channel_GetEncodeSettings(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_EncodeSettings *pstChEncodeSettings
         );

/* BVCE_Channel_BeginNewRAP() - Instructs VCE to create a Random Access Point (I/IDR frame) for the next picture received */
BERR_Code
BVCE_Channel_BeginNewRAP(
         BVCE_Channel_Handle hVceCh
         );

typedef struct BVCE_Channel_EncodeSettings_OnInputChange
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BVCE_Channel_GetDefaultEncodeSettings_OnInputChange() */

      BVCE_BitRate stBitRate;
} BVCE_Channel_EncodeSettings_OnInputChange;

/* BVCE_Channel_SetEncodeSettings_OnInputChange -
 * Sets the encode parameters to be used in sync with change to
 * the input to the encoder (e.g. resolution change)
 */
BERR_Code
BVCE_Channel_SetEncodeSettings_OnInputChange(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_EncodeSettings_OnInputChange *pstChEncodeSettings
         );

/* BVCE_Channel_GetEncodeSettings_OnInputChange -
 * Returns the pending settings (if any) otherwise the current settings
 */
BERR_Code
BVCE_Channel_GetEncodeSettings_OnInputChange(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_EncodeSettings_OnInputChange *pstChEncodeSettings
         );

#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_INVALID_INPUT_DIMENSION          0x00000001
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_LATE                   0x00000002
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_USER_DATA_DUPLICATE              0x00000004
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_ADJUSTS_WRONG_FRAME_RATE         0x00000008
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_BVN_FRAME_RATE       0x00000010
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_UNSUPPORTED_RESOLUTION           0x00000020
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_MIN_FRAME_RATE      0x00000040
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_MISMATCH_BVN_PIC_RESOLUTION      0x00000080
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_MAX_BITRATE_EXCEEDED             0x00000100
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_BIN_BUFFER_FULL                  0x00000200
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_CDB_FULL                         0x00000400
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_PICARC_TO_CABAC_DINO_BUFFER_FULL 0x00000800
#define BVCE_CHANNEL_STATUS_FLAGS_ERROR_EBM_FULL                         0x00001000


#define BVCE_CHANNEL_STATUS_FLAGS_EVENT_INPUT_CHANGE                    0x00000001
#define BVCE_CHANNEL_STATUS_FLAGS_EVENT_EOS                             0x00000002

/* Channel Information */
typedef struct BVCE_Channel_Status
{
      uint32_t uiErrorFlags; /* See BVCE_CHANNEL_STATUS_FLAGS_ERROR_XXX */
      uint32_t uiEventFlags; /* See BVCE_CHANNEL_STATUS_FLAGS_EVENT_XXX */
      unsigned uiTotalErrors;
      unsigned uiTotalPicturesReceived;
      unsigned uiTotalPicturesDroppedDueToFrameRateConversion;
      unsigned uiTotalPicturesDroppedDueToErrors;
      unsigned uiTotalPicturesEncoded;
      uint32_t uiLastPictureIdEncoded;
      /* TBD */

      uint32_t uiBVNMetaDataRegisterStartAddress; /* *DEPRECATED* (Read-Only) Register Start Address of BVN metadata */
      uint64_t uiSTCSnapshot; /* Read-Only */
      unsigned uiTotalPicturesDroppedDueToHRDUnderflow;
      unsigned uiEtsDtsOffset; /* The ETS to DTS offset for the encode session as determined by RC (in 27Mhz units)*/
} BVCE_Channel_Status;

/* BVCE_Channel_GetStatus - queries the FW for current status.  PI increments counts and ORs error flags bits as needed. */
BERR_Code
BVCE_Channel_GetStatus(
         BVCE_Channel_Handle hVceCh,
         BVCE_Channel_Status *pChannelStatus
         );

/* BVCE_Channel_ClearStatus - clears the specified fields of the status block
 * If pChannelStatus = NULL, then all status is cleared
 *
 * Set the bits in the flags (uiErrorFlags and uiEventFlags) for each bit to be cleared
 * Set the counters to be cleared to a non-zero value. All others should be zero.
 *
 * E.g. if you want to reset all error bits and uiTotalErrors, this is the sequence,
 *       BVCE_Channel_Status status;
 *       BKNI_Memset(&status, 0, sizeof(BVCE_Channel_Status));
 *       status.uiErrorFlags = 0xFFFFFFFF;
 *       status.uiTotalErrors = 1;
 *       BVCE_Channel_ClearStatus(hVceCh, &status);
 *
 * Note: Only state relative from a previous call to BVCE_Channel_GetStatus is cleared.
 *       E.g. 1) Call BVCE_Channel_GetStatus()
 *            2) Wait for 4 pictures to be transcoded
 *            3) Call BVCE_ChannelClearStatus(hVceCh, NULL) (to clear all state)
 *            4) Then immediately call BVCE_Channel_GetStatus(), uiTotalPicturesReceived will be >=4 (NOT 0).
 */
BERR_Code
BVCE_Channel_ClearStatus(
         BVCE_Channel_Handle hVceCh,
         const BVCE_Channel_Status *pChannelStatus
         );

/*************/
/* User Data */
/*************/
/* BVCE_Channel_UserData_AddBuffers - adds user data field info structs(s) to user data queue for stream insertion */
BERR_Code
BVCE_Channel_UserData_AddBuffers_isr(
         BVCE_Channel_Handle hVceCh,
         const BUDP_Encoder_FieldInfo *pstUserDataFieldInfo, /* Pointer to first field info descriptor */
         size_t uiCount, /* Count of user data field buffer info structs */
         size_t *puiQueuedCount /* Count of user data field info structs queued by encoder (*puiQueuedCount <= uiCount) */
         );

/* BVCE_Channel_UserData_GetStatus_isr -
* Returns the user data status
*
*/
BERR_Code
BVCE_Channel_UserData_GetStatus_isr(
      BVCE_Channel_Handle hVceCh,
      BAVC_VideoUserDataStatus *pstUserDataStatus
);

/********************/
/* Helper Functions */
/********************/
typedef struct BVCE_A2PDelay
{
      unsigned uiMin; /* In 27Mhz clock ticks.  The minimum A2P delay given the specified settings */
      unsigned uiMax; /* In 27Mhz clock ticks.  The maximum supported A2P delay. */
} BVCE_A2PDelay;

BERR_Code
BVCE_GetA2PDelayInfo(
         const BVCE_Channel_EncodeSettings *pstChEncodeSettings, /* uiEndToEndDelay parameter is not used by this function */
         const BVCE_Channel_StartEncodeSettings *pstChStartEncodeSettings,
         BVCE_A2PDelay *pstA2PDelay
         );

/* Power Management */

/* BVCE_Power_Standby -
 * All channels must be explicitly stopped before VCE can be put into standby.
 * Channels DO NOT need to be closed.
 * All VCE device/channel handles remain valid after resume
 */
BERR_Code
BVCE_Power_Standby(
         BVCE_Handle hVce
         );

/* BVCE_Power_Resume -
 * Channels that were opened when standby was called are re-opened.
 * All VCE device/channel handles remain valid after resume
 */
BERR_Code
BVCE_Power_Resume(
         BVCE_Handle hVce
         );

#ifdef __cplusplus
}
#endif

#endif /* BVCE_H_ */



