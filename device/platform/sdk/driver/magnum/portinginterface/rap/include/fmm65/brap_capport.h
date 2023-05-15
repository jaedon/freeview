/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_capport.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 2/10/10 8:54p $
*
* Module Description:
*   Module name: CAPPORT
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Capture Port abstraction, which are exposed 
*   to the application developer. It covers the functionality of the Capture 
*   Hardware Block. This object is only used by a PCM Capture Channel.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_capport.h $
* 
* Hydra_Software_Devel/4   2/10/10 8:54p sgadara
* SW3556-1031: [3548,3556] Adding India Mode
* -->Merge to main line
* 
* Hydra_Software_Devel/SW3556-1031/1   2/2/10 5:04p jgarrett
* SW3556-1031: [3556] Adding India Mode
* 
* Hydra_Software_Devel/3   10/20/08 2:25p gautamk
* PR47961: [7405] Fixing warning and compilation for 7405 platform due to
* changes of this PR.
* 
* Hydra_Software_Devel/2   8/8/08 9:56a speter
* PR 45176: [3548,3556] Adding the code for the RF Audio Nicam mode
* changes
* 
* Hydra_Software_Devel/1   7/21/08 2:18p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   6/18/08 9:12p speter
* PR 38950: [3548,3556] Fixing the NICAM modes
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   6/11/08 7:32a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/28/08 5:13a speter
* PR 39442: [3548,3556] Moving the countrycode parameter of BTSC from
* open to Inputconfig
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   5/28/08 2:45a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   5/20/08 2:34p speter
* PR 38950: [3548,3556] Adding APIs for new features like Audio
* Descriptors and NICAM
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   5/20/08 1:53p speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/5   4/9/07 8:20p sushmit
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
* Hydra_Software_Devel/4   2/4/07 5:12p bhanus
* PR 25607 : Fixed compilation warnings.
* 
* Hydra_Software_Devel/3   12/28/06 1:57p speter
* PR 25346: [3563] SPDIF/HDMI In Feature - Removing #if 3563 code in
* common header file
* 
* Hydra_Software_Devel/2   12/13/06 4:11p speter
* PR 25346: [3563] SPDIF/HDMI In Feature
* 
* Hydra_Software_Devel/1   8/28/06 4:34p speter
* PR 20654: Adding capture related files
***************************************************************************/
#ifndef _BRAP_CAPPORT_H_
#define _BRAP_CAPPORT_H_     

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************
Summary:  SPDIF Receiver Insert Mode
Description:  Enum provides various type of insert modes while receiving SPDIF In
**********************************************************************/
typedef enum BRAP_SPDIF_RX_InsertMode
{
   BAUD_SPDIF_RX_InsertMode_eNoInsert = 0,
   BAUD_SPDIF_RX_InsertMode_eInsertZero = 1,
   BAUD_SPDIF_RX_InsertMode_eRepeat = 2
} BRAP_SPDIF_RX_InsertMode;

/**********************************************************************
Summary: SPDIF Receiver HDMI Number
Description:  Enum provides values for the various HDMI Numbers
**********************************************************************/
typedef enum BRAP_SPDIF_RX_HdmiNo
{
   BAUD_SPDIF_RX_eHdmi0 = 0,
   BAUD_SPDIF_RX_eHdmi1 = 1
} BRAP_SPDIF_RX_HdmiNo;

/***************************************************************************
Summary:	
	US output modes.
Description:	
	This enum represents the output modes for BTSC standard.	There are three main 
	output mode: Mono, Stereo, and Sap. 
****************************************************************************/
typedef enum BRAP_RfAudioBtscOutputMode
{	
	BRAP_RfAudioBtscOutputMode_eMono,        /* Mono  */	
	BRAP_RfAudioBtscOutputMode_eStereo,      /* Stereo*/	
	BRAP_RfAudioBtscOutputMode_eSap = 6,     /* Sap   */	
	BRAP_RfAudioBtscOutputMode_eSapMono	     /* Sap Mono */
}BRAP_RfAudioBtscOutputMode;

/***************************************************************************
Summary:	
	Japan output modes.
Description:	
	This enum represents the output modes for EIAJ standard. DualMono and stereo 
	are the main modes.
****************************************************************************/
typedef enum BRAP_RfAudioEiajOutputMode
{	
	BRAP_RfAudioEiajOutputMode_eMain = 0,   /* Mono  L=Main R=Main */	
	BRAP_RfAudioEiajOutputMode_eStereo,	    /* Stereo L = L R = R*/	
	BRAP_RfAudioEiajOutputMode_eSub,        /*  Special mode L=Sub R=Sub */	
	BRAP_RfAudioEiajOutputMode_eMainSub     /* DualMono L=main(LanguageA) R=Sub(LanguageB) */
}BRAP_RfAudioEiajOutputMode;

