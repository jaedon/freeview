/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_mixer.c $
* $brcm_Revision: Hydra_Software_Devel/23 $
* $brcm_Date: 3/29/11 4:13p $
*
* Module Description:
*   Module name: Mixer
*   This file contains the implementation of all PIs for the Mixer 
*   abstraction.  
* 
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_mixer.c $
* 
* Hydra_Software_Devel/23   3/29/11 4:13p gautamk
* SW7405-4713: [7405] Waiting for ramp to complete as per ramp step
* value.
* 
* Hydra_Software_Devel/22   11/8/10 6:46p gautamk
* SW7405-4965: [7405] Mixer volume should not get overridden by default
* if it is already open.
* 
* Hydra_Software_Devel/21   5/20/10 7:45p gautamk
* SW7405-3063: [7405] not modifying handle in watchdog.
* 
* Hydra_Software_Devel/20   2/2/10 2:44p gautamk
* SW7405-3808:[7405] Do not wait for RAMP to complete, when Mixer is in
* stop condition.
* 
* Hydra_Software_Devel/19   10/26/09 12:51p gautamk
* SW7325-543:[7405] fixing for Rightvol as well.
* 
* Hydra_Software_Devel/18   10/16/09 4:35p gautamk
* SW7405-3063: [7405]Changing DBG_WRN to DBG_MSG
* 
* Hydra_Software_Devel/17   10/9/09 4:12p gautamk
* SW7405-3063: [7405] correcting debug mesg
* 
* Hydra_Software_Devel/16   10/2/09 2:35a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/15   9/3/09 5:44p gautamk
* SW7325-388: [7405] Storing Volume Level in both Db and 5.23 Format.
* Also both these paramter remains in Sync when calling different Volume
* APIs
* 
* Hydra_Software_Devel/14   8/18/09 5:15p gautamk
* PR57729: [7405] Programming RAMP_STEP/STEP_SIZE of Mixer and SRC to 0
* only when all the channels are stopped.
* 
* Hydra_Software_Devel/13   7/23/09 3:13p gautamk
* PR56712: [7405] fixing spurious warning
* 
* Hydra_Software_Devel/12   6/18/09 11:50a gautamk
* PR56010: [7405] using BKNI_Sleep(1) instead of some higher value.
* 
* Hydra_Software_Devel/11   6/17/09 4:50p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/10   6/12/09 3:40p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/9   5/28/09 3:29p gautamk
* PR50455: [7405]
* - During DestinationMute, Muting at both Mixer and O/P level to achieve
* ramping.
* - Exposing API to set the ramp step of Pin/pong coefficents, so that
* ramping could happen at input side of mixer.
* 
* Hydra_Software_Devel/8   5/14/09 4:22p gautamk
* PR50455: [7405]Adding APIs BRAP_GetMixerRampStepSize()
* BRAP_SetMixerRampStepSize()
* BRAP_GetSrcRampStepSize()
* BRAP_SetSrcRampStepSize()
* 
* Hydra_Software_Devel/7   3/18/09 12:24p gautamk
* PR53328: [7405]
* PR53268: [7405] Fixing coverity
* 
* Hydra_Software_Devel/6   3/10/09 6:57p gautamk
* PR52383: [7405] WRN to MSG
* 
* Hydra_Software_Devel/5   3/10/09 1:48p gautamk
* PR52383: [7405] correcting debug infomration
* 
* Hydra_Software_Devel/4   2/23/09 11:56a gautamk
* PR52383: [7405] Correcting check for DB value.
* 
* Hydra_Software_Devel/3   1/22/09 5:27p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/2   10/12/08 11:50p gautamk
* PR43454: [7405] fixing compilation and warning.
* 
* Hydra_Software_Devel/1   10/10/08 11:18a speter
* PR 38950: [3548,3556,7405,7325,7335] Moving references from base2 to
* base3
* 
* Hydra_Software_Devel/51   10/10/08 10:26a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/2/08 10:44a speter
* PR 44835: [3548,3556] Adding Mute feature for Destination
* 
* Hydra_Software_Devel/50   9/16/08 3:39p speter
* PR 46942: [3548,3556] Merging back to main line of development to get
* the distinct output feature
* 
* Hydra_Software_Devel/RAP_354856_20080904_BRANCH/1   9/5/08 8:48p sgadara
* PR 44835: [3556] Implementing destination-based volume APIs for volume
* control on capture data.
* 
* Hydra_Software_Devel/49   9/4/08 9:48p sushmit
* PR 46426:[7405] Need to store Mixer Volume levels so that we can
* reapply if needed when one mixer is feeding to other mixers.
* 
* Hydra_Software_Devel/48   8/11/08 5:10p bhanus
* PR43971: [7601] Merging DVD family for 7601 and onward chips
* 
* Hydra_Software_Devel/47   7/21/08 12:33p speter
* PR 44984: [3548,3556,7405] Merging the changes from Rap_NewFwArch
* branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   6/19/08 11:12a sushmit
* PR 43454: [7405] Fixing right volume level
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   6/13/08 2:20a speter
* PR 38950: [3548,3556] Fixing the SRC changes in 3548
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/5/08 11:26a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   4/28/08 4:36p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/22/08 2:17p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 2:12p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch/RAP_NEWFWARCH_WDOG/1   3/28/08 4:06p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/46   12/6/07 4:30p sushmit
* PR 37947: [7405] Checking in the change
* 
* Hydra_Software_Devel/45   11/27/07 10:28a speter
* PR 37577: [3563] Merging branch Phase2.9 to main line
*
* Hydra_Software_Devel/44   11/16/07 2:21p sushmit
* PR 37093: [7405] Fixing Mixer Start/Stop on watchdog recovery
* 
* Hydra_Software_Devel/43   11/2/07 12:13p bhanus
* PR 36673 : [7440] Resetting the mixing coeff to zeros during mixer
* stop.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_9/1   11/5/07 3:52p speter
* PR 36083: [3563] Fixing the Pong part for the volume control
* 
* Hydra_Software_Devel/42   10/18/07 2:14p speter
* PR 36083: [3563] Checking in the new Volume APIs related changes.
* 
* Hydra_Software_Devel/41   10/11/07 6:50p bhanus
* PR 36013: [7440,3563,7405] Programming the defualt volume ramping value
* to 0x200.
* - Enabling DP_PACING_FIX for DP
* 
* Hydra_Software_Devel/40   10/9/07 6:02p gdata
* PR 35751 : [7440] Exposing the control of enable/disable soft limit
* cofficients to application.
* 
* Hydra_Software_Devel/39   9/19/07 10:11a sushmit
* PR 34648: [7405] Fixing Compilation warnings.
* 
* Hydra_Software_Devel/38   8/30/07 10:45a kagrawal
* PR34399: [7440][HiDefDvd 5311,4701] Added code for recovering from
* erroneous DP1 mixer state
* 
* Hydra_Software_Devel/37   7/10/07 6:38p sushmit
* PR 31453: SPDIF Input Watchdog support
* 
* Hydra_Software_Devel/36   6/25/07 5:11p bmishra
* PR 23951: Merged Autotrap related changes
* 
* Hydra_Software_Devel/RAP_UnitTest_Devel/2   6/20/07 7:04p bmishra
* PR 23951: Merging the latest changes
* 
* Hydra_Software_Devel/35   6/19/07 10:44a kagrawal
* PR30069: Added BRAP_MIXER_P_ConvertVolInDbTo5_23Format function.
* 
* Hydra_Software_Devel/34   6/12/07 9:13p sushmit
* PR 27646: Updating 7405 FMM PI
*
* Hydra_Software_Devel/RAP_UnitTest_Devel/1   6/8/07 4:23p bmishra
* PR 23951: Modified for autotrap compatibility.
*
* Hydra_Software_Devel/33   6/5/07 10:40a speter
* PR 31827: [3563] Merging from Phase 2.0 release branch to main
* 
* Hydra_Software_Devel/32   5/28/07 1:33p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/RAP_3563_PH_2_0/1   5/29/07 10:59a speter
* PR 31246: Noise on first time reboot has been fixed. Warnings also
* supressed.
* 
* Hydra_Software_Devel/31   5/23/07 5:52p nitinb
* PR 31498: Merge RAP PI development from phase 1.7 branch to main branch
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/1   5/18/07 4:11p nitinb
* PR 31247: API BRAP_SetOutputVolume doesn't set right volume correctly
* 
* Hydra_Software_Devel/30   5/16/07 4:23p bhanus
* PR 30069 : Removed redundant debug messages
* 
* Hydra_Software_Devel/29   5/16/07 3:08p bhanus
* PR 30069 : Fixed the issue of DP FCI PB ID.
* - Added Function BRAP_MIXER_P_UpdateParams()
* 
* Hydra_Software_Devel/28   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/27   5/8/07 4:14p bhanus
* PR 30069: Fixed issues in Start and open count of mixer and mixer
* inputs.
* 
* Hydra_Software_Devel/26   5/3/07 5:08p gautamk
* PR26996: Exposing RAP PI interface to access toSet/Get RAMP Step size
* for mixer vol control.
* 
* Hydra_Software_Devel/25   5/3/07 4:20p speter
* PR 30400: [3563] Merged the changes done as part of Phase 1.6 Release
* to the main line. Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/24   5/2/07 8:48p bhanus
* PR 30069 : Updated code for allocating Mixer and SRC's from DP Block 1
* and SRC Block 1 for Sequential Paths
* 
* Hydra_Software_Devel/23   5/2/07 12:16p kagrawal
* PR 30069: Adding initial support for Watchdog Recovery
* 
* Hydra_Software_Devel/22   4/25/07 4:45p speter
* PR 29493: [3563/7440] Interface change in SetGain. It now takes 5.23
* format. Code modified to reflect this change.
* 
* Hydra_Software_Devel/21   4/25/07 12:32p nitinb
* PR 28187: Added function BRAP_MIXER_P_SetOutputVolumeGain
* 
* Hydra_Software_Devel/20   4/24/07 5:28p gautamk
* PR30069: Changes for independent volume control. Adding warning
* 
* Hydra_Software_Devel/19   4/24/07 12:16p bhanus
* PR 30069 : Fixed a issue where Mixer GroupID was not getting reset on
* mixer stop.
* 
* Hydra_Software_Devel/18   4/11/07 1:46p kagrawal
* PR 28561: Fixed an incorrect for loop termination logic; Code clean up.
* 
* Hydra_Software_Devel/17   4/9/07 8:17p sushmit
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
* Hydra_Software_Devel/16   3/21/07 10:30p bhanus
* PR 28934 : Fixed the offset calculation for Mixer output Volume
* control.
* 
* Hydra_Software_Devel/15   3/15/07 2:14a bhanus
* PR 28040 : [7440 B0] Merging the changes done during bring up from 7440
* B0 Bring up branch to main branch
* 
* Hydra_Software_Devel/14   3/7/07 11:14a kagrawal
* PR 27912: Checking in SetGain/GetGain code [on behalf of Bhanu]
* 
* Hydra_Software_Devel/13   2/22/07 12:21p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/12   2/16/07 1:08p bhanus
* PR 27912 : Modified for  Static Downmixing Support.
* 
* Hydra_Software_Devel/11   2/8/07 11:09a bhanus
* PR 25607 : Added code for programming user provided mixing
* coefficients.
* 
* Hydra_Software_Devel/10   2/5/07 2:39p bhanus
* PR 25607 : Fixed the Mixer Start Count issue.
* 
* Hydra_Software_Devel/9   1/31/07 4:12p kagrawal
* PR 21368: Added software workaround for DP can mis-align bug
* 
* Hydra_Software_Devel/8   11/17/06 5:17p bhanus
* PR 25607 : Code modified for Mixing bringup
* 
* Hydra_Software_Devel/7   10/6/06 5:42p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/6   10/4/06 5:24p kagrawal
* PR 24742: Fix for no audio from right channel in a stereo content
* 
* Hydra_Software_Devel/5   9/26/06 6:39p kagrawal
* PR 20654: Fixed bugs in indexed accessing of mixers
* 
* Hydra_Software_Devel/4   9/20/06 2:16p kagrawal
* PR 20654: Fixed compilation warnings
* 
* Hydra_Software_Devel/3   9/19/06 6:00p kagrawal
* PR 20654: Added channel change support
* 
* Hydra_Software_Devel/2   9/7/06 4:45p kagrawal
* PR 20654: Fixed bugs found during bring up
* 
* Hydra_Software_Devel/1   8/17/06 12:06p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/10   8/17/06 11:38a kagrawal
* PR 20654: Fixed errors found during emulation
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/9   8/4/06 7:11p bhanus
* PR 20654: Added BRAP_GetGain() and BRAP_GetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   8/3/06 6:47p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   8/2/06 9:15p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   7/28/06 10:51p kagrawal
* PR 20654: Added and modifiled StartFmmModules()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/28/06 3:03p bhanus
* PR 20654: Added and modified code for BRAP_P_OpenFmmModuleHandles()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   7/28/06 10:37a bhanus
* PR 20654 : Added code for BRAP_SetGain() and BRAP_SetPan()
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   6/19/06 11:28a bhanus
* PR 20654: Initial version
* 
***************************************************************************/
#include "brap_mixer_priv.h"

