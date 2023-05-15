/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_op.h $
* $brcm_Revision: Hydra_Software_Devel/13 $
* $brcm_Date: 8/24/12 5:02p $
*
* Module Description:
*   Module name: Output
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Output ports abstraction, which are exposed to the 
*   application developer.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_op.h $
* 
* Hydra_Software_Devel/13   8/24/12 5:02p purush
* SW7425-3090: [7425] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/12   8/24/12 11:52a purush
* SW7420-2271: [7420] Added support for run-time programmable
* VCXO/timebase settings
* 
* Hydra_Software_Devel/11   9/30/10 1:44p gautamk
* SW7420-850: [7420] Adding PI support for 24bit SPDIF PCM sample
* support.
* 
* Hydra_Software_Devel/10   4/29/10 5:38p gautamk
* SW7325-547: [7405] Changing the exopsed paramter name
* 
* Hydra_Software_Devel/9   4/29/10 5:32p gautamk
* SW7325-547: [7405] Adding CGMS bit SPDIF Ch bits
* 
* Hydra_Software_Devel/8   3/8/10 8:53p sgadara
* SW7420-611: [3548,3556] Reverting back the changes as the existing
* interface of exclusive API for programming DAC volume is made use of.
* DAC Scale register continues to get programmed using SetOutputConfig
* --> For Volume register SetOutputConfig will introduce audio drop since
* there will be mute->unmute happening
* 
* Hydra_Software_Devel/7   2/19/10 3:56p sgadara
* SW3556-1033: [3548,3556] Changing the start of enum to zero.
* 
* Hydra_Software_Devel/6   1/8/10 5:24p sgadara
* SW3556-940: [3548,3556] Adding the interface to program the SCALE and
* DAC_VOLUME registers in HIFIDAC_CTRL
* 
* Hydra_Software_Devel/5   10/2/09 2:36a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/4   6/12/09 3:39p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/3   1/22/09 3:41p gautamk
* PR48850: [7405] Fixing run time error. Which will later be fixed when
* Jon will make DAC changes.
* 
* Hydra_Software_Devel/2   1/20/09 2:34p gautamk
* PR48850:[7405] Changing the MAPPER_Mute Type for 65nm chips.
* 
* Hydra_Software_Devel/1   7/21/08 2:17p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   5/5/08 11:32a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/28/08 4:38p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 2:13p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch/RAP_NEWFWARCH_WDOG/1   3/28/08 4:06p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/9   2/22/08 4:42a bhanus
* PR 39821 :[7440] Exposing HDMI crossbar.
* 
* Hydra_Software_Devel/8   11/19/07 3:14p sushmit
* PR 36822: [7405] Making MAI take data straight from Mixer.
* 
* Hydra_Software_Devel/7   8/1/07 1:28p kagrawal
* PR 33698: [7440] Enabling channel pair mapping by crossbar programming
* for I2s5-I2s8
* 
* Hydra_Software_Devel/6   3/28/07 5:11p nitinb
* PR 28177: Added struct BRAP_OP_SpdifChannelStatusBits
* 
* Hydra_Software_Devel/5   2/24/07 2:10p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/4   2/6/07 1:45p kagrawal
* PR 25607: Fixed compilation warnings
* 
* Hydra_Software_Devel/3   10/6/06 5:44p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/2   9/20/06 4:22p kagrawal
* PR 20654: Cleaned exposed API prototypes
* 
* Hydra_Software_Devel/1   9/1/06 2:02p speter
* PR 20654: Adding output files to base2 as they are going to be 7440
* arch specific
* 
* Hydra_Software_Devel/7440_bringup_0.1/1   8/18/06 9:40a speter
* PR 20654:Creating new branch for 0.1 7440 bringup
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   8/2/06 9:18p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   7/31/06 12:26p bhanus
* PR 20654: Merging code from the hydra_software_devel branch
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/1   5/29/06 3:44p sushmit
* PR18604: Putting encoder PI files for 7440
* 
* Hydra_Software_Devel/25   4/4/06 4:14p rjain
* PR 20318: Implementing code review comments for AM
* 
* Hydra_Software_Devel/24   4/3/06 4:33p bhanus
* PR 19360 :Moving the Declaration of BRAP_P_GetChannelsUsingOpPort()
* from brap_op.h to brap_op_priv.h.
* 
* Hydra_Software_Devel/23   3/30/06 3:18p bhanus
* PR 19360 : Added a check to return error if PCM data is mixed with
* compressed data
* 
* Hydra_Software_Devel/21   2/17/06 9:54a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   2/6/06 4:17p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/20   1/19/06 7:20p bselva
* PR 19168: Checking in the changes required for 7400 bringup
* 
* Hydra_Software_Devel/19   1/19/06 2:03p kagrawal
* PR 18264: 1. Rearchitected SRC to accomodate capture channel
* 2. Implemented review comments
* 
* Hydra_Software_Devel/18   11/30/05 4:14p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/17   11/22/05 1:57p bselva
* PR 17490: Fix for 44.1KHz I2S output
* 
* Hydra_Software_Devel/16   11/15/05 8:01p bselva
* PR 17590: Added support for MAI linked to Flex
* 
* Hydra_Software_Devel/15   11/13/05 4:32p kagrawal
* PR 17590: Rearchitected BRAP_OP_SetSamplingRate
* 
* Hydra_Software_Devel/14   11/10/05 11:53a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   11/1/05 9:43a kagrawal
* 1. Implemented review comments on PCM playback and mixing
* 2. Added BRAP_OP_SetSamplingRate and BRAP_OP_GetSamplingRate APIs
* 
* Hydra_Software_Devel/13   11/9/05 4:05p bselva
* PR 17590: Added support for HDMI output
* 
* Hydra_Software_Devel/12   9/19/05 7:14p bmishra
* PR 16148: Removed chip specific code from interface files
* 
* Hydra_Software_Devel/11   9/13/05 7:42p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/10   9/1/05 4:14p rjain
* PR 16888: adding BRAP_OP_SetMute and GetMute. These can be used for all
* output ports, for either PCM/compressed data. The mute status is
* preserved across channel restart.
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   8/31/05 6:40p bselva
* PR 16148: Adding DAC PI support
* 
* Hydra_Software_Devel/9   7/22/05 6:27p rjain
* PR 16369: added PI's BRAP_OP_SetDataEnable() and
* BRAP_OP_GetDataEnable() which can be used to disable data flowing out
* of any ouput port whether compressed or uncompressed. Current
* implementation only takes care of SPDIF output port.
* 
* Hydra_Software_Devel/8   7/8/05 2:04p rjain
* PR 16148: Modifiying for 7401
* 
* Hydra_Software_Devel/7   6/2/05 10:46a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/6   5/13/05 2:40p rjain
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/5   5/4/05 11:08a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/4   2/17/05 7:35p bmishra
* PR 14143: Implemented some Phase 1 code review comments for the
* interfaces only
* 
* Hydra_Software_Devel/3   2/16/05 1:45p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/2   1/12/05 5:02p rjain
* PR13082: Updating with changes after compilation of FMM, AM and RM
* together.
* 
* Hydra_Software_Devel/1   1/11/05 11:39a rjain
* PR 13082: Moved to new directory structure
* 
* 
***************************************************************************/
#ifndef _BRAP_OP_H_
#define _BRAP_OP_H_

