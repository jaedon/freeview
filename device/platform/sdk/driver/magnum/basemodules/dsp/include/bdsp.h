/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 2/6/13 6:22p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp.h $
 * 
 * Hydra_Software_Devel/12   2/6/13 6:22p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/11   2/4/13 12:53p nihar
 * SW7425-4229: Adding support for BDSP Version - Merging changes to
 * Hydra_Software_Devel Branch
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/2   1/30/13 11:49a nihar
 * SW7425-4229: Adding support for BDSP Version - Modification after Jon's
 * feedback.
 * 
 * Hydra_Software_Devel/BDSP_VERSION_BRANCH/1   1/29/13 5:02p nihar
 * SW7425-4229: Adding support for BDSP Version
 * 
 * Hydra_Software_Devel/10   11/26/12 1:12p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * $brcm_Log: /magnum/basemodules/dsp/include/bdsp.h $
 * 
 * Hydra_Software_Devel/12   2/6/13 6:22p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/3   1/28/13 10:52a jgarrett
 * SW7425-4216: Merge with 7p5 release
 * 
 * Hydra_Software_Devel/SW7425-4216/2   1/8/13 4:57p gprasad
 * SW7425-4216: Merging mainline changes to rbuf capture branch as of
 * 01/08/2012
 * 
 * Hydra_Software_Devel/SW7425-4216/1   12/4/12 5:05p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/9   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/8   1/27/12 12:59p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/7   7/4/11 6:35p gprasad
 * SW7422-373: Add GetInfo routines for AudioType, AudioProcessing, and
 * AudioEncode in BDSP
 * 
 * Hydra_Software_Devel/6   6/17/11 6:09p gautamk
 * SW7422-373: [7422] Fixing compilation
 * 
 * Hydra_Software_Devel/5   6/17/11 5:56p gautamk
 * SW7422-373: [7422] Adding comment for APIS
 * 
 * Hydra_Software_Devel/4   6/17/11 3:50p gautamk
 * SW7422-373: [7422] Adding GetInfo routines for AudioType,
 * AudioProcessing, and AudioEncode support in BDSP
 * 
 * Hydra_Software_Devel/3   6/15/11 5:47p gautamk
 * SW7425-683: [7425] Adding support for Power management  in BDSP
 * 
 * Hydra_Software_Devel/2   6/8/11 4:49p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/1   4/6/11 2:15p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/5   1/19/11 7:22a gautamk
 * SW7422-191:Adding API for Default settings.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:25a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/13/11 5:56a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/14/10 2:16p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BDSP_H_
#define BDSP_H_
#include "bdsp_types.h"
#include "bdsp_context.h"
#include "bdsp_task.h"
#include "bdsp_audio_task.h"
#include "bdsp_video_task.h"
#include "bdsp_video_encode_task.h"
#include "bdsp_scm_task.h"
/*------------------------- ERROR CODES---------------------------------------*/
#define BDSP_ERR_DEVICE_UNINTIALIZED	    BERR_MAKE_CODE(BERR_DSP_ID, 1)
#define BDSP_ERR_BAD_DEVICE_STATE	        BERR_MAKE_CODE(BERR_DSP_ID, 2)
#define BDSP_ERR_RESOURCE_EXHAUSTED			BERR_MAKE_CODE(BERR_DSP_ID, 3)
#define BDSP_ERR_CHANNEL_ALREADY_OPENED		BERR_MAKE_CODE(BERR_DSP_ID, 4)
#define BDSP_ERR_CHANNEL_ALREADY_STARTED    BERR_MAKE_CODE(BERR_DSP_ID, 5)
#define BDSP_ERR_BUFFER_FULL	            BERR_MAKE_CODE(BERR_DSP_ID, 6)
#define BDSP_ERR_BUFFER_EMPTY		        BERR_MAKE_CODE(BERR_DSP_ID, 7)
#define BDSP_ERR_BUFFER_INVALID             BERR_MAKE_CODE(BERR_DSP_ID, 8)
#define BDSP_ERR_INVALID_TASK		        BERR_MAKE_CODE(BERR_DSP_ID, 9)
#define BDSP_ERR_DOWNLOAD_FAILED		        BERR_MAKE_CODE(BERR_DSP_ID, 10)

/*=************************ Module Overview ******************************** 
BDSP is a magnum basemodule interface to a DSP.  The DSP interface can be 
used by one or more porting interface (PI) modules in order to access 
services available on the DSP. 
 
In order to use a DSP device, you must open the DSP device handle that 
you wish to use.  For example, on a Raaga-based system you need to call 
BDSP_Raaga_GetDefaultSettings() and BDSP_Raaga_Open().   
***************************************************************************/

/***************************************************************************
Summary:
Close a DSP device
***************************************************************************/
void BDSP_Close(
    BDSP_Handle handle      /* Handle to be closed */
    );