/* TODO: 
    1. To program the soft coefficients at the FMM Init time 
    2. To progarm the ping pong buffers 
    3. To see if the ping pong buffer programming is required at start time
            - it should be called from BRAP_Mixer_P_start or
            - It should be called separately from AM 
    4. To program Stream Resolution in IOP. Read uiOutputBitsPerSample from
       hRap->sOpSettings[eOp].
*/

BDBG_MODULE(rap_mixer);

#define BRAP_MIXER_P_INVALID_MIXER_INPUT        0xffffffff


#define BRAP_MIXER_P_SAME_VOLUME                1

#define BRAP_MIXER_P_COEFF_FRAC_PART1           12
#define BRAP_MIXER_P_COEFF_FRAC_PART2           11
#define BRAP_MIXER_P_COEFF_FRACTION             (BRAP_MIXER_P_COEFF_FRAC_PART1 + \
                                                 BRAP_MIXER_P_COEFF_FRAC_PART2)
#define BRAP_MIXER_P_COEFF_INTEGRAL             5

/* Software workaround for PR 21368: "DP can misalign channel group 
   mixing." Only for 7440 A0.
   
   Algorithm for the workaround:
   -----------------------------
   Mixer Open (Malloc): 
        - All mixer inputs will be enabled & coeffs 0
        - Program FCI MSB and LSB to unused & reserved 12th SRC either in Blk1/2 
   Mixer Start: 
        - Just program the proper mixing coeffs
   Mixer Stop: 
        - Just reset the mixing coeffs to 0
   Mixer Close (Free): 
        - All mixer inputs will be disabled
*/


/******************************************************************************
The array to represent the value of volume in hex corresponding to the value 
in DB. The application inputs the volume in terms of DB and the Corresponding
HEX value is mentioned here. The formula used for the same is:

    HEX = (2^23) * 10^(DB/20)

Note: 23 is the number of bits in the volume control field. 

The volume can range from 0-1. 0 in hex corresponds to the 139 DB from the above
Formula. If application enters more than this value, it is forced to 0.
******************************************************************************/
static const unsigned int  BRAP_Vol_DB[] = 
{
    0x800000,	0x721482,	0x65AC8C,	0x5A9DF7,	0x50C335,
    0x47FACC,	0x4026E7,	0x392CED,	0x32F52C,	0x2D6A86,
    0x287A26,	0x241346,	0x2026F3,	0x1CA7D7,	0x198A13,
    0x16C310,	0x144960,	0x12149A,	0x101D3F,	0xE5CA1,
    0xCCCCC,	0xB6873,	0xA2ADA,	0x90FCB,	0x81385,
    0x732AE,	0x66A4A,	0x5B7B1,	0x51884,	0x48AA7,
    0x40C37,	0x39B87,	0x33718,	0x2DD95,	0x28DCE,
    0x246B4,	0x20756,	0x1CEDC,	0x19C86,	0x16FA9,
    0x147AE,	0x1240B,	0x10449,	0xE7FA,		0xCEC0,
    0xB844,		0xA43A,		0x925E,		0x8273,		0x7443,
    0x679F,		0x5C5A,		0x524F,		0x495B,		0x4161,
    0x3A45,		0x33EF,		0x2E49,		0x2940,		0x24C4,
    0x20C4,		0x1D34,		0x1A07,		0x1732,		0x14AC,
    0x126D,		0x106C,		0xEA3,		0xD0B,		0xBA0,
    0xA5C,		0x93C,		0x83B,		0x755,		0x689,
    0x5D3,		0x531,		0x4A0,		0x420,		0x3AD,
    0x346,		0x2EB,		0x29A,		0x251,		0x211,
    0x1D7,		0x1A4,		0x176,		0x14D,		0x129,
    0xEC,		0xD2,		0xA7,		0x95,		0x84,
    0x76,		0x69,		0x5E,		0x53,		0x4A,
    0x42,		0x3B,		0x34,		0x2F,		0x2A,
    0x25,		0x21,		0x1D,		0x1A,		0x17,
    0x15,		0x12,		0x10,		0xE,		0xD,
    0xB,		0xA,		0x9,		0x8,		0x7,
    0x6,		0x5,		0x5,		0x4,		0x4,
    0x3,		0x3,		0x2,		0x2,		0x2,
    0x2,		0x1,		0x1,		0x1,		0x1,
    0x1,		0x1,		0x1,		0x0
};

static const BRAP_MIXER_P_Settings  defMixerSettings=
{
    {
        BRAP_INVALID_VALUE, /* Mixer Input[4] */
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE
    }

};


static const BRAP_MIXER_P_Params  defMixerParams =
{
    false,                          /* bPriorityHigh */
    false,                          /* bCompress */
    false,                          /* bOutputEnable0 */
    false,                          /* bOutputEnable1 */
    BRAP_INVALID_VALUE,             /* uiGroupId */
    {                               /* ui32InputFciId[8] */
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE,
        BRAP_INVALID_VALUE
    },
    false
};

static BERR_Code BRAP_MIXER_P_HWConfig ( 
    BRAP_MIXER_P_Handle   hMixer,       /* [in] Mixer handle */
    unsigned int          uiMixerInput  /* [in] Id of the mixer input 
                                            associated with this audio stream*/    
);

static 
BERR_Code BRAP_MIXER_P_UpdateParams(
    BRAP_MIXER_P_Handle         hMixer,
    unsigned int                uiMixerInput,
    const BRAP_MIXER_P_Params   *pParams
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
    BRAP_MIXER_P_Settings       *pDefSettings  /* Pointer to memory where default
                                                  settings should be written */
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_MIXER_P_GetDefaultSettings);

    BDBG_ASSERT (pDefSettings);            
    
    *pDefSettings = defMixerSettings;
    BDBG_LEAVE (BRAP_MIXER_P_GetDefaultSettings);
    return ret;
}

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
    BRAP_MIXER_P_Params     *pDefParams   /* Pointer to memory where default
                                             settings should be written */    
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_MIXER_P_GetDefaultParams);

    BDBG_ASSERT (pDefParams);            
    
    *pDefParams = defMixerParams;
    BDBG_LEAVE (BRAP_MIXER_P_GetDefaultParams);
    return ret;
}

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