#include "brap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Channel status word count: 39 bits = 2 32-bit words */
#define BRAP_OP_SPDIF_CHANNEL_STATUS_WORD_COUNT			(0x2)

    
/***************************************************************************
Summary:
    This enumeration lists all PLLs that can be used by the Output block.

Description:

    
See Also:

***************************************************************************/  
typedef enum BRAP_OP_Pll
{
    BRAP_OP_Pll_ePll0=0,    /* FMM PLL0 */
    BRAP_OP_Pll_ePll1,      /* FMM PLL1 */
    BRAP_OP_Pll_ePll2,      /* FMM PLL2 */
    BRAP_OP_Pll_ePll0Div2,  /* FMM PLL0 with half the frequency */
    BRAP_OP_Pll_ePll1Div2,  /* FMM PLL1 with half the frequency */
    BRAP_OP_Pll_ePll2Div2,  /* FMM PLL2 with half the frequency */
    BRAP_OP_Pll_eHifiDac0,  /* Hi Fi DAC 0 */
    BRAP_OP_Pll_eHifiDac1,  /* Hi Fi DAC 1 */
    BRAP_OP_Pll_eMax
}BRAP_OP_Pll;
  
/***************************************************************************
Summary:
    This enumeration lists the external Master Clocks that can be used by the 
    Output block.

Description:

    
See Also:

***************************************************************************/  
typedef enum BRAP_OP_ExtMClk
{
    BRAP_OP_ExtMClk_eExternal0=0, /* External Master Clock 0 */
    BRAP_OP_ExtMClk_eExternal1      /* External Master Clock 1 */
}BRAP_OP_ExtMClk;

