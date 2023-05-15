/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_types.h $
* $brcm_Revision: Hydra_Software_Devel/26 $
* $brcm_Date: 8/3/10 4:49p $
*
* Module Description: Public data types common to all the modules of
*					  Raptor PI.
*
* Revision History:
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_types.h $
* 
* Hydra_Software_Devel/26   8/3/10 4:49p gautamk
* SW7405-3063: [7405] Exposed header files should not define _P_ i.e.
* private MACROS.
* 
* Hydra_Software_Devel/25   11/12/09 2:36p gautamk
* SW7420-448 : [7420] Adding new API to Program RFMOD source
* 
* Hydra_Software_Devel/24   11/12/09 11:19a bhanus
* SW7468-26:[7468]Adding 7468 support
* 
* Hydra_Software_Devel/23   10/2/09 2:37a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/22   9/2/09 10:37a sgadara
* SW3548-2443: [3548,3556,7405] Changing the HIFIDAC version as all the
* three of 3548,3556 and 7405 chips are of 65nm.
* 
* Hydra_Software_Devel/21   8/20/09 11:39a bhanus
* PR 55499 : [7125]Adding single PLL support for 7125
* 
* Hydra_Software_Devel/20   8/11/09 3:18p gautamk
* PR55224: [7405] Fixing compilatio for 7340,7342
* 
* Hydra_Software_Devel/19   7/23/09 4:50p bhanus
* PR 55499 : [7340,7342,7125]Adding 7420 Family support
* 
* Hydra_Software_Devel/18   7/9/09 3:29p gautamk
* PR56556:[7405] Programming Hifidac correctly for 130nm and 65nm chips,
* to fix dac polarity issue.
* 
* Hydra_Software_Devel/17   6/18/09 2:20p gautamk
* PR56112:[7420] Adding PI support for HDMI 5.1PCm for 7420
* 
* Hydra_Software_Devel/16   6/12/09 3:36p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/15   1/31/09 5:59p gautamk
* PR48850: [7336] support for 7336
* 
* Hydra_Software_Devel/14   11/11/08 2:47p bhanus
* PR 47757 : [7420]Checking in 7420 Support
* 
* Hydra_Software_Devel/13   9/9/08 6:50p sushmit
* PR 46211: [7405] Adding Data swapping support for PCM Playback
* 
* Hydra_Software_Devel/12   8/6/08 2:54p speter
* PR 45444: [3548,3556] Adding the hardware equalizer support for 5 band
* equalization and Tone control
* 
* Hydra_Software_Devel/11   6/15/08 4:04a speter
* PR 38950: [3548,3556] Adding support for hardware equalizer. Adding the
* interface
* 
* Hydra_Software_Devel/10   6/11/08 7:29a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/9   5/20/08 2:35p speter
* PR 38950: [3548,3556] Adding APIs for new features like Audio
* Descriptors and NICAM
* 
* Hydra_Software_Devel/8   5/20/08 1:56p speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/7   5/14/08 4:30p speter
* PR 38950: [3548,3556] Adding code to support HIFIDAC1 and HIFIDAC2
* 
* Hydra_Software_Devel/6   5/5/08 11:45a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/5   5/2/08 2:48p gautamk
* PR41726: [7405] Adding PI support for Fw Verification in mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch_FwVerfication/1   5/2/08 2:19p gautamk
* PR41726: [7405] Adding PI support for Fw Verification.
* 
* Hydra_Software_Devel/4   2/18/08 5:53p gautamk
* PR34648: [7405] Modifying OPmode enum
* 
* Hydra_Software_Devel/3   2/15/08 8:45p gautamk
* PR34648: [7405]Adding User Config for Passthru
* Adding spdif CBit buffer.
* 
* Hydra_Software_Devel/2   1/24/08 12:05a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/13   12/16/07 7:16p sushmit
* PR 36084: [7405] Adding support for adding separate mixer per output
* port.
* 
* Hydra_Software_Devel/12   11/19/07 6:18p gdata
* PR 33255: [7440] Merging the UOD2 timing marker on main branch with new
* interface
* 
* Hydra_Software_Devel/11   10/12/07 12:29p gautamk
* PR35688: [7405] Implementing FW authentication support (NDS support)
* for 7405 code base.
* 
* Hydra_Software_Devel/1   1/22/08 4:48p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/2   10/31/07 1:43p nitinb
* PR 34648: [7405] Implemented review comments of new API interface as
* mentioned in PR progress-log (dated 10/31/2007)
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   10/25/07 6:28p nitinb
* PR 34648: [7405] Added handle for audio processing stage
* 
* Hydra_Software_Devel/10   4/24/07 2:29p gautamk
* PR30069: Changes for 7.1 Output mode support
* 
* Hydra_Software_Devel/9   2/27/07 9:02p kagrawal
* PR 27912: Modifications to take Audio Processing config params
* 
* Hydra_Software_Devel/8   2/24/07 2:11p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/7   1/23/07 1:10p sushmit
* PR 26864: Fixing CC issues
* 
* Hydra_Software_Devel/6   1/22/07 7:28p sushmit
* PR 26864: Added channel pair for ext capture
* 
* Hydra_Software_Devel/5   12/27/06 4:05p sushmit
* PR 25607: Changing Cap Input Port enums to enable transcode to work
* till proper fix is done.
* 
* Hydra_Software_Devel/4   12/13/06 4:25p speter
* PR 25346: [3563] SPDIF/HDMI In Feature
* 
* Hydra_Software_Devel/3   11/16/06 12:23p sushmit
* PR 25607: Encode/Transcode Additions.
* 
* Hydra_Software_Devel/2   10/6/06 5:50p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/1   8/31/06 12:12p kagrawal
* PR 20654: Delinked from base and added a fresh file for base2
* 
* Hydra_Software_Devel/7440_bringup_0.1/1   8/18/06 9:41a speter
* PR 20654:Creating new branch for 0.1 7440 bringup
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/8   8/2/06 9:18p kagrawal
* PR 20654: Fixed compilation error
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/7   7/31/06 11:47a kagrawal
* PR 20654: Removed BRAP_7440_ARCH to brap_priv.h
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/6   7/26/06 10:10p kagrawal
* PR 20654: Added BRAP_OutputMode_eLast
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/5   7/17/06 4:46p bhanus
* PR 20654: Added I2S3 and I2S4 to the BRAP_OutputPort enum
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/4   7/17/06 1:21p bmishra
* PR 20654 : Updated BRAP_OutputChannel and BRAP_OutputChannelPair enums
* to include Rear Left and Right channels
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/3   7/3/06 3:46p bhanus
* PR 20654: After Merging to latest code as on 6/28.
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/2   4/17/06 6:28p kagrawal
* PR 20654: Defined BRAP_7440_ARCH macro for code using 7440 FMM
* 
* Hydra_Software_Devel/BLR_RAP_7440A0_Bring_Up_Branch/1   4/17/06 11:21a kagrawal
* PR 20654: Initial check in for AM
* 
* Hydra_Software_Devel/21   3/28/06 12:24p rnayak
* PR20439: Restore Bangalore Raptor files after magnum vob corruption on
* 17th March 2006
* 
* Hydra_Software_Devel/21   3/21/06 6:24p sushmit
* PR18604: Updating review comments.
* 
* Hydra_Software_Devel/20   2/27/06 4:15p sushmit
* PR18604: Fixing 7411 compilation issues.
* 
* Hydra_Software_Devel/19   2/17/06 9:50a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/4   2/16/06 4:44p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/3   2/6/06 4:14p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   1/16/06 5:36p sushmit
* PR18604: Updating Audio Encoder PI.
* 
* Hydra_Software_Devel/16   12/9/05 5:28p rjain
* PR 18264: Moving PB and CAP related public structure etc from
* brap_pcm.h .
* 
* Hydra_Software_Devel/15   11/29/05 6:32p bmishra
* PR 18264: Modified PCM Capture related enums for 7401
* 
* Hydra_Software_Devel/14   11/29/05 1:58p bmishra
* PR 18264: Added PCM Capture related enums
* 
* Hydra_Software_Devel/13   10/26/05 2:31p nitinb
* PR 17415: Assigned value of 2 to enum value OutputMode_eTrueMono to
* keep backward compatibility
* 
* Hydra_Software_Devel/12   10/24/05 5:32p nitinb
* PR 17234: Add ability to change BRAP_OutputMode after decode started
* 
* Hydra_Software_Devel/11   9/19/05 7:14p bmishra
* PR 16148: Removed chip specific code from interface files
* 
* Hydra_Software_Devel/10   9/13/05 7:41p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   7/12/05 8:16p bmishra
* PR 16148: Modified the file for 7401 compatibility
* 
* Hydra_Software_Devel/8   6/24/05 3:33p bmishra
* PR 14143: Implemented code review comments
* 
* Hydra_Software_Devel/7   2/16/05 8:29p bmishra
* PR 14143: Corrected BRAP_OutputPortXXXX to BRAP_OutputXXXX where the
* structures and enums are not output port specific
* 
* Hydra_Software_Devel/6   2/16/05 1:46p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/5   2/15/05 8:08p bmishra
* PR 14143: Implemented RAP PI Phase 1 code review comments affecting
* interface change.
* 
* Hydra_Software_Devel/4   1/28/05 12:24a nitinb
* PR 13082: Fixed compilation warnings
* 
* Hydra_Software_Devel/3   1/11/05 11:47a rjain
* PR13082: corrected Rfmod to RfMod
* 
* Hydra_Software_Devel/2   1/10/05 8:21p nitinb
* PR 13082: Initial version
* 
***************************************************************************/
#ifndef _BRAP_TYPES_H__
#define _BRAP_TYPES_H__


