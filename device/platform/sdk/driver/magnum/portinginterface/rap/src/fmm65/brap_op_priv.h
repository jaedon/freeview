/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_op_priv.h $
* $brcm_Revision: Hydra_Software_Devel/13 $
* $brcm_Date: 2/11/11 7:40a $
*
* Module Description:
*   Module name: OUTPUT
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Output Port abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_op_priv.h $
* 
* Hydra_Software_Devel/13   2/11/11 7:40a srajapur
* SW7550-601:[7550]Add BTSC Support in FW PI and CIT -------->
* Programming PEAK A1,A2 and GAIN value to reset values when BTSC is
* enabled on DAC
* 
* Hydra_Software_Devel/12   8/25/10 6:42p srajapur
* SW7405-4713: [7405] Fixing compilation error for 7550
* 
* Hydra_Software_Devel/11   7/26/10 2:51p gautamk
* SW7405-4646: [7405] Restoring mute status of O/P at mixer Level in
* every Start.
* 
* Hydra_Software_Devel/10   3/8/10 8:54p sgadara
* SW7420-611: [3548,3556] Reverting back the changes as the existing
* interface of exclusive API for programming DAC volume is made use of.
* DAC Scale register continues to get programmed using SetOutputConfig
* --> For Volume register SetOutputConfig will introduce audio drop since
* there will be mute->unmute happening
* 
* Hydra_Software_Devel/9   1/8/10 5:25p sgadara
* SW3556-940: [3548,3556] Adding the interface to program the SCALE and
* DAC_VOLUME registers in HIFIDAC_CTRL
* 
* Hydra_Software_Devel/8   1/6/10 4:42p gautamk
* SW7405-3063: [7405] Adding support for I2s Stereo 1
* 
* Hydra_Software_Devel/7   10/2/09 2:36a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/6   8/17/09 5:12p gautamk
* PR57743:[7405] using Pvt functions when calling from internal RAP PI
* 
* Hydra_Software_Devel/5   6/18/09 2:19p gautamk
* PR56112:[7420] Adding PI support for HDMI 5.1PCm for 7420
* 
* Hydra_Software_Devel/4   6/12/09 3:39p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/3   4/2/09 11:23a speter
* PR 52729: [3548,3556] Adding bHbrEnable flag to SPDIF Output as High
* bit rate is required to be supported through SPDIF out as well.
* 
* Hydra_Software_Devel/2   10/13/08 5:46p gautamk
* PR42213: [7405] Programming SRC for the PB when O/P sample rate is
* different from input, During decode's sample rate interrupt.
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
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 2:14p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch/RAP_NEWFWARCH_WDOG/1   3/28/08 4:07p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/18   9/30/07 1:54p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/17   9/28/07 10:47a speter
* PR 32625: [3563] Move volume amplification states from output port
* handle to rap handle
* 
* Hydra_Software_Devel/16   6/11/07 9:03p nitinb
* PR 29166: Moved volume states from output handle to RAP handle. This
* removes restriction of calling add destination API before calling
* volume APIs.
* 
* Hydra_Software_Devel/15   6/11/07 7:00p nitinb
* PR 29166: Moved output port mute states from output handle to RAP
* handle. This removes restriction of calling add destination API before
* calling mute API.
* 
* Hydra_Software_Devel/14   6/8/07 3:19p bhanus
* PR 30069 : [7440 B0] Added code for HDMI HBR Compressed Support.
* 
* Hydra_Software_Devel/13   5/24/07 2:43p sushmit
* PR 27646: 7405 Compilation & linking
* 
* Hydra_Software_Devel/12   5/11/07 6:19p kagrawal
* PR 30069: Added BRAP_P_ConfigureFsTimingSource()
* 
* Hydra_Software_Devel/11   5/3/07 4:13p speter
* PR 30400: [3563] Merged the changes done as part of Phase 1.6 Release
* to the main line. Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/RAP_3563_PH_1_6/1   4/27/07 8:05p speter
* PR 29493: [3563] Added Set/Get VolumeAmplification to amplify/attenuate
* the volume on an output port based on 5.23 format mixer coefficients.
* 
* Hydra_Software_Devel/10   4/25/07 12:34p nitinb
* PR 28187: Added field bVolIndB to output port handle
* 
* Hydra_Software_Devel/9   4/9/07 8:22p sushmit
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
* Hydra_Software_Devel/8   3/23/07 8:34p bhanus
* PR 28955 : [7440A0/3563] Modified the code for adding on-the-fly
* configuration for I2S output ports.
* 
* Hydra_Software_Devel/7   2/4/07 5:13p bhanus
* PR 25607 : Fixed compilation warnings.
* 
* Hydra_Software_Devel/6   12/19/06 5:50p bselva
* PR 25346: Checking in the changes for 3563 new features
* 
* Hydra_Software_Devel/5   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/RAP_3563_Merge/2   12/4/06 12:19p sushmit
* PR 24037: Adding 3563 specific preprocessor directive.
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/3/06 11:21p sushmit
* PR 24037: Merging 7440 to 3563.
* 
* Hydra_Software_Devel/4   11/9/06 11:41a bhanus
* PR 24717 : Added Support for PCM Playback.
* - Added BRAP_OP_P_ProgramOutputClock() to program Output port clock.
* 
* Hydra_Software_Devel/3   9/20/06 4:22p kagrawal
* PR 20654: Cleaned exposed API prototypes
* 
* Hydra_Software_Devel/2   9/12/06 11:49a kagrawal
* PR 20654: Delinked from base and added to base2
* 
***************************************************************************/


