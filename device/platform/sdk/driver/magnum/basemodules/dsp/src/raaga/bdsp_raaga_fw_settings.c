/***************************************************************************
 *     Copyright (c) 2004-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fw_settings.c $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-4771/SW7425-5412/1 $
 * $brcm_Date: 12/23/13 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fw_settings.c $
 * 
 * Hydra_Software_Devel/SW7425-4771/SW7425-5412/1   12/23/13 3:55p sriniva
 * SW7425-5412: Adding programmable 'ID' for ADTS in AACHE Encoder
 * 
 * Hydra_Software_Devel/SW7425-4771/1   5/3/13 6:08p raghurat
 * SW7425-4771: patches for following JIRAs ( SW7425-4399, SW7425-4386,
 * SW7425-4405, FWRAA-633, FWRAA-643  ) on
 * RAAGA_BDSP_RELEASE_7p6_20130211 for HUMAX
 * 
 * Hydra_Software_Devel/66   1/29/13 12:12p sriniva
 * SW7425-4405: Adding user config to AACHE encoder to select channel
 * while encoding dual mono streams and encode it as mono
 * 
 * Hydra_Software_Devel/65   1/22/13 1:21p dlimaye
 * FWRAA-543: Add 7.1 support to PCMWAV decoder  Changing default
 * userconfig to stereo
 * 
 * Hydra_Software_Devel/64   1/21/13 10:15p dlimaye
 * FWRAA-543: Add 7.1 support to PCMWAV decoder  Checking in src, ids and
 * host interface files
 * 
 * Hydra_Software_Devel/63   1/21/13 10:29a dlimaye
 * FWRAA-543: Add 7.1 support to PCMWAV decoder  Checking in src, ids and
 * host interface files
 * 
 * Hydra_Software_Devel/62   1/16/13 12:07p ramanatm
 * SW7425-3204:[7425] BTSC Soft-Encoder Fails Stereo Separation
 * Measurement
 * 
 * Hydra_Software_Devel/61   1/11/13 2:00p nihar
 * FWRAA-531: Add support for CC data inclusion in the encoded stream -
 * Initilizaing CC flag default value.
 * 
 * Hydra_Software_Devel/60   1/10/13 12:00p nihar
 * SW7425-4327: Change the default user config for dts lbr. Second port
 * channel matrix is changed to stereo instead of multichannel.
 * 
 * Hydra_Software_Devel/59   1/10/13 11:35a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/58   1/10/13 11:04a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/MAGNUM_ILBC_ISAC_integration/1   12/20/12 11:41a skalapur
 * FWRAA-474:iLBC and iSAC integration
 * 
 * Hydra_Software_Devel/57   12/12/12 10:11a dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Changing the default
 * userconfig values
 * 
 * Hydra_Software_Devel/56   11/21/12 11:45a dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Changing the default
 * userconfig values
 * 
 * Hydra_Software_Devel/55   9/18/12 6:35p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/54   8/24/12 12:51p rshijith
 * FWRAA-424: Removing warnings
 * 
 * Hydra_Software_Devel/53   8/23/12 12:16a rshijith
 * FWRAA-424: Code modification to support VP8 encoder in raaga.
 * 
 * Hydra_Software_Devel/52   8/9/12 2:40p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Adding userconfig for
 * AMRWB decoder
 * 
 * Hydra_Software_Devel/51   8/7/12 4:20p shivak
 * FWRAA-305 : added I4x4 mode configurable
 * 
 * Hydra_Software_Devel/50   7/20/12 12:00p ramanatm
 * SW7425-3204:[7425] BTSC Soft-Encoder Fails Stereo Separation
 * Measurement
 * 
 * Hydra_Software_Devel/49   7/16/12 4:14p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/48   7/3/12 11:41a dlimaye
 * FWRAA-453: Modify the LPCM userconfig to reduce it's size Removed the
 * user defined downmix tables from userconfig
 * 
 * Hydra_Software_Devel/47   6/28/12 6:21p ramanatm
 * SW7425-3116: [7425] src addition -48 to 32 Khz in aache encoder
 * 
 * Hydra_Software_Devel/46   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/45   6/8/12 7:55a gprasad
 * FWRAA-445: Updating the default kmode user config param for ddp decoder
 * 
 * Hydra_Software_Devel/44   5/28/12 4:44p ramanatm
 * FWRAA-434: [7425] BTSC merge the Mono channel changes.
 * 
 * Hydra_Software_Devel/43   5/21/12 7:58p ramanatm
 * FWRAA-434: [7425] Raaga BTSC Tuning 7425 New board- Take 1
 * 
 * Hydra_Software_Devel/42   5/11/12 3:37p dlimaye
 * FWRAA-433: Adding G722 encoder to raaga Adding bdsp changes and
 * sys_top_raaga changes
 * 
 * Hydra_Software_Devel/41   5/7/12 11:04p ramanatm
 * FWRAA-434:[7425] Raaga BTSC Encode to bit exactness and sync with
 * latest raptor code
 * 
 * Hydra_Software_Devel/40   5/4/12 1:04p shivak
 * FWRAA-432  : added the default values for SubPel and VarModeDecision
 * 
 * Hydra_Software_Devel/39   4/12/12 12:28p gprasad
 * SW7425-2700: Adding Per frame STC snapshot support - currently disabled
 * 
 * Hydra_Software_Devel/38   4/11/12 8:02p gprasad
 * SW7425-2700: Adding Per frame STC snapshot support - currently disabled
 * 
 * Hydra_Software_Devel/37   4/11/12 5:56p ramanatm
 * FWRAA-420:[7425] adding downmix scale index
 * 
 * Hydra_Software_Devel/36   3/30/12 5:25p gprasad
 * SW7435-71: Modifying the default user config for SRS TVOL
 * 
 * Hydra_Software_Devel/35   3/30/12 1:59p gprasad
 * FWRAA-418: Add prefilter stage to AVL for loudness level equalization
 * Userconfig value i32DTFPCNT should be mentioned in m-sec and not in
 * seconds
 * 
 * Hydra_Software_Devel/34   3/30/12 12:34a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/33   3/22/12 7:01p gprasad
 * FWRAA-382: Broadcom 3d surround changes
 * 
 * Hydra_Software_Devel/32   3/22/12 6:50p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/31   3/16/12 2:38p ramanatm
 * SW7425-2378:[7425] Add support for Monkey Audio Codec (MAC)
 * 
 * Hydra_Software_Devel/30   2/23/12 5:37p gprasad
 * FWRAA-397: removing joint stereo control from mp3 encoder usercfg
 * structure
 * 
 * Hydra_Software_Devel/29   2/9/12 12:08a gprasad
 * SW7425-500: Updating the default user cfg fpr G.711/G.729 mono output
 * ports
 * 
 * Hydra_Software_Devel/28   2/3/12 12:23p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/27   2/1/12 2:15p gprasad
 * SW7425-500: Updating the user config defaults for G.7xx decoders
 * 
 * Hydra_Software_Devel/26   1/31/12 11:15p gprasad
 * SW7425-500: Updating the user config defaults for G.7xx decoders
 * 
 * Hydra_Software_Devel/25   1/31/12 2:47p gprasad
 * SW7425-500: Updating the user config defaults for G.7xx decoders
 * 
 * Hydra_Software_Devel/24   1/27/12 1:32p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/23   1/20/12 7:33p gprasad
 * FWRAA-377 : Merging h264 video encoder to mainline
 * 
 * Hydra_Software_Devel/22   1/13/12 9:51p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/21   1/8/12 8:04p gprasad
 * FWRAA-366: Adding default values for input output levels for mp3
 * encoder
 * 
 * Hydra_Software_Devel/20   1/6/12 11:34a ashoky
 * SW7231-524: Adding changes for 6 channels PCMWAV
 * 
 * Hydra_Software_Devel/19   12/29/11 5:56p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/18   12/15/11 7:03p ramanatm
 * SW7425-1018:[7425] Add Software support for ATSC A/85 volume leveling
 * support
 * 
 * Hydra_Software_Devel/17   12/5/11 6:20p ramanatm
 * FWRAA-366:[7425] A85 Support
 * 
 * Hydra_Software_Devel/16   12/5/11 12:40p ashoky
 * FWRAA-330: WMA Encoder Development on Raaga.
 * 
 * Hydra_Software_Devel/15   11/8/11 1:43p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/14   8/19/11 3:46p gprasad
 * FWRAA-327: Updates to g.729 decoder user config
 * 
 * Hydra_Software_Devel/13   8/9/11 11:36a gprasad
 * SWDTV-6761: Correcting the default user config for studio sound
 * 
 * Hydra_Software_Devel/12   8/5/11 5:46p ananthan
 * SW7425-724: [7425] Add BTSC audio encoder support
 * 
 * Hydra_Software_Devel/11   8/1/11 3:12p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/10   7/26/11 2:33p gprasad
 * SWDTV-6761: Add StudioSound support
 * 
 * Hydra_Software_Devel/9   6/24/11 10:40p ashoky
 * SW7425-499: [7425] Adding to mainline
 * 
 * Hydra_Software_Devel/8   6/22/11 11:38a gprasad
 * SW7425-756: Fixing the compile error
 * 
 * Hydra_Software_Devel/7   6/22/11 11:28a gprasad
 * SW7425-756: Audio descriptor support in mpeg decoder
 * 
 * Hydra_Software_Devel/6   6/15/11 12:53a gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP - Adding mix
 * balance element ot mixer's user cfg
 * 
 * Hydra_Software_Devel/5   6/14/11 10:22a jgarrett
 * SW7425-406: Removing extra element from mixer default settings
 * 
 * Hydra_Software_Devel/4   6/14/11 9:45p ashoky
 * SW7425-462: Adding metadata entries and algo info entries
 * 
 * Hydra_Software_Devel/3   6/14/11 10:05a gprasad
 * SW7422-389: Add Dolby MS11 support - Merging changes to main line
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/2   6/7/11 6:50p gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/1   5/30/11 4:26p gprasad
 * SW7422-389: [7425] Add Dolby MS11 support - Increasing the number of
 * input forks to 3
 * 
 * Hydra_Software_Devel/2   5/27/11 6:22p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/1   5/27/11 4:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 ***************************************************************************/

#include "bdsp_raaga_fw_settings.h"

const BDSP_Raaga_Audio_MpegConfigParams  BDSP_sMpegDefaultUserConfig =
{
    0x0,                                    /*eDecoderType*/
    2,                                      /*ui32OutMode*/
    0x7FFFFFFF,	                            /*ui32LeftChannelGain*/
    0x7FFFFFFF,	                            /*ui32RightChannelGain*/
    2,                                      /*ui32DualMonoMode*/
	-23,									/* Input Volume Level */
	-23,									/* Output Volume Level */
	0,										/* Anc Data Parsing Enable */
	0,										/* Anc Data Dram Buffer Configuration Address */
    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 
    0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF},                 /* ui32OutputChannelMatrix */
};

