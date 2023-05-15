/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_tsmux.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/3/11 4:34p $
 *
 * Porting interface for TS muxing via playback. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_tsmux.h $
 * 
 * Hydra_Software_Devel/9   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/3   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/SW7425-653/2   5/28/11 12:53p gmullen
 * SW7425-653: Revised API proposal.
 * 
 * Hydra_Software_Devel/SW7425-653/1   5/26/11 5:44p gmullen
 * SW7425-653: Prototypes for NRT support
 * 
 * Hydra_Software_Devel/8   12/30/10 3:47p gmullen
 * SW7425-15: Added  BXPT_TsMux_RemovePlayback()
 * 
 * Hydra_Software_Devel/6   12/28/10 2:16p gmullen
 * SW7425-15: Removed settings from _Create() call
 * 
 * Hydra_Software_Devel/5   12/21/10 2:04p gmullen
 * SW7425-15: Switched uiMuxDelay back to mS units
 * 
 * Hydra_Software_Devel/4   12/21/10 9:25a gmullen
 * SW7425-15: Use PCR Offset handle to configure offset hw
 * 
 * Hydra_Software_Devel/3   12/7/10 2:22p gmullen
 * SW7425-15: Checked in implementation
 * 
 * Hydra_Software_Devel/2   12/1/10 8:01a gmullen
 * SW7425-15: Updated header and stub calls for TS mux
 * 
 * Hydra_Software_Devel/1   11/23/10 4:56p gmullen
 * SW7425-15: Added transcoding extensions
 * 
 * 
 ***************************************************************************/

/***************************************************************************
Overview

***************************************************************************/

#ifndef BXPT_TSMUX_H__
#define BXPT_TSMUX_H__

#include "bxpt.h"
#include "bxpt_playback.h"
#include "bxpt_pcr_offset.h"

#define BXPT_TSMUX_MAX_PACING_SPEED 128

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Handle for accessing the TsMux API via a channel. Users should not directly
access the contents of the structure.
****************************************************************************/
typedef struct BXPT_P_TsMuxHandle *BXPT_TsMux_Handle;

/***************************************************************************
Summary: 
Status bits for the TsMux channel. 
****************************************************************************/
typedef struct BXPT_TsMux_Status
{      
    uint64_t uiSTC;     /* 42-bit value in 27 Mhz */
    uint32_t uiESCR;    /* 32-bit value in 27Mhz; */
}
BXPT_TsMux_Status;

/***************************************************************************
Summary: 
Settings for the TsMux. 
****************************************************************************/
typedef struct BXPT_TsMux_Settings
{
    unsigned uiMuxDelay;	/* in mSec */
    
    bool bAFAPMode;       /* As fast as possible mode. */
    struct 
    {
        /* If bAFAPMode true, the following parameters are required.
        They affect all playback channels involved in the mux instance. */
        bool bEnablePacingPause;     /* Defaults to true. Currently not implemented */
        unsigned uiPacingSpeed;      /* Multiplier applied to the pacing counter increment. 
                                    This must be a power of 2, ie 1, 2, 4 ... up to 
                                    BXPT_TSMUX_MAX_PACING_SPEED. Defaults to 8. */
        unsigned uiPacingCounter;   /* Current value of the pacing counter. */
    }
    AFAPSettings;

    /*
    ** The PCR offset channel to be used in TS muxing. 
    */
    BXPT_PcrOffset_Handle hPcrOffset;
}
BXPT_TsMux_Settings;

/***************************************************************************
Summary:
Return the TsMux default settings.

Description:
Return the TsMux default settings.

Returns: 
    void 
****************************************************************************/
void BXPT_TsMux_GetDefaultSettings(
    BXPT_TsMux_Settings *Defaults   /* [out] The defaults */
    );  