#ifndef _BRAP_OP_PRIV_H_
#define _BRAP_OP_PRIV_H_

#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    This enumeration lists the byte positions of Flex. 
    
Description:
    Specifies the bit order of each byte in the the 64-bit stream data as 
    it goes out the flexible interface. Depending on other settings, all 
    64-bits might not be tansmitted.

See Also:

***************************************************************************/ 
typedef enum BRAP_OP_P_FlexByteOrder
{
    BRAP_OP_P_FlexByteOrder_eRight_07_00=0,
    BRAP_OP_P_FlexByteOrder_eRight_15_08,
    BRAP_OP_P_FlexByteOrder_eRight_23_16,
    BRAP_OP_P_FlexByteOrder_eRight_31_24,
    BRAP_OP_P_FlexByteOrder_eLeft_07_00,
    BRAP_OP_P_FlexByteOrder_eLeft_15_08,    
    BRAP_OP_P_FlexByteOrder_eLeft_23_16,
    BRAP_OP_P_FlexByteOrder_eLeft_31_24,
    BRAP_OP_P_FlexByteOrder_eNone=15
    
}BRAP_OP_P_FlexByteOrder;



    
/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a MAI ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_MaiSettings
{
    BRAP_OP_MaiSettings     sExtSettings;  /* External parameters */
}BRAP_OP_P_MaiSettings;  


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a MAI ouput port.
    Note that this includes both public and private parameters.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_MaiParams
{
    unsigned int        uiSampleWidth;       /* Specifies the sample width field 
                                              in the MAI format word. This 
                                              field does not affect the actual 
                                              audio sample bits the MAI 
                                              formatter transmits. */
    BAVC_Timebase       eTimebase;          /* Timebase for the PLL */
    uint32_t            ui32InputFciId;     /* The FCI ID of the input resource to 
                                               the output port*/
    bool                bHbrEnable;         /* TRUE: If this MAI Port is running
                                                     at HBR.
                                               FALSE : If MAI port is running
                                                     at Normal rate */

}BRAP_OP_P_MaiParams; 


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a SPDIF ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_SpdifSettings
{
    BRAP_OP_SpdifSettings    sExtSettings;    /* External parameters */
}BRAP_OP_P_SpdifSettings; 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a SPDIF ouput port.
    Note that this includes both public and private parameters.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_SpdifParams
{
    BAVC_Timebase           eTimebase;          /* Timebase for the PLL */
    uint32_t                ui32InputFciId;     /* The FCI ID of the input resource to 
                                                   the output port*/


    bool                    bHbrEnable;         /* TRUE: If this SPDIF port is running
                                                     at HBR.
                                                   FALSE : If SPDIF port is running
                                                     at Normal rate */
}BRAP_OP_P_SpdifParams; 


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening an I2S ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_I2sSettings
{
    BRAP_OP_I2sSettings    sExtSettings;    /* External parameters */

}BRAP_OP_P_I2sSettings; 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting an I2S ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_I2sParams
{
    unsigned int        uiBitsPerSample;    /* The number of bits per sample.
                                               This field is used for LSB 
                                               justified data and ignored for 
                                               MSB-justified data.
                                               Valid values 1-32 */
    BAVC_Timebase       eTimebase;          /* Timebase for the PLL */
    uint32_t            ui32InputFciId;     /* The FCI ID of the input resource to 
                                               the output port*/
    bool                bIsMulti;           /* TRUE: If this I2S Port is part of 
                                                     Multichannel Group 
                                               FALSE : If I2S port is Stereo */
    bool                bHbrEnable;         /* TRUE: If this I2S Port is running
                                                     at HBR.
                                               FALSE : If I2S port is running
                                                     at Normal rate */

}BRAP_OP_P_I2sParams; 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a Flex ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_FlexSettings
{
    int tbd;
}BRAP_OP_P_FlexSettings; 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting an Flex ouput port.
    Note that this includes both public and private settings.