/***************************************************************************
Summary:
BDSP Version Settings
***************************************************************************/
typedef struct BDSP_Version
{
    uint32_t majorVersion;                       /* Major Version of BDSP Release   */
    uint32_t minorVersion;                       /* Minor Version of BDSP Release   */
    uint32_t branchVersion;                      /* Branch Version of BDSP Release  */
    uint32_t branchSubVersion;                   /* Branch SubVersion of BDSP Release */
} BDSP_Version;


/***************************************************************************
Summary:
DSP Status
***************************************************************************/
typedef struct BDSP_Status
{
    unsigned numDsp;                    /* Number of DSPs available, typically one on DTV/STB systems */
    unsigned numWatchdogEvents;         /* Number of watchdog events since the DSP handle was opened */
    BDSP_Version firmwareVersion;       /* Version information */
}BDSP_Status;

/***************************************************************************
Summary:
Standby settings
***************************************************************************/
typedef struct BDSP_StandbySettings
{
    bool dummy; /* placeholder to avoid compiler warning */
}BDSP_StandbySettings;

/***************************************************************************
Summary:
Get DSP Status
***************************************************************************/
void BDSP_GetStatus(
    BDSP_Handle handle,
    BDSP_Status *pStatus             /* [out] Current Status */
    );

/***************************************************************************
Summary:
Returns single null terminated line from the debug log
***************************************************************************/
BERR_Code BDSP_GetDebugLog(
    BDSP_Handle handle,
    char *pBuffer,
    size_t bufferLength
    );

/***************************************************************************
Summary:
	Enter standby mode with  DSP.

Description:
	This function puts the  DSP into standby mode, 
	if supported. All DSP tasks must be in a stopped state 
	in order to successfully enter standby mode.
	If standby mode is not supported, calling this function has no effect.

	When in standby mode, the device clocks are turned off,
	resulting in a minimal power state.

	No BDSP_* calls should be made until standby mode exitted by calling
	BDSP_Resume().

Returns:
	BERR_SUCCESS - If standby is successful, otherwise error

See Also:
	BDSP_Resume
***************************************************************************/
BERR_Code BDSP_Standby(
	BDSP_Handle 			handle, 		/* [in] DSP device handle */
	BDSP_StandbySettings 	*pSettings 	/* [in] standby settings */
);

/***************************************************************************
Summary:
	Exit standby mode with the  DSP

Description:
	This function exits the DSP from standby mode. 
	After exitting standby mode, upper-level SW is free to call
	BDSP_* functions.

Returns:
	BERR_SUCCESS - If resume is successful, otherwise error

See Also:
	BDSP_Standby
***************************************************************************/

BERR_Code BDSP_Resume(
	BDSP_Handle 			handle 		/* [in] DSP device handle */
    );

/***************************************************************************
Summary:
	Audio type info
***************************************************************************/
typedef struct BDSP_AudioTypeInfo
{
        bool supported;    /* true if this audio type is supported, false otherwise */
        bool supportsLsf; /* If true, SRC should be added to support low/quarter sampling rate content */
        const char *pName;
        bool outputTypeValid[BDSP_AF_P_DistinctOpType_eMax];    
} BDSP_AudioTypeInfo;

/***************************************************************************
Summary:
	Audio Processing info
***************************************************************************/
typedef struct BDSP_AudioProcessingInfo
{
        bool supported;    /* true if this audio post-processing type is supported, false otherwise */
        const char *pName;
        bool inputTypeValid[BDSP_AF_P_DistinctOpType_eMax];    
        bool outputTypeValid[BDSP_AF_P_DistinctOpType_eMax];    
        BDSP_AF_P_OutModeType outputMode;
        /* TODO: Sample Rate mapping */
} BDSP_AudioProcessingInfo;

/***************************************************************************
Summary:
	Audio Encode info
***************************************************************************/
typedef struct BDSP_AudioEncodeInfo
{
        bool supported;    /* true if this audio encode type is supported, false otherwise */
        const char *pName;
        bool inputTypeValid[BDSP_AF_P_DistinctOpType_eMax];    
        /* TODO: Sample Rate mapping */
} BDSP_AudioEncodeInfo;

/***************************************************************************
Summary:
	Description of an external interrupt to the DSP 
***************************************************************************/
typedef struct BDSP_ExternalInterruptInfo
{
	/* BCHP address. CHIP Offset to be added to get full 32 bit address */
    uint32_t address;
	/* Bit number starts with 0 as LSB and goes upwards */
    uint32_t bitNum;
} BDSP_ExternalInterruptInfo;

/***************************************************************************
Summary:
	Description of RDB registers from DSP page
***************************************************************************/
typedef struct BDSP_RdbRegisterInfo
{
	/* BCHP address. CHIP Offset to be added to get full 32 bit address */
    uint32_t addressOfBaseRdb;
} BDSP_RdbRegisterInfo;

