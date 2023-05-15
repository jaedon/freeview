/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_mixer_priv.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 9/3/09 5:44p $
*
* Module Description:
*   Module name: MIXER
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Mixer abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules. 
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_mixer_priv.h $
* 
* Hydra_Software_Devel/6   9/3/09 5:44p gautamk
* SW7325-388: [7405] Storing Volume Level in both Db and 5.23 Format.
* Also both these paramter remains in Sync when calling different Volume
* APIs
* 
* Hydra_Software_Devel/5   6/17/09 4:50p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/4   6/12/09 3:40p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/3   5/28/09 3:29p gautamk
* PR50455: [7405]
* - During DestinationMute, Muting at both Mixer and O/P level to achieve
* ramping.
* - Exposing API to set the ramp step of Pin/pong coefficents, so that
* ramping could happen at input side of mixer.
* 
* Hydra_Software_Devel/2   10/13/08 5:55p gautamk
* PR43454: [7405] Fixing compilation warning.
* 
* Hydra_Software_Devel/1   10/10/08 11:18a speter
* PR 38950: [3548,3556,7405,7325,7335] Moving references from base2 to
* base3
* 
* Hydra_Software_Devel/18   9/16/08 3:40p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 8:44p sgadara
* PR 44835: [3556] Implementing destination-based volume APIs for volume
* control on capture data.
* 
* Hydra_Software_Devel/17   9/4/08 9:48p sushmit
* PR 46426:[7405] Need to store Mixer Volume levels so that we can
* reapply if needed when one mixer is feeding to other mixers.
* 
* Hydra_Software_Devel/16   8/11/08 5:10p bhanus
* PR43971: [7601] Merging DVD family for 7601 and onward chips
* 
* Hydra_Software_Devel/15   7/21/08 12:39p speter
* PR 44984: [3548,3556,7405] Merging the changes from Rap_NewFwArch
* branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/22/08 2:17p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/14   12/4/07 8:26p gdata
* PR37880: [7440] Merging the Massproduction branch to main branch.
* 
* Hydra_Software_Devel/13   10/18/07 2:14p speter
* PR 36083: [3563] Checking in the new Volume APIs related changes.
* 
* Hydra_Software_Devel/12   10/9/07 6:02p gdata
* PR 35751 : [7440] Exposing the control of enable/disable soft limit
* cofficients to application.
* 
* Hydra_Software_Devel/11   6/19/07 10:44a kagrawal
* PR30069: Added BRAP_MIXER_P_ConvertVolInDbTo5_23Format function.
* 
* Hydra_Software_Devel/10   5/8/07 4:15p bhanus
* PR 30069: Fixed issues in Start and open count of mixer and mixer
* inputs.
* 
* Hydra_Software_Devel/9   4/25/07 12:32p nitinb
* PR 28187: Added function BRAP_MIXER_P_SetOutputVolumeGain,
* BRAP_MIXER_P_GetOutputVolumeGain
* 
* Hydra_Software_Devel/8   4/9/07 8:21p sushmit
* PR 29166:
* Volume Change: Left & Right volume level stored in hOp instead of
* hMixer. SetVolume added during Op_start.
* Mute Status Change: Mute status stored properly in hOp. SetMute added
* during Op_Start.
* Set/Get BTSC Mode: Includes open & run time BTSC mode & config changes.
* Added BtscParams in Inputconfig & CapportParams.
* Tone Control changes: Modified so that these can be changed even before
* start.
* 
* Hydra_Software_Devel/7   3/7/07 11:26a kagrawal
* PR 27912: Checking in SetGain/GetGain code [on behalf of Bhanu]
* 
* Hydra_Software_Devel/6   2/22/07 12:22p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/5   2/16/07 12:35p bhanus
* PR 27912 : Modified for  Static Downmixing Support.
* 
* Hydra_Software_Devel/4   2/8/07 11:09a bhanus
* PR 25607 : Added code for programming user provided mixing
* coefficients.
* 
* Hydra_Software_Devel/3   2/5/07 2:39p bhanus
* PR 25607 : Fixed the Mixer Start Count issue.
* 
* Hydra_Software_Devel/2   9/26/06 6:40p kagrawal
* PR 20654: Fixed bugs in indexed accessing in mixers
* 
* Hydra_Software_Devel/1   8/17/06 12:07p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   8/4/06 7:11p bhanus
* PR 20654: Added BRAP_GetGain() and BRAP_GetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   8/3/06 6:47p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   8/2/06 9:15p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/28/06 10:51p kagrawal
* PR 20654: Added and modifiled StartFmmModules()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   7/28/06 3:03p bhanus
* PR 20654: Added and modified code for BRAP_P_OpenFmmModuleHandles()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   7/28/06 10:37a bhanus
* PR 20654 : Added code for BRAP_SetGain() and BRAP_SetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   6/19/06 11:29a bhanus
* PR 20654: Initial version
***************************************************************************/