Description:

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_FlexParams
{
    BAVC_Timebase         eTimebase;     /* Timebase for the PLL */

}BRAP_OP_P_FlexParams; 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a DAC ouput port.
    Note that this includes both public and private settings.

Description:
    Presently empty

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_DacSettings
{
    BRAP_OP_DacSettings    sExtSettings;    /* External parameters */

}BRAP_OP_P_DacSettings; 


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a DAC ouput port.
    Note that this includes both public and private settings.

Description:
    Presently empty

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_DacParams
{
	BAVC_Timebase	    eTimebase;          /* Timebase for the PLL */
    uint32_t            ui32InputFciId;     /* The FCI ID of the input resource to 
                                               the output port*/
    bool                bBtscOnDAC;         /* BTSC Audio on DAC port */
}BRAP_OP_P_DacParams; 



/***************************************************************************
Summary:
    Abstraction of an Output port 
    
Description:
    It contains all the information required to handle the Output port 
    Particularly, it stores the indexes, handles for all required chip 
    related information, parent FMM handle etc

    At any given time, more that one audio channel may have been routed 
    to this output port (since upto 3 audio channels can be 'mixed' ie
    combined at the Mixer). In order to ensure that the Ouput port is not 
    accidently stopped/closed by one audio channel, while another channel
    is still using it.. we maintain "open" and "start" counts for each
    output port.

See Also:
    
***************************************************************************/
typedef struct BRAP_OP_P_Object
{
    union 
    {
        BRAP_OP_P_SpdifSettings     sSpdif;     /* SPDIF Open() settings */
        BRAP_OP_P_I2sSettings       sI2s;       /* I2S Open() settings */
        BRAP_OP_P_MaiSettings       sMai;       /* MAI Open() settings */
        BRAP_OP_P_MaiSettings       sMaiMulti;  /* MAI Open() settings */        
        BRAP_OP_P_FlexSettings      sFlex;      /* Flex Open() settings */
        BRAP_OP_P_DacSettings       sDac;       /* HiFi Open() settings */
    } uOpSettings;

    union 
    {
        BRAP_OP_P_SpdifParams       sSpdif;     /* SPDIF Start time parameters */
        BRAP_OP_P_I2sParams         sI2s;       /* I2S Start time parameters */
        BRAP_OP_P_MaiParams         sMai;       /* MAI Start time parameters */
        BRAP_OP_P_MaiParams         sMaiMulti;   /* MAI Start time parameters */        
        BRAP_OP_P_FlexParams        sFlex;      /* Flex Start time parameters */
        BRAP_OP_P_DacParams         sDac;       /* HiFi Start time parameters */
    } uOpParams;

    BRAP_OutputPort                 eOutputPort;/* Output port type: SPDIf etc*/

    BCHP_Handle                     hChip;      /* Handle to chip object */
    BREG_Handle                     hRegister;  /* Handle to register object */
    BMEM_Handle                     hHeap;      /* Handle to memory object */
    BINT_Handle                     hInt;       /* Handle to interrupt object */

    BRAP_FMM_P_Handle               hFmm;       /* Parent FMM handle */
    uint32_t                        ui32Offset; /* Used when there are multiple Outport
                                                   ports of the same kind. For eg 
                                                   multiple I2S ports. This field conveys
                                                   register offset of current output port 
                                                   from base register of port 0 of
                                                   the same kind */
    unsigned int                    uiOpenCnt;  /* No. of open audio channels currently
                                                   routed to this output port */
    unsigned int                    uiStartCnt; /* No. of active audio channels currently
                                                   routed to this output port */
    BAVC_AudioSamplingRate          eSamplingRate;
                                                /* Sampling Rate. Set by DSP for 
                                                   Decode channel. Set by AM for 
                                                   Playback channel. */
    bool		                    bDacMuteState;	
                                                /* Maintains the DAC mute status */
	bool		bRfmodMuteState; /* Maintains the RfMod Mute State */
    BRAP_OutputPort            eRfmodMuxSelect;  /* RFMOD source selected */

    uint32_t	                    ui32DacVolume;	
                                                /* Maintains the current volume level */

    uint32_t                        ui32InputFciId;
                                                /* The FCI ID of the input resource to 
                                                   the output port*/
}BRAP_OP_P_Object;