BERR_Code
BRAP_MIXER_P_Open (
    BRAP_FMM_P_Handle           hFmm,           /* [in] Parent FMM handle */
    BRAP_MIXER_P_Handle         *phMixer,       /* [out] Pointer to Mixer handle.
                                                   If this Mixer has been opened before, 
                                                   this will return the previously created
                                                   handle, else, it will return the newly 
                                                   created handle */
    unsigned int                uiIndex,        /* [in] Mixer index */
    unsigned int                uiDpIndex,      /* [in] DP index */
    const BRAP_MIXER_P_Settings *pSettings      /* [in] Mixer settings */
)
{
    BERR_Code               ret = BERR_SUCCESS;
    BRAP_MIXER_P_Handle     hMixer = NULL;
    unsigned int            i=0;
    bool                    bWdgRecovery = false;

    BDBG_ENTER (BRAP_MIXER_P_Open);

    bWdgRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag(hFmm);

    /* Check all input parameters to the function. Return error if
      - FMM handle is NULL
      - Given index exceeds maximum no. of Mixers
      - Pointer to Settings structure is NULL
      - Pointer to Mixer handle is NULL     */
    BDBG_ASSERT (hFmm);
    BDBG_ASSERT (pSettings);
    BDBG_ASSERT (phMixer);

    if (uiIndex > (BRAP_RM_P_MAX_MIXER_PER_DP_BLCK-1))
    {
        BDBG_ERR(("BRAP_MIXER_P_Open: Invalid Mixer index %d ",uiIndex));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    if (uiDpIndex > (BRAP_RM_P_MAX_DP_BLCK-1))
    {
        BDBG_ERR(("BRAP_MIXER_P_Open: Invalid DP index %d ",uiDpIndex));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    /* Check the Settings structure */
    for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        /* Atleast, one mixer input should be valid */
        if ((pSettings->uiMixerInput[i] != BRAP_INVALID_VALUE) && 
            (pSettings->uiMixerInput[i] < BRAP_RM_P_MAX_MIXER_INPUTS)) 
        {
            break;
        }
    }
    if(i > BRAP_RM_P_MAX_OP_CHANNEL_PAIRS)
    {
        BDBG_ERR(("BRAP_MIXER_P_Open: None Mixer Input is valid"));
        for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
        {
            BDBG_ERR(("pSettings->uiMixerInput[%d] = %d",
                i, pSettings->uiMixerInput[i]));
        }
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
    
    if (false == bWdgRecovery)
    {
        BDBG_MSG (("BRAP_MIXER_P_Open:"
                   " hFmm=0x%x, uiMixerIndex=%d,",
                    hFmm, uiIndex ));
        for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
        {
            BDBG_MSG ((" pSettings->uiMixerInput[%d]=%d",
                           i, pSettings->uiMixerInput[i]));
        }

        if (hFmm->hMixer[uiDpIndex][uiIndex] != NULL) 
        { 
            BDBG_MSG(("BRAP_MIXER_P_Open: Mixer %d is already open", uiIndex));
            /* This Mixer has already been Opened. We are only adding a new Input 
               to the Mixer. Dont allocate new memory or set common settings again. */
            hFmm->hMixer[uiDpIndex][uiIndex]->uiMixerOpenCount++;
            hMixer = hFmm->hMixer[uiDpIndex][uiIndex];
#if 0            /*Volume of the already opened Mixer should not be overwritten with Default value*/
#if (BRAP_7405_FAMILY == 1)
            /* But we do need to set volumes */
            for(i=0;i<BRAP_RM_P_MAX_MIXER_OUTPUTS;i++)
            {
                ret = BRAP_MIXER_P_SetOutputVolume(hMixer,i,
                        hMixer->uiLVolume[i],hMixer->uiRVolume[i]);
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR (("BRAP_MIXER_P_Open: Set Volume Failed"));
                    BKNI_Free(hMixer);
                    return BERR_TRACE(ret);
                }
            }
#endif            
#endif
        }
        else
        {
            /* Allocate memory for the Mixer handle. Fill in parameters in the Mixer
            handle. */
            /* Allocate Mixer handle */
            hMixer = (BRAP_MIXER_P_Handle) BKNI_Malloc (sizeof(BRAP_MIXER_P_Object));
            if (hMixer == NULL)
            {
                BDBG_ERR(("BRAP_MIXER_P_Open: Malloc failed"));
                return BERR_TRACE (BERR_OUT_OF_SYSTEM_MEMORY);
            }

            /* Clear the handle memory */
            BKNI_Memset(hMixer, 0, sizeof(BRAP_MIXER_P_Object));

            /* Initialise elements in Source Channel handle */
            hMixer->hChip = hFmm->hChip;
            hMixer->hRegister = hFmm->hRegister;
            hMixer->hHeap = hFmm->hHeap;
            hMixer->hInt = hFmm->hInt;
            hMixer->hFmm = hFmm;
            hMixer->uiMixerIndex = uiIndex;
            hMixer->bOutputMute = false;        /* UnMute Mixer outputs on open time */
            hMixer->uiDpIndex = uiDpIndex; 
            hMixer->bCompress = false;
            
            BDBG_MSG (("BRAP_MIXER_P_Open: Mixer Id=%d, DP Id=%d", 
               hMixer->uiMixerIndex, hMixer->uiDpIndex));

            /* Mixer Register offset */
            hMixer->ui32MixerOffset = (BCHP_AUD_FMM_DP_CTRL0_MIXER1_INPUT10_CONFIG -
                                            BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG ) * uiIndex;
#if ( BRAP_RM_P_MAX_DP_BLCK > 1 )

            /* DP Register offset for the current mixer */
            hMixer->ui32DpOffset = (BCHP_AUD_FMM_DP_CTRL1_MIXER_OUTPUT_ENA - 
                                        BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA)* hMixer->uiDpIndex;
#else
		    hMixer->ui32DpOffset = 0;
#endif

            for (i=0; i < BRAP_RM_P_MAX_MIXER_INPUTS; i++) 
            {
                hMixer->eInputState[i] = BRAP_P_State_eInvalid;
            }

            /* Set default volume */

            for(i=0;i<BRAP_RM_P_MAX_MIXER_OUTPUTS;i++)
            {
                ret = BRAP_MIXER_P_SetOutputVolume(hMixer,i,
                        BRAP_MIXER_P_DEFAULT_VOLUME,BRAP_MIXER_P_DEFAULT_VOLUME);
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR (("BRAP_MIXER_P_Open: Set Volume Failed"));
                    BKNI_Free(hMixer);
                    return BERR_TRACE(ret);
                }
            }
            
        }
    }
    else
    {
        hMixer = *phMixer;
        /* Set Volume*/
        if(false == hMixer->bCompress)
        {
            for(i=0;i<BRAP_RM_P_MAX_MIXER_OUTPUTS;i++)
            {
#if (BRAP_7405_FAMILY == 1)
                ret = BRAP_MIXER_P_SetOutputVolume(hMixer,i,hMixer->uiLVolume[i],hMixer->uiRVolume[i]);
#else
                ret = BRAP_MIXER_P_SetOutputVolume(hMixer,i,BRAP_MIXER_P_SAME_VOLUME,BRAP_MIXER_P_SAME_VOLUME);
#endif
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR (("BRAP_MIXER_P_Open: Set Volume Failed"));
                    return BERR_TRACE(ret);
                }
            }
        }
    }

    /* Set the Respective states */
    for(i=0;i<BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;i++)
    {
        if(pSettings->uiMixerInput[i] != BRAP_INVALID_VALUE)
        {
            if(BRAP_P_State_eInvalid == hMixer->eInputState[pSettings->uiMixerInput[i]])
            {
                hMixer->eInputState[pSettings->uiMixerInput[i]] = 
                BRAP_P_State_eOpened;
            }
            if (false == bWdgRecovery)
            {
                hMixer->uiMixerInputOpenCnt[pSettings->uiMixerInput[i]]++;
            }
            BDBG_MSG(("Mixer open count =%d",hMixer->uiMixerInputOpenCnt[pSettings->uiMixerInput[i]]));
        }
    }

    if (false == bWdgRecovery)
    { /* If not in WatchDog recovery */  
        
        if (hFmm->hMixer[uiDpIndex][uiIndex] == NULL) 
        {
            /* increment the open count */
            hMixer->uiMixerOpenCount++;
            /* Store the Mixer handle inside the FMM handle */
            hFmm->hMixer[uiDpIndex][uiIndex] = hMixer;
        }   

        /* Return the filled handle */
        *phMixer = hMixer;  
    }
    BDBG_LEAVE (BRAP_MIXER_P_Open);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;
    unsigned int    count=0, i=0;

    BDBG_ENTER (BRAP_MIXER_P_Close);
    BDBG_ASSERT (hMixer);
    BDBG_MSG (("BRAP_MIXER_P_Close: hMixer->uiMixerIndex=%d, uiMixerInput=%d", 
                hMixer->uiMixerIndex, uiMixerInput));

    /* Check for valid Input */
    if (uiMixerInput > (BRAP_RM_P_MAX_MIXER_INPUTS -1))
    {
        BDBG_ERR(("BRAP_MIXER_P_close: Invalid Mixer Input"));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
    hMixer->uiMixerInputOpenCnt[uiMixerInput]--;

    BDBG_MSG(("Mixer open count =%d",hMixer->uiMixerInputOpenCnt[uiMixerInput]));
    if ( 0 == hMixer->uiMixerInputOpenCnt[uiMixerInput])
    {
        hMixer->eInputState[uiMixerInput] = BRAP_P_State_eInvalid;
    }

    /* If there are no valid inputs left to this Mixer, remove all reference
       to it and free the handle. */
       
    for (i=0; i<BRAP_RM_P_MAX_MIXER_INPUTS; i++) 
    {
        if (hMixer->eInputState[i] != BRAP_P_State_eInvalid) 
        { 
            count ++;
        }
    }
    if (count == 0) 
    {

        /* Remove referrence to this Mixer from the parent FMM */ 
        hMixer->hFmm->hMixer[hMixer->uiDpIndex][hMixer->uiMixerIndex] = NULL;

        /* Free the Mixer Handle memory*/
        BKNI_Free (hMixer);     
    }
    else
    {
        /* Reduce the open Count */
        hMixer->uiMixerOpenCount--; 
    }
                 
    BDBG_LEAVE (BRAP_MIXER_P_Close);
    return ret;
}


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
    BRAP_MIXER_P_Handle         hMixer,         /* [in] Mixer handle */
    unsigned int                uiMixerInput,   /* [in] Id of the mixer input 
                                                   associated with this audio stream*/
    const BRAP_MIXER_P_Params   *pParams        /* [in] Pointer to start
                                                   parameters for this stream */ 
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    bool            bWdgRecovery = false;
    int             i = 0;   
    unsigned int    uiTemp = 0;
    
    BDBG_ENTER (BRAP_MIXER_P_Start);
    BDBG_ASSERT (hMixer);
    bWdgRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag(hMixer->hFmm);
    
    /* Check for Mixer Input */
    if (uiMixerInput > (BRAP_RM_P_MAX_MIXER_INPUTS -1))
    {
        BDBG_ERR(("BRAP_MIXER_P_Start: Invalid Mixer Input"));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }   

    if ((false == bWdgRecovery) 
        && (0 != hMixer->uiMixerInputStartCnt[uiMixerInput] ) )
    {
        BDBG_MSG(("BRAP_MIXER_P_Start: Input stream %d to Mixer %d is already active."
                    "Ignoring the start command.",  
                    uiMixerInput, hMixer->uiMixerIndex));
        
        hMixer->uiMixerInputStartCnt[uiMixerInput]++;

        BDBG_MSG(("Mixer Start Count =%d",hMixer->uiMixerInputStartCnt[uiMixerInput]));
        
        BDBG_LEAVE (BRAP_MIXER_P_Start);
        return BERR_SUCCESS;        
    }
    else if (hMixer->eInputState[uiMixerInput] == BRAP_P_State_eInvalid) 
    {
        BDBG_ERR(("BRAP_MIXER_P_Start: Input stream %d to Mixer %d has not been opened."
                    "Cant start it!!",  
                    uiMixerInput, hMixer->uiMixerIndex));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    if(false == bWdgRecovery)
    {
        BDBG_ASSERT (pParams);


        ret = BRAP_MIXER_P_UpdateParams(hMixer, uiMixerInput,pParams);
        if (ret != BERR_SUCCESS)
        {
            BDBG_ERR(("BRAP_MIXER_P_Start: BRAP_MIXER_P_UpdateParams returned %d",ret));
            return BERR_TRACE(ret);
        }

    }
    
    /* Configure Mixer Hardware as per the channel parameters */
    BRAP_MIXER_P_HWConfig (hMixer, uiMixerInput);

    /* Read the Output Enable Register */
    ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                   (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA));

    /* Check if all the inputs are running on same Priority */
    for (i =0; i<BRAP_RM_P_MAX_MIXER_INPUTS; i++)
    {
        if(BRAP_P_State_eStarted == hMixer->eInputState[i])
        {
            uiTemp = (ui32RegVal >> 
                     (BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA_MIXER0_HIGH_PRIORITY_SHIFT + hMixer->uiMixerIndex))&
                     0x1;
            if(uiTemp != hMixer->sParams.bPriorityHigh)
            {
                BDBG_ERR(("BRAP_MIXER_P_Start: Two Inputs of one Mixer can not be started with Different priority "));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
    }
    
    /* Program the Priority of the client and Enable Mixer output */
    switch (hMixer->uiMixerIndex)
    {
        case 0:
            if(true == hMixer->sParams.bPriorityHigh)
            {
				ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_HIGH_PRIORITY, Normal);
            }
			
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT1_ENA, Enable);
            }
            break;
        case 1:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_OUTPUT1_ENA, Enable);
            }
            break;
        case 2:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_OUTPUT1_ENA, Enable);
            }
            break;
        case 3:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_OUTPUT1_ENA, Enable);
            }
            break;
        case 4:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_OUTPUT1_ENA, Enable);
            }
            break;
        case 5:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_OUTPUT1_ENA, Enable);
            }
            break;
        case 6:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_OUTPUT1_ENA, Enable);
            }
            break;
        case 7:
            if(true == hMixer->sParams.bPriorityHigh)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_HIGH_PRIORITY, High);
            }
            else
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_HIGH_PRIORITY));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_HIGH_PRIORITY, Normal);
            }
            if(true == hMixer->sParams.bOutputEnable0)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_OUTPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_OUTPUT0_ENA, Enable);
            }
            if(true == hMixer->sParams.bOutputEnable1)
            {
                ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_OUTPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_OUTPUT1_ENA, Enable);
            }
            break;
        default:
            BDBG_ERR(("BRAP_MIXER_P_Start: Invalid Mixer Index %d",hMixer->uiMixerIndex));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    BRAP_Write32 ( hMixer->hRegister,
                               (hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA),
                                ui32RegVal);
    

    if(false == bWdgRecovery)
    {
        hMixer->uiMixerInputStartCnt[uiMixerInput]++;
    /* Set the Mixer state to Started */
    hMixer->eInputState[uiMixerInput] = BRAP_P_State_eStarted;
    }
    BDBG_MSG(("Mixer Start Count =%d",hMixer->uiMixerInputStartCnt[uiMixerInput]));

    /* Fix for DP1 SM machine error recovery - HiDef-DVD PR5311 & PR4701 */
    if(hMixer->uiDpIndex == 1)
    {
        ui32RegVal = 0;
        ui32RegVal = BRAP_Read32 (hMixer->hRegister,(hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_DP_SM));
        ui32RegVal &= (BCHP_MASK(AUD_FMM_DP_CTRL0_DP_SM, DP_MIXER_SM));

        if(ui32RegVal != 0)
        {
            ui32RegVal = 0;
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,(hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_DP_MISC_CTRL));
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_DP_MISC_CTRL, RESET_DP_MIXER_SM));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_DP_MISC_CTRL, RESET_DP_MIXER_SM, Reset);

            BRAP_Write32 (hMixer->hRegister,(hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_DP_MISC_CTRL), ui32RegVal);
            ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_DP_MISC_CTRL, RESET_DP_MIXER_SM));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_DP_MISC_CTRL, RESET_DP_MIXER_SM, Unreset);
            BRAP_Write32 (hMixer->hRegister,(hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_DP_MISC_CTRL), ui32RegVal);            
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,(hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_DP_SM));            
        }
    }

    BDBG_LEAVE (BRAP_MIXER_P_Start);
    return ret;
}

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
    BRAP_MIXER_P_Handle     hMixer,        /* [in] Mixer handle */
    unsigned int            uiMixerInput   /* [in] Id of the mixer input 
                                              associated with this audio stream*/
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0,ui32RegOffset=0,ui32RegAdd=0;
    unsigned int    temp= 0;
    bool            bDisableMixer = true;
    bool            bWdgRecovery = false;

    BDBG_ENTER (BRAP_MIXER_P_Stop);
    BDBG_ASSERT (hMixer);
    BDBG_MSG (("BRAP_MIXER_P_Stop:"
               "hMixer->uiMixerIndex=%d, uiMixerInput=%d", 
                hMixer->uiMixerIndex, uiMixerInput));
    bWdgRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag(hMixer->hFmm);
  
    if (uiMixerInput > (BRAP_RM_P_MAX_MIXER_INPUTS -1))
    {
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }       

    if (hMixer->eInputState[uiMixerInput] != BRAP_P_State_eStarted)
    {
        BDBG_WRN(("BRAP_MIXER_P_Stop: Input stream %d to Mixer %d is not active."
                    "Ignoring the stop command.",  
                    uiMixerInput, hMixer->uiMixerIndex));
        BDBG_LEAVE (BRAP_MIXER_P_Stop);
        return BERR_SUCCESS;
    }

    for (temp=0; temp<BRAP_RM_P_MAX_MIXER_INPUTS; temp++) 
    {
        if ((BRAP_P_State_eStarted == hMixer->eInputState[temp]) && 
            (temp != uiMixerInput))
        {
            /* Some other input of the mixer is in the running state, so
               do not disable the mixer output */
            bDisableMixer = false;
        }
    }

    if(false == bWdgRecovery)
        hMixer->uiMixerInputStartCnt[uiMixerInput]--;
    BDBG_MSG(("Mixer Start Count =%d",hMixer->uiMixerInputStartCnt[uiMixerInput]));


    if ((true==bDisableMixer) && (0==hMixer->uiMixerInputStartCnt[uiMixerInput]))
    {
	    /* Read the Output Enable Register */
        ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                   (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA));
    
        /* Disable Mixer output */
        switch (hMixer->uiMixerIndex)
        {
            case 0:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER0_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT1_ENA, Disable);
                }
                break;
            case 1:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER1_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER1_OUTPUT1_ENA, Disable);
                }
                break;
            case 2:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER2_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER2_OUTPUT1_ENA, Disable);
                }
                break;
            case 3:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER3_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER3_OUTPUT1_ENA, Disable);
                }
                break;
            case 4:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER4_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER4_OUTPUT1_ENA, Disable);
                }
                break;
            case 5:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER5_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER5_OUTPUT1_ENA, Disable);
                }
                break;
            case 6:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER6_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER6_OUTPUT1_ENA, Disable);
                }
                break;
            case 7:
                if(true == hMixer->sParams.bOutputEnable0)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_OUTPUT0_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_OUTPUT0_ENA, Disable);
                }
                if(true == hMixer->sParams.bOutputEnable1)
                {
                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA,MIXER7_OUTPUT1_ENA));
                    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER7_OUTPUT1_ENA, Disable);
                }
                break;
            default:
                BDBG_ERR(("BRAP_MIXER_P_Start: Invalid Mixer Index %d",hMixer->uiMixerIndex));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
        }

        BRAP_Write32 ( hMixer->hRegister,
                               (hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA),
                                ui32RegVal);

        /* Reset the Grouping Field */
        ui32RegVal = 0;
        ui32RegVal = BRAP_Read32(hMixer->hRegister, (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                                        + hMixer->ui32MixerOffset));
        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, MIXER_GROUP_BEGIN));
        ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, MIXER_GROUP_BEGIN, hMixer->uiMixerIndex);


        BRAP_Write32 (hMixer->hRegister, 
                                (hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                                + hMixer->ui32MixerOffset), 
                                ui32RegVal);
    }


    if(0 == hMixer->uiMixerInputStartCnt[uiMixerInput])
    {
        /* Disable mixing for this mixer input */
        switch (uiMixerInput)
        {
            case 0:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT0_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 1:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT1_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 2:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT2_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT2_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 3:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT3_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT3_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 4:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT4_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT4_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 5:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT5_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT5_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 6:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT6_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT6_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            case 7:
                ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                               (hMixer->ui32DpOffset 
                                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                                + hMixer->ui32MixerOffset));
                ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT7_ENA));
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT7_ENA, Disable);
                BRAP_Write32 (hMixer->hRegister, 
                                        (hMixer->ui32DpOffset 
                                        + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                        + hMixer->ui32MixerOffset), 
                                        ui32RegVal);
                break;
            default:
                BDBG_ERR(("BRAP_MIXER_P_Start: Invalid Mixer Index %d",hMixer->uiMixerIndex));
                return BERR_TRACE (BERR_INVALID_PARAMETER);
        }
    }

    if (0 == hMixer->uiMixerInputStartCnt[uiMixerInput])
    {
#if ((!((BCHP_CHIP == 7440)&&(BCHP_VER == BCHP_VER_A0))) && (BCHP_CHIP != 3563))    
        unsigned int j=0;
#endif        
        hMixer->eInputState[uiMixerInput] = BRAP_P_State_eOpened;
        hMixer->bCompress = false;

        for(j=0;j<BRAP_RM_P_MAX_MIXER_PB_FCI_ID;j++)
        {
            if (hMixer->sParams.ui32InputFciId[uiMixerInput] == 
                               hMixer->hFmm->ui32MixerPbFciId[hMixer->uiDpIndex][j])
            {
                /* The matching FCI Id is already in the Array */
                BDBG_ASSERT(0 != hMixer->hFmm->ui32MixerPbFciIdCount[hMixer->uiDpIndex][j]);
                            
                hMixer->hFmm->ui32MixerPbFciIdCount[hMixer->uiDpIndex][j]--;
                if(0 == hMixer->hFmm->ui32MixerPbFciIdCount[hMixer->uiDpIndex][j])
                {
                    hMixer->hFmm->ui32MixerPbFciId[hMixer->uiDpIndex][j]= 0xFFFFFFFF;

                    BRAP_Write32 (hMixer->hRegister,(hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE
                                + (4*j)),0x0);
                }
                break;
            }
        }

        hMixer->sParams.ui32InputFciId[uiMixerInput] = BRAP_INVALID_VALUE;
    }

     /* Reset the Mixing Coeff to 0 */

    if (0 == hMixer->uiMixerInputStartCnt[uiMixerInput])
    {
        ui32RegOffset = \
              (  BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * 
                 BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                 BRAP_RM_P_MAX_MIXER_INPUTS       * 
                 hMixer->uiMixerIndex
              )                                   +

              (  BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * 
                 BRAP_MIXER_P_NUM_COEFF_PER_INPUT * 
                 uiMixerInput
              );
            
        /* Calculate the Ping Address for Left 0 Coefficient and program coeff */
        ui32RegAdd = hMixer->ui32DpOffset + 
                    BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE + ui32RegOffset;

        for(temp=0;temp<BRAP_MIXER_P_NUM_COEFF_PER_INPUT;temp++)
        {
            
            BRAP_Write32(hMixer->hRegister, ui32RegAdd, 0);
            ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
        }

        /* Calculate the Pong Address for Left 0 Coefficient and program coeff */
        ui32RegAdd = hMixer->ui32DpOffset + 
                    BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE + ui32RegOffset;
        for(temp=0;temp<BRAP_MIXER_P_NUM_COEFF_PER_INPUT;temp++)
        {
            
            BRAP_Write32(hMixer->hRegister, ui32RegAdd, 0);
            ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
        }
    }


    
    BDBG_LEAVE (BRAP_MIXER_P_Stop);
    
    return ret;
}