#ifndef _BRAP_MIXER_PRIV_H_
#define _BRAP_MIXER_PRIV_H_
#include "brap.h"
#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BRAP_MIXER_P_DEFAULT_BITS_PER_SAMPLE    24
#define BRAP_MIXER_P_DEFAULT_SCALING_COEFF      0x800000 
#define BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23      0x800000 
#define BRAP_MIXER_P_DEFAULT_VOLUME             0
#define BRAP_MIXER_P_MAX_DB_VOLUME              138
#define BRAP_MIXER_P_FRACTION_IN_DB             100

#define BRAP_MIXER_P_COEFF_SIZE_IN_BYTES        4
#define BRAP_MIXER_P_NUM_COEFF_PER_INPUT        4

/***************************************************************************
Summary:
    This enumeration lists various permissible Stream Resolutions at the Mixer.

***************************************************************************/
typedef enum BRAP_MIXER_P_StreamRes
{
	BRAP_MIXER_P_StreamRes_e16=0,   /* 16 bit resolution */
	BRAP_MIXER_P_StreamRes_e17,     /* 17 bit resolution */
	BRAP_MIXER_P_StreamRes_e18,     /* 18 bit resolution */
	BRAP_MIXER_P_StreamRes_e19,     /* 19 bit resolution */
	BRAP_MIXER_P_StreamRes_e20,     /* 20 bit resolution */
	BRAP_MIXER_P_StreamRes_e21,     /* 21 bit resolution */
	BRAP_MIXER_P_StreamRes_e22,     /* 22 bit resolution */
	BRAP_MIXER_P_StreamRes_e23,     /* 23 bit resolution */
	BRAP_MIXER_P_StreamRes_e24      /* 24 bit resolution */
}BRAP_MIXER_P_StreamRes;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening the Mixer.

Description:
    This lists all the parameters required to open the Mixer, 
    including both internal and external parameters.
    Note that the internal parameters are not exposed to the application.
    Currently no settings are required.

See Also:

***************************************************************************/
typedef struct BRAP_MIXER_P_Settings
{
    unsigned int  uiMixerInput[BRAP_RM_P_MAX_OP_CHANNEL_PAIRS]; /* Mixer input */
} BRAP_MIXER_P_Settings;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting the Mixer.
    
Description:
    This lists all the parameters required to start the Mixer, 
    including both internal and external parameters.
    Note that the internal parameters are not exposed to the application.

See Also:

***************************************************************************/
typedef struct BRAP_MIXER_P_Params
{
    bool                        bPriorityHigh;          /* Priority of the client */
    bool                        bCompress;              /* TRUE: The input is compressed data
                                                           FALSE: The input is uncompressed */
    bool                        bOutputEnable0;         /* TRUE: Output 0 of Mixer enabled */
    bool                        bOutputEnable1;         /* TRUE: Output 1 of Mixer enabled */
    unsigned int                uiGroupId;              /* Group Id */
    uint32_t                    ui32InputFciId[BRAP_RM_P_MAX_MIXER_INPUTS];
                                                        /* FCI ID of the input of the Mixer*/
    bool                        bEnableSoftLimit;       /* Enable the softlimit programming*/                                                 
}BRAP_MIXER_P_Params; 

/***************************************************************************
Summary:
    BRAP_P_MixerCoefficientsInfo: Structure that stores the info about the
    programming of the Gain and Pan coefficients and passes this info to the 
    mixer API to program this value.
***************************************************************************/     
typedef struct BRAP_P_MixerCoefficientsInfo
{
    BRAP_MIXER_P_Handle     hMixer[BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC];
                                        /* Array of mixer handles */
    unsigned int            uiMixerIp[BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC]
                                     [BRAP_OutputChannelPair_eMax];
                                        /* Array for respective Mixer Inputs */
    BRAP_Coefficient        sCoeff0[BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC]
                                   [BRAP_OutputChannelPair_eMax];
    BRAP_Coefficient        sCoeff1[BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC]
                                   [BRAP_OutputChannelPair_eMax];
}BRAP_P_MixerCoefficientsInfo;

