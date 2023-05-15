/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pcr.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 6/20/12 2:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_pcr.h $
 * 
 * Hydra_Software_Devel/10   6/20/12 2:58p gmullen
 * SW7429-186: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-186/1   6/20/12 2:48p gmullen
 * SW7429-186: Added B0 support
 * 
 * Hydra_Software_Devel/9   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/8   5/8/12 10:15a gmullen
 * SW7429-115: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-115/2   4/20/12 10:19a gmullen
 * SW7429-115: Return BERR_Code
 * 
 * Hydra_Software_Devel/SW7429-115/1   4/6/12 2:04p gmullen
 * SW7429-115: Implemented workaround
 * 
 * Hydra_Software_Devel/7   9/12/11 4:36p gmullen
 * SW7344-193: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7344-193/1   9/7/11 4:34p gmullen
 * SW7344-193: Added tri-state logic to jitter API
 * 
 * Hydra_Software_Devel/6   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/5   5/16/11 4:58p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/4   11/8/10 4:02p gmullen
 * SW7422-20: PI can now generate the required INT IDs
 * 
 * Hydra_Software_Devel/3   10/27/10 2:08p gmullen
 * SW7422-20: Added BXPT_PCR_TimeRef_eHD_DVI_H1 and
 * BXPT_PCR_TimeRef_eHD_DVI_V1 as placeholders
 * 
 * Hydra_Software_Devel/2   10/27/10 12:03p gmullen
 * SW7422-20: Ported to 7422
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/20   9/2/10 10:26a gmullen
 * SW35230-2: Fixed incorrect timebase mapping in enum
 * 
 * Hydra_Software_Devel/19   11/18/09 2:38p gmullen
 * SW35230-2: eHD_DVI_V0/V1 is supported on the 35230
 * 
 * Hydra_Software_Devel/18   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/17   5/11/09 5:11p piyushg
 * PR53746: Added xpt interface for retrieving phase error at isr time
 * BXPT_PCR_GetPhaseError_isr().
 * 
 * Hydra_Software_Devel/16   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/15   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/14   11/10/08 9:07a gmullen
 * PR48128: Added playback support for 7325/7335. Removed redundant code.
 * 
 * Hydra_Software_Devel/13   9/12/08 2:21p gmullen
 * PR46659: Added 3556/3548 B0 support
 * 
 * Hydra_Software_Devel/12   9/12/08 11:27a gmullen
 * PR46659: Added B0 support
 * 
 * Hydra_Software_Devel/11   8/8/08 5:47p gmullen
 * PR45362: Merged SanJose_CDI_Devel changes to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/10   4/10/08 3:36p gmullen
 * PR38954: Updated number of playbacks, added HDDVI support to PCR API,
 * and added packetizer support.
 * 
 * Hydra_Software_Devel/9   2/5/08 8:19a gmullen
 * PR39329: Added ISR version of GetLastPcr.
 * 
 * Hydra_Software_Devel/8   8/15/07 4:56p gmullen
 * PR34082: Removed incorrect comment from description of
 * BXPT_PCR_XptStreamPcrCfg
 * 
 * Hydra_Software_Devel/7   8/15/07 4:52p gmullen
 * PR34082: Added filter coefficients to BXPT_PCR_XptStreamPcrCfg struct.
 * 
 * Hydra_Software_Devel/6   7/18/07 1:38p gmullen
 * PR33106: Added BXPT_PCR_SetCenterFrequency()
 * 
 * Hydra_Software_Devel/5   11/10/06 5:47p katrep
 * PR25737: Added BXPT_PCR_SetTimeRefTrackRange API.
 * 
 * Hydra_Software_Devel/4   8/18/06 10:13a gmullen
 * PR23345: Added MaxPcrError to Get/SetSettings calls.
 * 
 * Hydra_Software_Devel/3   8/8/06 6:36p gmullen
 * PR15309: Added proto for BXPT_PCR_GetTimeBaseFreqRefDefaults
 * 
 * Hydra_Software_Devel/2   8/2/06 3:56p gmullen
 * PR23137: Added support for playback data to PCR block.
 * 
 * Hydra_Software_Devel/3   9/26/05 4:40p gmullen
 * PR15309: Removed support for loading user STCs. That support is now in
 * bxpt_pcr_offset.c
 * 
 * Hydra_Software_Devel/2   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 ***************************************************************************/