/***************************************************************************
Summary:
    Configures the HW registers for the Mixer
Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    None.
**************************************************************************/

static BERR_Code BRAP_MIXER_P_HWConfig ( 
    BRAP_MIXER_P_Handle   hMixer,       /* [in] Mixer handle */
    unsigned int          uiMixerInput  /* [in] Id of the mixer input 
                                            associated with this audio stream*/    
)
{
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            ui32RegVal = 0;
    uint32_t            ui32RegAdd = 0;
    uint32_t            ui32FciIdMsb = 0;
    uint32_t            ui32FciIdLsb = 0; 
    BRAP_MIXER_P_Params sParams;
    bool            bWdgRecovery = false;
    unsigned int        j=0;
    uint32_t            ui32IndexAvailable = 0xFFFFFFFF;

    
    BDBG_ENTER (BRAP_MIXER_P_HWConfig);
    BDBG_ASSERT (hMixer);
    BDBG_MSG (("BRAP_MIXER_P_HWConfig:"
               "hMixer->uiMixerIndex=%d, uiMixerInput=%d, DP Id =%d", 
                hMixer->uiMixerIndex, uiMixerInput, hMixer->uiDpIndex));
  
    if (uiMixerInput > (BRAP_RM_P_MAX_MIXER_INPUTS -1))
    {
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }       
    
    bWdgRecovery = BRAP_FMM_P_GetWatchdogRecoveryFlag(hMixer->hFmm);
    sParams = hMixer->sParams;

    /* Program the Stream Resolution */
    /* TODO */

    /* Program the Input FCI ID.
       FCI ID[9:0] is combination of the Block and the resource in the block. Its a 10 bit
       value. Bits[9:6] give the block Id and bits[5:0] give the rsource in that block */

    BRAP_P_DECODE_FCI_ID(sParams.ui32InputFciId[uiMixerInput],ui32FciIdMsb,ui32FciIdLsb);    

    BDBG_MSG(("FCI ID = 0x%x , FCI_ID_LSB = 0x%x, FCI_ID_MSB = 0x%x",
                sParams.ui32InputFciId[uiMixerInput],ui32FciIdLsb,ui32FciIdMsb));

    /* 7440 B0 based chips FCI Logic should come here */
    for(j=0;j<BRAP_RM_P_MAX_MIXER_PB_FCI_ID;j++)
    {
        if (sParams.ui32InputFciId[uiMixerInput] == 
                           hMixer->hFmm->ui32MixerPbFciId[hMixer->uiDpIndex][j])
        {
            /* The matching FCI Id is already in the Array */
            ui32IndexAvailable = j;
            break;
        }
        else if ((0xFFFFFFFF == hMixer->hFmm->ui32MixerPbFciId[hMixer->uiDpIndex][j])&&
                 (0xFFFFFFFF == ui32IndexAvailable))
        {
            ui32IndexAvailable = j;
        }
    }
	BDBG_MSG((" ui32IndexAvailable =%d", ui32IndexAvailable));

    if(false == bWdgRecovery)
    {
    hMixer->hFmm->ui32MixerPbFciId[hMixer->uiDpIndex][ui32IndexAvailable]=
                                            sParams.ui32InputFciId[uiMixerInput];
    hMixer->hFmm->ui32MixerPbFciIdCount[hMixer->uiDpIndex][ui32IndexAvailable]++;
    }

    ui32RegVal = BRAP_Read32 (hMixer->hRegister,(hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE
                                + (4*ui32IndexAvailable)));

    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_DP_CTRL0_PB_FCI_IDi, PB_NUMBER_FCI_ID));
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_DP_CTRL0_PB_FCI_IDi, PB_NUMBER_FCI_ID,
                                        sParams.ui32InputFciId[uiMixerInput]));

    BDBG_MSG(("DP sParams InputFciId[%d] = %d\n",uiMixerInput,sParams.ui32InputFciId[uiMixerInput]));
    
    BRAP_Write32 (hMixer->hRegister,(hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE
                                + (4*ui32IndexAvailable)),ui32RegVal);

    switch (uiMixerInput)
    {
        case 0:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT0_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT0_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_ENA, Enable);


            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 1:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT1_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG,MIXER_INPUT1_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 2:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT2_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT2_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT2_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT2_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 3:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT3_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG,MIXER_INPUT3_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT3_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG, MIXER_INPUT3_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT32_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 4:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT4_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT4_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT4_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT4_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 5:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT5_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG,MIXER_INPUT5_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT5_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG, MIXER_INPUT5_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT54_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 6:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                            + hMixer->ui32MixerOffset));

            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT6_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT6_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT6_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT6_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        case 7:
            ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                                           (hMixer->ui32DpOffset 
                                            + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                            + hMixer->ui32MixerOffset));


            ui32RegVal &= ~( BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT7_PB_NUMBER)
                            |BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG,MIXER_INPUT7_ENA));

            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT7_PB_NUMBER,ui32IndexAvailable);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG, MIXER_INPUT7_ENA, Enable);

            BRAP_Write32 (hMixer->hRegister, 
                                    (hMixer->ui32DpOffset 
                                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT76_CONFIG
                                    + hMixer->ui32MixerOffset), 
                                    ui32RegVal);
            break;
        default:
            BDBG_ERR(("BRAP_MIXER_P_Start: Invalid Mixer Index %d",hMixer->uiMixerIndex));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
   
    /* Start afresh */
    ui32RegVal = 0;

    if(false == hMixer->sParams.bCompress)
    {
#ifdef AUTOTRAP
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA, Disable); 
#else
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA, Enable); 
#endif /* AUTOTRAP */ 
    }
    else
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA, Disable);     
    }

    if(true == hMixer->sParams.bOutputEnable0)
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT0, Disable);
    }
    else
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT0, Enable);
    }

    if(true == hMixer->sParams.bOutputEnable1)
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT1, Disable);
    }
    else
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT1, Enable);
    }
    

    if(false == hMixer->sParams.bEnableSoftLimit)
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, SOFT_LIMIT_EN, Disable); 
    }
    else
    {
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, SOFT_LIMIT_EN, Enable); 
    }

    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, MIXER_GROUP_BEGIN, sParams.uiGroupId);
    BRAP_Write32 (hMixer->hRegister, 
                                (hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                                + hMixer->ui32MixerOffset), 
                                ui32RegVal);

    /* Program the Scaling coefficients to Default value */
    /* Note: 
            Lout = Left_Coef_0 * Lin + Right_Coef_0 * Rin
            Rout = Left_Coef_1 * Lin + Right_Coef_1 * Rin 
    */
    
    /* Ping: Left_Coef_0 */
    ui32RegAdd = hMixer->ui32DpOffset + 
                 BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE +
                 (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                 BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) +
                 (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * uiMixerInput);

        BRAP_Write32(hMixer->hRegister, ui32RegAdd, BRAP_MIXER_P_DEFAULT_SCALING_COEFF);


    /* Ping: Right_Coef_1 */
    ui32RegAdd = hMixer->ui32DpOffset + 
                 BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE +
                 (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                 BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) + 
                 (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * (BRAP_MIXER_P_NUM_COEFF_PER_INPUT-1)) +
                 (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * uiMixerInput);
    

        BRAP_Write32(hMixer->hRegister, ui32RegAdd, BRAP_MIXER_P_DEFAULT_SCALING_COEFF);

    /* Pong: Left_Coef_0 */
    ui32RegAdd = hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE+
                (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) +
                (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * uiMixerInput);

        BRAP_Write32(hMixer->hRegister, ui32RegAdd, BRAP_MIXER_P_DEFAULT_SCALING_COEFF);

    /* Pong: Right_Coef_1 */
    ui32RegAdd = hMixer->ui32DpOffset + BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE+
                (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) +
                (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * (BRAP_MIXER_P_NUM_COEFF_PER_INPUT-1)) +
                (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * uiMixerInput);

        BRAP_Write32(hMixer->hRegister, ui32RegAdd, BRAP_MIXER_P_DEFAULT_SCALING_COEFF);


    BDBG_LEAVE (BRAP_MIXER_P_HWConfig);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;    
    BREG_Handle     hRegister =NULL ;
    uint32_t        ui32AttenDb = 0, ui32Delta=0, ui32Temp=0;
    uint32_t        ui32RegVal = 0;
    uint32_t        ui32LVolRegOffset =0;
    uint32_t        ui32RVolRegOffset =0;
    uint32_t        ui32OpVolRegOffset =0;
    
    BDBG_ENTER (BRAP_MIXER_P_SetOutputVolume);
    BDBG_ASSERT (hMixer);
    
    if (true == hMixer->bCompress)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetOutputVolume(): valid only for uncompressed data."));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }    

    if(BRAP_RM_P_MAX_MIXER_OUTPUTS <= uiMixerOutput)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetOutputVolume(): Invalid output of the Mixer"));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    BDBG_MSG(("uiMixerOutput > %d", uiMixerOutput));
    BDBG_MSG (("BRAP_MIXER_SetOutputVolume():" 
               "Mixer Index = %d, uiLVolume=%d, uiRVolume=%d", 
                hMixer->uiMixerIndex, uiLVolume, uiRVolume));

    hRegister = hMixer->hRegister;