/***************************************************************************
Summary:
    Opens an Output port.

Description:
    Initializes the Output port and returns a handle.    
    The handle can then be used for all other Output port function calls.
    More than one audio channel may use the same output port, therefore this
    function should be called muliple times. The settings etc are configured 
    only the first time. Each time after that, only the Open Count is 
    incremented - the settings are not changed.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_Close
**************************************************************************/
BERR_Code BRAP_OP_P_Open (
    BRAP_FMM_P_Handle  hFmm,       /*[in] Parent FMM Handle */
    BRAP_OP_P_Handle * phOp,       /*[out] Ouput Port handle */
    BRAP_OutputPort    eOutput,    /*[in] Ouput Port type */
    const void *  pSettings   /*[in] Ouput Port settings */ 
);

/***************************************************************************
Summary:
    Releases all the resources associated with this Output Port and frees 
    the handles.

Description:
    More than one audio channel may use the same output port, therefore this
    function should be called muliple times. Each time the Open Count is 
    decremented, and when it reaches 0, the output port handle is freed.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_Open
**************************************************************************/
BERR_Code
BRAP_OP_P_Close (
    BRAP_OP_P_Handle     hOp        /*[in] Output port handle */
);

/***************************************************************************
Summary:
    Starts an Output Port.

Description:
    More than one audio channel may use the same output port, therefore this
    function should be called muliple times. The parameters etc are configured 
    only the first time. Each time after that, only the Start Count is 
    incremented - the parameters are not changed.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_Stop.
**************************************************************************/
BERR_Code
BRAP_OP_P_Start (
    BRAP_OP_P_Handle    hOp,        /*[in] Output port handle */
    const void * pParams     /*[in] Output Port start time 
                                    parameters. Typecast it to proper
                                    setting structure based on the 
                                    port type */ 
);

/***************************************************************************
Summary:
    Stops an Output Port.

Description:
    More than one audio channel may use the same output port, therefore this
    function should be called muliple times. Each time the Start Count is 
    decremented, and when it reaches 0, the output port is "stopped".

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_Start
**************************************************************************/
BERR_Code
BRAP_OP_P_Stop (
    BRAP_OP_P_Handle    hOp        /*[in] Output port handle */
);

/***************************************************************************
Summary:
    Programs the on-the-fly changeable parameters to the HW registers 
    for the I2S ie AUD_FMM_OP_CTRL_I2S_CFG
Description:
    This PI takes a BRAP_OP_P_I2sSettings structure.
    Based on this it programs the register AUD_FMM_OP_CTRL_I2S_CFG
    It then saves the BRAP_OP_P_I2sSettings structure in hOp.
    At present, the on-the-fly changeable parameter for i2s are  
    eChannelOrder and bLimitTo16Bits
Returns:
    BERR_SUCCESS on success

See Also:
    BRAP_OP_P_I2sStart(), BRAP_OP_P_I2sOpen(), BRAP_SetOutputConfig()
**************************************************************************/
BERR_Code
BRAP_OP_P_I2sHWConfig_Multi (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
);

BERR_Code
BRAP_OP_P_I2sHWConfig_Stereo (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_I2sSettings * pSettings  /*[in] Open time parameters */
);

BERR_Code BRAP_OP_P_GetI2sStereoIndex(
    BRAP_OutputPort                 eOutputPort,            /*[in] Output port  */
    unsigned int     *uiI2sStereoIndex      /*[in] Open time parameters */
);