/***************************************************************************
Summary:
    This enumeration lists all SClock (bit clock) rates that can be used.

Description:
    Indicates the frequency of SClock as a multiple of Fs.

See Also:

***************************************************************************/  
typedef enum BRAP_OP_SClkRate
{
    BRAP_OP_SClkRate_e64Fs=1, /* SCLK rate is 64fs */
    BRAP_OP_SClkRate_e128Fs   /* SCLK rate is 128fs */
}BRAP_OP_SClkRate;
    


/***************************************************************************
Summary:
    This enumeration lists all MClock rates that can be used.

Description:
    Indicates the frequency of MCLK as a multiple of Fs.

See Also:

***************************************************************************/  
typedef enum BRAP_OP_MClkRate
{
    BRAP_OP_MClkRate_e128Fs=0, /* MCLK rate is 128fs */
    BRAP_OP_MClkRate_e256Fs,   /* MCLK rate is 256fs */
    BRAP_OP_MClkRate_e384Fs,   /* MCLK rate is 384fs */
    BRAP_OP_MClkRate_e512Fs,   /* MCLK rate is 526fs */
    BRAP_OP_MClKRate_eMax
}BRAP_OP_MClkRate;
    

/***************************************************************************
Summary:
    This enumeration allows left and right output channels to be normal, 
    swapped, or copies of the left/right input channels.

Description:


See Also:

***************************************************************************/  
typedef enum BRAP_OP_ChannelOrder
{
    BRAP_OP_ChannelOrder_eNormal=0,     /* Normal */
    BRAP_OP_ChannelOrder_eBothLeft,     /* Both get Left */
    BRAP_OP_ChannelOrder_eBothRight,    /* Both get Right */
    BRAP_OP_ChannelOrder_eSwap          /* Swap */
    
}BRAP_OP_ChannelOrder;


/***************************************************************************
Summary:
    Specifies the MAI audio format. 
    
Description:
    This field does not affect the actual data the MAI formatter transmits. 
    The MAI formatter always sends a sample pair. In other words, setting 
    this field to PCM_Mono will not stop the MAI formatter from transmitting 
    the right channel data, but the receiver will think only the left data is 
    valid.

See Also:

***************************************************************************/ 
typedef enum BRAP_OP_MaiAudioFormat
{
    BRAP_OP_MaiAudioFormat_eIdle=0,             /* Idle */
    BRAP_OP_MaiAudioFormat_eSpdif2Channel,	    /* Carries SPDIF 2 channel */
    BRAP_OP_MaiAudioFormat_eSpdif6Channel,	    /* Carries SPDIF 6 channel */
    BRAP_OP_MaiAudioFormat_eSpdif8Channel,	    /* Carries SPDIF 8 Channel */
    BRAP_OP_MaiAudioFormat_eSpdifLinearPcm,     /* Uncompressed PCM in SPDIF format */
    BRAP_OP_MaiAudioFormat_eSpdifCompressed,    /* Compressed SPDIF */
    BRAP_OP_MaiAudioFormat_eSpdif2ChCompressed, /* Compressed SPDIF */
    BRAP_OP_MaiAudioFormat_eSpdif8ChCompressed  /* Compressed SPDIF */
}BRAP_OP_MaiAudioFormat;