#if (BRAP_7405_FAMILY == 1)
    hMixer->uiLVolume[uiMixerOutput] = uiLVolume;
    hMixer->uiRVolume[uiMixerOutput] = uiRVolume;
#endif    

    /* Calculate the Volume Register offsets */
    if(1 == uiMixerOutput)
    {
        ui32OpVolRegOffset = ( BCHP_AUD_FMM_DP_CTRL0_MIXER01_LT_VOL_LEVEL -
                                    BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL);
    }
    else if(0 == uiMixerOutput)
    {
        ui32OpVolRegOffset = 0;
    }
    
    ui32LVolRegOffset = (BCHP_AUD_FMM_DP_CTRL0_MIXER10_LT_VOL_LEVEL - 
                        BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL)* hMixer->uiMixerIndex ;
    ui32RVolRegOffset = (BCHP_AUD_FMM_DP_CTRL0_MIXER10_RT_VOL_LEVEL - 
                        BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL)* hMixer->uiMixerIndex ;
    BDBG_MSG(("ui32OpVolRegOffset = %d, ui32LVolRegOffset =%d, ui32RVolRegOffset = %d",
                            ui32OpVolRegOffset,ui32LVolRegOffset,ui32RVolRegOffset));
    /* Calculate register value for Left Channel volume */ 
	/* Since we take the volume level from application in multiple of 100 i.e. if application
       wants set volume level tp 80, it needs to provide 8000 as volume. This is done to provide
       more granularity in volume control. Now if application passes 8550 DB as volume. But we 
       do not have hex value corresponding to 8550/100 = 85.50, so we calculte it by finding the
       fraction part and calculating the hex value accordingly. SO the calculation for this ex is 
                 uiLvolume   = 8550
                 ui32AttenDb = 8550/100 = 85 (So we loose .50 here)
                 ui32temp    = BRAP_VOL_DB[85]
                 ui32Delta   = ui32temp - BRAP_VOL_DB[86]
                 ui32Delta   = ui32Delta * ((uiLVolume%100)/100)
                             = ui32Delta * ((8550%100)/100)
                             = 0.5 * ui32Delta
                 uiTemp     -= ui32delta (So we accomodate the extra .5DB entered by Applicaion)*/
                 
    ui32AttenDb = uiLVolume/BRAP_MIXER_P_FRACTION_IN_DB;
    
    if (ui32AttenDb < BRAP_MIXER_P_MAX_DB_VOLUME)
    {

        ui32Temp = BRAP_Vol_DB[ui32AttenDb];
        ui32Delta = ui32Temp - BRAP_Vol_DB[ui32AttenDb+1];
        ui32Delta = (ui32Delta * (uiLVolume%BRAP_MIXER_P_FRACTION_IN_DB))
                        /(BRAP_MIXER_P_FRACTION_IN_DB);
        ui32Temp -= ui32Delta;
    }       
    else if (ui32AttenDb == BRAP_MIXER_P_MAX_DB_VOLUME)    
    {
        uiLVolume = BRAP_MIXER_P_MAX_DB_VOLUME 
                    * BRAP_MIXER_P_FRACTION_IN_DB;
        ui32Temp = 0;
    }       
    else
    {
    	BDBG_WRN(("BRAP_MIXER_SetOutputVolume(): Left volume %#x [Out of valid range]"
				  "forcing it to 13800",uiLVolume));
        uiLVolume = BRAP_MIXER_P_MAX_DB_VOLUME 
                    * BRAP_MIXER_P_FRACTION_IN_DB;
        ui32Temp = 0;
    }
     
    
    ui32RegVal |= (BCHP_FIELD_DATA ( AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL,
                                     MIXER_OUTPUT_LEFT_VOLUME_LEVEL,
                                     ui32Temp));
    BRAP_Write32 (hRegister, 
                 ((hMixer->ui32DpOffset + ui32LVolRegOffset)+
                 BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL + ui32OpVolRegOffset), 
                 ui32RegVal);

    /* Calculate register value for Right Channel volume */ 
    ui32AttenDb = uiRVolume/BRAP_MIXER_P_FRACTION_IN_DB;
    
    if (ui32AttenDb < BRAP_MIXER_P_MAX_DB_VOLUME)
    {

        ui32Temp = BRAP_Vol_DB[ui32AttenDb];
        ui32Delta = ui32Temp - BRAP_Vol_DB[ui32AttenDb+1];
        ui32Delta = (ui32Delta * (uiRVolume%BRAP_MIXER_P_FRACTION_IN_DB))
                        /(BRAP_MIXER_P_FRACTION_IN_DB);
        ui32Temp -= ui32Delta;
    }       
    else if (ui32AttenDb == BRAP_MIXER_P_MAX_DB_VOLUME)    
    {
        uiRVolume = BRAP_MIXER_P_MAX_DB_VOLUME 
                    * BRAP_MIXER_P_FRACTION_IN_DB;
        ui32Temp=0;
    }
    else
    {
    	BDBG_WRN(("BRAP_MIXER_SetOutputVolume(): Right volume %#x [Out of valid range]"
				  "forcing it to 13800",uiRVolume));
        uiRVolume = BRAP_MIXER_P_MAX_DB_VOLUME 
                    * BRAP_MIXER_P_FRACTION_IN_DB;
        ui32Temp = 0;
    }
    
    ui32RegVal  = 0;	
    ui32RegVal |= (BCHP_FIELD_DATA ( AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL, 
                                     MIXER_OUTPUT_RIGHT_VOLUME_LEVEL,
                                     ui32Temp));  
    
    BRAP_Write32 (hRegister, 
                 ((hMixer->ui32DpOffset + ui32RVolRegOffset)+
                 BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL+ ui32OpVolRegOffset), 
                 ui32RegVal); 

    BDBG_LEAVE (BRAP_MIXER_P_SetOutputVolume);
    return ret;
}