#ifndef BXPT_PCR_H__
#define BXPT_PCR_H__

#include "bxpt.h"
#include "bchp_xpt_dpcr0.h"

#ifdef __cplusplus
extern "C"{
#endif

/*=************************ Module Overview ********************************
<verbatim>
Overview:
</verbatim>
***************************************************************************/

/***************************************************************************
Summary:
	The opaque handle for a PCR module. 
Description:
	This structure represents the PCR handle. It is created at BXPT_PCR_Open 
***************************************************************************/
typedef struct BXPT_P_PcrHandle_Impl *BXPT_PCR_Handle;

/***************************************************************************
Summary:
	Time base frequency reference control configuration.  It is for non-transport
	timebase reference. 
Description:
	The members in the structure controls the frequency of a timebase reference.
	It appies to analog references and the internal reference. This is not for 
	timebase pulse frequency which is controlled in rate managers of different
	hardware blocks.
***************************************************************************/
typedef struct BXPT_PCR_TimebaseFreqRefConfig
{
	uint32_t Prescale;	 /*	frequency control parm*/
	uint32_t Inc;		 /*	frequency control parm*/
	uint8_t FiltA;			/* integrator leak. */	
	uint8_t FiltB;			/* loop gain. */	
	uint8_t FiltC;			/* direct path gain. */	
}
BXPT_PCR_TimebaseFreqRefConfig;

/***************************************************************************
Summary:
Different format for the packet timestamps supported by the PCR block.  
***************************************************************************/
typedef enum 
{
    BXPT_PCR_JitterTimestampMode_e32 = 0,       /* 32 bit timestamp */
    BXPT_PCR_JitterTimestampMode_e28_4P = 1,    /* 28 bit timestamp with 4 bit parity.*/
    BXPT_PCR_JitterTimestampMode_e30_2U = 2,     /* 30 bit timstamp with 2 user bits. */
    BXPT_PCR_JitterTimestampMode_eAuto,         /* Do not change the current (global) setting. */
    BXPT_PCR_JitterTimestampMode_eMax
}
BXPT_PCR_JitterTimestampMode;

/***************************************************************************
Summary:
***************************************************************************/
typedef enum 
{
    BXPT_PCR_JitterCorrection_eEnable = 0, 
    BXPT_PCR_JitterCorrection_eDisable = 1,
    BXPT_PCR_JitterCorrection_eAuto,        /* Do not change the current (global) setting. */
    BXPT_PCR_JitterCorrection_eMax
}
BXPT_PCR_JitterCorrection;

/***************************************************************************
Summary:
Configuration for PCR module when transport stream is timebase reference.

Description:
This structure covers pcr pid, stream source, PcrSendEn, and sendmode 
configurations when locking to a transport stream. 
 
If jitter adjust is enabled, the jitter adjustment block is supposed to be 
programmed to match with the Playback timestamp format (i.e. 32-bit, 28-bit 
or 30-bit formats). This tells the jitter calculation hardware whether to use 
all 32-bits of the timestamp, or to ignore the upper 4 bits of the timestamp 
(4-bit parity + 28-bit, or 2-bit User + 30-bit). The 32-bit support in the 
DPCR is for both binary and Mod-300. The hardware uses the Packet type (MPEG 
or 130 byte) to decide whether timestamp mode is Mod-300 or binary. 
 
NOTE!! that timestamp mode and PCR type must match for jitter calculation to 
work correctly, i.e. if it is MPEG stream with PCRs then the timestamps must 
also be Mod-300. This means we can't use jitter adjustment for MPEG stream 
with PCRs but with binary/DLNA timestamps.
***************************************************************************/
typedef struct BXPT_PCR_XptStreamPcrCfg
{
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    uint32_t PidChannel;				/* PCR PID channel */

    /*
    ** Note: these params are unique in that the hw setting is *global* to all bxpt_pcr instances. 
    ** Thus, changing one param here will affect other blocks. This isn't a problem in the usage 
    ** cases the hw engineers know about, but it requires some care by sw. These params are 
    ** tri-state, as follows. _eAuto should be selected, unless the user knows this block must 
    ** have a unique setting. At some point in the future, these params may no longer be global. 
    **  
    ** The tri-state values are: 
    **      _eAuto - Do not change the existing global setting.
    **      _eEnable or _eDisable for PbJitterDisable and LiveJitterDisable.
    **      _e32, etc for JitterTimestamp.
    */
    BXPT_PCR_JitterTimestampMode JitterTimestamp; 
    BXPT_PCR_JitterCorrection PbJitterDisable;
    BXPT_PCR_JitterCorrection LiveJitterDisable;
#else
	uint32_t Pid;				      /* PCR Pid */
	BXPT_DataSource eStreamSelect;    /* Pcr stream source. NOTE: only input bands are supported. */
	unsigned int WhichSource;		  /* Which instance of the source */
#endif
	uint32_t MaxPcrError;			  /* Maximum PCR error thredhold*/

	BXPT_PCR_TimebaseFreqRefConfig TimebaseCfg;		/* Customized filter coefficients. */
}
BXPT_PCR_XptStreamPcrCfg;

/***************************************************************************
Summary:
	Configurations for modifying the rate of the STC extension counter.
Description:
	The members in the structure sets the rate control of STC extension.  
***************************************************************************/
typedef struct BXPT_PCR_STCExtRateConfig
{
	uint32_t Prescale;	 /*	frequency control parm*/
	uint32_t Inc;		 /*	frequency control parm*/
}
BXPT_PCR_STCExtRateConfig;

/***************************************************************************
Summary:
	Timebase references	for all the sources that a timebase can lock to.
Description:
	This enum represents all the time references that timebase
	may lock to. 
***************************************************************************/
typedef enum BXPT_PCR_TimeRef
{
#if BXPT_HAS_REDUCED_TIMEREFS
    /* Not available on all chips. Check your hw docs. */
	
	BXPT_PCR_TimeRef_eI656_Hl = 3,	  		/* Lock to Hsync from ITU656 input 0  */
	BXPT_PCR_TimeRef_eI656_Vl = 4,	  		/* Lock to Vsync from ITU656 input 0  */
	BXPT_PCR_TimeRef_eI656_Fl = 5,	  		/* Lock to Fsync from ITU656 input 0  */
	BXPT_PCR_TimeRef_eI2S0 = 6,	 			/* Lock to I2S input #0  */
	BXPT_PCR_TimeRef_eHD_DVI_H0 = 11,		/* Hsync from HD DVI input 0 */
	BXPT_PCR_TimeRef_eHD_DVI_V0 = 12,		/* Vsync from HD DVI input 0 */
	BXPT_PCR_TimeRef_eInternal = 14,		/* Lock to Internal Reference which has a period of 16384 cycles at 27MHz */
	BXPT_PCR_TimeRef_eXpt = 15,				/* Lock to transport PCR values */

	BXPT_PCR_TimeRef_eHDMI_Passthru_HL = 16, /* Lock to Hsync from DVP_HR for HDMI passthru mode */
	BXPT_PCR_TimeRef_eHDMI_Passthru_VL = 17, /* Lock to Vsync from DVP_HR for HDMI passthru mode */
	BXPT_PCR_TimeRef_eSPDIF = 18,			/* SPDIF input 1 */
	BXPT_PCR_TimeRef_eMaiIn = 19,			/* Frame Sync from DVP_HR */

	BXPT_PCR_TimeRef_eMax = 100,			/* Max value, marks the end of the supported inputs. */

    /* These are placeholders, in case hw for HD DVI input 1 is added in the future. */
	BXPT_PCR_TimeRef_eHD_DVI_H1 = 101,		/* Hsync from HD DVI input 1 */
	BXPT_PCR_TimeRef_eHD_DVI_V1 = 102		/* Vsync from HD DVI input 1 */
	
#else
	BXPT_PCR_TimeRef_eVDECHl = 0,	  		/* Lock to Hsync from  VDEC #0  */
	BXPT_PCR_TimeRef_eVDECVl = 1,	  		/* Lock to Vsync from VDEC #0 */
	BXPT_PCR_TimeRef_eVDECCl = 2,	  		/* Chroma lock to VDEC #0  */

	BXPT_PCR_TimeRef_eI656_Hl = 3,	  		/* Lock to Hsync from ITU656 input 0  */
	BXPT_PCR_TimeRef_eI656_Vl = 4,	  		/* Lock to Vsync from ITU656 input 0  */
	BXPT_PCR_TimeRef_eI656_Fl = 5,	  		/* Lock to Fsync from ITU656 input 0  */

	BXPT_PCR_TimeRef_eI2S0 = 6,	 			/* Lock to I2S input #0  */

    /* These time references are not available on all chips. The PI will return
       BERR_INVALID_PARAMETER if the reference isn't supported. */
	BXPT_PCR_TimeRef_eHD_DVI_V0 = 18,		/* Vsync from HD DVI input 0 */
	BXPT_PCR_TimeRef_eHD_DVI_H0 = 19,		/* Hsync from HD DVI input 0 */
	BXPT_PCR_TimeRef_eHD_DVI_V1 = 20,		/* Vsync from HD DVI input 1 */
	BXPT_PCR_TimeRef_eHD_DVI_H1 = 21,		/* Hsync from HD DVI input 1 */
	BXPT_PCR_TimeRef_eSPDIF = 22,			/* SPDIF input 1 */
	BXPT_PCR_TimeRef_eVDEC_SEC_HL = 26,		/* VDEC Hsync */
	BXPT_PCR_TimeRef_eVDEC_SEC_VL = 27,		/* VDEC Vsync */
	BXPT_PCR_TimeRef_eVDEC_SEC_CL = 28,		/* VDEC chroma */
	BXPT_PCR_TimeRef_eMaiIn = 29,			/* Frame Sync from DVP_HR */

    /* The remaining references are supported on all devices. */
	BXPT_PCR_TimeRef_eI2S1 = 7,	 			/* Lock to I2S input #0  */
	BXPT_PCR_TimeRef_eI656_Hl_1 = 8,		/* Lock to Hsync from ITU656 input 1  */
	BXPT_PCR_TimeRef_eI656_Vl_1 = 9,		/* Lock to Vsync from ITU656 input 1  */
	BXPT_PCR_TimeRef_eI656_Fl_1 = 10,  		/* Lock to Fsync from ITU656 input 1  */

	BXPT_PCR_TimeRef_eInternal = 14,		/* Lock to Internal Reference which has a period of 16384 cycles at 27MHz */
	BXPT_PCR_TimeRef_eXpt = 15				/* Lock to transport PCR values */
#endif	
}
BXPT_PCR_TimeRef;

/***************************************************************************
Summary:
	Timebase references	possible track Range values 
Description:
***************************************************************************/
typedef enum BXPT_PCR_RefTrackRange
{
    BXPT_PCR_TrackRange_PPM_8 = 0,
    BXPT_PCR_TrackRange_PPM_15,
    BXPT_PCR_TrackRange_PPM_30,
    BXPT_PCR_TrackRange_PPM_61,
    BXPT_PCR_TrackRange_PPM_122,
    BXPT_PCR_TrackRange_PPM_244

}BXPT_PCR_RefTrackRange;

/***************************************************************************
Summary:
Valid error clamping ranges for the phase error input. 
***************************************************************************/
typedef enum BXPT_PCR_PhaseErrorClampRange
{
    BXPT_PCR_PhaseErrorClampRange_e22_8mS = 5,		/* +/- 22.8 mS */
    BXPT_PCR_PhaseErrorClampRange_e11_4mS = 4,		/* +/- 11.4 mS */
    BXPT_PCR_PhaseErrorClampRange_e5_7mS = 3,		/* +/- 5.7 mS */
    BXPT_PCR_PhaseErrorClampRange_e2_8mS = 2,		/* +/- 2.8 mS */
    BXPT_PCR_PhaseErrorClampRange_e1_4mS = 1,		/* +/- 1.4 mS */
    BXPT_PCR_PhaseErrorClampRange_e0_7mS = 0		/* +/- -0.7 mS */
}
BXPT_PCR_PhaseErrorClampRange;


/***************************************************************************
Summary:
	Defaultsettings PCR modules
Description:
	This structure represents the default settings for a pcr module. 
***************************************************************************/
typedef struct BXPT_PCR_DefSettings
{
	uint32_t MaxPcrError;	/* Maximum PCR error thredhold*/
	bool PcrTwoErrReaquireEn; /* control of re-acquisition of PCR when TWO_PCR_ERROR occurs	*/
}
BXPT_PCR_DefSettings;

/***************************************************************************
Summary:
	Return the number of Pcr channels.
Description:
	This function returns the number of pcr channels supported.
Returns:
    BERR_SUCCESS                - returns the number of Pcr channel.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PCR_GetTotalChannels(
	BXPT_Handle hXpt,           /* [in] The transport handle */
	unsigned int *TotalChannels     /* [out] The number of Pcr channels. */
    );

