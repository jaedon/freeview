/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_types_priv.h $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 9/16/10 12:21a $
*
* Module Description: Private data structures common to all the modules of
*					  Raptor PI.
* Revision History:
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_types_priv.h $
* 
* Hydra_Software_Devel/7   9/16/10 12:21a srajapur
* SW7468-298 :[7468]Merging Real Video Code to main latest
* 
* Hydra_Software_Devel/6   8/3/10 4:49p gautamk
* SW7405-3063: [7405] Exposed header files should not define _P_ i.e.
* private MACROS.
* 
* Hydra_Software_Devel/5   4/7/10 6:44p gautamk
* SW7405-4170: [7405] Adding Support for Multi Stream Decoder. Master and
* Secodary channel will be mixed by FwMixer Processing stage.
* 
* Hydra_Software_Devel/4   1/13/10 2:47p gautamk
* SW7405-3202: [7405] implementing code review comments
* 
* Hydra_Software_Devel/3   6/12/09 3:36p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/2   6/5/09 5:20p gautamk
* PR55572: [7405] Adding State BRAP_P_State_eStopInvoked to handle race
* condition between Stopchannel and  Isr functions like SetStcValid_isr
* etc.
* 
* Hydra_Software_Devel/1   1/22/08 4:48p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/2   1/9/08 10:36p sushmit
* PR 34648: [7405] Updating FW-PI Interface after syncup meeting.
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   12/31/07 4:55p nitinb
* PR34648: [7405] Resolved circular dependancy in files brap_dsp_priv.h
* and brap_fwif.h by moving handle definitions to brap_types_priv.h.
* 
* Hydra_Software_Devel/3   6/14/07 3:55p bhanus
* PR 30069 : [7440 B0] Added support for playing unsigned content for PCM
* PB channel.
* 
* Hydra_Software_Devel/2   9/19/06 5:59p kagrawal
* PR 20654: Removed redundant BRAP_P_State_eStoped
* 
* Hydra_Software_Devel/1   8/17/06 12:07p speter
* PR 20654: Added from BLR_RAP_7440A0_Bring_Up_Branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/1   8/2/06 9:28p kagrawal
* PR 20654: Initial version
* 
***************************************************************************/
#ifndef _BRAP_TYPES_PRIV_H__ /*{{{*/
#define _BRAP_TYPES_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BRAP_UNSIGNED_PCM_SUPPORTED 1

#define HIFIDAC_VERSION 65

#if ((BCHP_CHIP == 7340)||(BCHP_CHIP == 7342)||(BCHP_CHIP == 7125)||(BRAP_7550_FAMILY == 1))
#define BRAP_PLL1_SUPPORTED 0
#else
#define BRAP_PLL1_SUPPORTED 1
#endif



#if (BCHP_CHIP == 7420)
#define BRAP_MAX_RFMOD_OUT  2
#else
#define BRAP_MAX_RFMOD_OUT  0
#endif


/***************************************************************************
Summary:
    This enumeration lists the 'states' - started and stopped.
    Following are the various operations and corresponding states:

    Operation   -   State
    ---------       -----
    Open        -   Stop
    Start       -   Start
    Stop        -   Stop
    Close       -   The channel/resource handle itself doesn't exist. So, 
                    state doesn't matter. 
***************************************************************************/     
typedef enum BRAP_P_State
{
    BRAP_P_State_eInvalid,
    BRAP_P_State_eOpened,
    BRAP_P_State_eStarted
}BRAP_P_State;

/***************************************************************************
Summary:
    This enumeration lists the various Downmix paths. 
    Note: In 7440, this is present only for the code reusability's sake. It 
    is not exposed to the user.
***************************************************************************/ 
typedef enum BRAP_DEC_DownmixPath
{
	BRAP_DEC_DownmixPath_eMain,			/* Main downmix path. This path 
											    supports multiple channels */
	BRAP_DEC_DownmixPath_eStereoDownmix,  /* Stereo downmix path */
	BRAP_DEC_DownmixPath_eMax
}BRAP_DEC_DownmixPath;


typedef struct BRAP_DSP_P_Device			*BRAP_DSP_Handle;

typedef struct BRAP_DSPCHN_P_Channel			*BRAP_DSPCHN_Handle;	/* Opaque DSP context handle */

typedef uint32_t TIME_45KHZ_TICKS;

#ifdef RAP_REALVIDEO_SUPPORT
#define RAP_VIDEOONDSP_SUPPORT 1
#endif


/***************************************************************************
Summary:
    Multi Stream Decoder handle
Description:
    This is an opaque handle for an Multi Stream Decoder object 
See Also:
***************************************************************************/ 
typedef struct BRAP_P_MultiStreamDecoderDetail *BRAP_MultiStreamDecoderHandle;


#ifdef __cplusplus
}
#endif

#endif /*}}} #ifndef _BRAP_TYPES_PRIV_H__ */

/* End of File */