#if ((BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1))

BERR_Code BRAP_MIXER_P_SetOutputVolumeGain ( 
    BRAP_MIXER_P_Handle     hMixer,         /* [in] Mixer Handle */
    unsigned int            uiMixerOutput,  /* [in] Mixer Output */
    unsigned int            uiLVolume,      /* [in] Left channel volume in 5.23 */
    unsigned int            uiRVolume       /* [in] Right channel volume in 5.23*/
)
{
    BERR_Code       ret = BERR_SUCCESS;    
    BREG_Handle     hRegister =NULL ;
    uint32_t        ui32RegVal = 0;
    uint32_t        ui32LVolRegOffset =0;
    uint32_t        ui32RVolRegOffset =0;
    uint32_t        ui32OpVolRegOffset =0;
    
    BDBG_ENTER (BRAP_MIXER_P_SetOutputVolumeGain);
    BDBG_ASSERT (hMixer);
    
    if (true == hMixer->bCompress)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetOutputVolumeGain(): valid only for uncompressed data."));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }    
    
    if(BRAP_RM_P_MAX_MIXER_OUTPUTS <= uiMixerOutput)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetOutputVolumeGain(): Invalid output of the Mixer"));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    
    BDBG_MSG(("uiMixerOutput > %d", uiMixerOutput));
    BDBG_MSG (("BRAP_MIXER_P_SetOutputVolumeGain():" 
               "Mixer Index = %d, uiLVolume=%d, uiRVolume=%d", 
                hMixer->uiMixerIndex, uiLVolume, uiRVolume));

    hRegister = hMixer->hRegister;

    /* Calculate the Volume Register offsets */
    if(1 == uiMixerOutput)
    {
        ui32OpVolRegOffset = ( BCHP_AUD_FMM_DP_CTRL0_MIXER01_LT_VOL_LEVEL -
                                    BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL);
    }
    else if(0 == uiMixerOutput)
    {
        ui32OpVolRegOffset = 0;
    }
    
    ui32LVolRegOffset = (BCHP_AUD_FMM_DP_CTRL0_MIXER10_LT_VOL_LEVEL - 
                        BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL)* hMixer->uiMixerIndex ;
    ui32RVolRegOffset = (BCHP_AUD_FMM_DP_CTRL0_MIXER10_RT_VOL_LEVEL - 
                        BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL)* hMixer->uiMixerIndex ;
    BDBG_MSG(("ui32OpVolRegOffset = %d, ui32LVolRegOffset =%d, ui32RVolRegOffset = %d",
                            ui32OpVolRegOffset,ui32LVolRegOffset,ui32RVolRegOffset));
    ui32RegVal |= (BCHP_FIELD_DATA ( AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL,
                                     MIXER_OUTPUT_LEFT_VOLUME_LEVEL,
                                     uiLVolume));
    BRAP_Write32 (hRegister, 
                 ((hMixer->ui32DpOffset + ui32LVolRegOffset)+
                 BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL + ui32OpVolRegOffset), 
                 ui32RegVal);

    ui32RegVal  = 0;	
    ui32RegVal |= (BCHP_FIELD_DATA ( AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL, 
                                     MIXER_OUTPUT_RIGHT_VOLUME_LEVEL,
                                     uiRVolume));  
    BRAP_Write32 (hRegister, 
                 ((hMixer->ui32DpOffset + ui32RVolRegOffset)+
                 BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL+ ui32OpVolRegOffset), 
                 ui32RegVal); 

    BDBG_LEAVE (BRAP_MIXER_P_SetOutputVolumeGain);
    return ret;
}

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
BERR_Code BRAP_MIXER_P_SetMute ( 
    BRAP_MIXER_P_Handle     hMixer,     /* [in] Mixer Handle */
    bool                    bMute,       /* [in] TRUE: Mute mixer output
                                                FALSE: UnMute mixer output */
    unsigned int        uiMixerOutput /*Mixer Output No.*/                                                
)
{
    BERR_Code   ret = BERR_SUCCESS;
    bool    bRampRequired=true;
    unsigned int uiCount=0;
    uint32_t     ui32RegVal = 0;
    unsigned int  uiRampTime;
    

    BDBG_ENTER(BRAP_MIXER_P_SetMute);
    BDBG_ASSERT(hMixer);

/*Ramp Time = Time required to pass (Max Volume/Ramp_STEP) samples through Mixer. In 1 milli sec, for 32K, 32 samples will be passed.*/
    uiRampTime = (BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23)/((hMixer->hFmm->hRap->uiMixerRampStepSize)*32);

    /* Call the internal private function */
    ret = BRAP_MIXER_P_SetMute_isr(hMixer,bMute,uiMixerOutput,&bRampRequired);
    if(BERR_SUCCESS != ret)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetMute: Could not Mute the Output"));
    }

    if(true == bRampRequired)
    {
        /* Wait for the RAMP_DONE Status */
        uiCount = 0;
        do
        {
            if(uiCount != 0)
            {
                BKNI_Sleep(1); /* Sleep for 20ms for Ramp to complete */
            }
        	ui32RegVal = BRAP_Read32(hMixer->hRegister,BCHP_AUD_FMM_DP_ESR00_STATUS);
        	ui32RegVal &= BCHP_MASK(AUD_FMM_DP_ESR00_STATUS,VOL_RAMP_DONE0)<< hMixer->uiMixerIndex;
        	uiCount++;
        }while((!ui32RegVal)&&(uiCount < uiRampTime));
        if((100 == uiCount)&&(!ui32RegVal))
        {
        	BDBG_MSG(("RAMP UP/DOWN not completed in %d ms",uiRampTime));
        }        

    }
    
    BDBG_LEAVE(BRAP_MIXER_P_SetMute);
    return ret;

}