/***************************************************************************
Summary:
	Returns the Pcr channel default settings.

Description:
	This function gets the default settings for a given channel and places 
	them in a structure. The default settings shall be retrieved and changed
	before channel openings. The values are set at channel openings.
Returns:
    BERR_SUCCESS                - Retrieved Pcr defaults.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PCR_GetChannelDefSettings(
	BXPT_Handle hXpt,                     	/* [in] The transport handle - need chip info */
	unsigned int WhichPcr,					  	/* [in] Which pcr module */
	BXPT_PCR_DefSettings *pcrSettings       /* [out] The default settings of a pcr module */
	);

/***************************************************************************
Summary: 
	Open a Pcr channel for a given PCR module
Description:
	This functions obtains a PCR handle. Sets default settings, 
	enable the PCR_TWO_ERROR interrupt.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PCR_Open(
	BXPT_Handle hXpt,				   /* [in] The transport handle*/
	unsigned int WhichPcr,			   /* [in] Which pcr module */
	BXPT_PCR_DefSettings *pcrSettings, /* [in] The default setting */
	BXPT_PCR_Handle *hPcr			   /* [out] The pcr handle */
	);

/***************************************************************************
Summary: 
	Close a Pcr channel.
Description:
	This function closes the pcr channel by disabling PCR and sets back to
	default settings.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PCR_Close(
	BXPT_PCR_Handle hPcr	 /* [in] The pcr handle */
);