#include "bstd.h"
#include "bmem.h"			/* Chip memory access. */
#include "bkni.h"			/* The Kernel Interface */
#include "bint.h"			/* The Interrupt Interface */
#include "bdbg.h"			/* debug header file */
#include "berr_ids.h"		/* error code header file */

#ifdef __cplusplus
extern "C" {
#endif

#if ((BCHP_CHIP == 7420)||(BCHP_CHIP == 7340)||(BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7468))
#define BRAP_7420_FAMILY    1
#endif

#if (BCHP_CHIP == 7550)
#define BRAP_7550_FAMILY    1
#endif

#if((BCHP_CHIP == 7405)||(BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) || (BRAP_7420_FAMILY == 1) || (BRAP_7550_FAMILY == 1))
#define BRAP_7405_FAMILY    1
#endif

#if((BCHP_CHIP == 3548)||(BCHP_CHIP == 3556))
#define BRAP_3548_FAMILY    1
#endif

/* Enable/Disable UOD2 Timing Marker Support */
#if (BRAP_TIMING_MARKER == 1)
#define BRAP_NEW_TIMING_MARKER 1
#else
#define BRAP_NEW_TIMING_MARKER 0
#endif



/***************************************************************************
Summary:
	Main Audio Device handle.

Description:
	This is an opaque handle that an application creates with BRAP_Open 
	which holds the device context of the Audio device.
See Also:
	BRAP_Open, BRAP_Close.
***************************************************************************/
typedef struct BRAP_P_Device			*BRAP_Handle;

/***************************************************************************
Summary:
	Audio channel handle.

Description:
	This is an opaque handle for each Audio channel

See Also:
	BRAP_DEC_OpenChannel, BRAP_DEC_CloseChannel.
***************************************************************************/
typedef struct BRAP_P_Channel			*BRAP_ChannelHandle;              
    
/***************************************************************************
Summary:
	Group of Audio channel handles associted together to produce an audio.
Description:
	This is an opaque handle for a group of audio channel associted together.
See Also:
    BRAP_AssociateChannnels, BRAP_DissociateChannnels.
***************************************************************************/
typedef struct BRAP_P_AssociatedChanGrp	*BRAP_AssociatedChannelHandle;              


/***************************************************************************
Summary:
    Destination handle
Description:
	This is an opaque handle for a destination
See Also:
    BRAP_AddDestination
***************************************************************************/
typedef struct BRAP_P_DstDetails	    *BRAP_DestinationHandle;

/***************************************************************************
Summary:
    Audio processing stage handle
Description:
    This is an opaque handle for an audio processing stage. 
See Also:
***************************************************************************/ 
typedef struct BRAP_P_AudioProcessingStage  *BRAP_ProcessingStageHandle;

/***************************************************************************
Summary:
    Audio descriptor fade handle
Description:
    This is an opaque handle for an audio descriptor object 
See Also:
***************************************************************************/ 
typedef struct BRAP_P_AudioDescriptor *BRAP_AudioDescriptorFadeHandle;

/***************************************************************************
Summary:
	Audio equalizer handle.

Description:
	This is an opaque handle for each Equalizer

See Also:
***************************************************************************/
typedef struct BRAP_P_Equalizer			*BRAP_EqualizerHandle;   

/***************************************************************************
Summary:
	Supported Audio Outputs.

Description:
	This enum defines the different types of supported Audio Outputs.
	
See Also:
	
****************************************************************************/
typedef enum BRAP_OutputPort
{
	BRAP_OutputPort_eSpdif = 0,
	BRAP_OutputPort_eDac0,
	BRAP_OutputPort_eI2s0,
	BRAP_OutputPort_eI2s1,
	BRAP_OutputPort_eI2s2,
	BRAP_OutputPort_eDac1,
	BRAP_OutputPort_eMai,
	BRAP_OutputPort_eFlex,
	BRAP_OutputPort_eRfMod,
    BRAP_OutputPort_eI2s3,
    BRAP_OutputPort_eI2s4,
    BRAP_OutputPort_eSpdif1,
    BRAP_OutputPort_eI2s5,
    BRAP_OutputPort_eI2s6,
    BRAP_OutputPort_eI2s7,
    BRAP_OutputPort_eI2s8,
	BRAP_OutputPort_eDac2,
    BRAP_OutputPort_eMaiMulti0,
    BRAP_OutputPort_eMaiMulti1,
    BRAP_OutputPort_eMaiMulti2,
    BRAP_OutputPort_eMaiMulti3,    
	BRAP_OutputPort_eMax
}BRAP_OutputPort;

/***************************************************************************
Summary:
	Supported Audio Capture Inputs.

Description:
	This enum defines the different types of supported Audio Capture Inputs.
	
See Also:
	
****************************************************************************/
typedef enum BRAP_Input
{
	BRAP_Input_eI2s,
	BRAP_Input_RfAudio
}BRAP_Input;

/***************************************************************************
Summary:
	Supported audio output modes.

Description:
	This enumeration defines the user's choice of audio output mode. This 
	enum defines number of front and back channels. eN_M represents
	N number of front channels and M number of back channels.

See Also:

***************************************************************************/
typedef enum BRAP_OutputMode
{
	BRAP_OutputMode_eRightMono = 0, /* Right Mono - Deprecated, Don't use */
	BRAP_OutputMode_eLeftMono = 0,	   	/* Left Mono - Deprecated, Don't use  */
	BRAP_OutputMode_eTrueMono = 2,		/* Left and Right Mono- Deprecated, Don't use  */
	BRAP_OutputMode_eStereo = 3,		/* Stereo - Deprecated, Don't use */
	BRAP_OutputMode_e1_0 = 0,
	BRAP_OutputMode_e1_1 = 1,
	BRAP_OutputMode_e2_0 = 3,
	BRAP_OutputMode_e3_0 = 4,
	BRAP_OutputMode_e2_1 = 5,
	BRAP_OutputMode_e3_1 = 6,
	BRAP_OutputMode_e2_2 = 7,
	BRAP_OutputMode_e3_2 = 8,
	BRAP_OutputMode_e3_3 = 9,
	BRAP_OutputMode_e3_4 = 10,
    BRAP_OutputMode_eLast
}BRAP_OutputMode; 


/***************************************************************************
Summary:
	Format of PCM output mode.
Description:
	Enum defining various PCM output modes - left mono, right mono and 
	stereo. In case of left / right mono, only left / right samples will be
	sent to both the L & R outputs. In case of stereo, both L & R samples 
	are respectively sent to L & R outputs.

	Supported configurations with various BRAP_PcmDataMode:
	------------- --------------------------------------------------	
	|			 |		  	*BRAP_BufDataMode*					   | 
	|O/p Mode 	 | eMono - StereoInterleaved - StereoNonInterleaved|		
	-------------|------   -----------------   --------------------|
	|Left Mono	 | NO	 | YES				 | YES				   |
	|Right Mono	 | NO 	 | YES				 | YES	 			   |
	|Stereo		 | YES	 | YES				 | YES				   |	
	----------------------------------------------------------------
See Also:
	BRAP_PB_ChannelSettings

****************************************************************************/
typedef enum BRAP_PcmOutputMode
{
    BRAP_PcmOutputMode_eLeftMono,   /* Left samples will be playback on both 
    								   the output channels (L and R) */
    BRAP_PcmOutputMode_eRightMono, 	/* Right samples will be playback on 
    								   both the output channels (L and R) */
    BRAP_PcmOutputMode_eStereo,    	/* Left and Right samples will be 
    								   playback on Left and Right output 
    								   channels respectively */
    BRAP_PcmOutputMode_eMaxNum		/* Max number of PcmOutput modes */
}BRAP_PcmOutputMode;


/***************************************************************************
Summary:
	Input bits per sample.

Description:
	This enumeration defines the input bits per sample for the PCM data. 
	This also indicates the way ring buffers are written by the application.
	
	Note: 
	
	For PCM Playback, 18, 20, 24 values are not allowed for the input 
	bits per sample. For such samples, caller has to pad the least 
	significant bits of the samples with zeros and pass input bits per 
	samples as 32 bits.

	8 bits/sample is not supported for capture channels.
	
See Also:
	BRAP_PB_AudioParams, BRAP_CAP_AudioParams
	
**************************************************************************/
typedef enum BRAP_InputBitsPerSample
{
	BRAP_InputBitsPerSample_e8 = 8,		/* 8 bits per input sample:4 samples per
	                                       32-bit. This is only supported for 
	                                       PCM Playback */    
	BRAP_InputBitsPerSample_e16 = 16,	/* 16 bits per input sample: 2 samples 
	                                       per 32-bit */
	BRAP_InputBitsPerSample_e18 = 18,	/* 18 bits per input sample :1 samples 
	                                       per 32-bit, padded LS bits with 0's*/ 
	BRAP_InputBitsPerSample_e20 = 20,	/* 20 bits per input sample :1 samples 
	                                       per 32-bit, padded LS bits with 0's*/ 	
	BRAP_InputBitsPerSample_e24 = 24,	/* 24 bits per input sample :1 samples 
	                                       per 32-bit, padded LS bits with 0's*/ 		
	BRAP_InputBitsPerSample_e32 =32 	/* 32 bits per input sample. For PCM 
	                                       playback, if input PCM data is 24 
	                                       bits per sample then application has 
	                                       to use BRAP_InputBitsPerSample_e32 
	                                       and pad the LSB 8 bits with zeros. */
} BRAP_InputBitsPerSample;


/***************************************************************************
Summary:
	Capture Input Ports.

Description:
	It enumerates various PCM Capture Input Ports present in Raptor.	
	
See Also:
	BRAP_CapInputType
	
**************************************************************************/
typedef enum BRAP_CapInputPort
{
    BRAP_CapInputPort_eIntCapPort0 = 0, /* Internal Capture port 0 */
    BRAP_CapInputPort_eIntCapPort1,     /* Internal Capture port 1 */
    BRAP_CapInputPort_eIntCapPort2,     /* Internal Capture port 2 */
    BRAP_CapInputPort_eIntCapPort3,     /* Internal Capture port 3 */
    BRAP_CapInputPort_eExtI2s0,         /* External I2S Capture port */
    BRAP_CapInputPort_eRfAudio,         /* RF Audio Capture port */
    BRAP_CapInputPort_eSpdif,           /* SPDIF Capture port */
    BRAP_CapInputPort_eHdmi,            /* HDMI */
    BRAP_CapInputPort_eAdc,    
    BRAP_CapInputPort_eIntCapPort4,     /* Internal Capture port 4 */
    BRAP_CapInputPort_eIntCapPort5,     /* Internal Capture port 5 */
    BRAP_CapInputPort_eIntCapPort6,     /* Internal Capture port 6 */
    BRAP_CapInputPort_eIntCapPort7,     /* Internal Capture port 7 */
    BRAP_CapInputPort_eMax              /* Invalid/last Entry */
} BRAP_CapInputPort;

/***************************************************************************
Summary:
	Capture Input Types.

Description:
	It enumerates various types of the PCM Capture Inputs. Each of the PCM 
	Capture input ports indicated by enum BRAP_CapInputPort can carry data 
	from different sources such as External I2S or the I2S and Flex output 
	ports present in the Device can be fed back to the Capture Input. This 
	enum defines such PCM data sources.

	There can be restriction on the Input Type for an Capture Input Port, 
	which can vary from chip to chip. For example, for 7411, Capture Inport 
	Port 1 can't carry External data. So it is used only if any of the I2S 
	or Flex output is looped back to it.
	
See Also:
	BRAP_CapInputPort
	
**************************************************************************/
typedef enum BRAP_CapInputType
{
      BRAP_CapInputType_eExtI2s = 0,  /* The input port carries External 
										 I2S data */
      BRAP_CapInputType_eIntI2s0,     /* I2S0 output is looped back to the 
										 Capture input */
      BRAP_CapInputType_eIntI2s1,     /* I2S1 output is looped back to the 
										 Capture input. */
      BRAP_CapInputType_eIntI2s2,     /* I2S2 output is looped back to the 
										 Capture input.  */
      BRAP_CapInputType_eIntFlex      /* Flex output is looped back to the 
										 Capture input */
} BRAP_CapInputType;

/***************************************************************************
Summary:
	Capture Modes.

Description:
	It enumerates various modes with which a PCM Capture Channels can be 
	configured. Followings are athe meaning of different capture modes.

	Capture Only:	Here PCM Data coming From Capture Input goes to the 
					associated Ring Buffers (DRAM).
	Full Duplex:	Here PCM Data coming From Capture Input goes to the 
					associated Ring Buffers (DRAM) and gets played back 
					from the ring buffer at the desired output port as well. 
					In this mode it is possible to introduce delay in the 
					playback since the data goes via ring buffers.
	By Pass:		Here PCM Data coming From Capture Input gets played back 
					at the desired output port without 
					geting stored in Ring Buffers (DRAM). In this mode it is 
					not possible to introduce delay in the playback since 
					the data doesn't go via ring buffers.
	
See Also:

	
**************************************************************************/
typedef enum BRAP_CaptureMode
{
        BRAP_CaptureMode_eCaptureOnly = 0,	/* Capture Only Mode. Refer to 
											   the comments in the Enum header 
											   for explanation */
        BRAP_CaptureMode_eFullDuplex,		/* Full duplex Mode. Refer to 
											   the comments in the Enum header 
											   for explanation  */
        BRAP_CaptureMode_eByPass,			/* Bypass Mode. Refer to 
											   the comments in the Enum header 
											   for explanation */
        BRAP_CaptureMode_eMaxCaptureMode
} BRAP_CaptureMode;

/***************************************************************************
Summary:
	Audio output channel selects.

Description:
	This enumeration defines different output channels corresponding to 
	various speaker systems.

See Also:

**************************************************************************/
typedef enum BRAP_OutputChannel
{
	BRAP_OutputChannel_eLeft = 0,			/*	Left Streo or Left Mono */
	BRAP_OutputChannel_eRight = 1,			/*	Right Streo or Right Mono */
	BRAP_OutputChannel_eCentreSurround = 2,	/*	Centre Surround, For 3.1 */
	BRAP_OutputChannel_eLeftSurround = 2,	/*	Left Surround,
												For 4.1 and above */
	BRAP_OutputChannel_eRightSurround = 3,	/*	Right Surround,
												For 4.1 and above */
	BRAP_OutputChannel_eCentre = 4,			/*	Centre,  For 5.1 and above */
	BRAP_OutputChannel_eLowFrequency = 5,	/*	Low Frequency */
	BRAP_OutputChannel_eLeftRear = 6,	    /*	Left Rear */
	BRAP_OutputChannel_eRightRear = 7,	    /*	Right Rear */
	BRAP_OutputChannel_eMax
} BRAP_OutputChannel;

/***************************************************************************
Summary:
	Audio output channel pair indexes.

Description:
	This enumeration defines different output channel pairs corresponding to 
	various speaker systems.

See Also:

**************************************************************************/
typedef enum BRAP_OutputChannelPair
{
	BRAP_OutputChannelPair_eLR = 0,				/* Left & Right Streo or Mono */
    BRAP_OutputChannelPair_eCompressed = 0,     /* For compressed channels */
    BRAP_OutputChannelPair_eCaptureLR = 0,     /* For Captured Input channels L/R or Compressed */
	BRAP_OutputChannelPair_eLRSurround = 1,		/* Left & Right Surround */
	BRAP_OutputChannelPair_eCentreSurround = 1,	/* Left channel used for Centre 
												   Surround for 3.1 */
	BRAP_OutputChannelPair_eCentreLF = 2,		/* Centre & Low Frequency */
	BRAP_OutputChannelPair_eLRRear = 3,         /* Rear Left and Rear Right 
	                                               channels */
	BRAP_OutputChannelPair_eMax
} BRAP_OutputChannelPair;

/***************************************************************************
Summary:
	Format of data mode.
Description:
	Enum defining various data mode - mono, stereo interleaved or 
	stereo non-interleaved. 
		In case of PCM Playback and mixing, the ring buffer required for mono 
	and stereo interleaved is 1, while 2 ring buffers are required for stereo 
	non-interleaved.
See Also:
	BRAP_PB_ChannelSettings, BRAP_CAP_ChannelSettings

****************************************************************************/
typedef enum BRAP_BufDataMode
{
    BRAP_BufDataMode_eMono = 0,       		/* Mono (requires 1 buffer) */
    BRAP_BufDataMode_eStereoInterleaved,    /* Stereo Interleaved (requires 1 
                                               buffer) */
    BRAP_BufDataMode_eStereoNoninterleaved, /* Stereo Non-Interleaved (requires 
                                               2 buffers) */
    BRAP_BufDataMode_eMaxNum				/* Max number of buffer data mode */		
} BRAP_BufDataMode;



/***************************************************************************
Summary:
	Endian-ness of data.
Description:
	Enum to specify whether data should be taken same as system endian or swapped 
    In case of PCM Playback, application can specify this mode.
See Also:
	BRAP_SRCCH_P_Params

****************************************************************************/
typedef enum BRAP_DataEndian
{
    BRAP_DataEndian_eSame = 0,           /* Same as System Endian */
    BRAP_DataEndian_eSwap,               /* Swap, little to big or big to little */
    BRAP_DataEndian_eMaxNum			  /* Invalid */		
} BRAP_DataEndian;

#ifdef __cplusplus
}
#endif

#endif /* _BRAP_TYPES_H__ */