/***************************************************************************
Summary:
  Isr version of   BRAP_MIXER_P_SetMute

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
BERR_Code BRAP_MIXER_P_SetMute_isr ( 
    BRAP_MIXER_P_Handle     hMixer,     /* [in] Mixer Handle */
    bool                    bMute,       /* [in] TRUE: Mute mixer output
                                                FALSE: UnMute mixer output */
    unsigned int        uiMixerOutput, /*Mixer Output No.*/                                       
    bool    *bRampRequired   /*Ramp required*/    
)
{
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t     ui32RegVal = 0;
    unsigned int i=0;
    
    BDBG_ENTER (BRAP_MIXER_P_SetMute_isr);    

    BDBG_ASSERT (hMixer);

    BDBG_MSG (("BRAP_MIXER_P_SetMute():" 
           "Mixer Index = %d, bMute=%d, uiMixerOutput =%d", 
            hMixer->uiMixerIndex, bMute,uiMixerOutput));
    
#if( (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1) )

    if (true == hMixer->bCompress)
    {
        BDBG_MSG(("BRAP_MIXER_P_SetMute: valid only for uncompressed data."));
        *bRampRequired = false;
        return BERR_SUCCESS;
    }

    ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                               (hMixer->ui32DpOffset 
                                + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                                + hMixer->ui32MixerOffset));


    /* Clear the RAMP_DONE Status */
    ui32RegVal = BRAP_Read32(hMixer->hRegister,BCHP_AUD_FMM_DP_ESR00_STATUS);
    ui32RegVal &= BCHP_MASK(AUD_FMM_DP_ESR00_STATUS,VOL_RAMP_DONE0) << hMixer->uiMixerIndex;
    BRAP_Write32(hMixer->hRegister,BCHP_AUD_FMM_DP_ESR00_STATUS_CLEAR,ui32RegVal);
    ui32RegVal = BRAP_Read32 (hMixer->hRegister,
                   (hMixer->ui32DpOffset 
                    + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                    + hMixer->ui32MixerOffset));

    if(0 == uiMixerOutput)
    {
        if((true == bMute)&&(ui32RegVal & (BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT0))))
        {
            /* Mixer output already muted, no need to ramp */
            *bRampRequired = false;
        }
        else if((false == bMute)&&(!(ui32RegVal & (BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT0)))))
        {
            /* Mixer output already unmuted, no need to ramp */
            *bRampRequired = false;
        }
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT0));
    if (true == bMute)
    {
        ui32RegVal |= BCHP_FIELD_ENUM ( AUD_FMM_DP_CTRL0_MIXER0_CONFIG, 
                                        VOLUME_MUTE_ENA_OUTPUT0, 
                                        Enable);
    }
    else
    {
        ui32RegVal |= BCHP_FIELD_ENUM ( AUD_FMM_DP_CTRL0_MIXER0_CONFIG, 
                                        VOLUME_MUTE_ENA_OUTPUT0, 
                                        Disable);
    }
    }
    else
    {
        if((true == bMute)&&(ui32RegVal&(BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT1))))
        {
            /* Mixer output already muted, no need to ramp */
            *bRampRequired = false;
        }
        else if((false == bMute)&&(!(ui32RegVal&(BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT1)))))
        {
            /* Mixer output already unmuted, no need to ramp */
            *bRampRequired = false;
        }
        ui32RegVal &= ~(BCHP_MASK (AUD_FMM_DP_CTRL0_MIXER0_CONFIG,VOLUME_MUTE_ENA_OUTPUT1));
        if (true == bMute)
        {
            ui32RegVal |= BCHP_FIELD_ENUM ( AUD_FMM_DP_CTRL0_MIXER0_CONFIG, 
            VOLUME_MUTE_ENA_OUTPUT1, 
            Enable);
        }
        else
        {
            ui32RegVal |= BCHP_FIELD_ENUM ( AUD_FMM_DP_CTRL0_MIXER0_CONFIG, 
            VOLUME_MUTE_ENA_OUTPUT1, 
            Disable);
        }
    }        
    
    if(*bRampRequired == true)        
    {
        BRAP_Write32 (hMixer->hRegister, 
                       (hMixer->ui32DpOffset 
                       + BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
                       + hMixer->ui32MixerOffset), 
                      ui32RegVal);
    }    
    hMixer->bOutputMute = bMute;

    if(*bRampRequired == true)
    {
        *bRampRequired = false;
        for(i=0;i<BRAP_RM_P_MAX_MIXER_INPUTS;i++)
        {
            if(hMixer->eInputState[i] == BRAP_P_State_eStarted)
            {
                *bRampRequired = true;
                break;
            }        
        }
    }

    
#else
    /* TODO : 7440 B0 Mute logic should come here */
    BSTD_UNUSED(hMixer);
    BSTD_UNUSED(bMute);