/***************************************************************************
Summary: 
	Gets PCR configuration when PCR is locking to a transport stream
Description:
	This functions gets the current setting of the PCR configuration. It includes
	PCR Pid, stream source, SEND_EN, and send mode for Video.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_GetStreamPcrConfig( 
	BXPT_PCR_Handle hPcr, 						  /* [in] The pcr handle */
	BXPT_PCR_XptStreamPcrCfg *PcrCfg             /* [out] Transport source configuration */
	);

/***************************************************************************
Summary: 
Sets PCR configuration when PCR is locking to a transport stream

Description:
This functions sets Pids, stream source, SEND_EN and timebase reference to 
transport stream and puts the PCR module into acqusition mode.

For 7401B0 and later devices, the PCR block can accept data from 
playback channels by setting PcrCfg->eStreamSelect = BXPT_DataSource_ePlayback

Returns:
    BERR_SUCCESS                - if successful 
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_SetStreamPcrConfig( 
	BXPT_PCR_Handle hPcr, 						 /* [in] The pcr handle */
	const BXPT_PCR_XptStreamPcrCfg *PcrCfg       /* [in] Transport source configuration */
	);

/***************************************************************************
Summary: 
Sets PCR configuration when PCR is locking to a transport stream.  This 
function is  only be to used from within an ISR.

Description:
This functions sets Pids, stream source, SEND_EN and timebase reference to 
transport stream and puts the PCR module into acqusition mode.

For 7401B0 and later devices, the PCR block can accept data from 
playback channels by setting PcrCfg->eStreamSelect = BXPT_DataSource_ePlayback

Returns:
    BERR_SUCCESS                - if successful 
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_SetStreamPcrConfig_isr( 
	BXPT_PCR_Handle hPcr, 						 /* [in] The pcr handle */
	const BXPT_PCR_XptStreamPcrCfg *PcrCfg       /* [in] Transport source configuration */
	);