/***************************************************************************
Summary:
    This enumeration lists the types of software mute used for muting the DAC output.
    Controls the DAC current enables for mute. When muted and this field is Use_reg_val, HIFIDAC_CTRL0_DAC_CONST_VAL specifies the enable values.

    3 = Reserved
    2 = Use_reg_val
    1 = Drive_5555
    0 = Drive_aaaa
    Reset value is Drive_aaaa.
Description:

    
See Also:

***************************************************************************/  
typedef enum BRAP_OP_DacMuteType
{
	BRAP_OP_DacMuteType_Drive_aaaa,		/* Drive_aaaa */
	BRAP_OP_DacMuteType_Drive_5555,		/* Drive_5555*/
	BRAP_OP_DacMuteType_Use_reg_val,	    /* Use_reg_val, In this case HIFIDAC_CTRL0_DAC_CONST_VAL 
	                                                                    specifies the enable values.*/    
	BRAP_OP_DacMuteType_Reserved    	/*Reserved */
/*Kept for Compilation */	
    #if 1
	,BRAP_OP_DacMuteType_ConstantLow=BRAP_OP_DacMuteType_Drive_aaaa,    	
	BRAP_OP_DacMuteType_ConstantHigh = 0xFFFF,    	/*Reserved */
	BRAP_OP_DacMuteType_SquareWaveOpp,    	/*Reserved */
	BRAP_OP_DacMuteType_SquareWaveSame    	/*Reserved */
#endif	
} BRAP_OP_DacMuteType;

/***************************************************************************
Summary:
    This enumeration lists all MClock rates that can be used for DAC.

Description:
    Indicates the frequency of MCLK as a multiple of Fs.

See Also:

***************************************************************************/  
typedef enum BRAP_OP_DacMClkRate
{
    BRAP_OP_DacMClkRate_e64Fs,   /* MCLK rate is 64fs */	
    BRAP_OP_DacMClkRate_e256Fs  /* MCLK rate is 256fs */
}BRAP_OP_DacMClkRate;


/***************************************************************************
Summary:
    This enumeration lists all the possible ways to output Left and Right Data through DAC.

Description:

    
See Also:

***************************************************************************/  
typedef enum BRAP_OP_DacLROutputCtrl
{
	BRAP_OP_DacLROutputCtrl_eBothLRZeros,	/* Outputs zeros for both Left and Right channel */
	BRAP_OP_DacLROutputCtrl_eOnlyLZeros,	/* Outputs zeros for Left channel only */
	BRAP_OP_DacLROutputCtrl_eOnlyRZeros,	/* Outputs zeros for Right channel only */    
	BRAP_OP_DacLROutputCtrl_eNoZeros		/* Ouputs the same value as it receives */
} BRAP_OP_DacLROutputCtrl;