/***************************************************************************
Summary:	
	Korea output modes.
Description:	
	This enum represents the output modes for Korea A2 standard. DualMono and stereo 
	are the main modes.
****************************************************************************/
typedef enum BRAP_RfAudioKoreaA2OutputMode
{	
	BRAP_RfAudioKoreaA2OutputMode_eMain = 0,/* Mono  L=Main R=Main */	
	BRAP_RfAudioKoreaA2OutputMode_eStereo,  /* Stereo L = L R = R*/	
	BRAP_RfAudioKoreaA2OutputMode_eSub,     /* Special mode L=Sub R=Sub */	
	BRAP_RfAudioKoreaA2OutputMode_eMainSub  /* DualMono L=main(LanguageA) R=Sub(LanguageB) */
}BRAP_RfAudioKoreaA2OutputMode;

/***************************************************************************
Summary:   
   Nicam output modes.
Description:   
   This enum represents the output modes for Nicam standard. 
****************************************************************************/
typedef enum BRAP_RfAudioNicamOutputMode
{
    BRAP_RfAudioNicamOutputMode_eFmAmMono,      /* AM / FM Mono */
    BRAP_RfAudioNicamOutputMode_eNicamMono,      /* Nicam Mono*/        
    BRAP_RfAudioNicamOutputMode_eNicamStereo,    /* NICAM stereo audio */
    BRAP_RfAudioNicamOutputMode_eDualMono1,      /* NICAM dual mono audio M1 or M2.
                                                Selection of M1 or M2 or M1 on L and M2 on R
                                                is done in IFD PI */
    BRAP_RfAudioNicamOutputMode_eDualMono2,      /* NICAM dual mono audio M1 or M2.
                                                Selection of M1 or M2 or M1 on L and M2 on R
                                                is done in IFD PI */
    BRAP_RfAudioNicamOutputMode_eDualMono1And2, /* NICAM dual mono audio M1 or M2.
                                                Selection of M1 or M2 or M1 on L and M2 on R
                                                is done in IFD PI */
                                                
                                                
    BRAP_RfAudioNicamOutputMode_eMax
} BRAP_RfAudioNicamOutputMode;

/***************************************************************************
Summary:   
   PAL-A2 output modes.
Description:   
   This enum represents the output modes for PAL-A2 standard. 
****************************************************************************/
typedef enum BRAP_RfAudioPalA2OutputMode
{   
   BRAP_RfAudioPalA2OutputMode_eMain = 0,   /* Mono  L=Main R=Main */   
   BRAP_RfAudioPalA2OutputMode_eStereo,     /* Stereo L = L R = R*/   
   BRAP_RfAudioPalA2OutputMode_eSub,        /*  Special mode L=Sub R=Sub */   
   BRAP_RfAudioPalA2OutputMode_eMainSub     /* DualMono L=main(LanguageA) R=Sub(LanguageB) */
}BRAP_RfAudioPalA2OutputMode;

/***************************************************************************
Summary:	
	Configurations for SAP mode - BTSC only.
Description:	
	This is for BTSC standard only.  It has the options to control the SAP to 	
	Mono autoswitch, SAP to Mono mute, and the on/off power level.
****************************************************************************/
typedef struct BRAP_RfAudioSapConfig
{	
	bool bSapPswOn;             /* Sap and Mono Auto switch on/off */	
	bool bSapMono1Mute0;		/* mute on/off at Mono mode */ 	
	uint16_t ui16SapOffLevel;	/* Sap's pilot power level lower threshold */	
	uint16_t ui16SapOnLevel;	/* Sap's pilot power level upper threshold*/
}BRAP_RfAudioSapConfig;

/***************************************************************************
Summary:	
	Configurations for automatic mode switch - for either BTSC or Eiaj standards.
Description:	
	This structure represents the configuration of automatic mode switch for BTSC 
	(between stereo and mono mode) and Eiaj(Stereo/Main/DaulMono mode).  
****************************************************************************/
typedef struct BRAP_RfAudioStereoConfig
{	
	bool bPltPswOn;			    /* Mode autoswitch (stereo/mono for BTSC, Stereo/Main/DualMono for Eiaj */	
	uint16_t ui16PltOffLevel;	/* Pilot power level lower threshold */	
	uint16_t ui16PltOnLevel;	/* Pilot power level upper threshold*/
}BRAP_RfAudioStereoConfig;

