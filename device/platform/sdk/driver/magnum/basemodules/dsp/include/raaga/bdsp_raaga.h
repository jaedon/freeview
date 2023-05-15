/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 2/6/13 6:14p $
 *
 * Module Description: Raaga DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/raaga/bdsp_raaga.h $
 * 
 * Hydra_Software_Devel/11   2/6/13 6:14p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/3   12/4/12 5:06p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:25p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/SW7425-4216/1   11/12/12 10:40p gprasad
 * SW7425-4216: Initial changes for ring buffer capture - verified to be
 * working for single decode with raaga test
 * 
 * Hydra_Software_Devel/10   8/20/12 7:26p ananthan
 * FWRAA-480: Annotate BDSP_Raaga_Open describing the open close sequence.
 * Addding appropriate comments
 * 
 * Hydra_Software_Devel/9   12/5/11 5:41p rshijith
 * FWRAA-365 : Adding Target print support to raaga subsystem.
 * 
 * Hydra_Software_Devel/8   11/10/11 11:05a gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/7   8/25/11 4:05p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/6   8/18/11 12:53p gprasad
 * SW7422-400: Updating the APIs with review comments
 * 
 * Hydra_Software_Devel/5   7/4/11 6:35p gprasad
 * SW7422-400: Please add hooks to support dumping the UART debug buffer
 * into DRAM
 * 
 * Hydra_Software_Devel/4   4/29/11 11:45a srajapur
 * SW7422-368 : [7422] BDSP_AudioTask_GetDefaultXxxSettings is Raaga-
 * specific ---------> Moved raaga specific API from common files to
 * raaga files and renamed the API's accordingly.
 * 
 * Hydra_Software_Devel/3   4/27/11 5:08p srajapur
 * SW7422-419 : [7425] Added BDSP_Raaga_GetDownloadStatus and
 * BDSP_Raaga_Initialize API to support authentication
 * 
 * Hydra_Software_Devel/2   4/12/11 2:24p srajapur
 * SW7422-419:[7425]Adding BDSP_Raaga_GetFwDwnldInfo and
 * BDSP_Raaga_StartDsp API to support authentication
 * 
 * Hydra_Software_Devel/1   4/6/11 2:27p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/4   1/19/11 7:14a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/3   1/18/11 4:22a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/14/10 2:17p jgarrett
 * SW7422-146: Adding initial API for APE/DSP
 * 
 ***************************************************************************/

#ifndef BDSP_RAAGA_H_
#define BDSP_RAAGA_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bimg.h"
#include "bdsp_types.h"
#include "bdsp.h"

/***************************************************************************
Summary:
    This enumeration defines various debug features that can be enabled in the firmware.

***************************************************************************/
typedef enum BDSP_Raaga_DebugType
{
	BDSP_Raaga_DebugType_eDramMsg = 0,
	BDSP_Raaga_DebugType_eUart,
	BDSP_Raaga_DebugType_eCoreDump,
	BDSP_Raaga_DebugType_eTargetPrintf,
	BDSP_Raaga_DebugType_eLast,
	BDSP_Raaga_DebugType_eInvalid = 0xFF
} BDSP_Raaga_DebugType;

/***************************************************************************
Summary:
Raaga Debug Type Settings
***************************************************************************/
typedef struct BDSP_Raaga_DebugTypeSettings
{
	bool enabled;        /* If true, debug of this type is enabled. */
	uint32_t bufferSize; /* Size of debug buffer (in bytes) for a particular type of debug.  
                                                        Only required if you want to override the default value. */
} BDSP_Raaga_DebugTypeSettings;

/***************************************************************************
Summary:
Raaga DSP Status
***************************************************************************/
typedef enum BDSP_Raaga_FwStatus
{
	BDSP_Raaga_FwStatus_eRunnning = 0,
	BDSP_Raaga_FwStatus_eCoreDumpInProgress,
	BDSP_Raaga_FwStatus_eCoreDumpComplete,
	BDSP_Raaga_FwStatus_eInvalid = 0xFF
} BDSP_Raaga_FwStatus;


/***************************************************************************
Summary:
Raaga DSP Settings
***************************************************************************/
typedef struct BDSP_RaagaSettings
{
    bool authenticationEnabled; /* If authentication is enabled, all the firmware execs needs to be downloaded at open time and
                                   the DSP must be booted via a separate call to BDSP_Raaga_Boot() after HSM has been authenticated. */

    bool preloadImages;         /* If true, all firmware images will be loaded on startup.  Default=false. */ 

	BDSP_Raaga_DebugTypeSettings debugSettings[BDSP_Raaga_DebugType_eLast]; /* Debug information for the different types of debug logs */
	
    BMEM_Heap_Handle firmwareHeap;  /* Optional, this is the heap handle the firmware itself will be loaded into if 
                                       specified otherwise the heap specified in the open call will be used */

    const BIMG_Interface *pImageInterface;      /* Interface to access firmware image. This interface must be
                                                   implemented and the function pointers must be stored here. */

    void **pImageContext;                       /* Context for the Image Interface. This is also provided by
                                                   the implementation of the Image interface */
} BDSP_RaagaSettings;