/***************************************************************************
Summary: 
XPT interface for retrieving phase error at isr time

Returns:
    BERR_SUCCESS                - if successful 
***************************************************************************/
BERR_Code   BXPT_PCR_GetPhaseError_isr( 
    BXPT_PCR_Handle hPcr,             
    int32_t *p_error
    );

/***************************************************************************
Summary:
	controls modifying the rate of the STC extension counter.
Description:
	This function configures the pre-scale and increment parameters of the
	PCR STC EXT control register. The PCR_INC_VAL (M) and PCR_PRESCALE (N) 
	can be used to alter the rate of the STC counter to 27 MHz * M/(N+1). 
	For example, to run the STC counter at 18 MHz, (i.e. 2/3 of the normal rate),
	program M=2 and N=2.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_SetStcExtRateControl( 
	BXPT_PCR_Handle hPcr, 					     /* [in] The pcr handle */
	const BXPT_PCR_STCExtRateConfig  *StcExtRateCfg /* [in] Stc extension rate control configuration */
	);	

/***************************************************************************
Summary:
	enables PCR processing on PCR_PID and puts PCR into an acquisition mode.
Description:
	This function enables PCR processing on PCR_PID. Writing a "1" to Valid bit
	also puts the PCR module into an acquisition mode.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
void	BXPT_PCR_RefreshPcrPid( 
	BXPT_PCR_Handle hPcr 			   /*[in] The pcr handle  */
	);