/***************************************************************************
Summary:
	Get Audio Type info
***************************************************************************/
BERR_Code BDSP_GetAudioTypeInfo(
BDSP_Handle handle,
BDSP_AudioType type, /* [in] */
const BDSP_AudioTypeInfo **pInfo /* [out] */
);

/***************************************************************************
Summary:
	Get Audio Processing info
***************************************************************************/
BERR_Code BDSP_GetAudioProcessingInfo(
BDSP_Handle handle,
BDSP_AudioProcessing type, /* [in] */
const BDSP_AudioProcessingInfo **pInfo /* [out] */
);

/***************************************************************************
Summary:
	Get Audio Encode info
***************************************************************************/
BERR_Code BDSP_GetAudioEncodeInfo(
BDSP_Handle handle,
BDSP_AudioEncode type, /* [in] */
const BDSP_AudioEncodeInfo **pInfo /* [out] */
);

/***************************************************************************
Summary:
	Allocate an external interrupt.

Description:
	This function allocates one interrupt handle that can be used to interrupt DSP.

Returns:
	BERR_SUCCESS - If allocation is successful, otherwise error

See Also:
	BDSP_FreeExternalInterrupt
	BDSP_GetExternalInterruptInfo
***************************************************************************/
BERR_Code BDSP_AllocateExternalInterrupt(
    BDSP_Handle hDsp,
    uint32_t	dspIndex,
    BDSP_ExternalInterruptHandle *pInterruptHandle /* [out] */
    );

/***************************************************************************
Summary:
	Frees an allocated external interrupt.

Description:
	This function frees one interrupt handle that was already allocated.

Returns:
	BERR_SUCCESS if sucessful else error

See Also:
	BDSP_AllocateExternalInterrupt
	BDSP_GetExternalInterruptInfo
***************************************************************************/
BERR_Code BDSP_FreeExternalInterrupt(
	BDSP_ExternalInterruptHandle	hInterrupt
	);

/***************************************************************************
Summary:
	Retrieve external interrupt information.

Description:
	This function provides the interrupt register and the bit that can be used to interrupt DSP.

Returns:
	BERR_SUCCESS - If successful, otherwise error

See Also:
	BDSP_FreeExternalInterrupt
	BDSP_AllocateExternalInterrupt
***************************************************************************/
BERR_Code BDSP_GetExternalInterruptInfo(
    BDSP_ExternalInterruptHandle hInterrupt,
    BDSP_ExternalInterruptInfo **pInfo /* [out] */
    );

/***************************************************************************
Summary:
	Allocate RDB registers from dsp page for upper SW layer

Description:
	This function allocates RDB registers handle that can be used to send msg etc to DSP.

Returns:
	BERR_SUCCESS - If allocation is successful, otherwise error

See Also:
	BDSP_FreeRdbRegisters
	BDSP_GetRdbRegistersInfo
***************************************************************************/

BERR_Code BDSP_AllocateRdbRegisters(
    BDSP_Handle hDsp,
    uint32_t	dspIndex,
    uint32_t	numRdbToAllocate,
    BDSP_RdbRegisterHandle *pRdbRegisterHandle /* [out] */
    );

/***************************************************************************
Summary:
	Frees an allocated Rdb Registers handle.

Description:
	This function frees an Rdb Registers handle that was already allocated.

Returns:
	BERR_SUCCESS if sucessful else error

See Also:
	BDSP_AllocateRdbRegisters
	BDSP_GetRdbRegistersInfo
***************************************************************************/
BERR_Code BDSP_FreeRdbRegisters(
			BDSP_RdbRegisterHandle	hRdbRegister
			);

/***************************************************************************
Summary:
	Retrieve Rdb Registers information.

Description:
	This function provides the address (offset) of base RDB register.

Returns:
	BERR_SUCCESS - If successful, otherwise error

See Also:
	BDSP_FreeRdbRegisters
	BDSP_AllocateRdbRegisters
***************************************************************************/
BERR_Code BDSP_GetRdbRegistersInfo(
    BDSP_RdbRegisterHandle hRdbRegister,
    BDSP_RdbRegisterInfo **pInfo /* [out] */
    );

/***************************************************************************
Summary:
    Process audio capture data

Description:
    This function must be called in a background thread at a frequent 
    interval (e.g. 10ms). for the data to be copied into the capture buffers.

Returns:
    BERR_SUCCESS - If audio capture is successful

See Also:
    BDSP_AudioCapture_GetBuffer
    BDSP_AudioCapture_ConsumeData
***************************************************************************/
BERR_Code BDSP_ProcessAudioCapture(
    BDSP_Handle hDsp
    );

#endif