/***************************************************************************
Summary:
    Parameters to be passed by external application on Opening the MAI output port.
    
Description:
    Note: MAI Format Word parameters are the values that go into 
       the MAI Format Word. These parameters are mainly dependent upon how the 
       MAI client want to receive the data. For example, if input data is 
       Stereo, both Right and Left Channel data will flow on the MAI bus. 
       But if eMAIAudioFormat indicates Left Mono, then the MAI client will 
       only read the Left Mono data. 

See Also:

***************************************************************************/
typedef struct BRAP_OP_MaiSettings
{
    BRAP_OutputPort         eMaiMuxSelector; /* MAI Input: SPDIF or FLEX. 
                                                Start-time changeable */     
    BRAP_OP_ChannelOrder    eChannelOrder;   /* LR output channels to be normal,
                                                swapped, or copies of the LR 
                                                input channels. On-the-fly 
                                                changeable */
    BRAP_OP_MaiAudioFormat  eMaiAudioFormat; /* Specifies the MAI format word. 
                                                Start-time changeable */
    bool                    bSpdifoutput;    /* States if the o/p of MAI should
                                                be of SPDIF format Or PCM. 
                                                TRUE = SPDIF format
                                                FALSE= PCM format. 
                                                Start-time changeable */ 
#if (BRAP_7550_FAMILY == 1)
    BRAP_OP_MClkRate        eMClkRate;
#elif ( (BRAP_7405_FAMILY == 1) || (BRAP_3548_FAMILY == 1) )
    BRAP_OP_Pll             ePll;           /* PLL selection for MAI port. Start-time changeable. */
#endif


}BRAP_OP_MaiSettings;  

/***************************************************************************
Summary:
    Parameters to be passed by external application on Opening the SPDIF output port.

Description:
 
See Also:

***************************************************************************/ 
typedef struct BRAP_OP_SpdifSettings
{
    BRAP_OP_ChannelOrder    eChannelOrder;  /* LR output channels to be normal,
                                               swapped, or copies of the LR
                                               input channels. On-the-fly 
                                               changeable.*/
    bool                    bLimitTo16Bits; /* Limit to 16 bits. On-the-fly 
                                               changeable.*/
    BRAP_OP_MClkRate        eMClkRate;      /* MClk Rate selection. Start-time changeable. */
    BRAP_OP_Pll             ePll;           /* PLL selection for SPDIF port. Start-time changeable. */
}BRAP_OP_SpdifSettings;
    
/**********************************************************************
Summary:
    SPDIF Channel Status Bits in Packed Format

Description:
   This structure is used to set the channel status bits of a SPDIF output port. The 
   channel status bits are packed into two 32-bit words. Apply only to SPDIF, 
   MAI and Flex ports. 

See Also: BRAP_OP_SpdifChanStatusParams

**********************************************************************/
typedef struct BRAP_OP_SpdifPackedChannelStatusBits
{
	uint32_t	ui32ChStatusBits[BRAP_OP_SPDIF_CHANNEL_STATUS_WORD_COUNT];
							/* ui32ChStatusBits[0] contains channel status bits [31:0] 
   							    ( as per spec ) and ui32ChStatusBits[1] contains 
   							    channel status bits [39:32] ( as per spec ) */
} BRAP_OP_SpdifPackedChannelStatusBits;

/***************************************************************************
Summary:
    Parameters to be passed by external for the SPDIF channel status bits
    Refer to SPDIF specs for details on the channel status bits.

Description:
    These are Start-time changeable only ie can be set only before starting the 
    output port/channel. Cannot be changed while the output port is running.
    Apply only to SPDIF, MAI and Flex ports.
 
See Also: BRAP_OP_SpdifPackedChannelStatusBits

***************************************************************************/ 
typedef struct BRAP_OP_SpdifChanStatusParams
{
    bool           bProfessionalMode;    /* [0:0] The professional mode flag.
                                        TRUE: Professional mode. Other user
                                        options will not be considered.
                                        FALSE: Consumer mode.*/
    bool           bSwCopyRight;         /* [2:2] Software CopyRight assert.
                                        TRUE: CopyRight is asserted
                                        FALSE: CopyRight is not asserted */
    uint16_t       ui16CategoryCode;     /* [8:15] Category Code */
    uint16_t       ui16ClockAccuracy;    /* [28:29] Clock Accuracy */
    bool           bSeparateLRChanNum;   /* TRUE:  Left channel num = 0000
                                               Right Channel Num = 0000
                                        FALSE: Left channel num = 1000
                                               Right Channel Num = 0100 */
    uint16_t        uiCGMS_A; /* [40:41] */
                                            /* Range values for this field.		
                                        		0   	CopyFreely  	Unlimited copies may be made of the content.
                                        		1 		CopyNoMore	 	One generation of copies has already been made; no further copying is allowed.
                                        		2 		CopyOnce 		One generation of copies may be made
                                        		3	 	CopyNever 		No copies may be made of the content.
                                    		default =0	
                                            	*/                                             
    uint16_t ui16SpdifPcmWordWidth;  /* PCM width : Possible values : 16,24 : Default value is 16 (16bit)*/
                                            	
} BRAP_OP_SpdifChanStatusParams;