/***************************************************************************
Summary:
    Abstraction of a Mixer 
    
Description:
    It contains all the information required to handle the Mixer.
    Particularly, it stores the Mixer index, handles for all required chip 
    related information, parent FMM handle, offset required to access 
    different Mixers etc

    Mute can be done at 2 places:
    i) At source channel o/p: This will mute the source channel o/p itself, 
       so if this source channel goes to multiple mixer inputs all of them 
       will be muted. Handled in the Source Channel submodule.
   ii) At mixer output: This will mute the mixer ouput itself.
       Handled in the Mixer submodule.
 
See Also:
***************************************************************************/
typedef struct BRAP_MIXER_P_Object
{
    BCHP_Handle             hChip;              /* Handle to chip object */
    BREG_Handle             hRegister;          /* Handle to register object */
    BMEM_Handle             hHeap;              /* Handle to memory object */
    BINT_Handle             hInt;               /* Handle to interrupt object */
    BRAP_FMM_P_Handle       hFmm;               /* Parent FMM handle */
    
    unsigned int            uiDpIndex;          /* Parent DP Index */
    unsigned int            uiMixerIndex;       /* Mixer Index */
    
    uint32_t                ui32DpOffset;       /* DP Register Offset from start of 
                                                   AUD_FMM_DP_CTRL0_STRM_ENA */
    uint32_t                ui32MixerOffset;    /* Offset of register of current mixer 
                                                   from the corresponding register of 
                                                   first mixer */
    BRAP_P_State            eInputState[BRAP_RM_P_MAX_MIXER_INPUTS]; 
                                               /* State of the various Mixer 
                                                  Input */

    unsigned int            uiMixerOpenCount;   /* Mixer open count */

    bool                    bCompress;          /* TRUE:If the input to the Mixer 
                                                   has compressed data */
    bool                    bOutputMute;        /* Flag to indicate if this mixer 
                                                   output has been muted. This bit 
                                                   is valid only for uncompressed 
                                                   data */
    unsigned int            uiMixerInputOpenCnt[BRAP_RM_P_MAX_MIXER_INPUTS];   
                                                /* Mixer Input open count */

    unsigned int            uiMixerInputStartCnt[BRAP_RM_P_MAX_MIXER_INPUTS];   
                                                /* Mixer Input start count */
    BRAP_MIXER_P_Settings   sSettings;          /* The input settings during OPEN */
    BRAP_MIXER_P_Params     sParams;            /* The input settings during STRAT */
#if (BRAP_7405_FAMILY == 1)
    unsigned int            uiLVolume[BRAP_RM_P_MAX_MIXER_OUTPUTS]; /* Left Volume */
    unsigned int            uiRVolume[BRAP_RM_P_MAX_MIXER_OUTPUTS]; /* Right Volume */
#endif    

}BRAP_MIXER_P_Object;

/***************************************************************************
Summary:
    Opens a Mixer.

Description:
    Initializes the Mixer and returns a Mixer handle.    
    The handle can then be used for all other Mixer function calls.
    Each call to this function configures only one Mixer input.
    It should be called multiple times in order to configure
    different mixer inputs.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_Close, BRAP_MIXER_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_MIXER_P_Open (
    BRAP_FMM_P_Handle           hFmm,       /* [in] Parent FMM handle */
    BRAP_MIXER_P_Handle *       phMixer,    /* [out] Pointer to Mixer handle.
                                               If this Mixer has been opened before, 
                                               this will return the previously created
                                               handle, else, it will return the newly 
                                               created handle */
    unsigned int                uiIndex,    /* [in] Mixer index */
    unsigned int                uiDpIndex,      /* [in] DP index */
    const BRAP_MIXER_P_Settings *pSettings  /* [in] Mixer settings */
);


/***************************************************************************
Summary:
    Releases all the resources associated with this Mixer and frees 
    the handles.

Description:
    Each call to this function 'closes' only one Mixer input.
    It should be called multiple times in order to close
    different mixer inputs. When all Mixer inputs have been closed, 
    this function frees the Mixer handle.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_Open
**************************************************************************/
BERR_Code BRAP_MIXER_P_Close ( 
    BRAP_MIXER_P_Handle     hMixer,       /* [in] Mixer handle */
    unsigned int            uiMixerInput  /* [in] Id of the mixer input 
                                             associated with this audio stream*/
);


/***************************************************************************
Summary:
    Starts ie "Enables" a particular Mixer input.

Description:
    Has to be called mutliple times for various mixer inputs.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_Stop
    
**************************************************************************/
BERR_Code BRAP_MIXER_P_Start ( 
    BRAP_MIXER_P_Handle         hMixer,        /* [in] Mixer handle */
    unsigned int                uiMixerInput,  /* [in] Id of the mixer input 
                                                  associated with this audio stream*/
    const BRAP_MIXER_P_Params   *pParams        /* [in] Pointer to start
                                                   parameters for this stream */ 
);


/***************************************************************************
Summary:
    Stops i.e. "Disables" a particular Mixer input.

Description:
    This routine should be called once per mixer input.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_Start
    
**************************************************************************/
BERR_Code BRAP_MIXER_P_Stop ( 
    BRAP_MIXER_P_Handle         hMixer,        /* [in] Mixer handle */
    unsigned int                uiMixerInput   /* [in] Id of the mixer input 
                                                  associated with this audio stream*/
);