/***************************************************************************
Summary:
    Programs the on-the-fly changeable parameters to the HW registers 
    for the SPDIF ie AUD_FMM_OP_CTRL_SPDIF_CFG_0
Description:
    This PI takes a BRAP_OP_P_SpdifSettings structure.
    Based on this it programs the register AUD_FMM_OP_CTRL_SPDIF_CFG_X
    It then saves the BRAP_OP_P_SpdifSettings structure in hOp.
    At present, the on-the-fly changeable parameter for Spdif are  
    eChannelOrder and bLimitTo16Bits
Returns:
    BERR_SUCCESS on success

See Also:
    BRAP_OP_P_SpdifStart(), BRAP_OP_P_SpdifOpen(), BRAP_SetOutputConfig()
**************************************************************************/
BERR_Code BRAP_OP_P_SpdifHWConfig (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_SpdifSettings * pSettings /*[in] Parameters */
);

/***************************************************************************
Summary:
    Programs the on-the-fly changeable parameters to the HW registers 
    for the Mai ie AUD_FMM_OP_CTRL_Mai_CFG_0
Description:
    This PI takes a BRAP_OP_P_MaiSettings structure.
    Based on this it programs the register AUD_FMM_OP_CTRL_Mai_CFG_X
    It then saves the BRAP_OP_P_MaiSettings structure in hOp.
    At present, the only on-the-fly changeable parameter for Mai is 
    eChannelOrder
Returns:
    BERR_SUCCESS on success

See Also:
    BRAP_OP_P_MaiStart(), BRAP_OP_P_MaiOpen(), BRAP_SetOutputConfig()
**************************************************************************/
BERR_Code
BRAP_OP_P_MaiHWConfig (
    BRAP_OP_P_Handle        hOp,       /*[in] Rap handle */
    const BRAP_OP_P_MaiSettings *pMaiSettings /* [in]Pointer to MAI settings */
);



/***************************************************************************
Summary:
    Returns default values for Output port Open time settings.

Description:
    For settings that the system cannot assign default values to, 
    an invalid value is returned. 

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_GetDefaultParams.
**************************************************************************/
BERR_Code 
BRAP_OP_P_GetDefaultSettings ( 
    BRAP_OutputPort  eOutput,       /*[in] Ouput Port type */
    void *       pDefSettings   /*[out] Pointer to memory where default
                                 settings should be written */     
);

/***************************************************************************
Summary:
    Returns default values for Output port Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_OP_P_GetDefaultParams ( 
    BRAP_OutputPort  eOutput,     /*[in] Ouput Port type */
    void *       pDefParams   /*[out] Pointer to memory where default
                                 settings should be written */     
);

/***************************************************************************
Summary:
    Returns default values for SPDIF channel status parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code 
BRAP_OP_P_GetDefaultSpdifChanStatusParams ( 
    BRAP_OP_SpdifChanStatusParams *       pDefParams   /*[out] Pointer to 
                           memory where default parameters should be written */
);

/***************************************************************************
Summary:
    Change the sampling rate of an output port

Description:
    This function sets the sampling rate by just selecting MACRO_SELECT
    
    TODO: This function will cause a lock interrupt after 20 uS.
    So  handle that interrupt

    TODO:  Hasnt this shifted to AM?? output is configured from the start.
       sampling rate cant change dynamically.
    
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    
**************************************************************************/
BERR_Code 
BRAP_OP_P_ProgramTimebase (
    BRAP_OP_P_Handle       hOp, 
    BRAP_OP_Pll            ePll,
    BAVC_Timebase          eTimebase
);

BERR_Code 
BRAP_OP_P_ProgramTimebase_isr (
    BRAP_OP_P_Handle       hOp, 
    BRAP_OP_Pll            ePll,
    BAVC_Timebase          eTimebase
);


/* Following functions enable/disable the upstream request from the o/p port temporarily */
BERR_Code
BRAP_OP_P_EnableStream (
    BRAP_OP_P_Handle    hOp,       /*[in] Output port handle */
    bool                bEnable    /*[in] TRUE: Enable Stream 
                                          FALSE: Disable */
);

BERR_Code
BRAP_OP_P_EnableStream_isr (
    BRAP_OP_P_Handle    hOp,       /*[in] Output port handle */
    bool                bEnable    /*[in] TRUE: Enable Stream 
                                          FALSE: Disable */
);