/***************************************************************************
Summary:
    Parameters to be passed by application on Opening the I2S output port.

Description:

See Also:

***************************************************************************/ 
typedef struct BRAP_OP_I2sSettings
{
    BRAP_OP_ChannelOrder     eChannelOrder;      /* LR output channels to be 
                                                    normal, swapped, or copies
                                                    of the LR input channels. 
                                                    On-the-fly changeable.*/
    bool                     bLsbAtLRClk;        /* Data Justification.
                                                    Controls whether LSB or 
                                                    MSB is at LRCK transition
                                                    TRUE: LSB, FALSE: MSB 
                                                    On-the-fly changeable.*/
    bool                     bAlignedToLRClk;    /* Controls whether data is 
                                                    aligned with LRCK or delayed
                                                    by one SCLK period
                                                    FALSE: Delayed. 
                                                    TRUE: Aligned 
                                                    On-the-fly changeable.*/
    bool                     bLRClkPolarity;     /* Sets the polarity of the 
                                                    left/right clock
                                                    TRUE: High for Left
                                                    FALSE: Low for Left 
                                                    On-the-fly changeable.*/
    BRAP_OP_SClkRate         eSClkRate;          /* SClk (bit Clock) rate 
                                                    On-the-fly changeable.*/
    BRAP_OP_MClkRate         eMClkRate;          /* MClk rate. Start-time 
                                                    changeable. */
    BRAP_OP_Pll              ePll;               /* PLL selection. Start-time 
                                                    changeable. */
    BRAP_OutputChannelPair   eMappedChPair;      /* I2s_multi1 mapped channel 
                                                    pair. Note: This param is 
                                                    used only for output ports
                                                    I2s5, I2s6, I2s7 and I2s8. 
                                                    For all other I2s, this is 
                                                    ignored. */
}BRAP_OP_I2sSettings;


/***************************************************************************
Summary:
    Parameters passed by application on Opening the DAC output port
    
Description:

See Also:

***************************************************************************/ 
typedef struct BRAP_OP_DacSettings
{
    BRAP_OP_ChannelOrder     eChannelOrder;      /* LR output channels to be 
                                                    normal, swapped, or copies
                                                    of the LR input channels.
                                                    On-the-fly changeable.*/
    BRAP_OP_DacMuteType     eMuteType;  /* S/W Mute type.
                                           On-the-fly changeable.*/
    uint32_t                ui32RightDacOutputVal; /*When muted and eMuteType field is Use_reg_val, 
                                                            HIFIDAC_CTRL0_DAC_CONST_VAL.RIGHT specifies 
                                                           the enable values. Controls the right DAC output value when muted */                                            
    uint32_t                ui32LeftDacOutputVal; /*When muted and eMuteType field is Use_reg_val, 
                                                            HIFIDAC_CTRL0_DAC_CONST_VAL.LEFT specifies 
                                                           the enable values. Controls the left DAC output value when muted */                                            
    uint32_t                ui32Scale;            /* ASRC Scale factor */
}BRAP_OP_DacSettings;