/***************************************************************************
Summary:
Create a new TsMux instance. 
 
Description:
This just creates the empty TsMux framework. No playback channels are 
allocated or configured at this time.  
 
Returns:
    BERR_SUCCESS                - TsMux instance created.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_TsMux_Create(
    BXPT_Handle hXpt,                        /* [in] Handle for this transport */
    BXPT_TsMux_Handle *hTsMux                /* [out] Handle for opened record channel */
    );  

/***************************************************************************
Summary:
Destroy the TsMux instance. 
 
Description:
Undo the bindings between the playback channels and PCR Offsets that were 
created by the calls to BXPT_TsMux_AddPlaybackToTsMux() and 
BXPT_TsMux_AddPcrOffsetToTsMux() Release any other resources held by the TsMux 
instance. 
 
Returns:
    void
****************************************************************************/
void BXPT_TsMux_Destroy(
    BXPT_TsMux_Handle hTsMux     /* [in] Handle for the TsMux to destroy */
    );  

/***************************************************************************
Summary:
Add a playback channel to be used in TS muxing. 
 
Description: 
Add a playback channel to the TS muxer instance. The channel must be already 
opened before calling this function. The appropriate binding between this 
playback and any playbacks already in the mux instance are done here.  
 
Returns:
    BERR_SUCCESS                - Playback allocated succesfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_TsMux_AddPlayback(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_Playback_Handle PlaybackHandle    /* [in] Handle for allocated playback channel */
    );

/***************************************************************************
Summary:
Remove a playback channel from the TS object.
 
Description: 
Remove a playback channel from the TS muxer instance.   
 
Returns:
	void
****************************************************************************/
void BXPT_TsMux_RemovePlayback(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_Playback_Handle PlaybackHandle    /* [in] Handle for allocated playback channel */
    );

/***************************************************************************
Summary:
Get the current channel settings.

Description:
The current channel settings are read from the hardware cores and returned in
the Settings structure.

Returns:
	void
****************************************************************************/
void BXPT_TsMux_GetSettings(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_TsMux_Settings *Settings   /* [out] The current settings  */
    );

/***************************************************************************
Summary:
Set the TsMux instance parameters. 
 
Description:
Set the TsMux instance parameters. 

Returns:
    BERR_SUCCESS                - New settings are now in effect.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_TsMux_SetSettings(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    const BXPT_TsMux_Settings *Settings /* [in] New settings to use */
    );

/***************************************************************************
Summary:
Get the current status.

Description:
Get the current status.

Returns:
	void
****************************************************************************/
void BXPT_TsMux_GetStatus(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_TsMux_Status *Status 		/* [out] Channel status. */
    );
      
/***************************************************************************
Summary:
Configure a playback linked-list descriptor for TS muxing. 

Description:
Initialize the contents of a playback descriptor. The caller passes in the
starting address of the buffer that the descriptor will point to, along with
the length of that buffer.  

Returns:
	void
****************************************************************************/
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
void BXPT_Playback_ConfigTsMuxDesc( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *Desc,              /* [in] Descriptor to be configured */
    const BAVC_TsMux_DescConfig *Config     /* [in] Data to be loaded */
    );

/***************************************************************************
Summary:
Return the muxing flags from the given descriptor.

Returns:
	void
****************************************************************************/
void BXPT_Tsmux_GetDescConfig(
    const BXPT_PvrDescriptor *Desc,      
    BAVC_TsMux_DescConfig *Config     /* [out] muxing flags unpacked from the descriptor */
    );

#else
void BXPT_Tsmux_ConfigDesc( 
    BXPT_PvrDescriptor8 *Desc,              /* [in] Descriptor to be configured */
    const BAVC_TsMux_DescConfig *Config     /* [in] Data to be loaded */
    );
#endif
/* Private functions. Should not be called directly by user code. */
void BXPT_TsMux_P_ResetBandPauseMap( 
    BXPT_Handle hXpt
    );

void BXPT_TsMux_P_ResetPacingPauseMap( 
    BXPT_Handle hXpt
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_TSMUX_H__ */

/* end of file */