const BDSP_Raaga_Audio_AacheConfigParams BDSP_sAacheDefaultUserConfig =
{
	1,				                        /* i32NumOutPorts*/
	0,	                                    /* DownmixType */
	0x0,									/* i32AribMatrixMixdownIndex */
	0x40000000,								/* ui16DrcGainControlCompress */
	0x40000000,								/* ui16DrcGainControlBoost */
	127,	                                /* ui16DrcTargetLevel */
	{{                                      /* sUserOutputCfg[0] */
		false,		                        /* i32OutLfe */
		2,			                        /* i32OutMode */
		0,			                        /* i32DualMode */
		false,			                    /* i32ExtDnmixEnabled */
	     {{ 0, 0, 0, 0, 0, 0},  		    /* i32ExtDnmixTab[0] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[1] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[2] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[3] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[4] */	
            { 0, 0, 0, 0, 0, 0}},   		/* i32ExtDnmixTab[5] */		
		 {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */		            
	},
	{                                       /* sUserOutputCfg[1] */
		false,		                        /* i32OutLfe */
		2,			                        /* i32OutMode */
		0,			                        /* i32DualMode */
		false,			                    /* i32ExtDnmixEnabled */
	     {{ 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[0] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[1] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[2] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[3] */	
            { 0, 0, 0, 0, 0, 0},  			/* i32ExtDnmixTab[4] */	
            { 0, 0, 0, 0, 0, 0}},   		/* i32ExtDnmixTab[5] */		
		 {0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */		            
	}},
	0,                                      /* i32PcmBoost6dB */
	0,                                      /* i32PcmBoostMinus4p75dB */
	0,                                      /* ui32FreqExtensionEnable */
	0,                                       /* ui32SbrUserFlag */
	-32,									/* Input Volume Level */
	-32,									/* Output Volume Level */
	50										/*ui32DownmixCoefScaleIndex*/
};

const BDSP_Raaga_Audio_DolbyPulseUserConfig BDSP_sDolbyPulseDefaultUserConfig =
{
    0,										/* eDecoderType */
    0,                                      /* eDolbyPulseUsageMode */
    1,                                      /* ui32NumOutPorts */
    124,                                    /* ui32RefDialnormLevel */
    108,                                    /* ui32DefDialnormLevel */
    {
        {
            2,                              /* ui32OutMode */
            0,                              /* ui32OutLfe */
            0,                              /* ui32DualMode */
            0,                              /* ui32LoRoDownmix */
            0,                              /* ui32RfMode */
            100,                            /* ui32DrcCut */
            100,                            /* ui32DrcBoost */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 
            0xFFFFFFFF,0xFFFFFFFF, 
            0xFFFFFFFF, 0xFFFFFFFF},        /* ui32OutputChannelMatrix */
            0,                              /* ui32ApplyGain */
            0x00B504F3,                     /* i32GainFactor */
        },
        {
            2,                              /* ui32OutMode */
            0,                              /* ui32OutLfe */
            0,                              /* ui32DualMode */
            0,                              /* ui32LoRoDownmix */
            0,                              /* ui32RfMode */
            100,                            /* ui32DrcCut */
            100,                            /* ui32DrcBoost */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 
            0xFFFFFFFF,0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF},        /* ui32OutputChannelMatrix */                    
            0,                              /* ui32ApplyGain */
            0x00B504F3,                     /* i32GainFactor */                    
        },
    }
};

const BDSP_Raaga_Audio_DDPMultiStreamConfigParams  BDSP_sDDPDefaultUserConfig =
{
#ifdef BDSP_MS10_SUPPORT        
    1,                                      /* eDdpUsageMode */
#else
    0,                                      /* eDdpUsageMode */
#endif
    0,                                      /* eDecoderType */
    0,                                      /* ui32SubstreamIDToDecode */                 
    1,                                      /* i32NumOutPorts */
    1,                                      /* i32StreamDialNormEnable */
    0,                                      /* i32UserDialNormVal */
    1,                                      /* eMetaDataLogsEnable */            
    {{                                      /* sUserOutputCfg[0] */
        3,                                  /* i32CompMode */
        0x7FFFFFFF,                         /* i32PcmScale */
        0x7FFFFFFF,                         /* i3DynScaleHigh */
        0x7FFFFFFF,                         /* i32DynScaleLow */
        false,                              /* i32OutLfe */
        2,                                  /* i32OutMode */
        0,                                  /* i32StereoMode */
        0,                                  /* i32DualMode */
        3,                                  /* i32Kmode */
        false,                              /* i32ExtDnmixEnabled */
        {{ 0, 0, 0, 0, 0, 0},               /* i32ExtDnmixTab[0] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[1] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[2] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[3] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[4] */               
        { 0, 0, 0, 0, 0, 0}},               /* i32ExtDnmixTab[5] */                               
        false,                              /* i32ExtKaraokeEnabled */
        0,                                  /* i32Extv1Level */
        0,                                  /* i32Extv1Pan */
        0,                                  /* i32Extv2Level */
        0,                                  /* i32Extv2Pan */
        0,                                  /* i32ExtGmLevel */
        0,                                  /* i32ExtGmPan */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                                                     
    },
    {                                       /* sUserOutputCfg[0] */
        3,                                  /* i32CompMode */
        0x7FFFFFFF,                         /* i32PcmScale */
        0x7FFFFFFF,                         /* i3DynScaleHigh */
        0x7FFFFFFF,                         /* i32DynScaleLow */
        false,                              /* i32OutLfe */
        2,                                  /* i32OutMode */
        0,                                  /* i32StereoMode */
        0,                                  /* i32DualMode */
        3,                                  /* i32Kmode */
        false,                              /* i32ExtDnmixEnabled */
        {{ 0, 0, 0, 0, 0, 0},               /* i32ExtDnmixTab[0] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[1] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[2] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[3] */               
        { 0, 0, 0, 0, 0, 0},                /* i32ExtDnmixTab[4] */               
        { 0, 0, 0, 0, 0, 0}},               /* i32ExtDnmixTab[5] */                               
        false,                              /* i32ExtKaraokeEnabled */
        0,                                  /* i32Extv1Level */
        0,                                  /* i32Extv1Pan */
        0,                                  /* i32Extv2Level */
        0,                                  /* i32Extv2Pan */
        0,                                  /* i32ExtGmLevel */
        0,                                  /* i32ExtGmPan */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                                                     
    }}                
};

const BDSP_Raaga_Audio_DtsHdConfigParams  BDSP_sDtsHdDefaultUserConfig =
{
    /*-------------------------sDtsHdConfigParam--------------------------*/
    true,                                   /* ui32DecodeCoreOnly */
    false,                                  /* ui32DecodeDtsOnly */
    true,                                   /* ui32DecodeXLL */
    true,                                   /* ui32DecodeX96 */
    true,                                   /* ui32DecodeXCH */
    true,                                   /* ui32DecodeXXCH */
    true,                                   /* ui32DecodeXBR */
    false,                                  /* ui32EnableSpkrRemapping */
    2123,                                   /* ui32SpkrOut */
    false,                                  /* ui32MixLFE2Primary */
    0,                                      /* ui32ChReplacementSet */
    2,                                      /* i32NumOutPorts */
    false,                                  /* ui32EnableMetadataProcessing */
    {{                                      /* sUserOutputCfg[0] */
        false,                              /* i32UserDRCFlag */
        0x7fffffff,                         /* i32DynScaleHigh */
        0x7fffffff,                         /* i32DynScaleLow */
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        2,                                  /* ui32DualMode */
        0,                                  /* ui32StereoMode */
        48000,                              /* ui32AppSampleRate */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */
        0,                                  /* ui32ExtdnmixEnabled */
        {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[1] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[2] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[3] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[4] */	
    { 0, 0, 0, 0, 0, 0}}   			        /* i32ExtDnmixTab[5] */	                                        
    },
    {                                       /* sUserOutputCfg[1 ]*/
        false,                              /* i32UserDRCFlag */
        0x7fffffff,                         /* i32DynScaleHigh */
        0x7fffffff,                         /* i32DynScaleLow */
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        2,                                  /* ui32DualMode */
        0,                                  /* ui32StereoMode */
        48000,                              /* ui32AppSampleRate */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */
        0,                                  /* ui32ExtdnmixEnabled*/
        {{ 0, 0, 0, 0, 0, 0},  				/* i32ExtDnmixTab[0] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[1] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[2] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[3] */	
    { 0, 0, 0, 0, 0, 0},  				    /* i32ExtDnmixTab[4] */	
    { 0, 0, 0, 0, 0, 0}}   			        /* i32ExtDnmixTab[5] */	                                        
    } }               
};

const BDSP_Raaga_Audio_DtslbrConfigParams  BDSP_sDtsLbrDefaultUserConfig = 
{
	 1,                                                    /*Default: 1, Range [0,1] , 0 -> primary, 1-> secondary*/
     
	 0,                                   /*Default: 0, Range [0,1] , 0 -> disabled, 1-> decode embedded stereo if present*/
     
	 0,                                     /*default: 0, range: [0,1], [0, 1] = disableing/enabling Embedded downmix*/
     
	 0xFFFFFFFF,                               /*Dram address for communicating Mixer metadata between Primary, Secondary and Mixer*/

     1,                                                                                             /* Default =1; Range : 1,2 (2-> enables PCM output and concurrent stereo)*/
                
     0,              /* Default =0; 1-> If Mixing is enabled */                       /* Default =0; 1-> DVD */
    
	 0xFFFFFFFF,                 /* When the decoder is configured for primary decoder or if there is no active primary decoder, the value of this variable will be invalid i.e. 0xFFFFFFFF ; If it is called in secondary mode then this variable will carry outmode of primary */

    {
		{
			1,                                   /*  Default : 1     Range : [0,1] (0-> Dialnorm disabled; 1-> Dialnorm enabled) 
                                                This Flag is used to turn on/off Dialnorm */
    
    		0,                                                                                /*  Default =0    Range : [0,1] (0-> DRC disabled; 1-> DRC enabled) 
                                                This Flag is used to turn on/off dynamic range compression */
                                                
            0x7fffffff,                                                                        /*  Default = 0x7fffffff (100 %)
                                                Range 0 to 0x7fffffff (0 to 100%) */
    
            7,                                                                  /*Default =7;
                                                Output channel configuration, this is according to BCOM_eACMOD */

            1,                                                                                        /* Default = 1 
                                                   Range : 0,1 (0-> LFE disabled;1-> LFE enabled)
                                                   Flag used to enable/disable LFE channel output */
                                                
            2,                                                                                 /* Default =2
                                                   Range : 0-3 (0-> DUALLEFT_MONO; 1->DUALRIGHT_MONO;2->STEREO;3->DUAL MIX MONO)
                                                   Configure decoder output for dual mode */
                                                
            0,                                                                            /*  Default=0
                                                    Range : 0,1 (1->Lt/Rt downmix;0->Normal output)
                                                    Perform stereo downmix of decoder output */

    		0,                                                      /*Default =0; Range : 0,1 (1-> Mix LFE to primary while downmixing, when Lfe output is disabled)*/

    		0,                      /*Default =0; Range : 0,1 (1-> enable speaker remapping , 1-> disable speaker remapping )*/
    
			15,                                                                      /*[required output speaker configurations, default 15 , can take value from the following values*/
                                                /*
                                                ui32SpkrOut = 
                                                0       - Use Encoded Native, no speaker remapping
                                                15      - C L R         Ls                            Rs                           LFE1
                                                8207    - C L R       Ls                            Rs                           LFE1       Lhs                         Rhs
                                                2123    - C L R       LFE1       Lsr                          Rsr                          Lss                          Rss
                                                47      - C L R         Ls                            Rs                           LFE1       Lh                           Rh
                                                79      - C L R         Ls                            Rs                           LFE1       Lsr                          Rsr
                                                78      - L R Ls        Rs                           LFE1       Lsr                          Rsr                          
                                                159     - C L R        Ls                            Rs                           LFE1       Cs                           Ch
                                                287     - C L R        Ls                            Rs                           LFE1       Cs                           Oh
                                                1039    - C L R       Ls                            Rs                           LFE1       Lw                          Rw
                                                31      - C L R         Ls                            Rs                           LFE1       Cs                           
                                                30      - L R Ls        Rs                           LFE1       Cs*/


    {0, 1, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
                                                /*Here BDSP_AF_P_MAX_CHANNELS = 8 */
                                                /*This matrix decides the output order of channels
                                                
                                                    For Multi-Channel (5.1) ports:
                                                
                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    ui32OutputChannelMatrix[2] = 2;
                                                    ui32OutputChannelMatrix[3] = 3;
                                                    ui32OutputChannelMatrix[4] = 4;
                                                    ui32OutputChannelMatrix[5] = 5;
                                                    ui32OutputChannelMatrix[6] = 0xffffffff; this value shows invalid 
                                                    ui32OutputChannelMatrix[7] = 0xffffffff; this value shows invalid 
                                                
                                                    For Stereo Output Port:

                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    for(i=2; i<8; i++)
                                                        ui32OutputChannelMatrix[i] = 0xffffffff; this value shows invalid 
                                                    */    
		
		}, 
		{
			1,                                   /*  Default : 1     Range : [0,1] (0-> Dialnorm disabled; 1-> Dialnorm enabled) 
                                                This Flag is used to turn on/off Dialnorm */
    
    		0,                                                                                /*  Default =0    Range : [0,1] (0-> DRC disabled; 1-> DRC enabled) 
                                                This Flag is used to turn on/off dynamic range compression */
                                                
            0x7fffffff,                                                                        /*  Default = 0x7fffffff (100 %)
                                                Range 0 to 0x7fffffff (0 to 100%) */
    
            7,                                                                  /*Default =7;
                                                Output channel configuration, this is according to BCOM_eACMOD */

            1,                                                                                        /* Default = 1 
                                                   Range : 0,1 (0-> LFE disabled;1-> LFE enabled)
                                                   Flag used to enable/disable LFE channel output */
                                                
            2,                                                                                 /* Default =2
                                                   Range : 0-3 (0-> DUALLEFT_MONO; 1->DUALRIGHT_MONO;2->STEREO;3->DUAL MIX MONO)
                                                   Configure decoder output for dual mode */
                                                
            0,                                                                            /*  Default=0
                                                    Range : 0,1 (1->Lt/Rt downmix;0->Normal output)
                                                    Perform stereo downmix of decoder output */

    		0,                                                      /*Default =0; Range : 0,1 (1-> Mix LFE to primary while downmixing, when Lfe output is disabled)*/

    		0,                      /*Default =0; Range : 0,1 (1-> enable speaker remapping , 1-> disable speaker remapping )*/
    
			15,                                                                      /*[required output speaker configurations, default 15 , can take value from the following values*/
                                                /*
                                                ui32SpkrOut = 
                                                0       - Use Encoded Native, no speaker remapping
                                                15      - C L R         Ls                            Rs                           LFE1
                                                8207    - C L R       Ls                            Rs                           LFE1       Lhs                         Rhs
                                                2123    - C L R       LFE1       Lsr                          Rsr                          Lss                          Rss
                                                47      - C L R         Ls                            Rs                           LFE1       Lh                           Rh
                                                79      - C L R         Ls                            Rs                           LFE1       Lsr                          Rsr
                                                78      - L R Ls        Rs                           LFE1       Lsr                          Rsr                          
                                                159     - C L R        Ls                            Rs                           LFE1       Cs                           Ch
                                                287     - C L R        Ls                            Rs                           LFE1       Cs                           Oh
                                                1039    - C L R       Ls                            Rs                           LFE1       Lw                          Rw
                                                31      - C L R         Ls                            Rs                           LFE1       Cs                           
                                                30      - L R Ls        Rs                           LFE1       Cs*/


    {0, 1, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff},
                                                /*Here BDSP_AF_P_MAX_CHANNELS = 8 */
                                                /*This matrix decides the output order of channels
                                                
                                                    For Multi-Channel (5.1) ports:
                                                
                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    ui32OutputChannelMatrix[2] = 2;
                                                    ui32OutputChannelMatrix[3] = 3;
                                                    ui32OutputChannelMatrix[4] = 4;
                                                    ui32OutputChannelMatrix[5] = 5;
                                                    ui32OutputChannelMatrix[6] = 0xffffffff; this value shows invalid 
                                                    ui32OutputChannelMatrix[7] = 0xffffffff; this value shows invalid 
                                                
                                                    For Stereo Output Port:

                                                    ui32OutputChannelMatrix[0] = 0;
                                                    ui32OutputChannelMatrix[1] = 1;
                                                    for(i=2; i<8; i++)
                                                        ui32OutputChannelMatrix[i] = 0xffffffff; this value shows invalid 
                                                    */    
		
		}
	}	
	
};

const BDSP_Raaga_Audio_LpcmUserConfig  BDSP_sLcpmDvdDefaultUserConfig =
{
    1,                                      /* ui32NumOutports */
    {{                                      /* sOutputCfg[0] */
        2,                                  /* ui32OutMode */
        false,                              /* ui32LfeOnFlag */
        0,                                  /* ui32DualMonoMode */
    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */                         
    },
    {                                       /* sOutputCfg[0] */
        2,                                  /* ui32OutMode */
        false,                              /* ui32LfeOnFlag */
        0,                                  /* ui32DualMonoMode */
    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */                         
    }}
};

const BDSP_Raaga_Audio_PassthruConfigParams  BDSP_sPcmDefaultUserConfig =
{
    BDSP_Raaga_ePassthruType_PCM,
	BDSP_Raaga_eAacHeaderType_Raw
};

const BDSP_Raaga_Audio_PcmWavConfigParams  BDSP_sPcmWavDefaultUserConfig =
{
    1,				/* ui32NumOutputPorts */
	{
		{
			2,		/* i32OutMode */
			0,		/* i32StereoMode */
			0,		/* i32DualMode */
			{0x2902de00, 0x2902de00, 0x1CFDF3B6, 0x1CFDF3B6, 0x1CFDF3B6, 0x1CFDF3B6, 0x5A827999, 0x5A827999},/* i32DownmixCoeffs */
			{0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}					 /* ui32OutputChannelMatrix */
		},
		{
			2,		/* i32OutMode */
			0,		/* i32StereoMode */
			0,		/* i32DualMode */
			{0x2902de00, 0x2902de00, 0x1CFDF3B6, 0x1CFDF3B6, 0x1CFDF3B6, 0x1CFDF3B6, 0x5A827999, 0x5A827999},/* i32DownmixCoeffs */
			{0, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}					 /* ui32OutputChannelMatrix */
		}
	}
};

const BDSP_Raaga_Audio_AmrConfigParams BDSP_sAmrDefaultUserConfig =
{
    1,                                  /*ui32NumOutPorts*/
    {{/*sUserOutputCfg[0]*/
        2,                              /* ui32OutMode */
        1,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    },
    {/*sUserOutputCfg[1]*/
        1,                              /* ui32OutMode */
        0,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    }}
};

const BDSP_Raaga_Audio_AmrwbdecConfigParams BDSP_sAmrwbdecDefaultUserConfig =
{
    1,                                  /*ui32NumOutPorts*/
    {{/*sUserOutputCfg[0]*/
        2,                              /* ui32OutMode */
        1,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    },
    {/*sUserOutputCfg[1]*/
        1,                              /* ui32OutMode */
        0,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    }}
};

const BDSP_Raaga_Audio_iLBCdecConfigParams BDSP_siLBCdecDefaultUserConfig =
{
    1,                                  /*ui32NumOutPorts*/
    {{/*sUserOutputCfg[0]*/
		20,								/* frame lenghth*/
        0,								/* plc*/
        2,                              /* ui32OutMode */
        0,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    },
    {/*sUserOutputCfg[1]*/
		20,								/* frame length*/
		0,								/* plc*/
        2,                              /* ui32OutMode */
        0,                              /* ui32ScaleOp */
        0,                              /* ui32ScaleIdx */
        {0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    }}
};


const BDSP_Raaga_Audio_iSACdecConfigParams BDSP_siSACdecDefaultUserConfig = 
{

    1,                                  /*ui32NumOutPorts*/
    {{/*sUserOutputCfg[0]*/       
		0,								/* uint32_t    ui32BandMode */
        0,								/* uint32_t    plc */
		1,                              /* uint32_t    ui32OutMode */
        0,                              /* uint32_t    ui32ScaleOp */
        0,                              /* uint32_t    ui32ScaleIdx */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    },
    {/*sUserOutputCfg[1]*/
		0,								/* uint32_t    ui32BandMode */
        0,								/* uint32_t    plc */
		1,                              /* uint32_t    ui32OutMode */
        0,                              /* uint32_t    ui32ScaleOp */
        0,                              /* uint32_t    ui32ScaleIdx */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF},  /*ui32OutputChannelMatrix*/
    }}

};


const BDSP_Raaga_Audio_WmaConfigParams  BDSP_sWmaDefaultUserConfig =
{
    /*-------------------------sWmaStdConfigParam-------------------------*/
    1,                                      /* i32NumOutPorts */
    0,                                      /* uidecodeOnlyPatternFlag */
    0,                                      /* uidecodePattern */
    {0, 1, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */
};
const BDSP_Raaga_Audio_WmaProConfigParams  BDSP_sWmaProDefaultUserConfig =
{
    /*-------------------------sWmaProConfigParam-------------------------*/
    2,                                      /* ui32NumOutports */
    {{                                      /* sOutputCfg[0] */
        0,                                  /* ui32DRCEnable */
        0,/* eDRCSetting */                            
        0X7FFFFFFF,                         /* i32RmsAmplitudeRef */
        0X7FFFFFFF,                         /* i32PeakAmplitudeRef */
        0X7FFFFFFF,                         /* i32DesiredRms */
        0X7FFFFFFF,                         /* i32DesiredPeak */
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        0,    /* ui32StereoMode */
    {0, 1, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */                         
    },
    {                                       /* sOutputCfg[1] */
        0,                                  /* ui32DRCEnable */
        0,/* eDRCSetting */                            
        0X7FFFFFFF,                         /* i32RmsAmplitudeRef */
        0X7FFFFFFF,                         /* i32PeakAmplitudeRef */
        0X7FFFFFFF,                         /* i32DesiredRms */
        0X7FFFFFFF,                         /* i32DesiredPeak */
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        0,    /* ui32StereoMode */
    {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}	/* ui32OutputChannelMatrix */                         
    }},                    
    0
};

const BDSP_Raaga_Audio_DraConfigParams  BDSP_sDraDefaultUserConfig =
{
    2,                                      /* ui32NumOutPorts */
    {{
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        0,                                  /* ui32StereoMode */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    },
    {
        2,                                  /* ui32OutMode */
        false,                              /* ui32OutLfe */
        0,                                  /* ui32StereoMode */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    }}
};

const BDSP_Raaga_Audio_RalbrConfigParams  BDSP_sRalbrDefaultUserConfig =
{
    2,                                      /* ui32NumOutPorts */
    {{
        2,                                  /* ui32OutMode */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    },
    {
        2,                                  /* ui32OutMode */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    }}
};

const BDSP_Raaga_Audio_AdpcmConfigParams  BDSP_sAdpcmDefaultUserConfig =
{
    2,                                      /* ui32NumOutPorts */
    {{
        2,                                  /* ui32OutMode */
        0,									/* ui32ApplyGain */
        0x016A09E6,							/* i32GainFactor */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    },
    {
        2,                                  /* ui32OutMode */
        0,									/* ui32ApplyGain */
        0x016A09E6,							/* i32GainFactor */
        {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
    }}
};


const BDSP_Raaga_Audio_G726ConfigParams BDSP_sG711G726DecUserConfig = 
{
    1,                                          /* ui32NumOutPorts */
    0,                                          /* ui32OutputType */
    {                                           /* sUsrOutputCfg */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
            1,                                  /* ui32ApplyGain */
            0x5A827999,                         /* i32GainFactor */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
        {                                       /* sUsrOutputCfg[1] */
            1,                                  /* ui32OutMode */
            0,                                  /* ui32ApplyGain */
            0x5A827999,                         /* i32GainFactor */
            {4, 4, 0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        }
    }
};

const BDSP_Raaga_Audio_G729DecConfigParams BDSP_sG729DecUserConfig = 
{
    1,                                          /* ui32NumOutPorts */
    {                                           /* sUsrOutputCfg */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
            1,                                  /* ui32ScaleOp */
            0,                                  /* ui32ScaleIdx */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
        {                                       /* sUsrOutputCfg[1] */
            1,                                  /* ui32OutMode */
            0,                                  /* ui32ScaleOp */
            0,                                  /* ui32ScaleIdx */
            {4, 4,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
    }
};


const BDSP_Raaga_Audio_VorbisDecConfigParams BDSP_sVorbisDecUserConfig = 
{
    1,                                          /* ui32NumOutPorts */
    {                                           /* sUsrOutputCfg */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
            0,                                  /* ui32OutLfe */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
        {                                       /* sUsrOutputCfg[1] */
            2,                                  /* ui32OutMode */
            0,                                  /* ui32OutLfe */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
    }
};

const BDSP_Raaga_Audio_MlpConfigParams BDSP_sMlpUserConfig = 
{
	2,											/* ui32OutMode */
	1,											/* DrcMode */
	100,										/* DrcBoost */
	100,										/* DrcCut */
	0xFF,										/* DialogueEnable */
	-31,										/* DialogueLevel */
	0,											/* EnableSpeakerRemapping */
	0x4F,										/* SpeakerOutLayout */
	{0, 1,2,3, 4,5, 0xFFFFFFFF, 0xFFFFFFFF},	/* ui32OutputChannelMatrix */                    
};

const BDSP_Raaga_Audio_G723_1DEC_ConfigParams BDSP_sG723_1_Configsettings =
{
    1,                                          /* ui32NumOutPorts */
    {                                           /* sUsrOutputCfg[2] */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
            1,                                  /* ui32ScaleOp */
            0,                                  /* ui32ScaleIdx */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix[8] */
        },
        {                                       /* sUsrOutputCfg[1] */
            1,                                  /* ui32OutMode */
            0,                                  /* ui32ScaleOp */
            0,                                  /* ui32ScaleIdx */
            {4, 4,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix[8] */
        }
    }
};

const BDSP_Raaga_Audio_FlacDecConfigParams BDSP_sFlacDecUserConfig = 
{
    1,                                          /* ui32NumOutPorts */
    {                                           /* sUsrOutputCfg */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
            0,                                  /* ui32OutLfe */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
        {                                       /* sUsrOutputCfg[1] */
            2,                                  /* ui32OutMode */
            0,                                  /* ui32OutLfe */
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
    }
};

const BDSP_Raaga_Audio_MacDecConfigParams BDSP_sMacDecUserConfig = 
{
    1,                                          /* ui32NumOutPorts */
    {                                           /* sUsrOutputCfg */
        {                                       /* sUsrOutputCfg[0] */
            2,                                  /* ui32OutMode */
			2, /*dual mode*/
			0, /*stereo mode*/
			0, /*scale enable*/
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
        {                                       /* sUsrOutputCfg[1] */
            2,                                  /* ui32OutMode */
			2, /*dual mode*/
			0, /*stereo mode*/
			0, /*scale enable*/
            {0, 1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},/* ui32OutputChannelMatrix */                    
        },
    }
};

const BDSP_Raaga_Audio_AMRWBConfigParams BDSP_sAmrwbDefaultUserConfig =
{
    1,                                  /*ui32allow_dtx*/
    1,						/*ui32bitstreamformat*/
    1						/*ui32mode*/
};

const BDSP_Raaga_Audio_G723EncoderUserConfig BDSP_sDefG723_1EncodeConfigSettings =
{
    0,                                          /* WrkRate */
    1,                                          /* UseHp */
    0,                                          /* UseVx */
};



const BDSP_Raaga_Audio_PassthruConfigParams   BDSP_sDefaultPassthruSettings =
{                                           
    BDSP_Raaga_ePassthruType_SPDIF,          /* ui32PassthruType */
	BDSP_Raaga_eAacHeaderType_Raw
};

const BDSP_Raaga_Audio_AVLConfigParams BDSP_sDefAVLConfigSettings = 
{
	true,                   /* ui32AVLEnableFlag */
	-20*32768,              /* iTarget */
	-31*32768,              /* iLowerBound */
	11*32768,               /* uiFixedBoost */
	2959245,				/* uiRef */
	32736,					/* uiAlpha */
	32256,					/* uiBeta */
	9830,                   /* uiThreshold */
	750,					/* uiDtfPcnt */
	32767,					/* uiAlpha2 */
	16384,                  /* uisNsfgr */
	9830,                   /* uiDtf */
    1,                      /* i32LoudnessLevelEq */
};

const BDSP_Raaga_Audio_TruVolumeUserConfig BDSP_sDefSrsTruVolumeUserConfig =
{
    1,
    2,
    256,
    1,
    0x00200000,
    0x00200000,
    0x007fffff,
    0x00400000,
    0,
    1,
    0,
    0x00080000,
    1,
    0x000ccccd,
    0x8000,
    1,
    {                                           /* sTopLevelConfig */
        0,                                      /* i32IsStudioSound */
        1,                                      /* i32StudioSoundMode */    
        1,                                      /* i32mEnable */
        0x7fffffff,                             /* i32mInputGain */
        0x7fffffff,                             /* i32mHeadroomGain */
        3,                                      /* i32mInputMode */
        0x7fffffff,                             /* i32mOutputGain */
        0x7fffffff,                             /* i32mBypassGain */                
    },
    {                                           /* sHighPassFilterConfig */
        1,                                      /* ui32mEnable */
        0,                                      /* ui32CoefGenMode */
		{										/* sFilterCoefHpf[] */
			{									/* sFilterCoefHpf[0] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fc81f80,				/* i32FilterCoefficientB0 */
						0xe06fc100,				/* i32FilterCoefficientB1 */
						0x0fc81f80,				/* i32FilterCoefficientB1 */
						0x7e36e680,				/* i32FilterCoefficientA1 */
						0xc1b4eec0				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3df563c0,				/* i32FilterCoefficientB0 */
						0x84153880,				/* i32FilterCoefficientB1 */
						0x3df563c0,				/* i32FilterCoefficientB1 */
						0x7be0e200,				/* i32FilterCoefficientA1 */
						0xc40b5300				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			},
			{									/* sFilterCoefHpf[1] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fd78db0,				/* i32FilterCoefficientB0 */
						0xe050e4a0,				/* i32FilterCoefficientB1 */
						0x0fd78db0,				/* i32FilterCoefficientB1 */
						0x7eb71980,				/* i32FilterCoefficientA1 */
						0xc13e3e40				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3e826c40,				/* i32FilterCoefficientB0 */
						0x82fb2780,				/* i32FilterCoefficientB1 */
						0x3e826c40,				/* i32FilterCoefficientB1 */
						0x7cff9680,				/* i32FilterCoefficientA1 */
						0xc2f5e600				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			},
			{									/* sFilterCoefHpf[2] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fdadc10,				/* i32FilterCoefficientB0 */
						0xe04a47e0,				/* i32FilterCoefficientB1 */
						0x0fdadc10,				/* i32FilterCoefficientB1 */
						0x7ed26000,				/* i32FilterCoefficientA1 */
						0xc1249f40				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3ea0f4c0,				/* i32FilterCoefficientB0 */
						0x82be1680,				/* i32FilterCoefficientB1 */
						0x3ea0f4c0,				/* i32FilterCoefficientB1 */
						0x7d3d7780,				/* i32FilterCoefficientA1 */
						0xc2b9a440				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			}
		},
		{										/* sFilterSpecHpf */
			180,								/* ui32CutoffFrequency */
			4									/* ui32Order */
		}
    }
    
};

const BDSP_Raaga_Audio_DDReencodeConfigParams BDSP_sDefDDReencodeUserConfig = 
{
	0,											/* ui32ExternalPcmEnabled */
	1,											/* ui32CompProfile */
	0,											/* ui32CmixLev */
	0,											/* ui32SurmixLev */
	0,											/* ui32DsurMod */
	31,											/* ui32DialNorm */
	2,											/* ui32NumOutPorts */
	{											/* sUserOutputCfg[2] */
		{										/* sUserOutputCfg[1] */
		    0,                                  /*ui32CompMode*/
		    0x7FFFFFFF,                         /*ui32DrcCutFac*/
		    0x7FFFFFFF,                         /*ui32DrcBoostFac*/
		    2,                                  /*ui32OutMode*/
		    0,                                  /*ui32OutLfe*/
		    0,                                  /*ui32StereoMode*/
		    0,                                  /*ui32DualMode*/
		    {									/* ui32OutputChannelMatrix[8] */
				0,								/* ui32OutputChannelMatrix[0] */
				1,								/* ui32OutputChannelMatrix[1] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[2] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[3] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[4] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[5] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[6] */
				0xFFFFFFFF						/* ui32OutputChannelMatrix[7] */
			}         
		},
		{										/* sUserOutputCfg[1] */
		    0,                                  /*ui32CompMode*/
		    0x7FFFFFFF,                         /*ui32DrcCutFac*/
		    0x7FFFFFFF,                         /*ui32DrcBoostFac*/
		    2,                                  /*ui32OutMode*/
		    0,                                  /*ui32OutLfe*/
		    0,                                  /*ui32StereoMode*/
		    0,                                  /*ui32DualMode*/
		    {									/* ui32OutputChannelMatrix[8] */
				0,								/* ui32OutputChannelMatrix[0] */
				1,								/* ui32OutputChannelMatrix[1] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[2] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[3] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[4] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[5] */
				0xFFFFFFFF,						/* ui32OutputChannelMatrix[6] */
				0xFFFFFFFF						/* ui32OutputChannelMatrix[7] */
			}         
		}
	}
};

const BDSP_Raaga_Audio_DV258ConfigParams BDSP_sDefDV258UserConfig = 
{
    1,                                          /*i32DolbyVolumeEnable*/
    0,                                          /*i32VlmMdlEnable*/
    0,                                          /*i32HalfmodeFlag*/
    1,                                          /*i32EnableMidsideProc*/
    1,                                          /*i32LvlEnable*/
    9,                                          /*i32LvlAmount*/
    1360,                                       /*i32InputReferenceLevel*/
    0,                                          /*i32Pregain*/
    1360,                                       /*i32OutputReferenceLevel*/
    0,                                          /*i32CalibrationOffset*/
    0,                                          /*i32AnalogVolumeLevel*/
    0,                                          /*i32DigitalVolumeLevel*/
    0,                                          /*i32ResetNowFlag*/
    1                                           /*i32LimiterEnable*/
};

const BDSP_Raaga_Audio_DolbyVolumeUserConfig BDSP_sDefDolbyVolUserConfig = 	
{
	true,								        /* i32DolbyVolumeEnable */
	512,								        /* i32BlockSize */
	20,								            /* i32nBands */
	2,                                          /* i32nChans*/
	0x4A000000,							                /* i32InputReferenceLevel */
	0x4A000000, 	                                    /* i32OutputReferenceLevel */
	0x00000000,                                          /* i32Calibration */
	true,                                       /* i32VlmEnable */
    false,                                      /* i32ResetNowFlag */	
    0x00000000,                                          /* i32DigitalVolumeLevel */
    0x00000000,                                          /* i32AnalogVolumeLevel */                
    9,                                          /* i32LvlAmount */
    true,                                       /* i32LvlEnable */	
    false,                                      /* i32EnableMidsideProc */
    false,                                      /* i32HalfmodeFlag */                                    
    true                                        /* i32LimiterEnable*/
};

const BDSP_Raaga_Audio_PL2ConfigParams BDSP_sDefProLogicIIConfigSettings =  
{
    1,                                          /* ui32enable */
	0,                                          /* ui32abalflg */
	3,                                          /* ui32decmode */
	7,                                          /* ui32chancfg */
	3,                                          /* ui32cwidthcfg */
	3,                                          /* ui32dimcfg */
	0,                                          /* ui32panoramaflg */
	0,                                          /* ui32sfiltflg */
	0,                                          /* ui32rspolinvflg */
	100,                                        /* ui32pcmscl */
	0,                                          /* ui32debugOn */
	0                                           /* ui32outputch */
};

const BDSP_Raaga_Audio_TruSurrndXTConfigParams BDSP_sDefTruSurroundXtSettings = 	
{
    true,								        /* bTruSurroundEnable */
    false,								        /* bTsHeadphoneEnable */
    100,							            /* uiTruSurroundInputGain */
    true,								        /* bDialogClarityEnable */
    50,									        /* uiDialogClarityLevel */
    true,								        /* bTruBassEnable */
    50,									        /* bTbLevel */
    BDSP_Raaga_Audio_SpeakerSize_eLFResponse60Hz,	        /* eTbSpeakerSize */
    false,                                      /* bCertificationEnable */
    BDSP_Raaga_Audio_AcMode_eMode20                  /* eAcMode */			
};

const BDSP_Raaga_Audio_SRCUserConfigParams   BDSP_sDefaultSrcSettings =
{                                           
    0                                       /* ui32SrcDummyUserConfig */
};

const  BDSP_Raaga_Audio_CustomSurroundUserConfig BDSP_sDefCustomSurroundConfigSettings = 	
{
	false,								        /* bBypassShSurround */
	false,			                            /* bCombOn */
	0,								            /* uiDelay */
	0,									        /* uiVolume1 */
	0,									        /* uiVolume2 */			
	0,									        /* uiVolume3 */			
	0x1E,								        /* uiVolume4 */			
	0x1E,								        /* uiVolume5 */			
	0x74,								        /* uiLpfFc */
	0x44,     							        /* uiLpfQ */
	0x56,                                       /* uiLpfGain */
	0x40,                                       /* uiFuncVol */
	0x14                                        /* uiTrim */
};

const BDSP_Raaga_Audio_CustomBassUserConfig BDSP_sDefaultCustomBassSettings =	
{
	true,                                       /* bBypassCustomBass */
	0,                                          /* eCustomBassOperation */
	0,                                          /* eCustomBassAgcType */
	0,                                          /* eCustomBassHarmonicsType */
	1,                                          /* iAgcGainMax */
	1,                                          /* iAgcGainMin */
	0x0,                                        /* uiAgcAttackTime */
	0x0,                                        /* uiAgcRelTime */
	1,                                          /* iAgcThreshold */
	1,                                          /* uiHarmonicsLevel */
	1,                                          /* uiLpfFc */
	1,                                          /* uiHpfFc */
	1,                                          /* uiAgcHpfFc */
	1,                                          /* uiHarmonicsLpfFc */
	1                                           /* uiHarmonicsHpfFc */
};

const BDSP_Raaga_Audio_CustomVoiceConfigParams BDSP_sDefCustomVoiceConfigSettings =	
{
    0x00000001, /*PROC_ENABLE*/
    0x00000004,
    0x00000001, /*param011*/
    0x00222222, /*param017*/
    0x00000025, /*param018*/
    0x000022f3, /*param019*/
    0x00111111, /*param020*/
    0x00200000, /*param021*/
    0x00051eb8,
    0x00028dce,
    0xffde0546,
    0x00000000,
    0x00f00000,
    0x00f00000,/*param027*/
    0x002d8621,
    0x00b3f300,
    0x0050f44d,/*param030*/
    0x0075b37e,
    0x0075b37e,
    0x001d6cdf,
    0x0074b1f6,
    0x0069d6fc,
    0x00025a1c,/*param036*/
    0x00000014,
    0x00000014,
    0x00000014,
    0x00000014,
    0x000361af,/*param041*/
    0x0014248f,/*param042*/
    0x001b4e81,
    0x00e4b17e,
    0x001b4e81,
    0x00e4b17e,/*param046*/
    0x00044444,
    0x002aaaaa,
    0x00000001,/*param049*/
    0x00015d86,
    0x0000aec3,
    0x00015d86,/*param052*/
    0x00888889,
    0x00400000,
    0x00400000,
    0x00333333,
    0x00666666,/*param057*/
    /*param058*/
    {
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x0002d6a8,
    0x0002d6a8,
    0x0002d6a8,
    0x0002d6a8,
    0x0002d6a8,
    0x0002d6a8,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000,
    0x00080000
    },
    /*param059*/
    {
    0x000a6ce2,  
    0x000b0b06,  
    0x0008f9e4,  
    0x0007616d,  
    0x0006e350,  
    0x0007ba78,  
    0x00080000,  
    0x0008795a,  
    0x0008795a,  
    0x0008795a,  
    0x0008795a,  
    0x0008f9e4,  
    0x0009babf,  
    0x000a1247,  
    0x000a1247,  
    0x000a1247,  
    0x000a1247,  
    0x000a1247,  
    0x000a1247,  
    0x000a1247
    },
    0x00000001,
    0x0000001e,
    0x007fffff,/*param062*/
    {
    {0x003ebbe4, 0xffe13b82},
    {0x003ecd9c, 0xffe12a4c},
    {0x003edf5a, 0xffe1190d},
    {0x003ef11f, 0xffe107c5},
    {0x003f09d8, 0xffe0ef83},
    {0x003f22a0, 0xffe0d72e},
    {0x003f3b77, 0xffe0bec6},
    {0x003f43ed, 0xffe0b6ea},
    {0x003f4246, 0xffe0b94d},
    {0x003f0a5d, 0xffe0f1e9},
    {0x003ec222, 0xffe13ac6}
    },
    {
    {0x0021551b, 0xffc1441c, 0x001d6f64},
    {0x002130be, 0xffc13264, 0x001da4f6},
    {0x00210f06, 0xffc120a6, 0x001dd7ed},
    {0x0020efd0, 0xffc10ee1, 0x001e086b},
    {0x0020a9bb, 0xffc0f628, 0x001e66c3},
    {0x00207379, 0xffc0dd60, 0x001eb558},
    {0x00204a3c, 0xffc0c489, 0x001ef6fd},
    {0x00201b0a, 0xffc0bc13, 0x001f2e0b},
    {0x001ffad1, 0xffc0bdba, 0x001f4be2},
    {0x001fda3e, 0xffc0f5a3, 0x001f33d8},
    {0x001fb17e, 0xffc13dde, 0x001f13bc}
    },
    {
    {0x00000000, 0xffed475f},
    {0xfffef5ee, 0xffec62ea},
    {0xfffde270, 0xffeb7647},
    {0xfffcc51e, 0xffea8102},
    {0xfffbabf3, 0xffea35ac},
    {0xfffa9020, 0xffe9e981},
    {0xfff971b5, 0xffe99c7d},
    {0xfff5ca04, 0xffe913a1},
    {0xffefc74c, 0xffe8f756},
    {0xffeacb96, 0xffeaf94d},
    {0xffe6a07e, 0xffed40fc}
    },
    {
    {0x002e5b31, 0x00000000, 0x00045d71},
    {0x002ca76f, 0x00010a12, 0x0006f5a6},
    {0x002b0d97, 0x00021d90, 0x00097c23},
    {0x00298cd2, 0x00033ae2, 0x000bf22c},
    {0x00273c31, 0x0004540d, 0x000e8e21},
    {0x002551c7, 0x00056fe0, 0x0010c4b8},
    {0x0023bd79, 0x00068e4b, 0x0012a609},
    {0x00215788, 0x000a35fc, 0x001594d6},
    {0x001fbfcc, 0x001038b4, 0x001748dd},
    {0x001e498d, 0x0015346a, 0x0016bd25},
    {0x001cb1c4, 0x00195f82, 0x00160d3f}
    },
    {0x001fda2c, 0xffc04ba8, 0x001fda2c},
    {0x003fb42c, 0xffe04b7a},
    0x00066300,
    0x00733800,
    0x00041100,
    0x0077dc00,
    0x007a6500,
    0x0074cc00,
    0x0000ea00,
    0x007e2900,
    0x002ecf00,
    0x0025ac00,
    0x0036dd00,
    0x003fff00,
    0x00333200,
    0x00266500,
    0x0016da00,
    0x00572b00,
    0x00000009,
    0x00000012,
    0x0000000a,
    0x00400000,
    0x000cc509,
    0x00266500,
    0x000bdf4d,
    0x00400000,
    0xffc00000,
    0xffd9999a,
    0x001e0000,
    0xffe20000,
    0x004b0000,
    0xffb50000,
    0x00000005,
    { 0x003fbee1, 0xffe02ac8 },
    { 0x00001564, 0x00000000, 0xffffea9c },
    { 0x003f4979, 0xffe02ac8 },
    { 0x00001564, 0x00000000, 0xffffea9c},
    0x00000005,
    0x0051e300,
    0x00400000,
    0x00266500,
    0x0016da00,
    0x00266500,
    0x003fff00,
    0x0006626b,
    {
         0x00160ecb,
         0x002c1d97,
         0x00160ecb
    },
    {
         0xffd70d41,
         0xfff0b791
    },
    0x00041893
    
};

const BDSP_Raaga_Audio_GenCdbItbConfigParams BDSP_sDefGenCdbItbConfigSettings = 
{
    BDSP_AF_P_eEnable,                          /* eEnableEncode */ 		  
	BDSP_AF_P_eDisable,							/* eSnapshotRequired */
    0,                                          /* ui32EncSTCAddr */
    0,                                          /* ui32A2PInMilliSeconds */  
    BDSP_AF_P_eDisable,                         /* eStcItbEntryEnable */
    0                                           /* ui32EncSTCAddrUpper */
};


const BDSP_Raaga_Audio_BtscEncoderConfigParams BDSP_sDefBtscEncoderConfigSettings = 
{
	BDSP_AF_P_eEnable,			/* eBTSCEnableFlag               - Default Enable*/
	BDSP_AF_P_eDisable,			/* eUseDeEmphasizedSourceSignals - Default Disable*/
	BDSP_AF_P_eEnable,			/* eMainChannelPreEmphasisOn     - Default Enable*/
	BDSP_AF_P_eDisable,			/* eEquivalenMode75MicroSec      - Default Disable*/
	BDSP_AF_P_eEnable,			/* eClipDiffChannelData          - Default Enable*/
	BDSP_AF_P_eEnable,			/* eDiffChannelPreEmphasisOn     - Default Enable*/
	BDSP_AF_P_eEnable,			/* eUseLowerThresholdSpGain      - Default Enable*/

	0x20000000,					/*ui32AttenuationFactor          - Q-format 3.29 format: Maximum 0x7fffffff corresponding to 8.0 and default is 0x20000000:   */
	
	0x843BC1E,					/*ui32SRDCalibrationFactor       - Q-format 4.28 format: Default :12BE76C9 for nextgen */
								/* SRD changes from platform to platform*/
	BDSP_AF_P_eEnable,			/*Default Enable Used to Control FM Deviation of Sum Channel to 25Khz*/
	BDSP_AF_P_eEnable,			/*Default Enable Used to Control FM Deviation of Diff Channel to 50Khz*/ 
	BDSP_AF_P_eEnable,			/*Default Enable Used to Control FM Deviation of Final Output to 50Khz*/
	BDSP_AF_P_eDisable,			/*Default Disable. Used to Generate Test Tones*/
	0,							/*Default 0 generates frequency sweep.  Programming to a fixed value generates tone*/
	0x20000000,					/*i32SumChannelLevelCtrl: Q-format 3.29 format: Maximum is 0x7fffffff and default is : 0x20000000  */
	0x20000000,					/*i32DiffChannelLevelCtrl: Q-format 3.29 format: Maximum is 0x7fffffff and default is : 0x20000000  */
	0,							/*Mono Mode default disable*/	
	0							/*eMuteRightChannel, Default Disable, Enable for Stereo Separation Tests*/
};	

const BDSP_Raaga_Audio_SpeexAECConfigParams BDSP_sDefSpeexAECConfigParams =
{
    0,                          /* ui32GainResolution */
};

const BDSP_Raaga_Audio_AudioDescPanConfigParams BDSP_sDefAdPanConfigSettings =	
{
    0x7fffffff,                                 /* uiVol */   
    0x00000000,                                 /* ui32PanFadeInterfaceAddr*/        
    BDSP_Raaga_Audio_ADChannelConfig_eADChannelOff, /* eADChannelConfig */
    0,                                          /*ui32PanFadeInterfaceValidFlag*/
    1000                                        /* uiAudioRampTimeInMs */
};

const BDSP_Raaga_Audio_DtsBroadcastEncConfigParams BDSP_sDefDTSENCConfigSettings = 
{
    1,                                          /* ui32SpdifHeaderEnable */
    0,                                          /* ui32CertificationEnableFlag */
    0,                                          /* ui32LFEEnableFlag */
    BDSP_Raaga_Audio_AcMode_eINVALID,           /* eInputDataAcMode */
    0                                           /* ui32IsIIRLFEInterpolation */
};

const BDSP_Raaga_Audio_AacheEncConfigParams BDSP_sDefAacHeENCConfigSettings = 
{
    BDSP_Raaga_Audio_AacEncodeBitRate_e32kbps,                                  /* eAacEncodeBitRate */
    false,																		/* ui32bEncodeMono */
    true,																		/* ui32EnableSBR */
	0,																			/*Enables/Disables SRC from 48khz to 32khz before encoding. 1 SRC Enable, 0 No SRC */
    1,																			/* ui32EncodeFormatType */
	-32,																		/* Input Volume Level */
	-32,																		/* Output Volume Level */
    BDSP_Raaga_Audio_AacEncodeComplexity_Low,                                   /* eAacEncodeComplexity */
    BDSP_Raaga_Audio_AacEncodeMono_Mix,											/* Channel select */
    BDSP_Raaga_Audio_AacEncodeAdtsMpeg4											/* ADTS ID field, MPEG Type */
};

const BDSP_Raaga_Audio_Ac3EncConfigParams BDSP_sDefAc3ENCConfigSettings = 
{
    8,                                          /* i32NumChans */            
    7,                                          /* i32AudCodMode */
    18,                                         /* i32DataRate */                
    0,                                          /* i32LoFreqEffOn */
    0x2,                						/* i32SampRateCod */
    false,                                      /* i32LfeFiltInUse */
    false,                                      /* i32CompChar */
    false,                                      /* i32CompChar2 */
    false,                                       /* i32SurDelayArg */
    true,                                       /* eSpdifHeaderEnable */    
    true,                                       /* eTranscodeEnable */
};


const BDSP_Raaga_Audio_DDTranscodeConfigParams BDSP_sDefDDTranscodeConfigSettings = 
{
    0,                                          /* i32AudCodMode*/
    0,                                          /* i32LoFreqEffOn */
    BDSP_AF_P_eEnable,                          /* eSpdifHeaderEnable */
    BDSP_AF_P_eEnable,                          /* eTranscodeEnable */
};

const BDSP_Raaga_Audio_Mpeg1L3EncConfigParams   BDSP_sDefMpeg1L3EncConfigSettings = 
{
    BDSP_Raaga_Audio_Mp3EncodeBitRate_e128kbps,		/* eMp3EncodeBitRate */    
    0,                                          /* ui32AddCRCProtect */
    BDSP_AF_P_eDisable,								/* ePrivateBit */
    BDSP_AF_P_eDisable,								/* eCopyright */
    BDSP_AF_P_eDisable,								/* eOriginal */
    BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eNone,   /* eEmphasisType*/
    -23,   /* i32InputVolLevel */
    -23,   /* i32OutputVolLevel */
   	 0,    /* Flag that indicates to encode in stereo or mono Default=0 (stereo)*/
	BDSP_Raaga_Audio_Mp3EncodeMono_Mix /* Channel Selection when bEncodeMono flag is set */
};

const BDSP_Raaga_Audio_SbcEncoderUserConfig   BDSP_sDefSbcEncConfigSettings = 
{
    8,                                          /* NumBlocks */
    8,                                          /* NumSubbands */
    true,                                       /* JointStereo */
    0,                                          /* BitAllocation */
    45                                          /* BitPool */
};

const BDSP_Raaga_Audio_G711_G726EncConfigParams BDSP_sDefG711G726EncConfigSettings = 
{
    BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726,     /* eCompressionType */
    BDSP_Raaga_Audio_eBitRate_64kbps,                       /* eBitRate */
};


const BDSP_Raaga_Audio_G729EncoderUserConfig BDSP_sDefG729EncConfigSettings =
{
    0,                                          /* ui32DtxEnable */
    0                                           /* ui32Bitrate */
};

const BDSP_Raaga_Audio_G722EncConfigParams BDSP_sDefG722EncConfigSettings =
{
    0                                          /* dummy */
};
const BDSP_Raaga_Audio_AmrEncoderUserConfig BDSP_sDefAmrEncConfigSettings =
{
    0,                                          /* ui32DtxEnable */
    0							/*  ui32Bitrate */
};
const BDSP_Raaga_Audio_AMRWBConfigParams BDSP_sDefAmrwbEncConfigSettings =
{
    1,                                          /* ui32allow_dtx */
    2,							/*  ui32bitstreamformat */
    0							/*  ui32mode */
};


const BDSP_Raaga_Audio_ILBCConfigParams BDSP_sDefiLBCEncConfigSettings =
{

    20          		/*uint32_t    ui16frameLen ;   */           /* range: 20 msec or 30 msec */	

};

const BDSP_Raaga_Audio_ISACConfigParams BDSP_sDefiSACEncConfigSettings =
{
    30,           		/* uint16_t    ui16frameLen ;  */              

	0,					/* uint16_t    ui16CodingMode; */               

	1,					/* uint16_t    ui16nbTest;  */                 

	1,					/* uint16_t    ui16fixedFL; */                 

	32000,				/* uint16_t    ui16rateBPS;  */                

	32000,				/* uint16_t    ui16bottleneck;   */             

	200,				/* uint16_t    ui16payloadSize;   */           

	32000				/* uint16_t    ui16payloadRate;  */                                        
	
};

const BDSP_Raaga_Audio_Brcm3DSurroundConfigParams   BDSP_sDefBrcm3DSurroundConfigSettings = 
{
    1,                                          /* i32BRCM3DSurroundEnableFlag */
    BDSP_Raaga_Audio_eBroadcom3DSurroundMode_Wide   /* eBroadcom3DSurroundMode */
};

const BDSP_Raaga_Audio_MonoDownMixConfigParams   BDSP_sDefMonoDownmixConfigSettings = 
{
    1                                           /* i32MonoDownMixEnableFlag */
};

const BDSP_Raaga_Audio_MixerConfigParams  BDSP_sDefFwMixerConfigSettings = 
{
    3,                                  		/* ui32NumInput */
	{
		{										/* MixingCoeffs [0][]*/
			0x08000000,							/* MixingCoeffs [0][0]*/
			0x08000000,							/* MixingCoeffs [0][1]*/
			0x08000000,							/* MixingCoeffs [0][2]*/
			0x08000000,							/* MixingCoeffs [0][3]*/
			0x08000000,							/* MixingCoeffs [0][4]*/
			0x08000000,							/* MixingCoeffs [0][5]*/
			0x08000000,							/* MixingCoeffs [0][6]*/
			0x08000000							/* MixingCoeffs [0][7]*/
		},
		{										/* MixingCoeffs [1][]*/
			0x08000000,							/* MixingCoeffs [1][0]*/
			0x08000000,							/* MixingCoeffs [1][1]*/
			0x08000000,							/* MixingCoeffs [1][2]*/
			0x08000000,							/* MixingCoeffs [1][3]*/
			0x08000000,							/* MixingCoeffs [1][4]*/
			0x08000000,							/* MixingCoeffs [1][5]*/
			0x08000000,							/* MixingCoeffs [1][6]*/
			0x08000000							/* MixingCoeffs [1][7]*/
		},
		{										/* MixingCoeffs [2][]*/
			0x08000000,							/* MixingCoeffs [2][0]*/
			0x08000000,							/* MixingCoeffs [2][1]*/
			0x08000000,							/* MixingCoeffs [2][2]*/
			0x08000000,							/* MixingCoeffs [2][3]*/
			0x08000000,							/* MixingCoeffs [2][4]*/
			0x08000000,							/* MixingCoeffs [2][5]*/
			0x08000000,							/* MixingCoeffs [2][6]*/
			0x08000000							/* MixingCoeffs [2][7]*/
		},
	},
	0,											/* i32UserMixBalance */
};

const BDSP_Raaga_Audio_AudysseyVolUserConfig BDSP_sDefAudysseyVolConfigSettings = 
{
    2,                                          /* ui32NumChannels */
    1,                                          /*  ui32AudysseyVolBypass    */
    0xFF,                                       /*  ui32ChannelMask */
    0,                                          /*  i32AudysseyVolInit */
    0,                                          /*  i32HeadroomOffset  */
    1,                                          /*  i32SwAudysseyVol  */
    0,                                          /*  i32SwDynEQ */
    1,                                          /*  i32SwDynSurrGain  */
    1,                                          /*  i32SwHiLoCmpress  */
    0,                                          /*  i32dBVolSetting    */
    0x733333,                                   /*  i32GCF */
    {0,0,0,0,0,0,0,0},                          /*  i32chCalbGain[8]   */
    {0,0,0,0,0,0,0,0}                           /*  i32chCalcLevel[8]  */                        
};

const BDSP_Raaga_Audio_ABXConfigParams BDSP_sDefAudysseyABXConfigSettings = 
{
    0,                                          /*  ui32OperationMode */
    1,                                          /*  ui32FilterSet    */
    6,                                          /*  ui32HarmonicGain */
    0                                           /*  ui32DryGain */
};

const BDSP_Raaga_Audio_SRS_CSDecTruDialogConfigParams BDSP_sDefSrsCsdecTDConfigSettings = 
{
    {                                           /* sTopLevelConfig */
        0,                                      /* i32IsStudioSound */
        1,                                      /* i32StudioSoundMode */    
        1,                                      /* i32mEnable */
        0x7fffffff,                             /* i32mInputGain */
        0x7fffffff,                             /* i32mHeadroomGain */
        3,                                      /* i32mInputMode */
        0x7fffffff,                             /* i32mOutputGain */
        0x7fffffff,                             /* i32mBypassGain */
    },
    {                                           /* sTruDialogConfig */
        1,                                      /* i32mEnable */
        0x0c000000,                             /* i32mTruDialogProcessGain */
        0x7fffffff,                             /* i32mTruDialogInputGain */
        0x7fffffff,                             /* i32mTruDialogOutputGain */
        0x7fffffff,                             /* i32mTruDialogDialogClarityGain */
        0x7fffffff,                             /* i32mTruDialogBypassGain */
    },
    {                                           /* sCSDecoderConfig */
        1,                                      /* i32mEnable */
        0x7fffffff,                             /* i32mInputGain */
        1,                                      /* i32mMode */
        0,                                      /* i32mOutputMode */
        0x10000000,                             /* i32mCSDecOutputGainLR */
        0x10000000,                             /* i32mCSDecOutputGainLsRs */
        0x10000000,                             /* i32mCSDecOutputGainC */
        0x10000000,                             /* i32mCSDecOutputGainSub */
    }
};

const BDSP_Raaga_Audio_SRS_EqualizerHardLimiterConfigParams BDSP_sDefSrsEqHlConfigSettings = 
{
    {                                           /* sTopLevelParams */
        0,                                      /* i32IsStudioSound */
        1,                                      /* i32StudioSoundMode */    
        1,                                      /* i32mEnable */
        0x7fffffff,                             /* i32mInputGain */
        0x7fffffff,                             /* i32mHeadroomGain */
        3,                                      /* i32mInputMode */
        0x7fffffff,                             /* i32mOutputGain */
        0x7fffffff,                             /* i32mBypassGain */                
    },
    {                                           /* sPEqParams */
        {
            1,                                  /* bEnable[0] */
            1                                   /* bEnable[1] */
        },
        {
            {
                1,                           /* i32mBandEnable[0][0] */
                1,                           /* i32mBandEnable[0][1] */
                1,                           /* i32mBandEnable[0][2] */
                1,                           /* i32mBandEnable[0][3] */
                1,                           /* i32mBandEnable[0][4] */
                1,                           /* i32mBandEnable[0][5] */
                1,                           /* i32mBandEnable[0][6] */
                1                            /* i32mBandEnable[0][7] */
            },
            {
                1,                           /* i32mBandEnable[1][0] */
                1,                           /* i32mBandEnable[1][1] */
                1,                           /* i32mBandEnable[1][2] */
                1,                           /* i32mBandEnable[1][3] */
                1,                           /* i32mBandEnable[1][4] */
                1,                           /* i32mBandEnable[1][5] */
                1,                           /* i32mBandEnable[1][6] */
                1                            /* i32mBandEnable[1][7] */
            }
        },
        0x08000000,                             /* i32mInputGain */
        0x08000000,                             /* i32mOutputGain */
        0x08000000,                             /* i32mBypassGain */
        0,                                      /* i32CoefGenMode */
		{										/* sFilterCoefPeq[][] */
			{									/* sFilterCoefPeq[0][] */
				{								/* sFilterCoefPeq[0][0] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][1] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][2] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][3] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][4] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][5] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][6] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[0][7] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x798f9f00,					/* i32FilterCoefficientA1 */
					0x86706100,					/* i32FilterCoefficientB1  */
					0xc40eb580,					/* i32FilterCoefficientA2 */
					0x3bf14a80					/* i32FilterCoefficientB2 */
				}
			},
			{									/* sFilterCoefPeq[1][] */
				{								/* sFilterCoefPeq[1][0] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][1] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][2] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][3] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][4] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][5] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][6] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[1][7] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7bc37b00,					/* i32FilterCoefficientA1 */
					0x843c8500,					/* i32FilterCoefficientB1  */
					0xc2f83300,					/* i32FilterCoefficientA2 */
					0x3d07cd00					/* i32FilterCoefficientB2 */
				}
			},
			{									/* sFilterCoefPeq[2][] */
				{								/* sFilterCoefPeq[2][0] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][1] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][2] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][3] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][4] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][5] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][6] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				},
				{								/* sFilterCoefPeq[2][7] */
					0x00000002,					/* i32Order */
					0x00000001,					/* i32Scale */
					0x40000000,					/* i32FilterCoefficientB0 */
					0x7c31eb80,					/* i32FilterCoefficientA1 */
					0x83ce1480,					/* i32FilterCoefficientB1  */
					0xc2bbbb80,					/* i32FilterCoefficientA2 */
					0x3d444480					/* i32FilterCoefficientB2 */
				}
			}
		},
		{										/* sFilterSpecPeq[] */
			{									/* sFilterSpecPeq[0] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[1] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[2] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[3] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[4] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[5] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[6] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			},
			{									/* sFilterSpecPeq[7] */
				0, 								/* i32BandGain */
				1000,							/* ui32BandFrequency */
				0x02000000						/* i32QFactor */
			}
		}
    },
    {                                           /* sHighPassFilterConfig */
        1,                                      /* ui32mEnable */
        0,                                      /* ui32CoefGenMode */
		{										/* sFilterCoefHpf[] */
			{									/* sFilterCoefHpf[0] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fdadc10,				/* i32FilterCoefficientB0 */
						0xe04a47e0,				/* i32FilterCoefficientB1 */
						0x0fdadc10,				/* i32FilterCoefficientB1 */
						0x7ed26000,				/* i32FilterCoefficientA1 */
						0xc1249f40				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3ea0f4c0,				/* i32FilterCoefficientB0 */
						0x82be1680,				/* i32FilterCoefficientB1 */
						0x3ea0f4c0,				/* i32FilterCoefficientB1 */
						0x7d3d7780,				/* i32FilterCoefficientA1 */
						0xc2b9a440				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			},
			{									/* sFilterCoefHpf[1] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fdadc10,				/* i32FilterCoefficientB0 */
						0xe04a47e0,				/* i32FilterCoefficientB1 */
						0x0fdadc10,				/* i32FilterCoefficientB1 */
						0x7ed26000,				/* i32FilterCoefficientA1 */
						0xc1249f40				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3ea0f4c0,				/* i32FilterCoefficientB0 */
						0x82be1680,				/* i32FilterCoefficientB1 */
						0x3ea0f4c0,				/* i32FilterCoefficientB1 */
						0x7d3d7780,				/* i32FilterCoefficientA1 */
						0xc2b9a440				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			},
			{									/* sFilterCoefHpf[2] */
				0x00000004,						/* ui32Order */
				{								/* sFilterCoefHpfTdf2[] */
					{							/* sFilterCoefHpfTdf2[0] */
						0x00000001,				/* i32Scale */
						0x0fdadc10,				/* i32FilterCoefficientB0 */
						0xe04a47e0,				/* i32FilterCoefficientB1 */
						0x0fdadc10,				/* i32FilterCoefficientB1 */
						0x7ed26000,				/* i32FilterCoefficientA1 */
						0xc1249f40				/* i32FilterCoefficientA2 */
					},			
					{							/* sFilterCoefHpfTdf2[1] */
						0x00000001,				/* i32Scale */
						0x3ea0f4c0,				/* i32FilterCoefficientB0 */
						0x82be1680,				/* i32FilterCoefficientB1 */
						0x3ea0f4c0,				/* i32FilterCoefficientB1 */
						0x7d3d7780,				/* i32FilterCoefficientA1 */
						0xc2b9a440				/* i32FilterCoefficientA2 */
    },
					{							/* sFilterCoefHpfTdf2[2] */
						0x00000000,				/* i32Scale */
						0x00000000,				/* i32FilterCoefficientB0 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientB1 */
						0x00000000,				/* i32FilterCoefficientA1 */
						0x00000000				/* i32FilterCoefficientA2 */
					},			
				}
			}
		},
		{										/* sFilterSpecHpf */
			180,								/* ui32CutoffFrequency */
			4									/* ui32Order */
		}
    },
    {                                           /* sGEqParams */
        {
            1,                              /* i32mEnable[0] */
            1                               /* i32mEnable[1] */
        },
        {
            {
                0x20000000,   /* i32mBandGain[0][0] */
                0x20000000,   /* i32mBandGain[0][1] */
                0x20000000,   /* i32mBandGain[0][2] */
                0x20000000,   /* i32mBandGain[0][3] */
                0x20000000,   /* i32mBandGain[0][4] */
                0x20000000,   /* i32mBandGain[0][5] */
                0x20000000,   /* i32mBandGain[0][6] */
                0x20000000,   /* i32mBandGain[0][7] */
                0x20000000,   /* i32mBandGain[0][8] */
                0x20000000    /* i32mBandGain[0][9] */
            },
            {
                0x20000000,   /* i32mBandGain[1][0] */
                0x20000000,   /* i32mBandGain[1][1] */
                0x20000000,   /* i32mBandGain[1][2] */
                0x20000000,   /* i32mBandGain[1][3] */
                0x20000000,   /* i32mBandGain[1][4] */
                0x20000000,   /* i32mBandGain[1][5] */
                0x20000000,   /* i32mBandGain[1][6] */
                0x20000000,   /* i32mBandGain[1][7] */
                0x20000000,   /* i32mBandGain[1][8] */
                0x20000000    /* i32mBandGain[1][9] */
            }
        },
        0x2872b021,        /* i32mInputGain */
        0x7fffffff,        /* i32mOutputGain */
        0x7fffffff,         /* ui32BypassGain */
        0                  /* i32mFilterMode */ 
    },
    { /* BDSP_Raaga_Audio_HardLimiterUserConfig */
        0,                  /* i32LimiterEnable */
        2,                  /* i32nchans */
        256,                /* i32blockSize */
        0x20000000,         /* i32InputGain */
        0x20000000,         /* i32OutputGain*/
        0x7fffffff,         /* i32BypassGain */
        0x0168f5c3,         /* i32LimiterBoost */
        0x7fffffff,         /* i32HardLimit */
        22                  /* i32DelayLength */                
    }
};

const BDSP_Raaga_Audio_DbeConfigParams BDSP_sDefCustomDbeConfigSettings = 
{
    1,                                          /* i32DbeEnable */
    {                                           
        {                                       /* sChannelConfig[0] */
            true,                               /* i32UseHighPass */
            0,                                  /* i32HeadRoom */
            0x72148300,                         /* i32TargetLevel */
            0x24FCC016,                         /* i32MaxGain */
            0x17B6F7,                           /* i32MinGain */
            0x40015D80,                         /* i32GainIncrement */
            0x3fdde700,                         /* i32GainDecrement */
            {                                   /* sDbeHighPassCoeffs */
                0x40000000,                     /* iA0Coeff */
                0x00000000,                     /* iA1Coeff */ 
                0x00000000,                     /* iA2Coeff */ 
                0x00000000,                     /* iB1Coeff */ 
                0x00000000                      /* iB2Coeff */   
            },
            {                                   /* sDbeBandPassCoeffs */
                0x40000000,                     /* iA0Coeff */                     
                0x00000000,                     /* iA1Coeff */ 
                0x00000000,                     /* iA2Coeff */ 
                0x00000000,                     /* iB1Coeff */ 
                0x00000000                      /* iB2Coeff */               
            }
        },
        {                                       /* sChannelConfig[1] */
            true,                               /* i32UseHighPass */
            0,                                  /* i32HeadRoom */
            0x72148300,                         /* i32TargetLevel */
            0x24FCC016,                         /* i32MaxGain */
            0x17B6F7,                           /* i32MinGain */
            0x40015D80,                         /* i32GainIncrement */
            0x3fdde700,                         /* i32GainDecrement */
            {                                   /* sDbeHighPassCoeffs */
                0x40000000,                     /* iA0Coeff */
                0x00000000,                     /* iA1Coeff */ 
                0x00000000,                     /* iA2Coeff */ 
                0x00000000,                     /* iB1Coeff */ 
                0x00000000                      /* iB2Coeff */   
            },
            {                                   /* sDbeBandPassCoeffs */
                0x40000000,                     /* iA0Coeff */                     
                0x00000000,                     /* iA1Coeff */ 
                0x00000000,                     /* iA2Coeff */ 
                0x00000000,                     /* iB1Coeff */ 
                0x00000000                      /* iB2Coeff */               
            }
        },
    }
};

const BDSP_Raaga_Audio_AvlPConfigParams BDSP_sDefCustomAvlplusConfigSettings = 
{
    true,                                       /* i32AvlOn */
    true,                                       /* i32AvlMute */
    2007,                                       /* i32PeakReductionPerSlot */
    3277,                                       /* i32PeakThreshold */
    205,                                        /* i32PeakIncrement */
    205,                                        /* i32Corrfactorlowampl */
    48,                                         /* i32Sp */
    32,                                         /* i32Sr */
    4,                                          /* i32Ap2bp */
    21,                                         /* i32NewRmsWeight */
    32,                                         /* i32L0 */
    54,                                         /* i32L1 */
    80,                                         /* i32L2 */
    48,                                         /* i32L3 */
    99,                                         /* i32L4 */
    6,                                          /* i32L20 */ 
    6,                                          /* i32L30 */
    12,                                         /* i32L41 */
    14,                                         /* i32Bp */
    40,                                         /* i32Br */
    4,                                          /* i32SlowRegionR */
    16,                                         /* i32SlowRegionP */
    15,                                         /* i32Gmax */
    45,                                         /* i32NewRmsAbovePrevRmsThresh */
    12,                                         /* i32WeightForhigherRms */
    1331,                                       /* i32StrongRmsCorThresh */
    29,                                         /* i32PeakBeatSlopeInv */
    {                                           /* sAvlpFilterConfig */
        0,                                      /* i32HeadRoom */
        {                                       /* sAvlPlusFilterCoeffs */
            {0x40000000, 0, 0, 0, 0},           /* sAvlPlusFilterCoeffs[0] */
            {0x40000000, 0, 0, 0, 0},           /* sAvlPlusFilterCoeffs[1] */
            {0x40000000, 0, 0, 0, 0}            /* sAvlPlusFilterCoeffs[2] */
        }
    },  
    0x8000000,                                  /* i32VolumeOffset */
    0                                           /* i32HeadRoomCreationParam */
};

const BDSP_Raaga_Audio_DsolaConfigParams BDSP_sDefDsolaConfigSettings =
{
    0x266                                       /* ui32InputPcmFrameSize */
};

const BDSP_Raaga_Audio_TruSurrndHDConfigParams BDSP_sDefTruSurrndHDConfigSettings =
{    
    1,                                                  /* i32TruSurrndHDEnableFlag */
    0,                                                  /* i32HeadPhoneEnableFlag */
    0x40000000,                                         /* i32TruSurrndHDInputGain */
    0x7fffffff,								            /* i32TruSurrndHDOutputGain */
    0x7FFFFFFF,									        /* i32TruSurrndHDByPassGain */
    BDSP_Raaga_Audio_SRSTSHDCrossoverFrequency_e200Hz,  /* eSubCrossOverFreq */
    1,                                                  /* i32TruBassFrontEnableFlag */
    0x26666666,	                                        /* i32TruBassFrontCtrl */
    BDSP_Raaga_Audio_SpeakerSize_eLFResponse150Hz,      /* eTruBassFrontSpeakerSize */
    BDSP_Raaga_Audio_SRSTruBassProcessMode_eStereo,     /* eSRSTruBassProcessMode */
    1,                                                  /* i32DefinitionFrontEnableFlag */
    0x26666666,                                         /* i32DefinitionFrontCtrl */
    1,                                                  /* i32DialogClarityEnableFlag */
    0x40000000,                                         /* i32DialogClarityCtrl */
    0x4CCCCCCD,                                         /* i32SurroundLevel */
    1,                                                  /* i32WowHDSRS3DEnableFlag */
    1,                                                  /* i32SRS3DHighBitRateFlag */
    BDSP_Raaga_Audio_SRS3DMode_eStereo,                 /* eWowHDSRS3DMode */
    0x66666666,                                         /* i32WowHDSRS3DSpaceCtrl */
    0x40000000,                                         /* i32WowHDSRS3DCenterCtrl */
    1,                                                  /* i32WowHDFocusEnableFlag */
    0x33333333,                                         /* i32WowHDFocusCtrl */
    1,                                                  /* i32Mono2StereoEnableFlag */
    BDSP_Raaga_Audio_AcMode_eMode20,                    /* eOutputAcMode */
    false,                                              /* i32OuputLFEEnableFlag */
    false,                                              /* i32CertificationEnableFlag */ 
    BDSP_Raaga_Audio_AcMode_eMode20,                    /* eInputAcmod */    
    false,                                              /* i32InputLFEPresentFlag */
    {                                                   /* sTopLevelConfig */
        0,                                              /* i32IsStudioSound */
        1,                                              /* i32StudioSoundMode */    
        true,                                           /* i32mEnable */
        0x7fffffff,                                     /* i32mInputGain */
        0x7fffffff,                                     /* i32mHeadroomGain */
        3,                                              /* i32mInputMode */
        0x7fffffff,                                     /* i32mOutputGain */
        0x7fffffff,                                     /* i32mBypassGain */                
    }   
};
const BDSP_Raaga_Audio_WMAEncoderConfigParams BDSP_sDefWmaEncodeConfigSettings = 
{
	192000,												/* ui32Bitrate */
		0,												/* EnableMono */
};
const BDSP_Raaga_VideoBH264UserConfig BDSP_sBH264EncodeUserConfigSettings = 
{
	BDSP_Raaga_VideoH264Profile_eBaseline, 
	31,
	BDSP_Raaga_VideoEncodeMode_eAfap,
	1000000,
	320,
	240,
	30,
	30,
	BDSP_Raaga_VideoGopStruct_eIP,
	false,
	false,
	0,
	{
		1,
		{8,8,8},
		{50,50,50},
		4,
		0
	},
	BDSP_VF_P_EncodeFrameRate_e30,
	1,
	0,
	0,
	1,
	0,
	0,
    1,
    0	
};

const BDSP_Raaga_VideoBVP8UserConfig BDSP_sBVP8EncodeUserConfigSettings = 
{
	BDSP_Raaga_VideoH264Profile_eBaseline, 
	31,
	BDSP_Raaga_VideoEncodeMode_eAfap,
	1000000,
	320,
	240,
	30,
	30,
	BDSP_Raaga_VideoGopStruct_eIP,
	true,
	true,
	0,
	{
		1,
		{4,4,4},
		{127,127,127},
		4,
		0
	},
	BDSP_VF_P_EncodeFrameRate_e30,
	1,
	0,
	0,
	1,
	0,
	0,
    1
};