/***************************************************************************
Summary:
    Parameters to be passed by external for enabling a test tone throuh DAC.
    
Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_DacToneInfo
{
	int32_t  aSample[64];  							/* test tone samples */
	BRAP_OP_DacLROutputCtrl	eLROutputControl;	/* 0 = Both L & R Zeros
													1=Left alone zero
													2=Right alone zero
													3=No Zeros on both L and R */
	bool		bSampleBufferMode;	/* True = share;  the same test tone
								is generated for both left and right
								from all samples 0-63.
								false = Dont Share; samples 0-31 for left and
								32-63 for right */
	uint8_t	uiRightRepeat;	/* Number of samples minus 1 in test 
							tone for right channel before repeating. 
							*/
	uint8_t	uiLeftRepeat;	/* Number of samples minus 1 in test 
							tone for left channel before repeating. 
							*/
	BAVC_AudioSamplingRate eSamplingRate;	/* Sampling frequecny
											* of the DAC to be set
											*/
} BRAP_OP_DacToneInfo;

/***************************************************************************
Summary:
    Parameters to be passed for configuring the external Master clocks
    
Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_ExtMClkSettings
{
	BRAP_OP_Pll			ePll;			/* Desired Pll to be used */
	BRAP_OP_MClkRate	eMClockRate;	/* Desired Master Clock rate */
} BRAP_OP_ExtMClkSettings;


#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Summary:    
	Settings to configure PLL.     

	Description:
	
	See Also:
	
***************************************************************************/
typedef struct BRAP_OP_PllSettings
{	
	unsigned		uiVcxo;			/* Which VCXO Rate Manager will drive this PLL */
} BRAP_OP_PllSettings;



/***************************************************************************
Summary:
    Returns the current PLL settings. 
	
Description:
	This API can be called from application layer to retreive current PLL settings. 
	
Returns:
	BERR_SUCCESS 
See Also:
	
****************************************************************************/
BERR_Code
BRAP_OP_Pll_GetSettings (    
	BRAP_Handle         hRap,		/* [in] Audio Device Handle */    
	BRAP_OP_Pll     	ePll,		/* [in] PLL Index */
	BRAP_OP_PllSettings *pSettings  /* [out] PLL Settings */
);

/***************************************************************************
Summary:
    Configure PLL based on the settings provided. 
	
Description:
	This API can be called from application layer to configure PLL. 
	
Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code
BRAP_OP_Pll_SetSettings (    
	BRAP_Handle         hRap,		/* [in] Audio Device Handle */    
	BRAP_OP_Pll     	ePll,	 	/* [in] PLL Index */    
	BRAP_OP_PllSettings *pSettings  /* [out] PLL Settings */
);
#endif

/***************************************************************************
Summary:
	Enable/Disable the Test tone

Description:
	This API plays the samples stored in the test tone buffer through
	the specified DAC. This can be used even before a decode channel is 
	opened and just after opening the raptor device. The samples for the
	test tone generaion is given by the application or if application has asked
	to use the default test-tone then PI uses that for test tone generation.
	If bEnable = False, it stops the enabled testtone.

Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code
BRAP_OP_DacEnableTestTone(
    BRAP_Handle     hRap,				/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,		/* [in] Output Type */
    bool	bEnable,					/* [in] True = Enable; False = Disable */
    const BRAP_OP_DacToneInfo *pToneInfo	/* [in] Test tone control structure */
);

/***************************************************************************
Summary:
	To get the default testtone info

Description:
	This API gives the default test tone settings for the app.
	
Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code
BRAP_OP_DacGetDefaultTestTone(
    BRAP_Handle     hRap,					/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,			/* [in] Output Type */
    BRAP_OP_DacToneInfo *pToneInfo		/* [out] Test tone control structure */
);


/***************************************************************************
Summary:
	Configures the external Master Clocks
Description:
	This API is used to configure the external master clock for the specified pll and
	MClock rate.
Returns:
	BERR_SUCCESS 
See Also:

****************************************************************************/
BERR_Code BRAP_OP_ExtMClkConfig ( 
	const BRAP_Handle 		hRap, 				/* [in] The RAP Channel handle */
	const BRAP_OP_ExtMClk	eMClock,			/* [in] The MClock output */
	BRAP_OP_ExtMClkSettings	*pExtMClkSettings	/* [in] External Master clock settings */
	);


#ifdef __cplusplus
}
#endif


#endif /* _BRAP_OP_H_ */

/* End of File */