/* Mutes/Unmutes the specified output port */
BERR_Code BRAP_OP_P_SetMute_isr ( 
    BRAP_Handle     hRap,    /* [in]Audio Device Handle */
    BRAP_OutputPort eOpType, /* [in]Output Type */
    bool            bMute    /* [in]TRUE: Mute data at the output port
                                    FALSE: UnMute data at the output port */
);


/***************************************************************************
Summary:
    Mute/UnMute data at the output port. 
    The Mute status is maintained over channel change.

Description:
    This Mute can be used for all output ports irrespective of the data type
    ie compressed or PCM data.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_OP_P_GetMute().
    
**************************************************************************/
BERR_Code BRAP_OP_P_SetMute ( 
    BRAP_Handle     hRap,    /* [in]Audio Device Handle */
    BRAP_OutputPort eOpType, /* [in]Output Type */
    bool            bMute    /* [in]TRUE: Mute data at the output port
                                    FALSE: UnMute data at the output port */
);

BERR_Code BRAP_OP_P_GetMute ( 
    BRAP_Handle             hRap,       /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,    /* [in]Output Type */
    bool *                  pMute       /* [out]Pointer to memory where the Mute 
                                                status is to be written
                                                TRUE: output port is Muted
                                                FALSE:  output port is not Muted */
);

/***************************************************************************
Summary:
    Change the sampling rate of an output port

Description:
    This function sets the sampling rate by just selecting MACRO_SELECT

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:

**************************************************************************/
#if (BRAP_7550_FAMILY !=1)
BERR_Code 
BRAP_OP_P_ProgramPll (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_Pll 			ePll,
    BAVC_AudioSamplingRate 	eSamplingRate
);

BERR_Code 
BRAP_OP_P_ProgramPll_isr (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_Pll 			ePll,
    BAVC_AudioSamplingRate 	eSamplingRate
);
#else
BERR_Code 
BRAP_OP_P_ProgramNCO (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_MClkRate 			eMClkRate,
    BAVC_AudioSamplingRate 	eSamplingRate
);
BERR_Code 
BRAP_OP_P_ProgramNCO_isr (
    BRAP_OP_P_Handle       	hOp, 
    BRAP_OP_MClkRate 			eMClkRate,
    BAVC_AudioSamplingRate 	eSamplingRate
);
#endif
/**************************************************************************
Summary:
	This function programs the PLL for the output port.
Description:
    This function programs the output sampling rate of an output by programming
    the PLL or Rate Manager.
Returns:
    BERR_SUCCESS on success else error
    
See Also:
 	None
**************************************************************************/

BERR_Code BRAP_OP_P_ProgramOutputClock(
    BRAP_ChannelHandle      hRapCh,
    BRAP_OutputPort         eOp, 
	BAVC_AudioSamplingRate  eOutputSR
    );
/***************************************************************************
Summary:
	Private function that updates sampling rate, program PLL and SRC.
Description:
	Private function that updates sampling rate, program PLL and SRC.
Returns:
	BERR_SUCCESS 
See Also:
	BRAP_OP_SetSamplingRate
****************************************************************************/
BERR_Code 
BRAP_OP_P_UpdateSamplingRate(
		BRAP_ChannelHandle hRapCh,
		BRAP_OutputPort eOutputPort, 
		BAVC_AudioSamplingRate eOutputSR, 
		BAVC_AudioSamplingRate eInputSR, 
    	bool bCalledFromISR 
);

/***************************************************************************
Summary:
	Private function that sets the sampling rate and time base associated 
	with the output port.
Description:
	This private function is used to set the sampling rate and time base 
	associated with a particular output port. Currently, this is supported 
	only for a PCM playback channel. For other channel, it will return an 
	error.

	Note: bCalledFromISR is needed because atomic code is very small part of 
	      this big routine and sub-routines called from it. If we have a 
	      XYZ_isr func, and a wrapper around it XYZ with enter and leave
	      critical section, all interrupts will get masked for a considerable
	      time.!!!
Returns:
	BERR_SUCCESS 
See Also:

****************************************************************************/
BERR_Code BRAP_OP_P_SetSamplingRate ( 
	const BRAP_Handle 		hRap, 			/* [in] The RAP Channel handle */   
	const BRAP_OutputPort	eOutputPort, 	/* [in] Output port */
	BAVC_AudioSamplingRate  eSamplingRate, 	/* [in] Output Sampling rate */
	bool                    bOverride,      /* [in] Flag indicating if a dec
	                                           channel's output SR be used
	                                           to override the output SR 
	                                           passed to this func, if both
	                                           output to the same output port */
	bool                    bCalledFromISR  /* [in] Flag indicating if this 
	                                           routine has been called from the
	                                           sampling rate change ISR or else
	                                           where */
);