/***************************************************************************
Summary:
	Gets the last PCR captured in last PCR Hi/Lo registers.
Description:
	This function reads the values from PCR_LAST_PCR_HI/LO registers.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_GetLastPcr( 
	BXPT_PCR_Handle hPcr, 			  /* [in] The pcr handle */
	uint32_t *p_pcrHi, 			  /* [out] Upper bits of PCR*/
	uint32_t *p_pcrLo 			  /* [out] Bit9-LSB of base bit[0-8]-extension*/
	);

/***************************************************************************
Summary:
	ISR version of BXPT_PCR_GetLastPcr.
Description:
	This function reads the values from PCR_LAST_PCR_HI/LO registers. It is
	callable from within an ISR context.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_GetLastPcr_isr( 
	BXPT_PCR_Handle hPcr, 			  
	uint32_t *p_pcrHi, 			  
	uint32_t *p_pcrLo 			  
	);

/***************************************************************************
Summary:
	Gets the STC counter values
Description: 
	This function read the PCR STC counters from STC_HI/LO registers
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_GetStc( 
	BXPT_PCR_Handle hPcr, 			 /* [in] The pcr handle */
	uint32_t *p_stcHi, 			 /* [out] Upper bits of STC*/
	uint32_t *p_stcLo 			 /* [out] bit9-LSB of base bit[0-8]-extension*/
	);

/***************************************************************************
Summary:
	Sets non-transport source for a timebase reference and configures frequency
	control for the reference.
Description:
	This function sets a timebase locking to a non transport source and sets 
	the frequency control for the reference.  It is recommanded to use
	BXPT_PCR_GetfreqRefConfig before calling this function to preserve
	the default values of frequency control if no intention to change the
	frequency. 
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_ConfigNonStreamTimeBase( 
	BXPT_PCR_Handle hPcr, 					      /* [in] The pcr handle */
	BXPT_PCR_TimeRef  eNonStreamTimeBaseCfg,      /* [in] Non transport source configuration */
	BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq  /* [in] Non transport source frequency config */
	);										 

/***************************************************************************
Summary:
	Gets timebase reference frequency control. It is for non transport time 
	references.
Description:
	This function gets the pre-scale and increment parameters of the 
	DPCR_REF_PCR control register.
Returns:
    BERR_SUCCESS                - if successful 
	BERR_INVALID_PARAMETER      - Bad input parameter 
***************************************************************************/
BERR_Code	BXPT_PCR_GetfreqRefConfig( 
	BXPT_PCR_Handle hPcr, 					     		/* [in] The pcr handle */
	BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig /* [out] Non transport source configuration */
	);	