#endif

    BDBG_LEAVE (BRAP_MIXER_P_SetMute_isr);
    return ret;
}

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
    BRAP_P_MixerCoefficientsInfo     *psMixingInfo,     /* [in] Mixer coefficient
                                                                Info */
    bool                            *pbProgrammingDone  /* [out] Programming of
                                                            new coefficents done
                                                            successfully */
    )
{
    BERR_Code           ret=BERR_SUCCESS;
    int                 i=0,j=0;
    BRAP_MIXER_P_Handle hMixer = NULL;
    uint32_t            ui32RegAdd = 0;

    BDBG_ENTER(BRAP_MIXER_P_ProgramPingPongCoeff);

    BDBG_ASSERT(psMixingInfo);

    for(i=0;i<BRAP_RM_P_MAX_RSRCS_CONNECTED_TO_SAME_SRC;i++)
    {
        if(NULL !=  psMixingInfo->hMixer[i])
        {
            hMixer = psMixingInfo->hMixer[i];
            for(j=0; j<BRAP_OutputChannelPair_eMax; j++)
            {
                if( BRAP_INVALID_VALUE == psMixingInfo->uiMixerIp[i][j])
                {
                    continue;
                }
                
                /* Get the Address in Ping Registers for Left Coefficient 0 */
                BDBG_MSG(("uiMixerIndex = %d, uiMixerIp = %d, ui32Right0=%d"
                          "ui32Left0=%d, ui32Right1=%d, ui32Left1=%d, Chp=%d i=%d",
                             hMixer->uiMixerIndex,psMixingInfo->uiMixerIp[i][j],
                             psMixingInfo->sCoeff0[i][j].ui32Right,
                             psMixingInfo->sCoeff0[i][j].ui32Left,
                             psMixingInfo->sCoeff1[i][j].ui32Right,
                             psMixingInfo->sCoeff1[i][j].ui32Left,j,i));
                /* base_for_mixer = 
                        array_base + ((element * 4) * max_mixer_input) * mixer Id */            
                ui32RegAdd = hMixer->ui32DpOffset + 
                    BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE +
                    (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                     BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) +
                    (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * 
                    psMixingInfo->uiMixerIp[i][j]);

                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff0[i][j].ui32Left);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff0[i][j].ui32Right);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff1[i][j].ui32Left);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff1[i][j].ui32Right);

                /* Get the Address in Ping Registers for Left Coefficient 0 */
                ui32RegAdd = hMixer->ui32DpOffset + 
                    BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE +
                    (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
                     BRAP_RM_P_MAX_MIXER_INPUTS * hMixer->uiMixerIndex) +
                    (BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * BRAP_MIXER_P_NUM_COEFF_PER_INPUT * 
                    psMixingInfo->uiMixerIp[i][j]);
                /* Program the Left coefficient 0 */
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff0[i][j].ui32Left);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff0[i][j].ui32Right);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff1[i][j].ui32Left);

                /* Update the Reg Address */
                ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
                BRAP_Write32(hMixer->hRegister, ui32RegAdd, psMixingInfo->sCoeff1[i][j].ui32Right);
            }
        }
    }

    *pbProgrammingDone = true;

    BDBG_LEAVE(BRAP_MIXER_P_ProgramPingPongCoeff);
    return ret;
}

/***************************************************************************
Summary:
    Programs the gain information in ping pong registers for scaling.

Description:
    Takes the gain as 5.23 format
        Examples (for 3563 A0 and 7440 A0/B0. These might change for other chips):
                    0x0800000 is no change
                    0x7800000 is max volume
                    0x0 is mute
                    0x0400000 is half volume
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
    )
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegAdd =0;
    uint32_t        ui32RegOffset =0;    

    BDBG_ENTER (BRAP_MIXER_P_SetGainCoeff);
    BDBG_ASSERT(hMixer);    

    /*
    The AUD_FMM_DP_CTRL0_PING_COEFFICIENTS[0..255] has 256 coefficients.
    There are a total of 8 mixers.  Each mixer output can be the mix of up to 8 
    input stream pairs.  Each mix input uses 4 coefficients.  
    Therefore, there are a total of 8*8*4 = 256 scaling coefficients.
    
    Lout = Left_Coef_0 * Lin + Right_Coef_0 * Rin  
    Rout = Left_Coef_1 * Lin + Right_Coef_1 * Rin.

    However these are mixing coefficients. In case of Gain we are concerned 
    only with Left_Coef_0 for Lout and Right_Coef_1 for Rout.
    The other two coefficients are not considered and are maintained at whatever 
    level there were previously in.
    */

    
    /* Gain applies to only PCM */
    if (true == hMixer->bCompress)
    {
        BDBG_ERR(("BRAP_MIXER_P_SetGainCoeff(): valid only for uncompressed data."));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }     

    BDBG_MSG (("BRAP_MIXER_P_SetGainCoeff():" 
               "Mixer Index = %d, uiLeftGain=%x, uiRightGain=%x", 
                hMixer->uiMixerIndex, uiLeftGain, uiRightGain));    

    ui32RegOffset = \
          (  BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * 
             BRAP_MIXER_P_NUM_COEFF_PER_INPUT *
             BRAP_RM_P_MAX_MIXER_INPUTS       * 
             hMixer->uiMixerIndex
          )                                   +

          (  BRAP_MIXER_P_COEFF_SIZE_IN_BYTES * 
             BRAP_MIXER_P_NUM_COEFF_PER_INPUT * 
             uiMixerInput
          );

    /* First programming the PING part */    
        
    /* Calculate the Ping Address for Left 0 Coefficient and program coeff */
    ui32RegAdd = hMixer->ui32DpOffset + 
                BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE + ui32RegOffset;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiLeftGain);

    /* Calculate the Ping Address for Right 0 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiRightBalance);

    /* Calculate the Ping Address for Left 1 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiLeftBalance);
    
    /* Calculate the Ping Address for Right 1 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiRightGain);


    /* Now programming the PONG part */

    /* Calculate the Pong Address for Left 0 Coefficient and program coeff */
    ui32RegAdd = hMixer->ui32DpOffset + 
                BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE + ui32RegOffset;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiLeftGain);

    /* Calculate the Pong Address for Right 0 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiRightBalance);

    /* Calculate the Pong Address for Left 1 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiLeftBalance);
    
    /* Calculate the Pong Address for Right 1 Coefficient and program coeff */
    ui32RegAdd += BRAP_MIXER_P_COEFF_SIZE_IN_BYTES;
    BRAP_Write32(hMixer->hRegister, ui32RegAdd, uiRightGain);
    

    BDBG_LEAVE (BRAP_MIXER_P_SetGainCoeff);

    return ret;
}

static 
BERR_Code BRAP_MIXER_P_UpdateParams(
    BRAP_MIXER_P_Handle         hMixer,
    unsigned int                uiMixerInput,
    const BRAP_MIXER_P_Params   *pParams
    )
{
    BERR_Code   ret = BERR_SUCCESS;
    int         i=0;
    bool        bMixerStarted = false;

    BDBG_ENTER(BRAP_MIXER_P_UpdateParams);

    BDBG_ASSERT(hMixer);
    BDBG_ASSERT(pParams);

    BDBG_MSG(("BRAP_MIXER_P_UpdateParams : Mixer DP_ID =%d Mixer_ID=%d, Mixer_Input=%d ",
               hMixer->uiDpIndex,hMixer->uiMixerIndex,uiMixerInput));
    

    for (i=0;i<BRAP_RM_P_MAX_MIXER_INPUTS;i++)
    {
        if(0 != hMixer->uiMixerInputStartCnt[i])
        {
            bMixerStarted = true;
            break;
        }
    }

    if(true == bMixerStarted)
    {
        BDBG_MSG(("BRAP_MIXER_P_UpdateParams:Mixer already Started, Validating"
                  " and Updating params"));
        /* Validate the Input Params */
        if((hMixer->sParams.bCompress != pParams->bCompress)||
           (hMixer->sParams.bPriorityHigh != pParams->bPriorityHigh)||
           (hMixer->sParams.bOutputEnable0 != pParams->bOutputEnable0)||
           (hMixer->sParams.bOutputEnable1 != pParams->bOutputEnable1)||
           (hMixer->sParams.uiGroupId != pParams->uiGroupId))
        {
            BDBG_ERR(("BRAP_MIXER_P_UpdateParams:Two mixer inouts can not be"
                      "started with different Params"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        /* If Everything is OK, Update FCI ID in Mixer Handle */
        hMixer->sParams.ui32InputFciId[uiMixerInput] = 
                                        pParams->ui32InputFciId[uiMixerInput];
    }
    else
    {
        BDBG_MSG(("BRAP_MIXER_P_UpdateParams:Mixer Not Started, Updating params"));
        hMixer->sParams = *pParams;
        hMixer->bCompress = pParams->bCompress;
    }
    
    BDBG_LEAVE(BRAP_MIXER_P_UpdateParams);
    return ret;
}

/***************************************************************************
Summary:
    Converts Volume from Db to 5.23 Format
**************************************************************************/
unsigned int BRAP_MIXER_P_ConvertVolInDbTo5_23Format(
    unsigned int    uiVol
    )
{
    if(uiVol > BRAP_MIXER_P_MAX_DB_VOLUME)
    {
        BDBG_ERR(("BRAP_MIXER_P_ConvertVolInDbTo5_23Format: Invalid Db value %d, Truncating it to %d "
            ,uiVol,BRAP_MIXER_P_MAX_DB_VOLUME));
        uiVol = BRAP_MIXER_P_MAX_DB_VOLUME;
    }
    return BRAP_Vol_DB[uiVol];    
}

/***************************************************************************
Summary:
    Converts Volume from 5.23  to 1/100 Db Format
**************************************************************************/
unsigned int BRAP_MIXER_P_ConvertVolIn5_23ToDbFormat(
    unsigned int    uiVol
    )
{

    unsigned int i =0, uiDbVal=0,uiRange= 0,uiDelta=0; 

    if(uiVol > BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23)
    {
        uiVol = BRAP_MIXER_P_DEFAULT_VOLUME_IN_5_23;
    }

    /*            
            uiVol   = 0x750000
            i =0;
            uiRange = 0x800000 - 0x721482 = 0xDEB7E
            uiDelta =  0x800000 - 0x750000 = 0xB0000

            uiDbVal = (0 * 100) + ( 100 *  0xB0000 ) / 0xDEB7E = 72089600/912254 
                        = 79                 
    */

    for(i=0 ; i < BRAP_MIXER_P_MAX_DB_VOLUME -1 ; i ++)
    {
        if(uiVol == BRAP_Vol_DB[i])
        {
            uiDbVal = i * 100;
            break;
        }
        else if(uiVol < BRAP_Vol_DB[i] && uiVol > BRAP_Vol_DB[i+1])
        {
            uiRange = BRAP_Vol_DB[i] - BRAP_Vol_DB[i+1];
            uiDelta = BRAP_Vol_DB[i] - uiVol;
            uiDbVal = (i * 100) + (100 * (uiDelta % uiRange))/uiRange;
            break;
        }
    }
    if(i >= BRAP_MIXER_P_MAX_DB_VOLUME -1)
    {
        uiDbVal = BRAP_MIXER_P_MAX_DB_VOLUME * 100;
    }
    return uiDbVal;    
}