/***************************************************************************
Summary:
	Gets all the direct and indirect channel handles associated to the output
	port.
Description:
    This function returns all the channel handles (direct/indirect) associated
	to the output port. 
	    Direct channels are those which feed data directly to this output port. 
	While indirect channels are ones that feed data to some other output port 
	which gets captured by a capture channel which directly feeds to this 
	output port. OR they capture data from the concerned output port and feed 
	to some other output port.
	    So, all associated channels feed/use data to/from the output port 
	directly or indirectly.
Returns:
    BERR_SUCCESS on success, else error.
    hChannels[x] will have non-NULL ptrs for channels using mixer.
See Also:
 	None
**************************************************************************/
BERR_Code BRAP_OP_P_GetAssociatedChannels(
    const BRAP_Handle       hRap, 	        /* [in] The RAP Channel handle */
    BRAP_ChannelHandle      *phDirChannels, /* [out] Pointer to the first index 
                                                of an array of direct channel 
                                                handles */
    BRAP_ChannelHandle      *phIndirChannels,/* [out] Pointer to the first index 
                                                of an array of indirect channel 
                                                handles */
	const BRAP_OutputPort   eOutputPort,    /* [in] Output port */                                           
	bool                    bCalledFromISR  /* [in] Flag indicating if this 
	                                           routine has been called from the
	                                           sampling rate change ISR or else
	                                           where */

);

/***************************************************************************
Summary:
	Returns handles of DEC/PB/CAP channels directly using a output port..
Description:
    This function returns all the channel handles using a particular output port. 
Returns:
    BERR_SUCCESS on success, else error.
    phChannels[x] will have non-NULL ptrs for channels using output port.
See Also:
 	None
**************************************************************************/
 
BERR_Code BRAP_P_GetChannelsUsingOpPort(
    const BRAP_Handle       hRap, 	        /* [in] The RAP Channel handle */
    const BRAP_OutputPort   eOutputPort,    /* [in] Output port */    
    BRAP_ChannelHandle      *phChannels /* [out] Pointer to the first index 
                                            of an array of direct channel handles */
    );

/***************************************************************************
Summary:
    Returns the DAC which is used as source for Rfmod
Description:
    This function returns DAC which is used as the source for the Rfmod output
Returns:
    BRAP_OutputPort_eMax for failure and correct port on success
See Also:
 	None
**************************************************************************/

BRAP_OutputPort
BRAP_OP_P_GetRfmodMuxSelector(
		BRAP_Handle     hRap    /* [in]Audio Device Handle */
);


#if (BRAP_7550_FAMILY != 1)    
#if ((BRAP_3548_FAMILY == 1)||(BRAP_7405_FAMILY == 1) )
/***************************************************************************
Summary:
    Generic helper routine that configures the MCLK rate and PLLCLKSEL registers
    for FS Timing Source.
    
    Note: Currently, FS timing source is used by CapPort and FCI sinkers.
****************************************************************************/
BERR_Code
BRAP_P_ConfigureFsTimingSource(
    BREG_Handle             hRegister,      /* [in] Register Handle */
    unsigned int            uiFsTmgSrcId,   /* [in] FS Timing Source Id */
    BRAP_OP_Pll 			ePll,           /* [in] PLL to be associated */
    BAVC_AudioSamplingRate 	eSamplingRate   /* [in] Sampling rate of the data at 
                                               capport or FCI sinker driven by 
                                               this FS timing source */
    );
#endif 
#endif

BERR_Code BRAP_OP_P_MuteMixerOfOutput_isr ( 
    BRAP_Handle             hRap,           /* [in]Audio Device Handle */
    BRAP_OutputPort         eOpType,        /* [in]Output Type */
    bool                    bMute           /* [in]TRUE: Mute data at the output port
                                                   FALSE: UnMute data at the output port */
);


#ifdef __cplusplus
}
#endif


#endif /* _BRAP_OP_PRIV_H_ */

/* End of File */