/***************************************************************************
Summary:
	Isr version of enabling PCR processing on PCR_PID and putting PCR into 
	an acquisition mode.
Description:
	This function enables PCR processing on PCR_PID. Writing a "1" to Valid bit
	also puts the PCR module into an acquisition mode.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
void BXPT_PCR_RefreshPcrPid_isr( 
	BXPT_PCR_Handle hPcr 			   /*[in] The pcr handle  */
	);

/***************************************************************************
Summary:
Freeze integration loop.

Description:
Stop loop tracking by freezing the integrator. Useful for cases like analog
decode. The integrator can be un-frozen by setting the Freeze argument to 
false. 

Returns:
	void
***************************************************************************/
void BXPT_PCR_FreezeIntegrator( 
	BXPT_PCR_Handle hPcr, 	 /* [in] The pcr handle  */
	bool Freeze				 /* [in] Freeze integrator if true, run if false. */
	);

/***************************************************************************
Summary:
Set the center frequency.

Description:
Set the absolute center frequency of the timebase relative to the crytal. A
value of 0x400000 will generate a timebase of 27MHz. Changing the center
frequency by one bit will change the timebase by about 3.8 ppm.

Returns:
	void
***************************************************************************/
void BXPT_PCR_SetCenterFrequency( 
	BXPT_PCR_Handle hPcr, 	 	/* [in] The pcr handle  */
	uint32_t CenterFreq			/* [in] Center frequency */
	);

/***************************************************************************
Summary:
Get default values for locking to the given timebase reference.

Description:
Return default values for the loop gain constants, pcr increment, and 
prescale. These defaults are tailored to the given timebase reference. These
defaults may be used in a subsequent call to BXPT_PCR_ConfigNonStreamTimeBase.

Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code	BXPT_PCR_GetTimeBaseFreqRefDefaults( 
	BXPT_PCR_Handle hPcr, 					       
	BXPT_PCR_TimeRef TimeBase,  
	BXPT_PCR_TimebaseFreqRefConfig *Def
	);

/***************************************************************************
Summary:
Set the time base tracking range in the PCR block.

Description:
The timebase has a limited tracking range. The tracking range is centered on 
the crystal reference  + pcr center. Note that changes to the tracking range 
may cause sudden changes to the timebase, and may affect the video output quality. 

Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
void BXPT_PCR_SetTimeRefTrackRange( 
	BXPT_PCR_Handle hPcr, 					       
	BXPT_PCR_RefTrackRange TrackRange
	);

/***************************************************************************
Summary:
Set the phase error clamp range for the phase error input to the loop filter

Description:

Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
void BXPT_PCR_SetPhaseErrorClampRange( 
	BXPT_PCR_Handle hPcr, 					       
	BXPT_PCR_PhaseErrorClampRange ClampRange
	);

/***************************************************************************
Summary:
Enumeration of all supported interrupts in a Rave context. 
****************************************************************************/
typedef enum BXPT_PCR_IntName
{
    /* Set whenever a PCR is detected */
    BXPT_PCR_IntName_ePhaseCompare = BCHP_XPT_DPCR0_INTR_STATUS_REG_PHASE_CMP_INT_SHIFT,               

    /* Set whenever 2 consecutive PCR errors are seen in the stream. */
    BXPT_PCR_IntName_eTwoPcrErrors = BCHP_XPT_DPCR0_INTR_STATUS_REG_TWO_PCR_ERROR_SHIFT,  
                 
    BXPT_PCR_IntName_eMax   /* Marks the end of the supported interrutps */
}
BXPT_PCR_IntName;

/***************************************************************************
Summary:
Generate the interrupt ID for the requested interrupt name. 
 
Description: 
Prevents the app from having to do it directly. 

Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PCR_GetIntId(
    unsigned WhichPcr,          /* Which PCR block. Same value as used in BXPT_PCR_Open() */
    BXPT_PCR_IntName Name,      /* The interrupt in question. */
    BINT_Id *IntId              /* IntId suitable for the BINT module. */
   );


BERR_Code BXPT_PCR_P_Integrator( 
    BXPT_Handle hXpt
    );


#ifdef __cplusplus
}
#endif

#endif