/***************************************************************************
Summary:
Firmware download status.
***************************************************************************/	
typedef struct BDSP_Raaga_DownloadStatus
{
    void    *pBaseAddress; /* Pointer to base of downloaded firmware executables */
    uint32_t    physicalAddress; /* Physical memory offset of downloaded firmware executables */
    size_t  length; /* Length of executables in bytes */
} BDSP_Raaga_DownloadStatus;

/***************************************************************************
Summary:
Get Default DSP Context Settings
***************************************************************************/
void BDSP_Raaga_GetDefaultSettings(
    BDSP_RaagaSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Open a Raaga DSP 
 
Description: 
Opens a Raaga DSP device.  BDSP_Close() should be called to close the handle.

See Also: 
BDSP_Close 
***************************************************************************/
BERR_Code BDSP_Raaga_Open(
    BDSP_Handle *pDsp,                      /* [out] */
    BCHP_Handle chpHandle,
    BREG_Handle regHandle,
    BMEM_Handle memHandle,
    BINT_Handle intHandle,
    BTMR_Handle tmrHandle,
    const BDSP_RaagaSettings *pSettings
    );


/***************************************************************************
Summary:
Returns physical memory offset and size of firmware executables.

Description:
Returns the physical memory offset where firmware executables of all the 
supported algorithms are downloaded in contiguous memory. It 
also returns the total size of the downloaded firmware executables. This 
function is supported only when firmware authentication is enabled.

See also:
BDSP_Raaga_Initialize
***************************************************************************/
BERR_Code BDSP_Raaga_GetDownloadStatus(
    BDSP_Handle handle,
    BDSP_Raaga_DownloadStatus *pStatus /* [out] */
);

/***************************************************************************
Summary:
Initialize (Boot) the DSP 
 
Description: 
This call will boot the DSP.  Normally, the DSP is booted on initialization, 
but if the DSP was opened with authenticationEnabled = true, this call 
is required to initialize the DSP. 
 
See Also: 
BDSP_Raaga_Open 
***************************************************************************/
BERR_Code BDSP_Raaga_Initialize(
    BDSP_Handle handle
    );

 /******************************************************************************
Summary:
	This Function returns true, If the decode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioTypeSupported(
		BDSP_AudioType audioType);

/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioProcessingSupported(
					BDSP_AudioProcessing audioProcessing);

/******************************************************************************
Summary:
	This Function returns true, If the Encode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_IsAudioEncodeSupported(
                                    BDSP_AudioEncode audioEncode);


/***************************************************************************
Summary: 
Get default Audio codec settings.
***************************************************************************/
BERR_Code BDSP_Raaga_GetDefaultAudioTypeSettings(
        BDSP_AudioType  audioType,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    );

/***************************************************************************
Summary: 
Get default Audio Processing settings.
***************************************************************************/

BERR_Code BDSP_Raaga_GetDefaultAudioProcessingSettings(
        BDSP_AudioProcessing  audioProcessingType,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    );

/***************************************************************************
Summary: 
Get default Audio Encode settings.
***************************************************************************/
BERR_Code BDSP_Raaga_GetDefaultAudioEncodeSettings(
        BDSP_AudioEncode    audioEncode,
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    );

/***************************************************************************
Summary:
Get Raaga Firmware Debug Data
***************************************************************************/
BERR_Code BDSP_Raaga_GetDebugBuffer(
	BDSP_Handle handle,
	BDSP_Raaga_DebugType debugType, /* [in] Gives the type of debug buffer for which the Base address is required ... UART, DRAM, CoreDump ... */
    uint32_t dspIndex, /* [in] Gives the DSP Id for which the debug buffer info is required */
	void **pBuffer, /* [out] Base address of the debug buffer data */
	size_t *pSize /* [out] Contiguous length of the debug buffer data in bytes */
);

/***************************************************************************
Summary:
Consume debug data from the debug ringbuffer.  
***************************************************************************/
BERR_Code BDSP_Raaga_ConsumeDebugData(
	BDSP_Handle handle,
	BDSP_Raaga_DebugType debugType, /* [in] Gives the type of debug buffer for which the Base address is required ... UART, DRAM, CoreDump ... */
    uint32_t dspIndex, /* [in] Gives the DSP Id for which the debug data needs to be consumed */
	size_t bytesConsumed    /* [in] Number of bytes consumed from the debug buffer */
);

/***************************************************************************
Summary:
Returns the Status of the DSP
***************************************************************************/
BDSP_Raaga_FwStatus BDSP_Raaga_GetCoreDumpStatus (
    BDSP_Handle handle,
    uint32_t dspIndex); /* [in] Gives the DSP Id for which the core dump status is required */

#endif