/***************************************************************************
Summary:
    Returns default values for Source Channel Open time settings.

Description:
    For settings that the system cannot assign default values to, 
    an invalid value is returned. Note that the default settings are common
    for all Mixers.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_GetDefaultParams.
**************************************************************************/
BERR_Code 
BRAP_MIXER_P_GetDefaultSettings ( 
    BRAP_MIXER_P_Settings       *pDefSettings   /* [out] Pointer to memory where default
                                                   settings should be written */    
);

/***************************************************************************
Summary:
    Returns default values for Source Channel Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned. Note that the default parameters are common
    for all Mixers.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_MIXER_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_MIXER_P_GetDefaultParams ( 
    BRAP_MIXER_P_Params     *pDefParams   /* [out] Pointer to memory where default
                                             settings should be written */    
);

/***************************************************************************
Summary:
    Sets the Mixer output volume for left and right channels

Description:
    This function sets the Mixer output left and right channel volume.
    Mixer output  volume control is linear in hardware, ranging from 0
    min to 800000 max volume.  This API has done a mapping
    from this linear range to 1/100 DB.  This function 
    gets values in 1/100 DB from 0 (max) to 139000 1/100 DB (min),
    and for all values above 9000 the volume will be set to 0 linear
    or 90 DB.  
    It can be used only for uncompressed data.
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/

    /* B0 Output volume logic should come here */
BERR_Code BRAP_MIXER_P_SetOutputVolume ( 
    BRAP_MIXER_P_Handle     hMixer,         /* [in] Mixer Handle */
    unsigned int            uiMixerOutput,  /* [in] Mixer Output */
    unsigned int            uiLVolume,      /* [in] Left channel volume in 1/100 DB */
    unsigned int            uiRVolume       /* [in] Right channel volume in 1/100 DB*/
);


#if  ((BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1))

BERR_Code BRAP_MIXER_P_SetOutputVolumeGain ( 
    BRAP_MIXER_P_Handle     hMixer,         /* [in] Mixer Handle */
    unsigned int            uiMixerOutput,  /* [in] Mixer Output */    
    unsigned int            uiLVolume,      /* [in] Left channel volume gain in 0.23 format */
    unsigned int            uiRVolume       /* [in] Right channel volume gain in 0.23 format */
);
#endif

/***************************************************************************
Summary:
    Mute/unmute Mixer Output for uncompressed data

Description:
    This function mutes or unmutes output of Mixer.
    It can be used only for uncompressed data.
    These mutes/umutes are ramp up/down mutes by hardware, so
    there is no need to bring the volume down when using these
    mute controls.  

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code 
BRAP_MIXER_P_SetMute ( 
    BRAP_MIXER_P_Handle     hMixer,     /* [in] Mixer Handle */
    bool                    bMute,       /* [in] TRUE: Mute mixer output
                                                FALSE: UnMute mixer output */
    unsigned int        uiMixerOutput /*Mixer Output No.*/                                                           
);

BERR_Code BRAP_MIXER_P_SetMute_isr ( 
    BRAP_MIXER_P_Handle     hMixer,     /* [in] Mixer Handle */
    bool                    bMute,       /* [in] TRUE: Mute mixer output
                                                FALSE: UnMute mixer output */
    unsigned int        uiMixerOutput, /*Mixer Output No.*/                                       
    bool    *bRampRequired   /*Ramp required*/    
);

/***************************************************************************
Summary:
    Programs the ping pong registers for the scaling and paning coefficents

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code BRAP_MIXER_P_ProgramPingPongCoeff (
    BRAP_P_MixerCoefficientsInfo     *psGainInfo,        /* [in] Gain coefficient 
                                                                Info */
    bool                            *pbProgrammingDone  /* [out] Programming of
                                                            new coefficents done
                                                            successfully */
);

/***************************************************************************
Summary:
    Programs the gain information in ping pong registers for scaling.

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
    
BERR_Code BRAP_MIXER_P_SetGainCoeff(
    BRAP_MIXER_P_Handle     hMixer,
    unsigned int            uiMixerInput,
    unsigned int            uiLeftGain,
    unsigned int            uiRightGain,
    unsigned int            uiLeftBalance,
    unsigned int            uiRightBalance    
    );

/***************************************************************************
Summary:
    Converts Volume from Db to 5.23 Format

**************************************************************************/
 unsigned int BRAP_MIXER_P_ConvertVolInDbTo5_23Format(
    unsigned int            uiVol
    );

/***************************************************************************
Summary:
    Converts Volume from 5.23  to 1/100 Db Format
**************************************************************************/
unsigned int BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(
    unsigned int    uiVol
    );
#ifdef __cplusplus
}
#endif
#endif /* _BRAP_MIXER_PRIV_H_ */

/*End of File*/