/***************************************************************************
Summary:	Standards that RfAudio hardware supports - BTSC, Eiaj.
Description:	This enum represents all the standards that hardware supports. 
****************************************************************************/
typedef enum BRAP_RfAudioStandard
{
	BRAP_RfAudioStandard_eBtsc = 0,		        /* Standard followed in US*/
	BRAP_RfAudioStandard_eBtscHighDeviation,    /* High deviation Standard followed in US */	
	BRAP_RfAudioStandard_eEiaj,			        /* A Japan standard */	
	BRAP_RfAudioStandard_eKoreaA2,              /* Korean A2 standard */	
    BRAP_RfAudioStandard_eNicam,                /* Nicam standard */
    BRAP_RfAudioStandard_ePalA2,	            /* Pal A2 standard */
    BRAP_RfAudioStandard_eSecamL,               /* SECAM-L standard i.e AM audio with Nicam*/    
    BRAP_RfAudioStandard_eIndia                 /* India High-Deviation FM Mono audio */
} BRAP_RfAudioStandard;

/* Input I2S Params */
typedef struct BRAP_CAPPORT_I2sInParams
{
    bool        bLsbAtLRClk;    /* Data Justification. Controls whether LSB or 
                                   MSB is at LRCK transition 
                                   TRUE: LSB, FALSE: MSB */
    bool        bAlignedToLRClk;   
                                /* Controls whether data is aligned with 
                                   LRCK or delayed  by one SCLK period 
                                   FALSE: Delayed. TRUE: Aligned */
    bool        bLRClkPolarity; /* Sets the polarity of the left/right clock 
                                   TRUE: High for Left FALSE: Low for Left */
} BRAP_CAPPORT_I2sInParams;

/**********************************************************************
Summary:   This structure gives the SPDIF capture port settings
**********************************************************************/
typedef struct BRAP_SpdifRxCapParams
{
   BRAP_SPDIF_RX_InsertMode eInsertMode;
   bool   bIgnorePcmParityErr;
   bool   bIgnoreCompParityErr;
   bool   bIgnoreValidity;
} BRAP_SpdifRxCapParams;

/**********************************************************************
Summary:   This structure gives the HDMI capture port settings
**********************************************************************/
typedef struct BRAP_HdmiRxCapParams
{
   BRAP_SPDIF_RX_HdmiNo   eHdmiNo;
   BRAP_SPDIF_RX_InsertMode eInsertMode;
   bool   bIgnorePcmParityErr;
   bool   bIgnoreCompParityErr;
   bool   bIgnoreValidity;
} BRAP_HdmiRxCapParams;

/**********************************************************************
Summary:   This structure gives the Rf Audio capture port settings
**********************************************************************/
typedef struct BRAP_RfAudioCapParams
{
    BRAP_RfAudioBtscOutputMode      eRfAudioBtscOpMode;
    BRAP_RfAudioEiajOutputMode      eRfAudioEiajOpMode;
    BRAP_RfAudioKoreaA2OutputMode   eRfAudioKoreaA2OpMode;
    BRAP_RfAudioNicamOutputMode     eRfAudioNicamOpMode;
    BRAP_RfAudioPalA2OutputMode     eRfAudioPalA2OpMode;    
    BRAP_RfAudioSapConfig           sRfAudioSapConfig;   
    BRAP_RfAudioStereoConfig        sRfAudioStereoConfig;
    bool                            bRfAudioMute;
    BRAP_RfAudioStandard            eRfAudioStandard;
} BRAP_RfAudioCapParams;


/**********************************************************************
Summary:   This structure gives the ADC capture port settings
**********************************************************************/
typedef struct BRAP_AdcCapParams
{
    uint16_t                uiInputSelect;  /* Select one of the six 
                                                stereo analog inputs */
} BRAP_AdcCapParams;


/* Capture Port External Params */
typedef struct BRAP_CAPPORT_Params
{
    union uCapPortParams
    {
        BRAP_CAPPORT_I2sInParams    sInputI2sParams;
                                                /* Input I2S Params. It is used 
                                                   only if the Capture input 
                                                   port is of type I2S */
        BRAP_SpdifRxCapParams       sSpdifRxParams;
                                                /* Input SPDIF Params. It is used 
                                                   only if the Capture input 
                                                   port is of type SPDIF */
        BRAP_HdmiRxCapParams        sHdmiRxParams;
                                                /* Input HDMI Params. It is used 
                                                   only if the Capture input 
                                                   port is of type HDMI */
        BRAP_RfAudioCapParams       sRfAudioParams;
                                                /* Input RF Audio Params. It is used 
                                                   only if the Capture input 
                                                   port is of type RF Audio */

        BRAP_AdcCapParams           sAdcParams;
                                                /* Input ADC Params. It is used 
                                                   only if the Capture input 
                                                   port is of type ADC */                                                
    }uCapPortParams;
                                                   
} BRAP_CAPPORT_Params;

#ifdef __cplusplus
}
#endif

#endif /* _BRAP_CAPPORT_H_ */


/* End of File */


