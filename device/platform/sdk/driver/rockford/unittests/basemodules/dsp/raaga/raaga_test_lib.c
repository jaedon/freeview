/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_lib.c $
 * $brcm_Revision: Hydra_Software_Devel/41 $
 * $brcm_Date: 10/17/12 1:44p $
 *
 ****************************************************************************/ 

#include "raaga_test.h"

BRAAGA_TEST_CaptureStruct captureInfo = {0};
BRAAGA_TEST_CaptureStruct *pcaptureInfo = &captureInfo;

BDBG_MODULE(raaga_test);

/* Static array defining CDB and ITB sizes for various 
   Buffer Configuration Modes */
#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
static const BAVC_CdbItbConfig sCdbItbCfg[BRAAGA_TEST_BufferCfgMode_eMaxMode] =
{
	/* BRAAGATEST_BufferCfgMode_eStdAudioMode */
	{ { 128 * 1024, 4, false }, { 64 * 1024, 4, false }, false },	
	/* BRAAGATEST_BufferCfgMode_eAdvAudioMode */
	{ { 256 * 1024, 4, false }, { 128 * 1024, 4, false }, false },	
	/* BRAAGATEST_BufferCfgMode_eIpStbMode */
	{ { 256 * 1024, 4, false }, { 256 * 1024, 4, false } , false},  
	/* BRAAGATEST_BufferCfgMode_eHdDvdMode */
	{ { 32 * 1024, 4, false }, { 32 * 1024, 4, false } , false},	
	/* BRAAGATEST_BufferCfgMode_eBlueRayMode */
	{ { 32 * 1024, 4, false }, { 32 * 1024, 4, false } , false}	    
	/* TODO: Pupulate this array as more new modes gets added */
};
#else
static const BAVC_CdbItbConfig sCdbItbCfg[BRAAGA_TEST_BufferCfgMode_eMaxMode] =
{
	/* BRAAGATEST_BufferCfgMode_eStdAudioMode */
	{ { 128 * 1024, 4, true }, { 64 * 1024, 4, false }, false },	
	/* BRAAGATEST_BufferCfgMode_eAdvAudioMode */
	{ { 256 * 1024, 4, true }, { 128 * 1024, 4, false }, false },	
	/* BRAAGATEST_BufferCfgMode_eIpStbMode */
	{ { 256 * 1024, 4, true }, { 256 * 1024, 4, false } , false},  
	/* BRAAGATEST_BufferCfgMode_eHdDvdMode */
	{ { 32 * 1024, 4, true }, { 32 * 1024, 4, false } , false},	
	/* BRAAGATEST_BufferCfgMode_eBlueRayMode */
	{ { 32 * 1024, 4, true }, { 32 * 1024, 4, false } , false}	    
	/* TODO: Pupulate this array as more new modes gets added */
};
#endif

static const bool bSrcRequired [BDSP_AudioType_eMax] =
{
    true , /*BDSP_AudioType_eMpeg*/
    true , /*BDSP_AudioType_eAacAdts*/
    true , /*BDSP_AudioType_eAacLoas*/
    true , /*BDSP_AudioType_eAacSbrAdts*/
    true , /*BDSP_AudioType_eAacSbrLoas*/
    false, /*BDSP_AudioType_eAc3*/
    false, /*BDSP_AudioType_eAc3Plus*/
    false, /*BDSP_AudioType_eLpcmBd*/
    false, /*BDSP_AudioType_eLpcmHdDvd*/
    false, /*BDSP_AudioType_eDtshd*/
    false, /*BDSP_AudioType_eLpcmDvd*/
    true , /*BDSP_AudioType_eWmaStd*/
    false, /*BDSP_AudioType_eMlp*/
    false, /*BDSP_AudioType_ePcm*/
    false, /*BDSP_AudioType_eDtsLbr*/
    false, /*BDSP_AudioType_eDdp7_1*/
    true , /*BDSP_AudioType_eMpegMc*/
    true , /*BDSP_AudioType_eWmaPro*/
    false, /*BDSP_AudioType_eLpcmDvdA*/
    false, /*BDSP_AudioType_eDtsBroadcast*/
    false , /*BDSP_AudioType_ePcmWav*/
    false, /*BDSP_AudioType_eAmr*/
    false, /*BDSP_AudioType_eDra*/
    false, /*BDSP_AudioType_eRealAudioLbr*/
    true , /*BDSP_AudioType_eAdpcm*/
    false, /*BDSP_AudioType_eG711G726*/
    false, /*BDSP_AudioType_eG729*/
    false, /*BDSP_AudioType_eVorbis*/
    false, /*BDSP_AudioType_eG723_1*/
    true, /*BDSP_AudioType_eFlac*/
	true, /*BDSP_AudioType_eMac*/
    false, /*BDSP_AudioType_eAmrwb*/

};

#define CENTER_CHANNEL_INDEX   0 

/* Helped function prototype define */
BERR_Code BRAAGA_TEST_WatchdogEventandThreadCreate(BRAAGA_TEST_Handle    hRaagaTest);
void * BRAAGA_TEST_WatchdogThread(void *iParam1);
BERR_Code BRAAGA_TEST_GetBufferConfig (	BRAAGA_TEST_BufferCfgMode	eCfgMode, BAVC_CdbItbConfig	*pCdbItbConfigInfo );
void BRAAGA_TEST_ConfigureStreamerInput( BXPT_Handle hXpt);

#if (RAAGA_TEST_STATIC_CDB_ITB == 1 )
void BRAAGA_TEST_LoadStaticCdbItb(BRAAGA_TEST_Handle hRaagaTest,BMEM_Handle hMemory,BREG_Handle  hReg,BRAAGA_TEST_CxtType		eChTaskType);

uint32_t BRAAGA_P_DRAMREAD(
    uint32_t	offset /* Memory offset to write */
    )
{
	uint32_t ui32ValRead;

	ui32ValRead = *((volatile uint32_t *)offset);
	return ui32ValRead;
}

void BRAAGA_P_DRAMWRITE(
    uint32_t	offset, /* Memory offset to write */
    uint32_t	data    /* Data to write */
    )
{
	*((volatile uint32_t *)offset) = data;
}
#endif


/*******************************************************************************
Summary: Transport Init.
*******************************************************************************/
#if RAAGA_TEST_ENABLE_TRANSPORT
BERR_Code BRAAGA_TEST_XptInit(
    BRAAGA_TEST_Handle            hRaagaTest,
    BRAAGA_TEST_ChHandle          hRaagaTestCh,
    BCHP_Handle                 hChip,
	BMEM_Handle                 hMemory,
	BINT_Handle                 hInt,
	BREG_Handle                 hReg
)
{
    BERR_Code                   err = BERR_SUCCESS;
    BXPT_DefaultSettings        sXptSettings;
    BXPT_Rave_ChannelSettings   sRaveSettings;
    BXPT_PcrOffset_Defaults     PcrOffsetDefaults;
    int                         i=0;

	BDBG_MSG(("hChip =%x hMemory=%x hInt=%x hReg=%x hRaagaTestCh=%x hRaagaTest=%x",
				hChip,hMemory,hInt,hReg,hRaagaTestCh,hRaagaTest));

	/* Get XPT device default settings */
	err = BXPT_GetDefaultSettings(&sXptSettings, hChip);
	BDBG_ASSERT( !err );

	/* Open the XPT Device */
	err = BXPT_Open (&(hRaagaTest->hXpt), hChip, hReg, hMemory, hInt, &sXptSettings);
	BDBG_ASSERT( !err );
	BDBG_MSG(("\nXPT device opened"));

    /* Code for XPT memory clients to work */
    BRAAGA_TEST_ConfigureStreamerInput(hRaagaTest->hXpt);

    /* Get RAVE defualt settings */
    err = BXPT_Rave_GetChannelDefaultSettings(hRaagaTest->hXpt, 0, 
                                               &sRaveSettings );
	BDBG_ASSERT( !err );

    /* Open RAVE channel */
	err = BXPT_Rave_OpenChannel(hRaagaTest->hXpt, &(hRaagaTest->hRave), 
	                             0, &sRaveSettings );
	BDBG_ASSERT( !err );

    /* Open PCR offset channels */
	err = BXPT_PcrOffset_GetChannelDefSettings (hRaagaTest->hXpt, hRaagaTestCh->uiChId, 
	                                            &PcrOffsetDefaults);
    BDBG_ASSERT( !err );       
	PcrOffsetDefaults.UsePcrTimeBase = true;
	PcrOffsetDefaults.OffsetThreshold = 0;
	PcrOffsetDefaults.WhichPcr = i;
	err = BXPT_PcrOffset_Open( hRaagaTest->hXpt, hRaagaTestCh->uiChId, 
                               &PcrOffsetDefaults, &(hRaagaTest->hPcrOff));   
    BDBG_ASSERT( !err );

    /* Spawn the data feeder tasks and 
       associated initialization for XPT Playback */
	if(hRaagaTest->bFilePb == true)
	{
		BDBG_LOG(("uiNumStreams:=%d", hRaagaTest->uiNumStreams));
	    hRaagaTest->hPbLib = BRAAGA_TEST_PbLib_Init(
                            hRaagaTest->hXpt, 
                            0);
	}
    
    return err;   
}

/*******************************************************************************
Summary : configures the streamer input
*******************************************************************************/
void BRAAGA_TEST_ConfigureStreamerInput( BXPT_Handle hXpt)
{
    
/* Route parallel data from PCMCIA connector to IB4 */
    BREG_Write32( hXpt->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, 0x22222222/* 0x2AAAC000 */);
    BREG_Write32( hXpt->hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, 0x00002022 /* 0x00000049 */ );
}

/*******************************************************************************
Summary: Opens required Rave Context.
*******************************************************************************/
BERR_Code BRAAGA_TEST_OpenRaveContext
(  
    BRAAGA_TEST_Handle          hRaagaTest,
    BRAAGA_TEST_TaskHandle		hRaagaTaskHandle
)  
{
    BERR_Code ret;
    BXPT_PCR_DefSettings sDefPCrSettings;
    BAVC_CdbItbConfig CdbItb;
    BXPT_PCR_GetChannelDefSettings(hRaagaTest->hXpt, hRaagaTaskHandle->uiChId, &sDefPCrSettings);   
    BXPT_PCR_Open(hRaagaTest->hXpt, hRaagaTaskHandle->uiChId, &sDefPCrSettings, &(hRaagaTest->hPcr));
#if BRAAGA_DDP_SUPPORT
    ret = BRAAGA_TEST_GetBufferConfig(BRAAGA_TEST_BufferCfgMode_eAdvAudioMode, &CdbItb);
#else
    ret = BRAAGA_TEST_GetBufferConfig(BRAAGA_TEST_BufferCfgMode_eStdAudioMode, &CdbItb);
#endif
    BDBG_ASSERT(ret == BERR_SUCCESS);
    
    ret =  BXPT_Rave_AllocContext( hRaagaTest->hRave, 
                                   BXPT_RaveCx_eAv, 
                                   &CdbItb, 
                                   &(hRaagaTaskHandle->hCtx));

	ret = BXPT_Rave_GetContextRegisters( hRaagaTaskHandle->hCtx, &(hRaagaTaskHandle->CtxMap) );
	BDBG_ASSERT( !ret );

    BDBG_ASSERT( !ret );
    
    return BERR_SUCCESS;
}

/*******************************************************************************
Summary: Starts required Rave Context.
*******************************************************************************/
BERR_Code BRAAGA_TEST_StartRaveContext
( 
    BRAAGA_TEST_Handle			hRaagaTest,
    BRAAGA_TEST_ChHandle        hRaagaTestCh,
    BRAAGA_TEST_TaskHandle		hTaskInfo,
    BXPT_PCR_Handle				hPcr
)  
{
	BERR_Code ret;
	BXPT_Rave_ContextPtrs CtxPtrs;
	BXPT_Rave_AvSettings AvCtxCfg;
	BXPT_PCR_XptStreamPcrCfg PcrCfg; 
	BXPT_PcrOffset_Settings PcrOffsetSettings;
	BXPT_InputBandConfig IbConfig;

    BXPT_Playback_Handle hXptPb;
    BXPT_Playback_PacketizeConfig PacketCfg; 


#if RAAGA_TEST_ENABLE_FRONTEND_INPUT
        IbConfig.ParallelInputSel = false;
#else
        IbConfig.ParallelInputSel = true;
#endif

    /* Don't configure the input band and don't link the input bands to parser bands for XPT Playback case */
	if(hTaskInfo->bXptPbEn == false)
	{
#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233) 	
    	/* Parser band number and Input band numbers are taken here same */
    	BXPT_GetInputBandConfig( hRaagaTest->hXpt, 2, &IbConfig );
    	IbConfig.SyncDetectEn = true;
    	IbConfig.ForceValid= true;        
    	BXPT_SetInputBandConfig( hRaagaTest->hXpt, 2, &IbConfig );
#else
    	/* Parser band number and Input band numbers are taken here same */
    	BXPT_GetInputBandConfig( hRaagaTest->hXpt, 5, &IbConfig );
    	IbConfig.SyncDetectEn = true;
    	IbConfig.ForceValid= true;        
    	BXPT_SetInputBandConfig( hRaagaTest->hXpt, 5, &IbConfig );
#endif        

    	/* Enable Input bands 0,1,2 and Parser bands 0,1,2 */
    	ret = BXPT_SetParserDataSource( hRaagaTest->hXpt, 0, BXPT_DataSource_eInputBand, hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiInputBand);
    	ret |= BXPT_SetParserDataSource( hRaagaTest->hXpt, 1, BXPT_DataSource_eInputBand, hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiInputBand );
    	ret |= BXPT_SetParserDataSource( hRaagaTest->hXpt, 2, BXPT_DataSource_eInputBand, hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiInputBand );
#if (BCHP_CHIP != 35230) && (BCHP_CHIP != 35125) && (BCHP_CHIP != 35233)        
    	ret |= BXPT_SetParserDataSource( hRaagaTest->hXpt, 5, BXPT_DataSource_eInputBand, hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiInputBand );
#endif
    	BDBG_ASSERT( !ret );
   
    	ret = BXPT_SetParserEnable( hRaagaTest->hXpt, 0, true );
    	ret |= BXPT_SetParserEnable( hRaagaTest->hXpt, 1, true );
    	ret |= BXPT_SetParserEnable( hRaagaTest->hXpt, 2, true );
#if (BCHP_CHIP != 35230) && (BCHP_CHIP != 35125) && (BCHP_CHIP != 35233)        
    	ret |= BXPT_SetParserEnable( hRaagaTest->hXpt, 5, true );
#endif
    	BDBG_ASSERT( !ret );
    }
    
	/* Form the Rave Av Settings */
    BXPT_Rave_GetAvConfig(hTaskInfo->hCtx, &AvCtxCfg);

    /* For both TS and ES streams, InputFormat should be set to 
       BAVC_StreamType_eTsMpeg */
    if((hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eTsMpeg) ||
       (hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eEs))
	    AvCtxCfg.InputFormat = BAVC_StreamType_eTsMpeg;
    else    
        AvCtxCfg.InputFormat = hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType;

	AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;            /* CDB Format is ES */
    BKNI_Memset(AvCtxCfg.EsRanges, 0, sizeof(AvCtxCfg.EsRanges)); /* all disabled */    
	switch(hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eAlgoType)                            /* Algorithm type */
	{
		case BDSP_AudioType_eMpeg:
            if(hRaagaTestCh->sSettings.uiItbformat ==0)
                AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
            else
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudio;
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpegAudioWithDescriptor;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xC0;         /* Stream ID lo = 0xC0 */
			break;

		case BDSP_AudioType_eAacAdts:
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAacAudio;
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xC0;         /* Stream ID lo = 0xC0 */
			break;

		case BDSP_AudioType_eAacSbrLoas:         
		case BDSP_AudioType_eAacSbrAdts:         
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAacHe;
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAudioDescriptor;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xC0;         /* Stream ID lo = 0xC0 */
			break;

		case BDSP_AudioType_eAc3:
            AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAc3gAudio;  
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
			break;

		case BDSP_AudioType_eAc3Plus:            
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAc3Plus;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
			break;

		case BDSP_AudioType_eDtsBroadcast:
		case BDSP_AudioType_eDtshd:            
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eDtsAudio;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
			break;			

		case BDSP_AudioType_eDra:
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eDra;
			AvCtxCfg.StreamIdHi =  0xEF;        /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
			break;

		case BDSP_AudioType_eLpcmDvd:
#if (BCHP_CHIP !=7422)            
		case BDSP_AudioType_eAdpcm:            
#endif
			AvCtxCfg.ItbFormat = BAVC_ItbEsType_eLpcmAudio;
			AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
			AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
			break;              
			
		case BDSP_AudioType_eLpcmBd:
		default:    
			printf("\n Algo type : Not supported");
			printf("\nAudio Format not supported yet\n");
			return BERR_TRACE(BERR_NOT_SUPPORTED);
	}


    if(hTaskInfo->bXptPbEn == true)
	{
	    /* This will make sure the CDB/ITB data is not corrupted due to RAVE 
	       overflows. This is not done for streamer input, because this will 
	       make the filling of CDB/ITB very fast. */
        AvCtxCfg.BandHoldEn = true;
    }
    
    if ( hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiAudioPid != 
         hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiPcrPid )
	{
		BXPT_ConfigurePidChannel(
            hRaagaTest->hXpt, 
			(hTaskInfo->uiChId*2)+1, 
			hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiPcrPid, 
			hTaskInfo->bXptPbEn ? 
			BXPT_PB_PARSER( hTaskInfo->uiChId):hTaskInfo->uiChId);

		BXPT_EnablePidChannel(hRaagaTest->hXpt, (hTaskInfo->uiChId*2)+1);
	}

	BXPT_ConfigurePidChannel(
        hRaagaTest->hXpt, 
		(hTaskInfo->uiChId*2), 
		hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiAudioPid, 
		hTaskInfo->bXptPbEn ? 
		BXPT_PB_PARSER( hTaskInfo->uiChId):hTaskInfo->uiChId);

	BXPT_EnablePidChannel(hRaagaTest->hXpt, (hTaskInfo->uiChId*2));

	BXPT_Rave_AddPidChannel(hTaskInfo->hCtx, (hTaskInfo->uiChId*2), false);

    BXPT_PcrOffset_EnableOffset (
        hRaagaTest->hPcrOff,      /* The PCR Offset handle */
        (hTaskInfo->uiChId*2), /* The PID channel to generate offsets for */
        false,                  /* false, we don't want fixed offsets */
        true );                 /* true. corrects jitter due to RS & XC buffers. */

	ret = BXPT_PcrOffset_GetSettings(hRaagaTest->hPcrOff, &PcrOffsetSettings );
    if(ret != BERR_SUCCESS)
    {
      BDBG_ERR(("Get PcrOffset Settings failed"));
      return BERR_TRACE(ret);
    }
	/* update the PID channel number */
	hTaskInfo->uiPidChnNo = (hTaskInfo->uiChId*2);

	if ( hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiAudioPid != 
         hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].uiPcrPid )
			PcrOffsetSettings.PidChannelNum = (hTaskInfo->uiChId*2)+1;       /* PID channel carrying PCRs */
	else
			PcrOffsetSettings.PidChannelNum = (hTaskInfo->uiChId*2);       /* PID channel carrying PCRs */		

	if((hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eDssEs) || 
	   (hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eDssPes))
	{
			PcrOffsetSettings.CountMode = BXPT_PcrOffset_StcCountMode_eBinary;
	}
	else if((hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eTsMpeg) ||
            (hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType == BAVC_StreamType_eEs))
	{
	    /* For MPEG streams, this is set to Mod 300 */
		PcrOffsetSettings.CountMode = BXPT_PcrOffset_StcCountMode_eMod300;
	}
	else
	{
		BDBG_ERR(("\nStream type %d not supported", 
                   hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	PcrOffsetSettings.OffsetThreshold = 0;
	PcrOffsetSettings.UseHostPcrs = false;
	ret = BXPT_PcrOffset_SetSettings(hRaagaTest->hPcrOff, &PcrOffsetSettings );
    if(ret != BERR_SUCCESS)
    {
      BDBG_ERR(("Set PcrOffset Settings failed"));
      return BERR_TRACE(ret);
    }
    
    BXPT_PCR_GetStreamPcrConfig(hPcr, &PcrCfg);     
    PcrCfg.PidChannel = (hTaskInfo->uiChId*2);       /* PID channel carrying PCRs */		
	BXPT_PCR_SetStreamPcrConfig(hPcr, &PcrCfg); 
    
    /* Configuration for different type of XPT playbacks */
    if(hTaskInfo->bXptPbEn == true)
	{
        /* Get the XPT Playback Handle */
        hXptPb = BRAAGA_TEST_PbLib_GetPbHandle(hRaagaTest->hPbLib);
        
        switch(hRaagaTest->sRaagaStrParams[hTaskInfo->uiStreamId].eStreamType)
        {
            case BAVC_StreamType_eEs:
           	    /* ES Playback */
                PacketCfg.Pid = 0xC0;
                PacketCfg.ChannelNum = 0x00; 
                PacketCfg.PacketizerMode = BXPT_Playback_PacketizerMode_Es;
                PacketCfg.FilterConfig.StreamIdRange.Hi = 0xC0;
                PacketCfg.FilterConfig.StreamIdRange.Lo = 0x80;

                ret = BXPT_Playback_PacketizeStream( hXptPb, 0, &PacketCfg, true );
                BDBG_ASSERT(ret == BERR_SUCCESS);
                break;
            case BAVC_StreamType_eTsMpeg:
                /* TS Playback */
                /* Don't do anything */
                break;
            default:
                /* XPT playback of other formats are not supported yet */
                return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
	}

	ret = BXPT_Rave_SetAvConfig( hTaskInfo->hCtx, &AvCtxCfg ); 
	BDBG_ASSERT( !ret );

	ret = BXPT_Rave_EnableContext( hTaskInfo->hCtx ); 
	BDBG_ASSERT( !ret );

	ret = BXPT_Rave_GetContextRegisters( hTaskInfo->hCtx, &hTaskInfo->CtxMap );
	BDBG_ASSERT( !ret );
	
	ret = BXPT_Rave_CheckBuffer( hTaskInfo->hCtx, &CtxPtrs );
	BDBG_ASSERT( !ret );

#if (XPT_DUMP_ENABLED==1)
	DumpPtrs( &CtxPtrs );
	DumpCdbItb(hRaagaTest->hXpt->hMemory, hRaagaTest->hXpt->hRegister, &hTaskInfo->CtxMap);
	DumpRaveCxtRegisters(hRaagaTest->hXpt->hRegister, 0);
#endif

	return BERR_SUCCESS;
}

/*******************************************************************************
Summary: Stops required Rave Context.
*******************************************************************************/
BERR_Code BRAAGA_TEST_StopRaveContext
(  
	BRAAGA_TEST_Handle			hRaagaTest,
    BRAAGA_TEST_TaskHandle      hTaskInfo		
)  
{
    BERR_Code        ret = BERR_SUCCESS;
	BSTD_UNUSED(hRaagaTest);
    ret = BXPT_Rave_DisableContext( hTaskInfo->hCtx );
	BXPT_Rave_FlushContext(hTaskInfo->hCtx);
    BDBG_ASSERT( !ret );

    return BERR_SUCCESS;
}

/*******************************************************************************
Summary: Closes required Rave Context.
*******************************************************************************/
BERR_Code BRAAGA_TEST_CloseRaveContext
( 
	BRAAGA_TEST_TaskHandle	hRaagaTask,
    BXPT_PCR_Handle         hPcr
)  
{
    BERR_Code        ret = BERR_SUCCESS;

    ret = BXPT_Rave_FreeContext( hRaagaTask->hCtx );
    BDBG_ASSERT( !ret );
    BXPT_PCR_Close( hPcr );

    return BERR_SUCCESS;
}
#endif

/*******************************************************************************
Summary: DSP Init. 
*******************************************************************************/
BERR_Code BRAAGA_TEST_DspInit(
    BRAAGA_TEST_Handle          hRaagaTest,
    BCHP_Handle                 hChip,
	BMEM_Handle                 hMemory,
	BINT_Handle                 hInt,
	BREG_Handle                 hReg,
	BTMR_Handle		            hTmr
)
{
	BERR_Code                   err = BERR_SUCCESS;
    BDSP_RaagaSettings 			raagaSettings;
	unsigned int                i=0;
#if RAAGA_FWAUTHENTICATION_ENABLE
    BDSP_Raaga_DownloadStatus pStatus;    
#endif
    BDSP_Raaga_GetDefaultSettings(&raagaSettings);

    raagaSettings.debugSettings[BDSP_Raaga_DebugType_eUart].enabled = true;
	raagaSettings.debugSettings[BDSP_Raaga_DebugType_eDramMsg].enabled = true;
	raagaSettings.debugSettings[BDSP_Raaga_DebugType_eCoreDump].enabled = true;
    raagaSettings.debugSettings[BDSP_Raaga_DebugType_eTargetPrintf].enabled = true;

#if RAAGA_FWAUTHENTICATION_ENABLE
    raagaSettings.authenticationEnabled = true;
#endif

    err = BDSP_Raaga_Open(&(hRaagaTest->hDsp),
						hChip, 
						hReg, 
						hMemory, 
						hInt,
						hTmr, 
						&raagaSettings);
	if(err)
    {
        BDBG_ERR(("Unable to open RAAGA\n"));
        return err;
    }      

#if RAAGA_FWAUTHENTICATION_ENABLE
    err = BDSP_Raaga_GetDownloadStatus (hRaagaTest->hDsp, &pStatus);
    if (err!=BERR_SUCCESS)
        return err;
    BDBG_LOG(("\nFirmware Info: address = 0x%x|0x%x, size=0x%x\n", pStatus.pBaseAddress, pStatus.physicalAddress, pStatus.length));


    err = BDSP_Raaga_Initialize (hRaagaTest->hDsp);
#endif

	/* Populate the stream database */
	BDBG_ASSERT(hRaagaTest->uiNumStreams <= MAX_STREAMS);
	for(i=0; i < hRaagaTest->uiNumStreams; i++)
	{
#if RAAGA_TEST_ENABLE_TRANSPORT
#if RAAGA_TEST_ENABLE_FRONTEND_INPUT
#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233) 
		hRaagaTest->sRaagaStrParams[i].uiInputBand = 2;/* Stream is coming from input band 5*/
#else
		hRaagaTest->sRaagaStrParams[i].uiInputBand = 5;/* Stream is coming from input band 5*/
#endif
#else
		hRaagaTest->sRaagaStrParams[i].uiInputBand = 0;/* Stream is coming from input band 0*/
#endif
#endif        
	}

    err = BRAAGA_TEST_WatchdogEventandThreadCreate(hRaagaTest);
	BDBG_ASSERT(err == BERR_SUCCESS);    	

    return err;
}

/*******************************************************************************
Summary: Opens required Raaga Task handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_OpenRaagaContext(
    BRAAGA_TEST_Handle      hRaagaTest,
	BRAAGA_TEST_CxtType		eChTaskType    
)
{
    BERR_Code               err = BERR_SUCCESS;
	unsigned int            i=0;
    BRAAGA_TEST_ChHandle    hRaagaTestCh;
    BDSP_ContextCreateSettings 	settings;
	
  	BDBG_ASSERT(hRaagaTest->uiNumAudioTask <= RAAGA_TEST_MAX_FW_TASK_PER_DSP);
	switch(eChTaskType)
	{
		case(BRAAGA_TEST_CxtType_eAudio):
		{
			for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
			{
				if(hRaagaTest->hRaagaTestCh[i]->hDspcxt == NULL)
				{
					hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
					hRaagaTestCh->sSettings.eChTaskType = BRAAGA_TEST_CxtType_eAudio;
					/* Open Audio context */
					BDSP_Context_GetDefaultCreateSettings(hRaagaTest->hDsp,BDSP_ContextType_eAudio,&settings);
					err = BDSP_Context_Create(hRaagaTest->hDsp,&settings,&(hRaagaTestCh->hDspcxt));
					if(err != BERR_SUCCESS)
					{
						BDBG_ERR(("BRAAGA_TEST_OpenRaagaContext: BDSP_Context_Create failed __LINE__=%d",__LINE__));
						BDBG_ASSERT(err == BERR_SUCCESS);		
					}
					hRaagaTestCh->bRunning = true;
					break;
				}
			}
			if(i == RAAGA_MAX_CONTEXT)
			{
				BDBG_ERR(("BRAAGA_TEST_OpenRaagaContext: no free context"));
				BDBG_ASSERT(0);		
			}
			break;
		}
		case(BRAAGA_TEST_CxtType_eGraphics):
		case(BRAAGA_TEST_CxtType_eSecurity):
		case(BRAAGA_TEST_CxtType_eVideo):
        {
			for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
			{
				if(hRaagaTest->hRaagaTestCh[i]->hDspcxt == NULL)
				{
					hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
					hRaagaTestCh->sSettings.eChTaskType = BRAAGA_TEST_CxtType_eVideo;
					/* Open video context */
					BDSP_Context_GetDefaultCreateSettings(hRaagaTest->hDsp,BDSP_ContextType_eVideo,&settings);
					err = BDSP_Context_Create(hRaagaTest->hDsp,&settings,&(hRaagaTestCh->hDspcxt));
					if(err != BERR_SUCCESS)
					{
						BDBG_ERR(("BRAAGA_TEST_OpenRaagaContext: BDSP_Context_Create failed __LINE__=%d",__LINE__));
						BDBG_ASSERT(err == BERR_SUCCESS);		
					}
					hRaagaTestCh->bRunning = true;
					break;
				}
			}
			if(i == RAAGA_MAX_CONTEXT)
			{
				BDBG_ERR(("BRAAGA_TEST_OpenRaagaContext: no free context"));
				BDBG_ASSERT(0);		
			}
			break;
		}            
		case(BRAAGA_TEST_CxtType_eMax):
		default:
			/* XPT playback of other formats are not supported yet */
			return BERR_TRACE(BERR_NOT_SUPPORTED);
	}
	return err;
}
/*******************************************************************************
Summary: Opens required Raaga Task handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_CloseRaagaContext(
    BRAAGA_TEST_Handle      hRaagaTest,
	BRAAGA_TEST_ChHandle	hRaagaTestCh
)
{
    BERR_Code	err = BERR_SUCCESS;
	int			i=0, j=0;
	
	BDBG_ASSERT(hRaagaTest);
	BDBG_ASSERT(hRaagaTestCh);

	for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
	{ 
		if((hRaagaTest->hRaagaTestCh[i])&&
		   (hRaagaTest->hRaagaTestCh[i] == hRaagaTestCh))
		{
			for(j=0; j<RAAGA_TEST_MAX_TASK_PER_CONTEXT; ++j)
			{
				if(hRaagaTestCh->hTaskInfo[j])
				{
					if(hRaagaTestCh->hTaskInfo[j]->bRunning)
					{
						BRAAGA_TEST_StopRaagaTask(hRaagaTest, hRaagaTestCh->hTaskInfo[j]);
					}
					BRAAGA_TEST_CloseRaagaTask(hRaagaTest, hRaagaTestCh, hRaagaTestCh->hTaskInfo[j]);
				}
			}
			BDSP_Context_Destroy(hRaagaTestCh->hDspcxt);
			hRaagaTestCh->bRunning = false;
			hRaagaTestCh->hDspcxt = NULL;
		}
	}
	return err;
}

/*******************************************************************************
Summary: Opens required Raaga Context handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_OpenRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	*hTaskHandle,
	BRAAGA_TEST_CxtType		eChTaskType,    
    unsigned int			uiStreamId,
	BRAAGA_TEST_TaskDstType	eDstType
)
{
    BERR_Code               	err = BERR_SUCCESS;
	unsigned int            	i=0, k=0,uiTempSize=0,ui32BaseAddr=0,ui32UsedSize=0;
    BRAAGA_TEST_ChHandle    	hRaagaTestCh;
    BDSP_TaskCreateSettings 	taskSettings;
    BDSP_TaskHandle 			task;
	BRAAGA_TEST_TaskHandle		hRaagaTaskHandle = NULL;
	void *						buff1;
	void *						buff2;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
	void *						pCached;
#endif
	uint32_t					ui32PhyAddr=0,ui32RegVal=0;
    uint32_t                    pcmOpStageIndex;


    uint32_t    encoderStageId=0;
    uint32_t    encoderBranchId=0;
    bool        bIsSpeechEncoder;
    uint32_t    index;
    const char  *pCdbFile = "MuxCdbOut";
    const char  *pItbFile = "MuxItbOut";
    
    BXDM_Picture *hXDMPicture = NULL;

	for(i=0; i < RAAGA_MAX_CONTEXT; i++)
	{
		if((hRaagaTest->hRaagaTestCh[i] != NULL) &&
		   (hRaagaTest->hRaagaTestCh[i]->sSettings.eChTaskType == eChTaskType))
		{
			hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
			for(k=0; k<RAAGA_TEST_MAX_TASK_PER_CONTEXT; k++)
			{
				if(hRaagaTestCh->hTaskInfo[k] == NULL)
				{
					hRaagaTaskHandle = (BRAAGA_TEST_TaskHandle)BKNI_Malloc(sizeof(BRAAGA_TEST_Task));
					if(hRaagaTaskHandle == NULL)
					{
						return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
					}		 
					BKNI_Memset(hRaagaTaskHandle, 0, sizeof(BRAAGA_TEST_Task));
					hRaagaTestCh->hTaskInfo[k] = hRaagaTaskHandle;
					*hTaskHandle = hRaagaTaskHandle;
					break;
				}
			}
			if(k == RAAGA_TEST_MAX_TASK_PER_CONTEXT)
			{
				BDBG_ERR(("Asserting k=%d",k));
				BDBG_ASSERT(0);
			}

			if(eDstType == BRAAGA_TEST_TaskDstType_eRaveBuf)
			{
				hRaagaTaskHandle->eDstType = eDstType;
				hRaagaTaskHandle->uiStreamId = uiStreamId;
				if(hRaagaTest->bXptPbEn == true)				
				{
#if (RAAGA_TEST_ENABLE_TRANSPORT==1)
				/* Open Rave Context */
				err = BRAAGA_TEST_OpenRaveContext(hRaagaTest, hRaagaTaskHandle);
				BDBG_ASSERT( !err );
#endif
				}
				
				if(hRaagaTest->bXptPbEn == false)				
				{
#if (RAAGA_TEST_STATIC_CDB_ITB == 1)
        	BRAAGA_TEST_LoadStaticCdbItb(hRaagaTest, hRaagaTest->hMem, hRaagaTest->hReg,eChTaskType);

            if(eChTaskType == BRAAGA_TEST_CxtType_eAudio)
            {
				hRaagaTaskHandle->CtxMap.ContextIdx = 0;
				hRaagaTaskHandle->CtxMap.CDB_Read = BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Base = BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Wrap = BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Valid = BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR;
				hRaagaTaskHandle->CtxMap.CDB_End =	BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Read = BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Base = BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Wrap = BCHP_XPT_RAVE_CX0_AV_ITB_WRAPAROUND_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Valid = BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR;
				hRaagaTaskHandle->CtxMap.ITB_End = BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR;	            

                if (g_encoder_audio_type != BDSP_AudioEncode_eMax)
                {
                    /* ENCODER outputs to Mux */
                    hRaagaTaskHandle->CtxMap1.ContextIdx = 1;
                    hRaagaTaskHandle->CtxMap1.CDB_Read = BCHP_XPT_RAVE_CX1_AV_CDB_READ_PTR;
                    hRaagaTaskHandle->CtxMap1.CDB_Base = BCHP_XPT_RAVE_CX1_AV_CDB_BASE_PTR;
                    hRaagaTaskHandle->CtxMap1.CDB_Wrap = BCHP_XPT_RAVE_CX1_AV_CDB_WRAPAROUND_PTR;
                    hRaagaTaskHandle->CtxMap1.CDB_Valid = BCHP_XPT_RAVE_CX1_AV_CDB_WRITE_PTR;
                    hRaagaTaskHandle->CtxMap1.CDB_End =	BCHP_XPT_RAVE_CX1_AV_CDB_END_PTR;
                    hRaagaTaskHandle->CtxMap1.ITB_Read = BCHP_XPT_RAVE_CX1_AV_ITB_READ_PTR;
                    hRaagaTaskHandle->CtxMap1.ITB_Base = BCHP_XPT_RAVE_CX1_AV_ITB_BASE_PTR;
                    hRaagaTaskHandle->CtxMap1.ITB_Wrap = BCHP_XPT_RAVE_CX1_AV_ITB_WRAPAROUND_PTR;
                    hRaagaTaskHandle->CtxMap1.ITB_Valid = BCHP_XPT_RAVE_CX1_AV_ITB_WRITE_PTR;
                    hRaagaTaskHandle->CtxMap1.ITB_End = BCHP_XPT_RAVE_CX1_AV_ITB_END_PTR;
                }
            }
            if(eChTaskType == BRAAGA_TEST_CxtType_eVideo)
            {
				hRaagaTaskHandle->CtxMap.ContextIdx = 1;
				hRaagaTaskHandle->CtxMap.CDB_Read = BCHP_XPT_RAVE_CX1_AV_CDB_READ_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Base = BCHP_XPT_RAVE_CX1_AV_CDB_BASE_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Wrap = BCHP_XPT_RAVE_CX1_AV_CDB_WRAPAROUND_PTR;
				hRaagaTaskHandle->CtxMap.CDB_Valid = BCHP_XPT_RAVE_CX1_AV_CDB_VALID_PTR;
				hRaagaTaskHandle->CtxMap.CDB_End =	BCHP_XPT_RAVE_CX1_AV_CDB_END_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Read = BCHP_XPT_RAVE_CX1_AV_ITB_READ_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Base = BCHP_XPT_RAVE_CX1_AV_ITB_BASE_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Wrap = BCHP_XPT_RAVE_CX1_AV_ITB_WRAPAROUND_PTR;
				hRaagaTaskHandle->CtxMap.ITB_Valid = BCHP_XPT_RAVE_CX1_AV_ITB_VALID_PTR;
				hRaagaTaskHandle->CtxMap.ITB_End = BCHP_XPT_RAVE_CX1_AV_ITB_END_PTR;	            
            }
            
#endif
				}
			}
            
			if(eChTaskType == BRAAGA_TEST_CxtType_eAudio)
			{
				BDSP_Task_GetDefaultCreateSettings(hRaagaTestCh->hDspcxt,&taskSettings);
				taskSettings.dspIndex =0 ;
				taskSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
				taskSettings.numBranches =1;
				taskSettings.pBranchInfo[0] =(BDSP_CIT_P_FwBranchInfo *)BKNI_Malloc(
												 sizeof(BDSP_CIT_P_FwBranchInfo));
				BKNI_Memset(taskSettings.pBranchInfo[0], 0 , sizeof(BDSP_CIT_P_FwBranchInfo));
				taskSettings.pBranchInfo[0]->ui32NumStages = 1;
                pcmOpStageIndex = 0;
                
                if (bSrcRequired[hRaagaTest->sRaagaStrParams[0].eAlgoType])
                {
                    taskSettings.pBranchInfo[0]->ui32NumStages++;
                    pcmOpStageIndex++;
                }

				/* Stage 0 */
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].eStageType= BDSP_CIT_P_StageType_eDecode;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32BranchId = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32StageId = 0;  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo = hRaagaTest->sRaagaStrParams[0].eAlgoType;		  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode = BDSP_DecodeMode_eDecode;		  
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Base;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_End;	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Read;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Valid;	  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Wrap;		 
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Base;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_End;	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Read;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Valid;	  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Wrap;  
				
                    
                
                if (bSrcRequired[hRaagaTest->sRaagaStrParams[0].eAlgoType])
                {
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 1;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[0] = 1;
                    
                    if (g_capture_decoder_multich)
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_e5_1_PCM;
                    }
                    else
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;
                    }

    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32StageId = 1;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32DistinctOpNum = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32OpNum = 0;

    				/* Stage 1 */
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].eStageType= BDSP_CIT_P_StageType_ePostProc;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].ui32StageId = 1;  
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].uAlgorithm.eProcAudioAlgo= BDSP_AudioProcessing_eSrc; 	  
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].uAudioMode.eProcessingAudioMode= BDSP_DSPCHN_ProcessingMode_ePP;		

    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.ui32NumSrc = 1; 	
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    				
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;
                }
                
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDistinctOp = 1;
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDst[0] = 1;
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;
                
                if (g_encoder_audio_type != BDSP_AudioEncode_eMax)
                {
                    bIsSpeechEncoder = BRGA_TST_IsSpeechEncoder(g_encoder_audio_type);

                    if (!(bIsSpeechEncoder))
                    {
                        encoderBranchId = 0;
                        encoderStageId = pcmOpStageIndex+1;
                    }
                    else
                    {
                        encoderBranchId = (pcmOpStageIndex == 0) ? 0 : 1;
                        encoderStageId = (pcmOpStageIndex == 0) ? 1 : 0;
                    }
                    
                    if (encoderBranchId)
                    {
                        taskSettings.numBranches++;
                        taskSettings.pBranchInfo[encoderBranchId] =(BDSP_CIT_P_FwBranchInfo *)BKNI_Malloc(
                                                                        sizeof(BDSP_CIT_P_FwBranchInfo));
                        BKNI_Memset(taskSettings.pBranchInfo[encoderBranchId], 0 , sizeof(BDSP_CIT_P_FwBranchInfo));
                        taskSettings.pBranchInfo[encoderBranchId]->ui32NumStages = 2;
                    }
                    else
                    {
                        taskSettings.pBranchInfo[0]->ui32NumStages += 2;
                    }

                    if (!(bIsSpeechEncoder))
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDistinctOp = 1;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDst[0] = 2; /* One to encoder and one to SRC/DAC */
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;

                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][1].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][1].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][1].uDetails.sFwStage.ui32StageId = encoderStageId;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][1].uDetails.sFwStage.ui32DistinctOpNum = 0;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][1].uDetails.sFwStage.ui32OpNum = 1;
                    }
                    else
                    {
    					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 2;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[1] = 1; /* to encoder */
    					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[1] = BDSP_MONO_DISTINCT_OP_TYPE;
    					
                        /* Mono output */
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sFwStage.ui32StageId = encoderStageId;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sFwStage.ui32DistinctOpNum = 1;
                        taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sFwStage.ui32OpNum = 0;
                    }

                    /* Encoder Stage */
                    if (g_enable_echo_canceller)
                    {
                        /* Setup the echo canceller module and the intertask input */
                        if (bIsSpeechEncoder)
                        {
                            /* Need to allocate the intertask buffer and fill it up with the echo canceller data */
                            BDSP_AF_P_sIO_BUFFER            *pIoBfrDesc;
                            BDSP_AF_P_sIO_GENERIC_BUFFER    *pIoGenericBfrDesc;
                            void                            *pIoBfr;
                            void                            *pIoGenericBfr;
                            uint32_t                        phyAddress;
                            uint32_t                        fread_bytes;
                            FILE                            *fp;
                            
                            taskSettings.pBranchInfo[encoderBranchId]->ui32NumStages++;
                            encoderStageId++;
                            /* Setup the echo canceller stage */
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].eStageType= BDSP_CIT_P_StageType_ePostProc;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].ui32BranchId = encoderBranchId;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].ui32StageId = encoderStageId-1;  
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].uAlgorithm.eProcAudioAlgo= BDSP_AudioProcessing_eSpeexAec;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].uAudioMode.eProcessingAudioMode= BDSP_DSPCHN_ProcessingMode_ePP;

                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.ui32NumSrc = 1; 	
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                            
                            /* For all speech codecs SRC is required and EC will get input from the decode stage - this is no applicable any more as the SRCs are integrated into the speech codec */
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = 0;
                            
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 1;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;

                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.ui32NumDistinctOp = 1;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.ui32NumDst[0] = 1;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.eDistinctOpType[0] = BDSP_MONO_DISTINCT_OP_TYPE;
                            
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32StageId = encoderStageId;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32DistinctOpNum = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32OpNum = 0;
                        
                            
                            /* Allocate the buffers */
                            pIoBfr = (void *)BMEM_AllocAligned(hRaagaTest->hMem, INTER_TASK_IO_BUFFER_SIZE,8,0);
            				if(!pIoBfr)
            				{
            					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
            				}
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            /* Converting all memory accesses to cached */
                            BMEM_ConvertAddressToCached(hRaagaTest->hMem, pIoBfr, &pCached);
                            *(char *)pCached = 0;
                            pIoBfr = pCached;
#endif
                            pIoGenericBfr = (void *)BMEM_AllocAligned(hRaagaTest->hMem, INTER_TASK_GENERIC_BUFFER_SIZE,8,0);
            				if(!pIoGenericBfr)
            				{
            					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
            				}
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            /* Converting all memory accesses to cached */
                            BMEM_ConvertAddressToCached(hRaagaTest->hMem, pIoGenericBfr, &pCached);
                            *(char *)pCached = 0;
                            pIoGenericBfr = pCached;
#endif
                            /* Allocate the buffer descriptors */
                            pIoBfrDesc = (void *)BMEM_AllocAligned(hRaagaTest->hMem, sizeof(BDSP_AF_P_sIO_BUFFER),8,0);
            				if(!pIoBfrDesc)
            				{
            					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
            				}
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            /* Converting all memory accesses to cached */
                            BMEM_ConvertAddressToCached(hRaagaTest->hMem, pIoBfrDesc, &pCached);
                            *(char *)pCached = 0;
                            pIoBfrDesc = pCached;
#endif
                            pIoGenericBfrDesc = (void *)BMEM_AllocAligned(hRaagaTest->hMem, sizeof(BDSP_AF_P_sIO_BUFFER),8,0);
            				if(!pIoGenericBfrDesc)
            				{
            					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
            				}
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            /* Converting all memory accesses to cached */
                            BMEM_ConvertAddressToCached(hRaagaTest->hMem, pIoGenericBfrDesc, &pCached);
                            *(char *)pCached = 0;
                            pIoGenericBfrDesc = pCached;
#endif
                            /* Populate the buffer descriptors */
                            pIoBfrDesc->ui32NumBuffers = 1;
                            pIoBfrDesc->eBufferType = BDSP_AF_P_BufferType_eDRAM;
                            BMEM_ConvertAddressToOffset(hRaagaTest->hMem, pIoBfr, &phyAddress);
                            pIoBfrDesc->sCircBuffer[0].ui32BaseAddr = phyAddress;
                            pIoBfrDesc->sCircBuffer[0].ui32ReadAddr = phyAddress;
                            pIoBfrDesc->sCircBuffer[0].ui32WriteAddr = phyAddress;
                            pIoBfrDesc->sCircBuffer[0].ui32WrapAddr = phyAddress+INTER_TASK_IO_BUFFER_SIZE-4;
                            pIoBfrDesc->sCircBuffer[0].ui32EndAddr = phyAddress+INTER_TASK_IO_BUFFER_SIZE-4;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            BMEM_FlushCache(hRaagaTest->hMem, pIoBfrDesc, sizeof(BDSP_AF_P_sIO_BUFFER));
#endif

                            pIoGenericBfrDesc->ui32NumBuffers = 1;
                            pIoGenericBfrDesc->eBufferType = BDSP_AF_P_BufferType_eDRAM;
                            BMEM_ConvertAddressToOffset(hRaagaTest->hMem, pIoGenericBfr, &phyAddress);
                            pIoGenericBfrDesc->sCircBuffer.ui32BaseAddr = phyAddress;
                            pIoGenericBfrDesc->sCircBuffer.ui32ReadAddr = phyAddress;
                            pIoGenericBfrDesc->sCircBuffer.ui32WriteAddr = phyAddress;
                            pIoGenericBfrDesc->sCircBuffer.ui32WrapAddr = phyAddress+INTER_TASK_GENERIC_BUFFER_SIZE-4;
                            pIoGenericBfrDesc->sCircBuffer.ui32EndAddr = phyAddress+INTER_TASK_GENERIC_BUFFER_SIZE-4;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            BMEM_FlushCache(hRaagaTest->hMem, pIoGenericBfrDesc, sizeof(BDSP_AF_P_sIO_BUFFER));
#endif
                            
                            /* Fill the echo cancelling data into the intertask buffer */
                            fp = fopen (ECHO_CANCELLER_INPUT_FILE, "rb");
                            if (fp != NULL)
                            {
                                fread_bytes = fread (pIoBfr, 1, INTER_TASK_IO_BUFFER_SIZE, fp);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                                BMEM_FlushCache(hRaagaTest->hMem, pIoBfr, fread_bytes);
#endif

                                pIoBfrDesc->sCircBuffer[0].ui32WriteAddr += fread_bytes-8;
                                fclose (fp);
                            }
                            else
                            {
                                BDBG_LOG(("Cannot open echo cancelling input file : %s\n", ECHO_CANCELLER_INPUT_FILE));
                                BKNI_Memset(pIoBfr, 0 , INTER_TASK_IO_BUFFER_SIZE);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                                BMEM_FlushCache(hRaagaTest->hMem, pIoBfr, INTER_TASK_IO_BUFFER_SIZE);
#endif
                                pIoBfrDesc->sCircBuffer[0].ui32WriteAddr += INTER_TASK_IO_BUFFER_SIZE-8;
                            }
                            
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.ui32NumSrc = 2;
            				taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[1].eType = BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf;
                            BMEM_ConvertAddressToOffset(hRaagaTest->hMem, pIoBfrDesc, &phyAddress);
            				taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[1].uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr = phyAddress;
                            BMEM_ConvertAddressToOffset(hRaagaTest->hMem, pIoGenericBfrDesc, &phyAddress);
            				taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId-1].sStgConnectivity.sSrcDetails[1].uDetails.sInterTaskDramBuffCfg.ui32IoGenericBuffCfgAddr = phyAddress;
                        }
                        /*******************************************/
                    }

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].eStageType= BDSP_CIT_P_StageType_eEncode;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].ui32BranchId = encoderBranchId;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].ui32StageId = encoderStageId;  
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].uAlgorithm.eEncAudioAlgo= g_encoder_audio_type;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].uAudioMode.eEncAudioMode= BDSP_EncodeMode_eRealtime;		

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.ui32NumSrc = 1; 	
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                    
                    if (g_enable_echo_canceller)
                    {
                        if (bIsSpeechEncoder)
                        {
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = encoderStageId-1;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;
                        }
                        else
                        {
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = pcmOpStageIndex;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 1;
                        }
                    }
                    else
                    {
                        if (bIsSpeechEncoder)
                        {
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 1;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;
                        }
                        else
                        {
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
                            taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = pcmOpStageIndex;
        	                taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
        	                taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 1;
                        }
                    }

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.ui32NumDistinctOp = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.ui32NumDst[0] = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eCdb;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32StageId = encoderStageId+1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32DistinctOpNum = 0;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32OpNum = 0;
                    
                    /* Gen CDB ITB */
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].eStageType= BDSP_CIT_P_StageType_ePostProc;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].ui32BranchId = encoderBranchId;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].ui32StageId = encoderStageId+1;  
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].uAlgorithm.eEncAudioAlgo= BDSP_AudioProcessing_eGenCdbItb; 	  
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].uAudioMode.eEncAudioMode= BDSP_DSPCHN_ProcessingMode_ePP;		

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.ui32NumSrc = 1; 	
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = encoderBranchId;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = encoderStageId;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.ui32NumDistinctOp = 2;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.ui32NumDst[0] = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eCdb;

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.CDB_Base;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.CDB_End;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.CDB_Read;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.CDB_Valid;	  
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.CDB_Wrap;		 

                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.ui32NumDst[1] = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.eDistinctOpType[1] = BDSP_AF_P_DistinctOpType_eItb;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 1;
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
                    
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.ITB_Base;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.ITB_End;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.ITB_Read;	 
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.ITB_Valid;	  
                    taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap1.ITB_Wrap;
                    
                    
                    /* Setup capture for GenCDBITB outputs */
                    if(g_capture_encoder_out)
                    {
                        index = BRGA_TST_Capture_Add(&(taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer),
                                                     pCdbFile, 1, hRaagaTest->hReg);
                        if (index != (uint32_t)-1)
                        {
                        	BRGA_TST_Capture_Start(index);
                        }
                        index = BRGA_TST_Capture_Add(&(taskSettings.pBranchInfo[encoderBranchId]->sFwStgInfo[encoderStageId+1].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer),
                                                     pItbFile, 1, hRaagaTest->hReg);
                        if (index != (uint32_t)-1)
                        {
                        	BRGA_TST_Capture_Start(index);
	                    }
	                }
                }

                /* Add the PP stage here */
                if (g_pp_audio_type != BDSP_AudioProcessing_eMax)
                {
                    taskSettings.pBranchInfo[0]->ui32NumStages++;
                    pcmOpStageIndex++;
                    
                    /* Re-configure the previous stage */
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.ui32NumDistinctOp = 1;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.ui32NumDst[0] = 1;
                    
                    if (g_capture_decoder_multich)
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_e5_1_PCM;
                    }
                    else
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;
                    }

    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32StageId = pcmOpStageIndex;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32DistinctOpNum = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex-1].sStgConnectivity.sDstDetails[0][0].uDetails.sFwStage.ui32OpNum = 0;

    				/* PP Stage */
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].eStageType= BDSP_CIT_P_StageType_ePostProc;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].ui32StageId = pcmOpStageIndex;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].uAlgorithm.eProcAudioAlgo= g_pp_audio_type;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].uAudioMode.eProcessingAudioMode= BDSP_DSPCHN_ProcessingMode_ePP;

    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumSrc = 1;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFwStg;
    				
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32BranchId = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32StageId = pcmOpStageIndex-1;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32DistinctOpNum = 0;
    				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sSrcDetails[0].uDetails.sFwStage.ui32OpNum = 0;
                    
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDistinctOp = 1;
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.ui32NumDst[0] = 1; /* One to encoder and one to SRC/DAC */
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;
                }
                
                /* Configure the PCM output stage params */
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
                if (g_capture_decoder_multich)
                {
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 6;
                }
                else
                {
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
                }
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eFMM;
                
				/* Allocating buffer*/
				buff1 = (void *)BMEM_AllocAligned(hRaagaTest->hMem, (1024*1024),8,0);
                if(!buff1)
                {
                    err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
                }
				err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, buff1, &ui32PhyAddr);

                BREG_Write32(hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR, 
					        ui32PhyAddr);				

				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
							ui32PhyAddr);

				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR,
							ui32PhyAddr);

				/* program the end address */
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
				ui32RegVal = (BCHP_FIELD_DATA (
									AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
									RINGBUF_ENDADDR, 
									(ui32PhyAddr + (1024*1024) - 1))); 
#else
				ui32RegVal = (BCHP_FIELD_DATA (
									AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR, 
									RINGBUF_ENDADDR, 
									(ui32PhyAddr + (1024*1024) - 1))); 
#endif
				
				
				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR , 
							ui32RegVal);

				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, 
							ui32RegVal-256);

				buff2 = (void *)BMEM_AllocAligned(hRaagaTest->hMem, (1024*1024),8,0);
				if(!buff2)
				{
					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
				}

				err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, buff2, &ui32PhyAddr);
			    BREG_Write32(hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR, 
					        ui32PhyAddr);				

				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR, 
							ui32PhyAddr);

				BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR,
							ui32PhyAddr);

				/* program the end address */
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
				ui32RegVal = (BCHP_FIELD_DATA (
									AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR, 
									RINGBUF_ENDADDR, 
									(ui32PhyAddr + (1024*1024) - 1))); 
#else
                ui32RegVal = (BCHP_FIELD_DATA (
                                    AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_ENDADDR, 
                                    RINGBUF_ENDADDR, 
                                    (ui32PhyAddr + (1024*1024) - 1))); 
#endif


                BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR , 
                            ui32RegVal);

                BREG_Write32 (hRaagaTest->hReg,
							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT, 
                            ui32RegVal-256);
				
                taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[0] = 0;

                /* Allocate the buffers 2-5 and assign the pointers */
                if (g_capture_decoder_multich)
                {
                    uint32_t    ui32Offset;
                    
                    /* Modify the output port info */
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 6;
                    taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_e5_1_PCM;
                    
                    ui32Offset = BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR - BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR;
                    for (i = 2; i < 6; i++)
                    {
                        taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[i] = i;

        				buff2 = (void *)BMEM_AllocAligned(hRaagaTest->hMem, (1024*1024),8,0);
        				if(!buff2)
        				{
        					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
        				}

        				err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, buff2, &ui32PhyAddr);
        			    BREG_Write32(hRaagaTest->hReg,
        							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + ui32Offset*i, 
        					        ui32PhyAddr);				

        				BREG_Write32 (hRaagaTest->hReg,
        							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + ui32Offset*i, 
        							ui32PhyAddr);

        				BREG_Write32 (hRaagaTest->hReg,
        							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + ui32Offset*i,
        							ui32PhyAddr);

        				/* program the end address */
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR
        				ui32RegVal = (BCHP_FIELD_DATA (
        									AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
        									RINGBUF_ENDADDR, 
        									(ui32PhyAddr + (1024*1024) - 1))); 
#else
                        ui32RegVal = (BCHP_FIELD_DATA (
                                            AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR, 
                                            RINGBUF_ENDADDR, 
                                            (ui32PhyAddr + (1024*1024) - 1))); 
#endif

                        BREG_Write32 (hRaagaTest->hReg,
        							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + ui32Offset*i, 
                                    ui32RegVal);

                        BREG_Write32 (hRaagaTest->hReg,
        							BRAAGA_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT + ui32Offset*i, 
                                    ui32RegVal-256);
                    }
                }
                
                if (g_capture_decoder_stereo || g_capture_decoder_multich)
                {
                    uint32_t index;
                    char capture_path[500];
                    void *ptr;
                    
                    ptr = getenv("raaga_test_rbuf_capture_path");
                    if (NULL == ptr)
                    {
                        ptr = "./";
                    }
                    sprintf (&capture_path[0], "%s/rbuf_cap", (char *)ptr);
                    index = BRGA_TST_Capture_Add((BDSP_AF_P_sIO_BUFFER *)&(taskSettings.pBranchInfo[0]->sFwStgInfo[pcmOpStageIndex].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId),
                                 &capture_path[0], ((g_afap_mode == true) ? 1 : 0), hRaagaTest->hReg);
                    
                    if (index != (uint32_t)-1)
                    {
                    	BRGA_TST_Capture_Start(index);
                        if (g_afap_mode)
                        {
                            /* Disabling FMM consumption in AFAP mode */
                            BREG_Write32(hRaagaTest->hReg, BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE, 0x00002800);
                        }
                    }
                }
                
				BDBG_LOG(("BRAAGA_TEST_OpenRaagaTask: eDecAudioAlgo=%d",taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo));
				err = BDSP_Task_Create(hRaagaTestCh->hDspcxt,&taskSettings,&task);
				if(err != BERR_SUCCESS)
				{
					BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: BDSP_Task_Create failed")); 
					BDBG_ASSERT( !err );
				}
                
                /* Set the user configuration to the Speech decoders here */
                if (taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp == 2)
                {
                    void *userConfig;
                    uint32_t    size, i;
                    
                    BERR_Code errCode=0;
                    BDSP_Raaga_Audio_G726ConfigParams       g711Settings;
                    BDSP_Raaga_Audio_G729DecConfigParams    g729Settings;
                    BDSP_Raaga_Audio_G723_1DEC_ConfigParams g723Settings;
                    BDSP_Raaga_Audio_PcmWavConfigParams     pcmwavSettings;
                    
                    switch (hRaagaTest->sRaagaStrParams[0].eAlgoType)
                    {
                        case BDSP_AudioType_eG711G726:
                            errCode = BDSP_Task_GetStageSettings(task, 0, 0, &g711Settings, sizeof(g711Settings));
                            size = sizeof(g711Settings);
                            userConfig = (void *)&g711Settings;
                            
                            g711Settings.ui32NumOutPorts = 2;
                            g711Settings.sUsrOutputCfg[1].ui32OutMode = 1;
                            g711Settings.sUsrOutputCfg[1].ui32ApplyGain = 0;
                            for (i = 0; i < BDSP_AF_P_MAX_CHANNELS; i++)
                                g711Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
                            g711Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[0] = CENTER_CHANNEL_INDEX;
                            g711Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[1] = CENTER_CHANNEL_INDEX;
                            break;
                        case BDSP_AudioType_eG729:
                            errCode = BDSP_Task_GetStageSettings(task, 0, 0, &g729Settings, sizeof(g729Settings));
                            size = sizeof(g729Settings);
                            userConfig = (void *)&g729Settings;
                            
                            g729Settings.ui32NumOutPorts = 2;
                            g729Settings.sUsrOutputCfg[1].ui32ScaleOp = 0;
                            g729Settings.sUsrOutputCfg[1].ui32OutMode = 1;
                            for (i = 0; i < BDSP_AF_P_MAX_CHANNELS; i++)
                                g729Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
                            g729Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[0] = CENTER_CHANNEL_INDEX;
                            g729Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[1] = CENTER_CHANNEL_INDEX;
                            break;
                        case BDSP_AudioType_eG723_1:
                            errCode = BDSP_Task_GetStageSettings(task, 0, 0, &g723Settings, sizeof(g723Settings));
                            size = sizeof(g723Settings);
                            userConfig = (void *)&g723Settings;
                            
                            g723Settings.ui32NumOutPorts = 2;
                            g723Settings.sUsrOutputCfg[1].ui32ScaleOp = 0;
                            g723Settings.sUsrOutputCfg[1].ui32OutMode = 1;
                            for (i = 0; i < BDSP_AF_P_MAX_CHANNELS; i++)
                                g723Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[i] = 0xFFFFFFFF;
                            g723Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[0] = CENTER_CHANNEL_INDEX;
                            g723Settings.sUsrOutputCfg[1].ui32OutputChannelMatrix[1] = CENTER_CHANNEL_INDEX;
                            break;
                        case BDSP_AudioType_ePcmWav:
                            errCode = BDSP_Task_GetStageSettings(task, 0, 0, &pcmwavSettings, sizeof(pcmwavSettings));
                            size = sizeof(pcmwavSettings);
                            userConfig = (void *)&pcmwavSettings;
                            
                            pcmwavSettings.ui32NumOutputPorts = 2;
                            for (i = 0; i < BDSP_AF_P_MAX_CHANNELS; i++)
                                pcmwavSettings.sUserOutputCfg[1].ui32OutputChannelMatrix[i] = 0xFFFFFFFF;

                            pcmwavSettings.sUserOutputCfg[1].i32OutMode = 1;
                            pcmwavSettings.sUserOutputCfg[1].ui32OutputChannelMatrix[0] = CENTER_CHANNEL_INDEX;
                            pcmwavSettings.sUserOutputCfg[1].ui32OutputChannelMatrix[1] = CENTER_CHANNEL_INDEX;
                            break;
                        default:
                            size = 0;
                            break;
                    }

                    if ( errCode )
                    {
                        return BERR_TRACE(errCode);
                    }

                    if (size)
                    {
                        errCode = BDSP_Task_SetStageSettings(task, 0, 0, userConfig, size);
                        if ( errCode )
                        {
                            return BERR_TRACE(errCode);
                        }
                    }
                }
                else
                {
                    uint32_t    file_size, usercfg_size;
                    void        *pUserCfgBuf;
                    BDSP_AF_P_AlgoId eAlgoId;
                    
                    FILE        *fp;
                    void        *ptr;
                    
                    /* Check and set the decoder stage user config params */
                    ptr = getenv("raaga_test_dec_user_cfg_file");
                    if (NULL == ptr)
                    {
                        ptr = DEC_USER_CFG_FILE;
                    }
                    
                    fp = fopen (ptr, "rb");
                    if (fp !=  NULL)
                    {
                        eAlgoId = BDSP_sDecAlgoExecInfo[hRaagaTest->sRaagaStrParams[0].eAlgoType][0].eAlgoIds[1];
                        usercfg_size = BDSP_sNodeInfo[eAlgoId].ui32UserCfgBuffSize;
                        
                        fseek (fp, 0, SEEK_END);
                        file_size = ftell(fp);
                        fseek (fp, 0, SEEK_SET);
                       
                        if (file_size >= usercfg_size)
                        {
                            pUserCfgBuf = BKNI_Malloc(usercfg_size);
                            fread(pUserCfgBuf, 1, usercfg_size, fp);
                            BDSP_Task_SetStageSettings(task, 0, 0, pUserCfgBuf, usercfg_size);
                            BKNI_Free(pUserCfgBuf);
                        }
                        
                        fclose (fp);
                    }
                    
                    /* Check and set the post proc stage user config params */
                    ptr = getenv("raaga_test_pp_user_cfg_file");
                    if (NULL == ptr)
                    {
                        ptr = PP_USER_CFG_FILE;
                    }
                    
                    fp = fopen (ptr, "rb");
                    if ((fp != NULL) && (g_pp_audio_type != BDSP_AudioProcessing_eMax))
                    {
                        eAlgoId = BDSP_sPpAlgoExecInfo[hRaagaTest->sRaagaStrParams[0].eAlgoType][0].eAlgoIds[1];
                        usercfg_size = BDSP_sNodeInfo[eAlgoId].ui32UserCfgBuffSize;

                        fseek (fp, 0, SEEK_END);
                        file_size = ftell(fp);
                        fseek (fp, 0, SEEK_SET);
                        
                        if (file_size >= usercfg_size)
                        {
                            pUserCfgBuf = BKNI_Malloc(usercfg_size);
                            fread(pUserCfgBuf, 1, usercfg_size, fp);
                            BDSP_Task_SetStageSettings(task, 0, pcmOpStageIndex, pUserCfgBuf, usercfg_size);
                            BKNI_Free(pUserCfgBuf);
                        }
                        
                        fclose (fp);
                    }

                    /* Check and set the encoder stage user config params */
                    ptr = getenv("raaga_test_enc_user_cfg_file");
                    if (NULL == ptr)
                    {
                        ptr = ENC_USER_CFG_FILE;
                    }
                    
                    fp = fopen (ptr, "rb");
                    if ((fp != NULL) && (g_encoder_audio_type != BDSP_AudioEncode_eMax))
                    {
                        eAlgoId = BDSP_sEncAlgoExecInfo[hRaagaTest->sRaagaStrParams[0].eAlgoType][0].eAlgoIds[1];
                        usercfg_size = BDSP_sNodeInfo[eAlgoId].ui32UserCfgBuffSize;

                        fseek (fp, 0, SEEK_END);
                        file_size = ftell(fp);
                        fseek (fp, 0, SEEK_SET);
                        
                        if (file_size >= usercfg_size)
                        {
                            pUserCfgBuf = BKNI_Malloc(usercfg_size);
                            fread(pUserCfgBuf, 1, usercfg_size, fp);
                            BDSP_Task_SetStageSettings(task, encoderBranchId, encoderStageId, pUserCfgBuf, usercfg_size);
                            BKNI_Free(pUserCfgBuf);
                        }
                        
                        fclose (fp);
                    }
                }
            
                /* Disable Target Sync for Datasync */
                {
                    BDSP_AudioTaskDatasyncSettings datasyncSettings;
                    BERR_Code errCode;

                    errCode = BDSP_AudioTask_GetDatasyncSettings(task, 0, 0, &datasyncSettings);
                    if ( errCode )
                    {
                        BDBG_ERR(("BDSP_AudioTask_GetDatasyncSettings: Failed with error %d", errCode));
                        return BERR_TRACE(errCode);
                    }

                    datasyncSettings.eEnableTargetSync = BDSP_AF_P_eDisable;
                    errCode = BDSP_AudioTask_SetDatasyncSettings(task, 0, 0, &datasyncSettings);
                    if ( errCode )
                    {
                        BDBG_ERR(("BDSP_AudioTask_SetDatasyncSettings: Failed with error %d", errCode));
                        return BERR_TRACE(errCode);
                    }
                }
                
				hRaagaTaskHandle->task = task;
				hRaagaTaskHandle->bused = true;
				break;
			}

			if(eChTaskType == BRAAGA_TEST_CxtType_eVideo)
            {
				BDSP_Task_GetDefaultCreateSettings(hRaagaTestCh->hDspcxt,&taskSettings);
				taskSettings.dspIndex =0 ;
				taskSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
				taskSettings.numBranches =1;
				taskSettings.pBranchInfo[0] =(BDSP_CIT_P_FwBranchInfo *)BKNI_Malloc(
												 sizeof(BDSP_CIT_P_FwBranchInfo));
				BKNI_Memset(taskSettings.pBranchInfo[0], 0 , sizeof(BDSP_CIT_P_FwBranchInfo));

				taskSettings.psVDecodeBuffCfg =(BDSP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(
												 sizeof(BDSP_VF_P_sVDecodeBuffCfg));
				BKNI_Memset(taskSettings.psVDecodeBuffCfg, 0 , sizeof(BDSP_VF_P_sVDecodeBuffCfg));
                
				taskSettings.pBranchInfo[0]->ui32NumStages = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].eStageType= BDSP_CIT_P_StageType_eVideoDecode;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32BranchId = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32StageId = 0;  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo= BDSP_VideoType_eVP6;		  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode = BDSP_DecodeMode_eDecode;		  
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Base;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_End;	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Read;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Valid;	  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.CDB_Wrap;		 
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Base;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_End;	
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Read;	 
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Valid;	  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = 0x10000000 + hRaagaTaskHandle->CtxMap.ITB_Wrap;  
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[0] = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eInvalid;
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 2;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eFMM;

                /* Video Memory allocation*/
                uiTempSize = (BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE+BDSP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE)
                                *BDSP_FWMAX_VIDEO_BUFF_AVAIL                     
                                +(BDSP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE+BDSP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE)
                                *BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;

                /* Video FW requires the buffers to be 256 byte alligned for DMA */
                ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem,uiTempSize, 8, 0);
                if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
                {
                    BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for SPDIF Channel status config buffer!"));
                    return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                }
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                /* Converting all memory accesses to cached */
                BMEM_ConvertAddressToCached(hRaagaTest->hMem, (void *)ui32BaseAddr, &pCached);
                ui32BaseAddr = (uint32_t)pCached;
#endif

                ui32UsedSize =0;
                /*sLumaFrameBuffParams and sChromaFrameBuffParams*/
                taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32NumBuffAvl = BDSP_FWMAX_VIDEO_BUFF_AVAIL;
                for(i=0; i<BDSP_FWMAX_VIDEO_BUFF_AVAIL ; i++)
                {
                    /* Allocate the UPB handle */
                    hXDMPicture = (BXDM_Picture *)BMEM_AllocAligned(hRaagaTest->hMem,(sizeof(BXDM_Picture)), 2, 0);
                    if(hXDMPicture == NULL)
                    {
                    	return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                    }
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                    /* Converting all memory accesses to cached */
                    BMEM_ConvertAddressToCached(hRaagaTest->hMem, (void *)hXDMPicture, &pCached);
                    hXDMPicture = pCached;
#endif

                    /* Reset the entire structure */
                    BKNI_Memset(hXDMPicture, 0, sizeof(BXDM_Picture));
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                    BMEM_FlushCache(hRaagaTest->hMem, hXDMPicture, sizeof(BXDM_Picture));
#endif

                    hXDMPicture->stBufferInfo.hHeap = hRaagaTest->hMem;
                    hXDMPicture->stBufferInfo.pLumaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
                        = (uint32_t)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes
                        = BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;
                    ui32UsedSize += BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;            

                    hXDMPicture->stBufferInfo.pChromaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                    BMEM_FlushCache(hRaagaTest->hMem, hXDMPicture, sizeof(BXDM_Picture));
#endif

                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress
                        = (uint32_t)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes
                        = BDSP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE;
                    ui32UsedSize += BDSP_REALVIDEO_DECODE_CHROMAFRAME_BUF_SIZE;

                    taskSettings.psVDecodeBuffCfg->sUPBs[i].ui32DramBufferAddress = (uint32_t)hXDMPicture;
                    taskSettings.psVDecodeBuffCfg->sUPBs[i].ui32BufferSizeInBytes= sizeof(BXDM_Picture);                   
                }

                taskSettings.psVDecodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl = BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;
                for(i=0; i<BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL ; i++)
                {
                    taskSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
                        = (uint32_t)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes
                        = BDSP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE;                           
                    ui32UsedSize += BDSP_REALVIDEO_DECODE_REF_LUMAFRAME_BUF_SIZE;

                    taskSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress
                        = (uint32_t)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes
                        = BDSP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE;                           
                    ui32UsedSize += BDSP_REALVIDEO_DECODE_REF_CHROMAFRAME_BUF_SIZE;
                }
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[0] = 0;
				BDBG_LOG(("BRAAGA_TEST_OpenRaagaTask: eVideoAlgo=%d",taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo));
				err = BDSP_Task_Create(hRaagaTestCh->hDspcxt,&taskSettings,&task);
				if(err != BERR_SUCCESS)
				{
					BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: BDSP_Task_Create failed")); 
					BDBG_ASSERT( !err );
				}
				hRaagaTaskHandle->task = task;
				hRaagaTaskHandle->bused = true;
				break;
			}            
		}
	}
	BDBG_LOG(("hTaskHandle=%x",hTaskHandle));
		
	return err;
}

/*******************************************************************************
Summary: Close required Raaga Task handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_CloseRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_ChHandle	hRaagaTestCh,
	BRAAGA_TEST_TaskHandle	hTaskHandle
)
{
    BERR_Code	err = BERR_SUCCESS;
	int			i=0;
	
	BDBG_ASSERT(hTaskHandle);
	BDBG_ASSERT(hRaagaTestCh);
	BDBG_ASSERT(hRaagaTest);

	for(i=0; i<RAAGA_TEST_MAX_TASK_PER_CONTEXT; ++i)
	{
		if((hRaagaTestCh->hTaskInfo[i])&&
		   (hRaagaTestCh->hTaskInfo[i] == hTaskHandle))	
		{
		   if(hRaagaTest->bXptPbEn == true)			
		   {
#if RAAGA_TEST_ENABLE_TRANSPORT		
			if(hTaskHandle->hCtx)
			{
				err = BRAAGA_TEST_CloseRaveContext(hTaskHandle,hRaagaTest->hPcr);
			}
#endif      
		   	}
			BDSP_Task_Destroy(hTaskHandle->task);
			BKNI_Free(hTaskHandle);
/*			
			hRaagaTestCh->hTaskInfo[i] = NULL;
*/
		}
	}
	return err;
}

/*******************************************************************************
Summary: Stop required Raaga Task handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_StopRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	hTaskHandle
)
{
    BERR_Code               	err = BERR_SUCCESS;

	BDBG_ASSERT(hTaskHandle);
	BDBG_ASSERT(hRaagaTest);

	hTaskHandle->bRunning = false;
	if(hRaagaTest->bXptPbEn == true)			
	{
#if RAAGA_TEST_ENABLE_TRANSPORT   
	if(hTaskHandle->hCtx)
	{
		if(hRaagaTest->bFilePb)
		{
			err = BRAAGA_TEST_PbLib_StopPlay(hRaagaTest->hPbLib);
		}
		err = BRAAGA_TEST_StopRaveContext(hRaagaTest,hTaskHandle);
	}
#endif    
	}
	err = BDSP_Task_Stop(hTaskHandle->task);
	return err;
}

/*******************************************************************************
Summary: Start required Raaga Task handles.
*******************************************************************************/
BERR_Code BRAAGA_TEST_StartRaagaTask(
	BRAAGA_TEST_Handle		hRaagaTest,
	BRAAGA_TEST_TaskHandle	hTaskHandle,
	BRAAGA_TEST_CxtType		eChTaskType,    
    unsigned int			uiStreamId,
	BRAAGA_TEST_TaskDstType	eDstType,
	bool					bXptPbEn
)
{
    BERR_Code               	err = BERR_SUCCESS;
	unsigned int            	i=0;
    BRAAGA_TEST_ChHandle    	hRaagaTestCh = NULL;

	BDBG_ASSERT(hRaagaTest);
	BDBG_ASSERT(hTaskHandle);
	
	for(i=0; i < RAAGA_MAX_CONTEXT; i++)
	{
		
		if(hRaagaTest->hRaagaTestCh[i] != NULL)
		{
			if(hRaagaTest->hRaagaTestCh[i]->sSettings.eChTaskType == eChTaskType)
			{
				hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
				if((hTaskHandle != NULL)&&
				   (hTaskHandle->task != NULL)&&
				   (hTaskHandle->uiStreamId == uiStreamId)&&
				   (hTaskHandle->eDstType = eDstType)&&
				   (hTaskHandle->bused == true))
				{
					hTaskHandle->bXptPbEn = bXptPbEn;
					if(hRaagaTest->bXptPbEn == true)			
		   			{
#if RAAGA_TEST_ENABLE_TRANSPORT                    
					err = BRAAGA_TEST_StartRaveContext(hRaagaTest, hRaagaTestCh, hTaskHandle, hRaagaTest->hPcr);
					BDBG_ASSERT( !err );
#endif              
					}
					err = BDSP_Task_Start(hTaskHandle->task);
					if(err != BERR_SUCCESS)
					{
						BDBG_ERR(("BRAAGA_TEST_StartRaagaTask: BDSP_Task_Start failed")); 
						BDBG_ASSERT( !err );
					}
					hTaskHandle->bRunning = true;
					break;
				}
			}
		}
	}
	if(eDstType == BRAAGA_TEST_TaskDstType_eRaveBuf)
	{
#if (RAAGA_TEST_ENABLE_TRANSPORT==1)
		/* Start Rave Context */
		/* Start the playback if XPT Playback is enabled */
		if((hRaagaTest->bXptPbEn == true)
            && (hRaagaTest->bFilePb == true))
		{
			
		    switch(hRaagaTest->sRaagaStrParams[uiStreamId].eStreamType)
		    {
	            case BAVC_StreamType_eEs:
	           	    /* ES Playback */
	                BRAAGA_TEST_PbLib_DoEsPlayNonBlocking( 
	                        hRaagaTest->hPbLib, 
	                        hRaagaTest->sRaagaStrParams[uiStreamId].aStreamName);
	                break;
	            case BAVC_StreamType_eTsMpeg:
	                /* TS Playback */
                    if (hRaagaTest->eTestmode == BRAAGA_TEST_Mode_ArgCmd)
                    {
    	                BRAAGA_TEST_PbLib_DoPlay( 
    	                        hRaagaTest->hPbLib, 
    	                        hRaagaTest->sRaagaStrParams[uiStreamId].aStreamName, 
    	                        BXPT_PB_SYNC_MPEG_BLIND
    	                        );
                    }
                    else
                    {
    	                BRAAGA_TEST_PbLib_DoPlayNonBlocking( 
    	                        hRaagaTest->hPbLib, 
    	                        hRaagaTest->sRaagaStrParams[uiStreamId].aStreamName, 
    	                        BXPT_PB_SYNC_MPEG_BLIND
    	                        );
                    }
	                break;
	            default:
	                /* XPT playback of other formats are not supported yet */
	                return BERR_NOT_SUPPORTED;
		    }
		}
#endif
	}
    return err;
}

/****************************************************************************
Summary: Add or Remove input source to a channel, default is XPT.
*****************************************************************************/
BERR_Code BRAAGA_TEST_WatchdogEventandThreadCreate(
    BRAAGA_TEST_Handle    hRaagaTest
)
{
    BERR_Code                   err = BERR_SUCCESS;

    /* Create Event for hanlding SPDIF RX interrupts */
	err = BKNI_CreateEvent(&hRaagaTest->watchdog_event);
    BDBG_ASSERT(err == BERR_SUCCESS);
    hRaagaTest->watchdog_thread =(pthread_t *)BKNI_Malloc(sizeof(pthread_t));

    err = pthread_create(
            hRaagaTest->watchdog_thread, 
            NULL,
            BRAAGA_TEST_WatchdogThread,
            &hRaagaTest);    
    BDBG_ASSERT(err == BERR_SUCCESS);

	/*TO DO */
#if 0
	/* Installing watchdog callback */
    err = BDSP_InstallDeviceLevelAppInterruptCb(
                hRaagaTest->hDsp,
                BRAAGA_DeviceLevelInterrupt_eWatchdog,
                (BRAAGA_CallbackFunc)BRAAGA_TEST_WatchdogHandler,
                (void *)hRaagaTest,
                0);

	BDBG_ASSERT(err == BERR_SUCCESS);    	
    BDBG_LOG (("Watchdog callback installed"));
#endif
    return err;    
}


/****************************************************************************
Summary: Helper function to test Watchdog
*****************************************************************************/

void * BRAAGA_TEST_WatchdogThread(
    void *iParam1
    )
{
    BERR_Code           err = BERR_SUCCESS;
    unsigned int        num =0, i=0, j=0;
    BRAAGA_TEST_Handle    hRaagaTest = hGlobalRaagaTestHandle;

#if RAAGA_TEST_STATIC_CDB_ITB    
    BSTD_UNUSED (num);
    BSTD_UNUSED (i);
    BSTD_UNUSED (j);    
    BSTD_UNUSED (hRaagaTest);
#endif

    BSTD_UNUSED (iParam1);

    while (1) /* Always be ready to process a Watchdog */
    {
        err = BKNI_WaitForEvent (hRaagaTest->watchdog_event, -1);
        
        if(BERR_TIMEOUT == err)
        {        
            printf ("\n The Watchdog Event Timeout Out");
            return NULL;
        }    

        BDBG_ERR (("\nIn Watchdog Thread -> Calling Process WD INT !!!\n"));
    	/* Disable and Flush all open rave contexts */
#if RAAGA_TEST_ENABLE_TRANSPORT    
		for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
		{
			if(hRaagaTest->hRaagaTestCh[i])
			{
				for(j=0; j<RAAGA_TEST_MAX_TASK_PER_CONTEXT; ++j)
				{
					if((hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]) &&
					   (hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->task))
					{
						if(hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->eDstType == 
								BRAAGA_TEST_TaskDstType_eRaveBuf)
						{
							BXPT_Rave_DisableContext(hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->hCtx);
							BXPT_Rave_FlushContext(hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->hCtx);						
							BDBG_ASSERT(err == BERR_SUCCESS);
						}
					}
				}
			}
		}
#endif  
		/* TO DO */
		/* Add watchdog processing calls of DSP */
#if 0
    	/* Call raaga to process the watchdog interrupt */
    	err = BRAAGA_ProcessWatchdogInterrupt ( );
    	BDBG_ASSERT( !err );
    	BDBG_MSG(("\nWatchdog Interrupt Processed!!!"));
#endif  
#if RAAGA_TEST_ENABLE_TRANSPORT    
		for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
		{
			if(hRaagaTest->hRaagaTestCh[i])
			{
				for(j=0; j<RAAGA_TEST_MAX_TASK_PER_CONTEXT; ++j)
				{
					if((hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]) &&
					   (hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->task))
					{
						if(hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->eDstType == 
								BRAAGA_TEST_TaskDstType_eRaveBuf)
						{
							BXPT_Rave_EnableContext(hRaagaTest->hRaagaTestCh[i]->hTaskInfo[j]->hCtx);
							BDBG_ASSERT(err == BERR_SUCCESS);
							BDBG_MSG(("\nXPT Context %d enabled", num));
						}
					}
				}
			}
		}
#endif
        BDBG_ASSERT(err == BERR_SUCCESS);
    }
    return NULL;
}


/****************************************************************************
Summary: Loads Static CDB/ITB from File.
*****************************************************************************/
#if (RAAGA_TEST_STATIC_CDB_ITB == 1 )
void BRAAGA_TEST_LoadStaticCdbItb( 
    BRAAGA_TEST_Handle    hRaagaTest,
    BMEM_Handle         hMemory,
    BREG_Handle         hReg,
	BRAAGA_TEST_CxtType		eChTaskType      
    )
{
#if (RAAGA_TEST_STATIC_CDB_ITB_BINARYFILE == 1)
{
    BERR_Code   err = BERR_SUCCESS;    
    FILE        *pCdbFile = NULL;
    FILE        *pItbFile = NULL;
    uint32_t    ui32ByteCount = 0;
	uint32_t	data = 0;
    uint32_t    uiFrameOffset = 0;
    uint32_t    cdb_offset = 0, j = 0, itb_offset = 0;
    uint32_t    uiCdbSize,file_start,file_end;
    uint32_t    uiItbSize;    
    uint32_t    *pCdbMemPtr = NULL;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    void        *pCached;
#endif
    
#if 0
    uint32_t    *pItbMemPtr = NULL;
#endif

    uint32_t    *pCdbMemPtr1 = NULL;
    uint32_t    *pItbMemPtr1 = NULL;

#if 0
    uint32_t    i=0, key, val, base_address=0;
#endif

    BDBG_ASSERT(hMemory);
    BDBG_ASSERT(hReg);

	pCdbFile = fopen(hRaagaTest->sRaagaStrParams[eChTaskType].aCdbFileName, "rb");    
    BDBG_ASSERT(pCdbFile);
    file_start = ftell(pCdbFile);
    fseek(pCdbFile,0,SEEK_END);
    file_end   = ftell(pCdbFile);
    fseek(pCdbFile,0,SEEK_SET);


    uiCdbSize = file_end - file_start + 256; 

    pItbFile = fopen(hRaagaTest->sRaagaStrParams[eChTaskType].aItbFileName, "rb");
    BDBG_ASSERT(pItbFile);

    file_start = ftell(pItbFile);
    fseek(pItbFile,0,SEEK_END);
    file_end   = ftell(pItbFile);
    fseek(pItbFile,0,SEEK_SET);

    uiItbSize = file_end - file_start; 

    BDBG_LOG(("CDB Size = %d, ITB size = %d", 
               uiCdbSize, uiItbSize));

    /* Allocate CDB memory */
    pCdbMemPtr = BMEM_AllocAligned(hMemory, uiCdbSize,
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;

    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pCdbMemPtr, &cdb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);
    
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    /* Converting all memory accesses to cached */
    BMEM_ConvertAddressToCached(hMemory, pCdbMemPtr, &pCached);
    *(char *)pCached = 0;
    pCdbMemPtr = pCached;
#endif

    printf("\nLoadStaticCdbItb: cdb_offset = 0x%x", cdb_offset);

#if 0
    /* Allocate DRAM for ITB */
    pItbMemPtr = BMEM_AllocAligned(hMemory, uiItbSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;
    
    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pItbMemPtr, &itb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    /* Converting all memory accesses to cached */
    BMEM_ConvertAddressToCached(hMemory, pItbMemPtr, &pCached);
    *(char *)pCached = 0;
    pItbMemPtr = pCached;
#endif

    BDBG_MSG(("LoadStaticCdbItb: itb_offset = 0x%x\n", itb_offset));
#endif 
    /* Load CDB and ITB data to DRAM */
    
	printf ("\nLoading CDB Values....\n");

    for(ui32ByteCount = 0, j=0;
        ui32ByteCount <uiCdbSize; 
        ui32ByteCount = ui32ByteCount+4, j++)
    {
        data = (uint32_t)(((fgetc(pCdbFile))|((fgetc(pCdbFile)<<8)) | ((fgetc(pCdbFile)<<16)) | ((fgetc(pCdbFile) << 24))));
		*((volatile uint32_t *)(pCdbMemPtr)+j) = data;        
    }
	BDBG_MSG(("Checking Loaded CDB Values Till 20"));

#ifdef ENABLE_CACHED_ACCESS_CHANGES
    BMEM_FlushCache(hMemory, pCdbMemPtr, uiCdbSize);
#endif

	for(ui32ByteCount = uiFrameOffset; ui32ByteCount < 20+uiFrameOffset; ui32ByteCount = ui32ByteCount+4)
	{

		data = BRAAGA_P_DRAMREAD((uint32_t)pCdbMemPtr+ui32ByteCount);

	    BDBG_MSG(("CDB VALUE READ at address %x is %x", (uint32_t)pCdbMemPtr+ui32ByteCount, data));
	}

#if 0
	BDBG_MSG(("Checking Loaded ITB Values Till 20"));

	for(ui32ByteCount = 0; ui32ByteCount < 20; ui32ByteCount = ui32ByteCount+4)
	{
        data = BRAAGA_P_DRAMREAD((uint32_t)pItbMemPtr+ui32ByteCount);
		BDBG_MSG(("ITB VALUE READ at address %x is %x", (uint32_t)pItbMemPtr+ui32ByteCount, data));
	}
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    BMEM_FlushCache(hMemory, pItbMemPtr, 20);
#endif

#endif 

    if(eChTaskType == BRAAGA_TEST_CxtType_eAudio)
    {
        /* Program the Registers accordingly - CDB and ITB for RAVE Context 0 */
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR, cdb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR, cdb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR,
                           cdb_offset + uiCdbSize -1 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR,
                           cdb_offset + uiCdbSize - 1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_DEPTH,0x00000000 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR, 
                           cdb_offset + uiCdbSize -1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR,
                           cdb_offset + uiCdbSize -1 );

        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR, itb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR, itb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR,
                           itb_offset + uiItbSize -1   );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_WRAPAROUND_PTR, 
                           itb_offset + uiItbSize - 1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_DEPTH,0x00000000 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_WRITE_PTR,
                           itb_offset + uiItbSize -1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR,
                           itb_offset + uiItbSize -1 ); 
                           
        if (g_encoder_audio_type != BDSP_AudioEncode_eMax)
        {
    		/* Allocate CDB memory */
    		uiCdbSize = 2*1024*1024;
    	    pCdbMemPtr1= BMEM_AllocAligned(hMemory, uiCdbSize,
                    			8, /* uiAlignBits, */
                    			0 /* uiBoundary */
                    			) ;
    		
    		err = BMEM_ConvertAddressToOffset(hMemory, (void *)pCdbMemPtr1, &cdb_offset);
    	    BDBG_ASSERT(err==BERR_SUCCESS);

#ifdef ENABLE_CACHED_ACCESS_CHANGES
            /* Converting all memory accesses to cached */
            BMEM_ConvertAddressToCached(hMemory, pCdbMemPtr1, &pCached);
            *(char *)pCached = 0;
            pCdbMemPtr1 = pCached;
#endif
            
    		/* Allocate DRAM for ITB */
    		uiItbSize = 1*1024*1024;
    	    pItbMemPtr1= BMEM_AllocAligned(hMemory, uiItbSize, 
    	                			8, /* uiAlignBits, */
    	                			0 /* uiBoundary */
    	                			) ;
    	    
    	    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pItbMemPtr1, &itb_offset);
    	    BDBG_ASSERT(err==BERR_SUCCESS);
            
#ifdef ENABLE_CACHED_ACCESS_CHANGES
            /* Converting all memory accesses to cached */
            BMEM_ConvertAddressToCached(hMemory, pItbMemPtr1, &pCached);
            *(char *)pCached = 0;
            pItbMemPtr1 = pCached;
#endif

            /* Program the Registers accordingly - CDB and ITB for RAVE Context 0 */
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_READ_PTR, cdb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_BASE_PTR, cdb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_END_PTR, cdb_offset + uiCdbSize - 1);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_WRAPAROUND_PTR, cdb_offset + uiCdbSize - 1);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_DEPTH,0x00000000 );
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_WRITE_PTR, cdb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_VALID_PTR, cdb_offset);

            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_READ_PTR, itb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_BASE_PTR, itb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_END_PTR, itb_offset + uiItbSize - 1);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_WRAPAROUND_PTR, itb_offset + uiItbSize - 1);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_DEPTH,0x00000000 );
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_WRITE_PTR, itb_offset);
            BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_VALID_PTR, itb_offset); 
        }
    }

    if(eChTaskType == BRAAGA_TEST_CxtType_eVideo)
    { 
        /* Program the Registers accordingly - CDB and ITB for RAVE Context 0 */
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_READ_PTR, cdb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_BASE_PTR, cdb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_END_PTR,
                           cdb_offset + uiCdbSize -1 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_WRAPAROUND_PTR,
                           cdb_offset + uiCdbSize - 1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_DEPTH,0x00000000 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_WRITE_PTR, 
                           cdb_offset + uiCdbSize -1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_CDB_VALID_PTR,
                           cdb_offset + uiCdbSize -1 );

        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_READ_PTR, itb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_BASE_PTR, itb_offset);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_END_PTR,
                           itb_offset + uiItbSize -1   );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_WRAPAROUND_PTR, 
                           itb_offset + uiItbSize - 1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_DEPTH,0x00000000 );
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_WRITE_PTR,
                           itb_offset + uiItbSize -1);
        BREG_Write32(hReg, BCHP_XPT_RAVE_CX1_AV_ITB_VALID_PTR,
                           itb_offset + uiItbSize -1 );           
    }
        
}
#else
    BERR_Code   err = BERR_SUCCESS;    
    uint32_t    *pCdbMemPtr = NULL;
    uint32_t    *pItbMemPtr = NULL;
    uint32_t    ui32ByteCount = 0;
	uint32_t	data = 0;
/*  uint32_t    uiCdbSize = 0x00505000 - 0x00405000; */
/*  uint32_t    uiItbSize = 0x00505ff0 - 0x00505000; */
    uint32_t    uiCdbSize = cdb_array_size;
    uint32_t    uiItbSize = itb_array_size;
    uint32_t    uiFrameOffset = 0;
    uint32_t    cdb_offset = 0, i=0, key, val, base_address=0, itb_offset = 0;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    void        *pCached;
#endif

    pCdbMemPtr = BMEM_AllocAligned(hMemory, uiCdbSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;

    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pCdbMemPtr, &cdb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);

#ifdef ENABLE_CACHED_ACCESS_CHANGES
    /* Converting all memory accesses to cached */
    BMEM_ConvertAddressToCached(hMemory, pCdbMemPtr, &pCached);
    *(char *)pCached = 0;
    pCdbMemPtr = pCached;
#endif
    
    printf("\nLoadStaticCdbItb: cdb_offset = 0x%x", cdb_offset);
	printf ("\nLoading CDB Values....\n");
    /* Write the CDB */
    for(ui32ByteCount = 0; ui32ByteCount < uiCdbSize; ui32ByteCount = ui32ByteCount+4)
    {
#if 1 /* check if the endian ness is correct */
        BRAAGA_P_DRAMWRITE((uint32_t)pCdbMemPtr + ui32ByteCount, cdb_array[ui32ByteCount/4]);
#else
        data = cdb_array[ui32ByteCount/4];
		data = ((data&0x0000ffff) << 16)|((data&0xffff0000)>>16);
		data = (data&0xffff0000) | ((data&0x0000ff00)>>8) | ((data&0x000000ff)<<8);
		data = (data&0x0000ffff) | ((data&0xff000000)>>8) | ((data&0x00ff0000)<<8);

        BRAAGA_P_DRAMWRITE(g_hRegister, (uint32_t)pCdbMemPtr + ui32ByteCount, data);
#endif
    }
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    BMEM_FlushCache(hMemory, pCdbMemPtr, uiCdbSize);
#endif

    /* Remapping the static ITB entries:
       Note: 
       Static CDB start address: 0x00405000 : dynamically allocated address will be different
       First Audio Frame start addr in CDB: 0x00405376: Constant offset 0x376
     */
    BDBG_MSG(("\n")); 
    for (i = 0; i < uiItbSize/4; i+=4) 
    {
        key = itb_array[i];
        val = itb_array[i+1];
        if ((key & 0xFF000000) == 0x20000000) 
        {
            /* We found a CDB Base Address Key, so
             * re-map the address */
            if (base_address == 0) 
            {
                /* We use the first CDB Base
                * Address to compute the
                * relative offsets of the
                * rest of the ITB entries.
                * NOTE: It is ASSUMED that
                * the CDB data is in a
                * continguous region of
                * memory */
                base_address = val;
            }
            val -= base_address;
            val += (cdb_offset + uiFrameOffset);
            BDBG_MSG(("val = 0x%x, cdb_offset = 0x%x, FrameOff = 0x%x\n",
                val, cdb_offset, uiFrameOffset));
            itb_array[i+1] = val;
        }
    }

    /* Allocate DRAM for ITB */
    pItbMemPtr = BMEM_AllocAligned(hMemory, uiItbSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;
    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pItbMemPtr, &itb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);
    
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    /* Converting all memory accesses to cached */
    BMEM_ConvertAddressToCached(hMemory, pItbMemPtr, &pCached);
    *(char *)pCached = 0;
    pItbMemPtr = pCached;
#endif

    BDBG_MSG(("LoadStaticCdbItb: itb_offset = 0x%x\n", itb_offset));

    /* Write ITB */
    for(ui32ByteCount = 0; ui32ByteCount < uiItbSize; ui32ByteCount = ui32ByteCount+4)
    {
        data = itb_array[ui32ByteCount/4];
        BRAAGA_P_DRAMWRITE((uint32_t)pItbMemPtr + ui32ByteCount, itb_array[ui32ByteCount/4]); 
    }
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    BMEM_FlushCache(hMemory, pItbMemPtr, uiItbSize);
#endif

	BDBG_MSG(("\n\n\n Checking Loaded CDB Values Till 80"));

	for(ui32ByteCount = uiFrameOffset; ui32ByteCount < 80+uiFrameOffset; ui32ByteCount = ui32ByteCount+4)
	{
		data = BRAAGA_P_DRAMREAD((uint32_t)pCdbMemPtr+ui32ByteCount); 
	    BDBG_MSG(("\n CDB VALUE READ at address %x is %x", (uint32_t)pCdbMemPtr+ui32ByteCount, data));
	}

	BDBG_MSG(("\n\n\n Checking Loaded ITB Values Till 80"));

	for(ui32ByteCount = 0; ui32ByteCount < 400; ui32ByteCount = ui32ByteCount+4)
	{
		data = BRAAGA_P_DRAMREAD((uint32_t)pItbMemPtr+ui32ByteCount); 
		BDBG_MSG(("\n ITB VALUE READ at address %x is %x", (uint32_t)pItbMemPtr+ui32ByteCount, data));
	}

    BDBG_MSG(("\nCDB size = %d, ITB size = %d", cdb_array_size, itb_array_size));

    /* Program the Registers accordingly - CDB and ITB for RAVE Context 0 */
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRITE_PTR,cdb_offset + uiCdbSize -1);
    
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_READ_PTR,cdb_offset);
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_BASE_PTR,cdb_offset);
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_VALID_PTR,cdb_offset + uiCdbSize -1 );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_END_PTR,cdb_offset + uiCdbSize -1 );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_WRAPAROUND_PTR,cdb_offset + uiCdbSize - 1);
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_CDB_DEPTH,0x00000000 );

    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_WRITE_PTR,itb_offset + uiItbSize -1);
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_READ_PTR, itb_offset );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_BASE_PTR,itb_offset );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_VALID_PTR,itb_offset + uiItbSize -1 );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_END_PTR,itb_offset + uiItbSize -1   );
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_WRAPAROUND_PTR, itb_offset + uiItbSize - 1);
    BRAAGA_Write32(hReg, BCHP_XPT_RAVE_CX0_AV_ITB_DEPTH,0x00000000 );
#endif    
}
#endif /* RAAGA_TEST_STATIC_CDB_ITB */

/****************************************************************************
Summary: Helper Function that sets the debug level.
*****************************************************************************/
#if BDBG_DEBUG_BUILD
void BRAAGA_TEST_SetModuleDebugLevel(
    const char              *modulelist, 
    BDBG_Level              level
)
{
    while (modulelist && *modulelist) {
        const char *end = strchr(modulelist, ',');
        if (!end) 
        {
            BDBG_SetModuleLevel(modulelist, level);
            break;
        }
        else 
        {
            int n = end-modulelist;
            char *buf = (char *)BKNI_Malloc(n+1);
            /* NOTE: memory leak, but this is debug */
            strncpy(buf, modulelist, n);
            buf[n] = 0;
            BDBG_SetModuleLevel(buf, level);
        }
        modulelist = ++end;
    }
}
#endif

/****************************************************************************
Summary: Function closes channel handles, test handle, Resets Transport etc.
*****************************************************************************/
BERR_Code BRAAGA_TEST_Uninit(
    BRAAGA_TEST_Handle            hRaagaTest
)
{
    BERR_Code       err = BERR_SUCCESS;
    unsigned int    uiCurChId=0, i=0;
	BDBG_ASSERT(hRaagaTest);
	
	/* Stop and close all decode channels */
	for(uiCurChId=0; uiCurChId < RAAGA_MAX_CONTEXT; uiCurChId++)
	{
		if((hRaagaTest->hRaagaTestCh[uiCurChId])&&
		   (hRaagaTest->hRaagaTestCh[uiCurChId]->hDspcxt != NULL))
		{
			if (hRaagaTest->hRaagaTestCh[uiCurChId]->bRunning == true)
			{
				for(i=0; i< RAAGA_TEST_MAX_TASK_PER_CONTEXT; i++)
				{
					if((hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i])&&
					   (hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task))
					{
						if((hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task != NULL)&&
						   (hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->bused == true))
						{
							if(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->eDstType == 
											BRAAGA_TEST_TaskDstType_eRaveBuf)
							{
								if(hRaagaTest->bXptPbEn == true)
								{
#if RAAGA_TEST_ENABLE_TRANSPORT
									if(hRaagaTest->bFilePb)
									{
										err = BRAAGA_TEST_PbLib_StopPlay(hRaagaTest->hPbLib);
									}
									err = BRAAGA_TEST_StopRaveContext(hRaagaTest, hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]);
									BDBG_ASSERT(err == BERR_SUCCESS);
									BDBG_LOG(("BRAAGA_TEST_Uninit: BRAAGA_TEST_StopRaveContext() %d Stopped",
												hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->uiPidChnNo));
#endif
								}
							}
							BDSP_Task_Stop(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task);
							hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->bused = false;

							if(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->eDstType == 
											BRAAGA_TEST_TaskDstType_eRaveBuf)
							{
							if(hRaagaTest->bXptPbEn)
								{
#if RAAGA_TEST_ENABLE_TRANSPORT
				                /* Close RAVE Context */
				    			err = BRAAGA_TEST_CloseRaveContext(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i],
				    			                                  hRaagaTest->hPcr);
				    			BDBG_ASSERT(err == BERR_SUCCESS);
				    			BDBG_LOG(("\nXPT Rave Context %d Closed", uiCurChId));
#endif
								}
							}
						}
						BDSP_Task_Destroy(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task);
						hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task = NULL;
						BKNI_Free(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]);
						hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i] = NULL;
					}
				}
				hRaagaTest->hRaagaTestCh[uiCurChId]->bRunning = false;
			}
			BDSP_Context_Destroy(hRaagaTest->hRaagaTestCh[uiCurChId]->hDspcxt);
			BKNI_Free(hRaagaTest->hRaagaTestCh[uiCurChId]);
			hRaagaTest->hRaagaTestCh[uiCurChId] = NULL;
		}
	}

	if(hRaagaTest->bXptPbEn)		
	{			
#if (RAAGA_TEST_ENABLE_TRANSPORT == 1)
    BXPT_PcrOffset_Close(hRaagaTest->hPcrOff);   
    BDBG_MSG(("PCR Offset for channel %d closed", uiCurChId));

    err = BXPT_Rave_CloseChannel(hRaagaTest->hRave);
    BDBG_ASSERT( !err );
    BDBG_MSG(("RAVE channel %d closed", uiCurChId));
#endif



#if (RAAGA_TEST_ENABLE_TRANSPORT == 1)
	/* Close the Xpt device */
	BXPT_Close( hRaagaTest->hXpt );
	BDBG_MSG(("XPT Device Closed"));
#endif    
	}  
    return err;
}

#if RAAGA_TEST_ENABLE_TRANSPORT
/***************************************************************************
Summary:
	API returns the buffer requirements for CDB and ITB.

Description:
	This API returns the size required for the CDB and ITB buffers
	based upon the algorithm type to be used.
	
Returns:
	BERR_SUCCESS 

See Also:
	None
	
****************************************************************************/
BERR_Code BRAAGA_TEST_GetBufferConfig ( 	
	BRAAGA_TEST_BufferCfgMode	eCfgMode,			/* [in] CDB/ITB Configuration Mode */
	BAVC_CdbItbConfig	*pCdbItbConfigInfo /* [out] Buffer information of CDB and ITB*/ 
	)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAAGA_TEST_GetBufferConfig);

	/* Check input parameters */
	BDBG_ASSERT(pCdbItbConfigInfo);
	BDBG_ASSERT(eCfgMode < BRAAGA_TEST_BufferCfgMode_eMaxMode);

	/* Get the CDB & ITB info corresponding to the buffer config mode */
	*pCdbItbConfigInfo = sCdbItbCfg[eCfgMode];

	BDBG_LEAVE(BRAAGA_TEST_GetBufferConfig);
   	return ret;
}
#endif

void BRAAGA_TEST_ConfigureSampleRate(
    void *pParam1,
    int param2,
    unsigned streamSampleRate,
    unsigned baseSampleRate)
{
    BREG_Handle hReg;
    
    hReg = (BREG_Handle)pParam1;
    
    BSTD_UNUSED(param2);
    BSTD_UNUSED(streamSampleRate);
    
    BDBG_LOG(("Configuring DAC sample rate to %d", baseSampleRate));
    switch (baseSampleRate)
    {
        case 32000:
            /* 32 kHz */
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_RATE_RATIO, 0x400);
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_SAMPLE_INC, 0x12F0003);
            BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_PHASE_INC, 0x9b583);
            break;

        case 44100:
            /* 44.1 kHz */
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_RATE_RATIO, 0x300);
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_SAMPLE_INC, 0x1330002);
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_PHASE_INC, 0xd6159);
            break;
            
        case 48000:
        default:
            /* 48 kHz */
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_RATE_RATIO, 0x200);
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_SAMPLE_INC, 0x650002);
        	BREG_Write32(hReg, BRAAGA_HIFIDAC_RM0_PHASE_INC, 0xe9045);
            break;
    }
}


uint32_t BRGA_TST_Capture_Add(BDSP_AF_P_sIO_BUFFER *pBufDesc, const char *outFile, bool move_read, BREG_Handle hReg)
{
    uint32_t i, j, retval;
    int k;
    
    char fName[300];
    
    retval = -1;
    
    for (i = 0; i < MAX_NUM_CAPTURES; i++)
    {
        if (pcaptureInfo->capChanInfo[i].enabled)
        {
            continue;
        }
        
        /* Open Capture files */
        for (j = 0; j < pBufDesc->ui32NumBuffers; j++)
        {
            sprintf (&fName[0], "%s.%d", outFile, j);
            pcaptureInfo->capChanInfo[i].OutFilePtr[j] = fopen (&fName[0], "wb");
            if (pcaptureInfo->capChanInfo[i].OutFilePtr[j] == NULL)
            {
                BDBG_ERR(("ERROR: Could not open file for writing : %s!!!", &fName[0]));
                /* Discard the captured data by writing into /dev/null - This will make sure that the buffer pointers continue to be updated */
                pcaptureInfo->capChanInfo[i].OutFilePtr[j] = fopen ("/dev/null", "wb");
                /*break;*/
            }
        }
        
        /* Close opened files and return on error */
        if (j < pBufDesc->ui32NumBuffers)
        {
            for (k = j-1; k >= 0; k--)
            {
                fclose (pcaptureInfo->capChanInfo[i].OutFilePtr[k]);
            }
            break;
        }
        
        /* Copy the buffer descriptor structure */
        if (pBufDesc->eBufferType == BDSP_AF_P_BufferType_eFMM)
        {
            BRGA_TST_TransFmmBuffId2Addr((BDSP_CIT_P_sIO_BUFF_ID *)pBufDesc, &pcaptureInfo->capChanInfo[i].sIoBfrDesc);
        }
        else
        {
            pcaptureInfo->capChanInfo[i].sIoBfrDesc = *pBufDesc;
        }
        
        /* Update the local read pointers and allocate the file write buffers */
        for (j = 0; j < pBufDesc->ui32NumBuffers; j++)
        {
            switch (pcaptureInfo->capChanInfo[i].sIoBfrDesc.eBufferType)
            {
                case BDSP_AF_P_BufferType_eRAVE:
                case BDSP_AF_P_BufferType_eFMM:
                case BDSP_AF_P_BufferType_eRDB:
                    pcaptureInfo->capChanInfo[i].readPointers[j] = BREG_Read32(hReg, 0x0FFFFFFF & pcaptureInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j].ui32ReadAddr);
                    break;
                case BDSP_AF_P_BufferType_eDRAM:
                case BDSP_AF_P_BufferType_eDRAM_IS:
                    pcaptureInfo->capChanInfo[i].readPointers[j] = pBufDesc->sCircBuffer[j].ui32ReadAddr;
                default:
                    /* Capture unsupported */
                    break;
            }
            
            pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr = (uint32_t)BKNI_Malloc(BRGA_TST_FILE_WRITE_BUFFER_SIZE);
            if ((void *)pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr == NULL)
            {
                BDBG_ERR(("SYSTEM OUT OF MEMORY in BRGA_TST_Capture_Add : %d:%d", i, j));
                BDBG_ASSERT(0);
            }
            else
            {
                pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32ReadAddr 
                            = pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr;
                pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32WriteAddr 
                            = pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr;
                pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32EndAddr /* End address is exclusive */
                            = pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr + BRGA_TST_FILE_WRITE_BUFFER_SIZE;
            }
        }
        
        pcaptureInfo->capChanInfo[i].start_capture = false;
        pcaptureInfo->capChanInfo[i].move_read = move_read;
        /* Enable the capture and return */
        pcaptureInfo->capChanInfo[i].enabled = 1;
        
        retval = i;
        break;
    }
    
    return (uint32_t)retval;
}

BERR_Code BRGA_TST_Capture_Start(uint32_t i)
{
    BERR_Code err;
    if (pcaptureInfo->capChanInfo[i].enabled)
    {
        if (!(pcaptureInfo->capChanInfo[i].running))
        {
            pcaptureInfo->capChanInfo[i].running = 1;
        }
        else
        {
            BDBG_ERR(("ERROR: Channel is already being captured : %d!!!", i));
            err = BERR_INVALID_PARAMETER;
        }
    }
    else
    {
        BDBG_ERR(("Capture is not enabled for channel : %d\n", i));
        err = BERR_INVALID_PARAMETER;
    }
    
    return err;
}

BERR_Code BRGA_TST_Capture_Stop(uint32_t i)
{
    BERR_Code err;
    if (pcaptureInfo->capChanInfo[i].enabled)
    {
        if (pcaptureInfo->capChanInfo[i].running)
        {
            pcaptureInfo->capChanInfo[i].running = 0;
        }
        else
        {
            BDBG_ERR(("Channel capture has already been disabled : %d\n", i));
            err = BERR_INVALID_PARAMETER;
        }
        pcaptureInfo->capChanInfo[i].start_capture = false;
    }
    else
    {
        BDBG_ERR(("Capture is not enabled for channel : %d\n", i));
        err = BERR_INVALID_PARAMETER;
    }
    
    return err;
}

void BRGA_TST_Capture_RemoveAll(void)
{
    uint32_t i;
    
    for (i = 0; i < MAX_NUM_CAPTURES; i++)
    {
        if (pcaptureInfo->capChanInfo[i].enabled)
        {
            BRGA_TST_Capture_Remove(i);
        }
    }
}

uint32_t BRGA_TST_Capture_Remove(uint32_t i)
{
    BERR_Code err;
    uint32_t j;
    
    if (pcaptureInfo->capChanInfo[i].enabled)
    {
        /* Critical Section */
        pcaptureInfo->capChanInfo[i].enabled = 0;
        
        /* Close opened files and return on error */
        for (j = 0; j < pcaptureInfo->capChanInfo[i].sIoBfrDesc.ui32NumBuffers; j++)
        {
            fclose (pcaptureInfo->capChanInfo[i].OutFilePtr[j]);
            BKNI_Free ((void *)pcaptureInfo->capChanInfo[i].sLocalBuffers[j].ui32BaseAddr);
        }
        /**********/
    }
    else
    {
        BDBG_ERR(("Capture is not enabled for channel : %d\n", i));
        err = BERR_INVALID_PARAMETER;
    }
    
    return err;
}

void BRGA_TST_BufferCaptureTask(BTST_TaskHandle Task, void *Parm)
{
    BMEM_Handle                 hMem;
	BREG_Handle                 hReg;
    BRAAGA_TEST_CaptureStruct   *pCapInfo;
    uint32_t                    dataPresent[BDSP_AF_P_MAX_CHANNELS];
    void                        *pBuffer;
#ifdef ENABLE_CACHED_ACCESS_CHANGES
    void                        *pCached;
#endif
    
    uint32_t i, j;
    
    BSTD_UNUSED(Task);
    
    pCapInfo = (BRAAGA_TEST_CaptureStruct *)Parm;
    hReg = pCapInfo->hReg;
    hMem = pCapInfo->hMem;
    
    BDBG_LOG(("Capture Task Created\n"));
    
    while (1)
    {
        for (i = 0; i < MAX_NUM_CAPTURES; i++)
        {
            if (pCapInfo->capChanInfo[i].enabled && pCapInfo->capChanInfo[i].running)
            {
                switch (pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType)
                {
                    case BDSP_AF_P_BufferType_eDRAM:
                        /* Need to add support */
                        break;
                    case BDSP_AF_P_BufferType_eRAVE:
                        BRGA_TST_GetBufferStatus(&pCapInfo->capChanInfo[i].sIoBfrDesc, &pCapInfo->capChanInfo[i].wrapBit[0], &pCapInfo->capChanInfo[i].readPointers[0], &dataPresent[0], hReg);
                        for (j = 0; j < pCapInfo->capChanInfo[i].sIoBfrDesc.ui32NumBuffers; j++)
                        {
                            BMEM_ConvertOffsetToAddress(hMem, pCapInfo->capChanInfo[i].readPointers[j], &pBuffer);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                            /* Converting all memory accesses to cached */
                            BMEM_ConvertAddressToCached(hMem, (void *)pBuffer, &pCached);
                            pBuffer = pCached;
                            BMEM_FlushCache(hMem, pBuffer, dataPresent[j]);
#endif
                            fwrite (pBuffer, 1, dataPresent[j], pCapInfo->capChanInfo[i].OutFilePtr[j]);
                            pCapInfo->capChanInfo[i].readPointers[j] = BRGA_TST_GetUpdatedReadPtr(&pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j], 
                                                                       pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType, 
                                                                       pCapInfo->capChanInfo[i].readPointers[j], 
                                                                       dataPresent[j],
                                                                       hReg);
                            if (pCapInfo->capChanInfo[i].move_read)
                            {
                                BRGA_TST_UpdateReadPtr(&pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j], 
                                                       pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType, 
                                                       pCapInfo->capChanInfo[i].readPointers[j],
                                                       hReg);
                            }
                        }
                        break;
                    case BDSP_AF_P_BufferType_eFMM:
                        if (pcaptureInfo->capChanInfo[i].start_capture == false)
                        {
                            uint32_t ui32WrPtr, ui32StartWrPtr, ui32NumBuffers;
                            BDSP_AF_P_sIO_BUFFER *psIoBfrDesc;
                            
                            psIoBfrDesc = &pCapInfo->capChanInfo[i].sIoBfrDesc;
                            ui32NumBuffers = psIoBfrDesc->ui32NumBuffers;
                            
                            ui32WrPtr = BREG_Read32(hReg, (0x0FFFFFFF & psIoBfrDesc->sCircBuffer[0].ui32WriteAddr));
                            ui32StartWrPtr = BREG_Read32(hReg, (0x0FFFFFFF & (psIoBfrDesc->sCircBuffer[0].ui32WriteAddr + 0x10)));
                            
                            if (ui32WrPtr >= ui32StartWrPtr)
                            {
                                /* Check if the start write pointer has been programmed even for the last buffer */
                                ui32WrPtr = BREG_Read32(hReg, (0x0FFFFFFF & psIoBfrDesc->sCircBuffer[ui32NumBuffers-1].ui32WriteAddr));
                                ui32StartWrPtr = BREG_Read32(hReg, (0x0FFFFFFF & (psIoBfrDesc->sCircBuffer[ui32NumBuffers-1].ui32WriteAddr + 0x10)));
                            
                                if (ui32WrPtr >= ui32StartWrPtr)
                                {
                                    for (j = 0; j < ui32NumBuffers; j++)
                                    {
                                        /* Capture needs to start from Start write pointer */
                                        pCapInfo->capChanInfo[i].readPointers[j] = BREG_Read32(hReg, (0x0FFFFFFF & (pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j].ui32WriteAddr + 0x10)));
									    BREG_Write32(hReg, (0x0FFFFFFF & (pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j].ui32ReadAddr)), BREG_Read32(hReg, (0x0FFFFFFF & (pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j].ui32WriteAddr))));
                                    }
                                    pcaptureInfo->capChanInfo[i].start_capture = true;
                                }
                            }
                        }
                        
                        if (pcaptureInfo->capChanInfo[i].start_capture == true)
                        {
                            BRGA_TST_GetBufferStatus(&pCapInfo->capChanInfo[i].sIoBfrDesc, &pCapInfo->capChanInfo[i].wrapBit[0], &pCapInfo->capChanInfo[i].readPointers[0], &dataPresent[0], hReg);
							BDBG_MSG(("Size = 0x%x, Read = 0x%x, Write = 0x%x", dataPresent[0], pCapInfo->capChanInfo[i].readPointers[0], BREG_Read32(hReg, (0x0FFFFFFF & (pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[0].ui32WriteAddr)))));
                            for (j = 0; j < pCapInfo->capChanInfo[i].sIoBfrDesc.ui32NumBuffers; j++)
                            {
                                if (dataPresent[j])
                                {
                                    BMEM_ConvertOffsetToAddress(hMem, pCapInfo->capChanInfo[i].readPointers[j], &pBuffer);
#ifdef ENABLE_CACHED_ACCESS_CHANGES
                                    /* Converting all memory accesses to cached */
                                    BMEM_ConvertAddressToCached(hMem, (void *)pBuffer, &pCached);
                                    pBuffer = pCached;
                                    BMEM_FlushCache(hMem, pBuffer, dataPresent[j]);
#endif
                                    
                                    if (g_afap_mode == true)
                                    {
										fwrite (pBuffer, 1, dataPresent[j], pCapInfo->capChanInfo[i].OutFilePtr[j]);
                                    }
                                    else
                                    {
	                                    BRGA_TST_WriteDataToFileWritebuffer(pBuffer, &pCapInfo->capChanInfo[i].sLocalBuffers[j], dataPresent[j]);
	                                    BRGA_TST_UpdateFileWriteBuffer(&pCapInfo->capChanInfo[i].sLocalBuffers[j], WRITE_UPDATE, dataPresent[j]);
                                    }

                                    pCapInfo->capChanInfo[i].readPointers[j] = BRGA_TST_GetUpdatedReadPtr(&pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j], 
                                                                               pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType, 
                                                                               pCapInfo->capChanInfo[i].readPointers[j], 
                                                                               dataPresent[j],
                                                                               hReg);

                                    /* The read pointer movement needs to be done by the capture for buffers 2-5 since the FMM buffers are configured only for stereo output to DACs */
                                    if ((pCapInfo->capChanInfo[i].move_read) || (j >= 2))
                                    {
                                        BRGA_TST_UpdateReadPtr(&pCapInfo->capChanInfo[i].sIoBfrDesc.sCircBuffer[j], 
                                                               pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType, 
                                                               (pCapInfo->capChanInfo[i].wrapBit[j] << 31) | pCapInfo->capChanInfo[i].readPointers[j],
                                                               hReg);
                                    }                                    
                                }
                            }
                        }
                        break;
                    case BDSP_AF_P_BufferType_eRDB:
                        /* Need to add support */
                        break;
                    case BDSP_AF_P_BufferType_eDRAM_IS:
                    default:
                        /* Capture unsupported */
                        break;
                }
            }
        }
    }
}

void BRGA_TST_FileWriteTask(BTST_TaskHandle Task, void *Parm)
{
    BMEM_Handle                 hMem;
	BREG_Handle                 hReg;
    BRAAGA_TEST_CaptureStruct   *pCapInfo;
    
    uint32_t i, j, ui32Size;
    
    BSTD_UNUSED(Task);
    
    pCapInfo = (BRAAGA_TEST_CaptureStruct *)Parm;
    hReg = pCapInfo->hReg;
    hMem = pCapInfo->hMem;
    
    BSTD_UNUSED(hReg);
    BSTD_UNUSED(hMem);
    
    BDBG_LOG(("File Write Task Created\n"));
    
    while (1)
    {
        for (i = 0; i < MAX_NUM_CAPTURES; i++)
        {
            if (pCapInfo->capChanInfo[i].enabled && pCapInfo->capChanInfo[i].running)
            {
                switch (pCapInfo->capChanInfo[i].sIoBfrDesc.eBufferType)
                {
                    case BDSP_AF_P_BufferType_eFMM:
                        for (j = 0; j < pCapInfo->capChanInfo[i].sIoBfrDesc.ui32NumBuffers; j++)
                        {
                            BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pLocalBuffer;
                            pLocalBuffer = &pCapInfo->capChanInfo[i].sLocalBuffers[j];
                            
                            if (pLocalBuffer->ui32WriteAddr != pLocalBuffer->ui32ReadAddr)
                            {
                                if (pLocalBuffer->ui32WriteAddr > pLocalBuffer->ui32ReadAddr)
                                {
                                    ui32Size = pLocalBuffer->ui32WriteAddr - pLocalBuffer->ui32ReadAddr;
                                }
                                else
                                {
                                    ui32Size = pLocalBuffer->ui32EndAddr - pLocalBuffer->ui32ReadAddr;
                                }
                            
                                fwrite ((void *)pLocalBuffer->ui32ReadAddr, 1, ui32Size, pCapInfo->capChanInfo[i].OutFilePtr[j]);
                                BRGA_TST_UpdateFileWriteBuffer(pLocalBuffer, READ_UPDATE, ui32Size);
                            }
                        }
                        break;
                    case BDSP_AF_P_BufferType_eDRAM:
                        /* Need to add suport */
                        break;
                    case BDSP_AF_P_BufferType_eDRAM_IS:
                        /* Need to add suport */
                        break;
                    case BDSP_AF_P_BufferType_eRAVE:
                    case BDSP_AF_P_BufferType_eRDB:
                        /* File write thread is not required for RDB and RAVE buffers as the data rate is relatively low */
                    default:
                        /* Capture unsupported */
                        break;
                }
            }
        }
    }
}



void BRGA_TST_UpdateReadPtr(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, BDSP_AF_P_BufferType eBufferType, uint32_t ui32ReadPtr, BREG_Handle hReg)
{
    switch (eBufferType)
    {
        case BDSP_AF_P_BufferType_eRAVE:
        case BDSP_AF_P_BufferType_eFMM:
        case BDSP_AF_P_BufferType_eRDB:
            BREG_Write32(hReg, 0x0FFFFFFF & pCircBuffer->ui32ReadAddr, ui32ReadPtr);
            break;
        case BDSP_AF_P_BufferType_eDRAM:
        case BDSP_AF_P_BufferType_eDRAM_IS:
        default:
            pCircBuffer->ui32ReadAddr = ui32ReadPtr;
            /* Capture unsupported */
            break;
    }
}

void BRGA_TST_WriteDataToFileWritebuffer (void *pBuffer, BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, uint32_t ui32Size)
{
    uint32_t ui32FreeSpace, ui32Chunk1Size;
    
    ui32FreeSpace = BRGA_TST_GetFileWriteBufferInfo(pCircBuffer);
    
    if (ui32FreeSpace <= (ui32Size+8))
    {
        BDBG_ERR(("Error writing to file : buffer overflow detected :%d>%d", ui32Size, ui32FreeSpace));
        BDBG_ASSERT(0);
    }
    
    if ((pCircBuffer->ui32WriteAddr + ui32Size) >= pCircBuffer->ui32EndAddr)
    {
        ui32Chunk1Size = pCircBuffer->ui32EndAddr - pCircBuffer->ui32WriteAddr;
        memcpy ((void *)pCircBuffer->ui32WriteAddr, pBuffer, ui32Chunk1Size);
        memcpy ((void *)pCircBuffer->ui32BaseAddr, (void *)((uint32_t)pBuffer + ui32Chunk1Size), (ui32Size-ui32Chunk1Size));
    }
    else
    {
        memcpy ((void *)pCircBuffer->ui32WriteAddr, pBuffer, ui32Size);
    }
}

uint32_t BRGA_TST_GetFileWriteBufferInfo(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer)
{
    uint32_t ui32FreeSpace;
    
    ui32FreeSpace = pCircBuffer->ui32ReadAddr - pCircBuffer->ui32WriteAddr;
    
    if (pCircBuffer->ui32WriteAddr >= pCircBuffer->ui32ReadAddr)
    {
        ui32FreeSpace = (pCircBuffer->ui32EndAddr - pCircBuffer->ui32WriteAddr) 
                        +  (pCircBuffer->ui32ReadAddr - pCircBuffer->ui32BaseAddr);
    }
    
    return ui32FreeSpace;
}


void BRGA_TST_UpdateFileWriteBuffer(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, uint32_t ui32AddrType, uint32_t ui32Size)
{
    if (ui32AddrType == READ_UPDATE)
    {
        pCircBuffer->ui32ReadAddr += ui32Size;
        if (pCircBuffer->ui32ReadAddr >= pCircBuffer->ui32EndAddr)
        {
            pCircBuffer->ui32ReadAddr = (pCircBuffer->ui32ReadAddr - pCircBuffer->ui32EndAddr) + pCircBuffer->ui32BaseAddr; 
        }        
    }
    else if (ui32AddrType == WRITE_UPDATE)
    {
        pCircBuffer->ui32WriteAddr += ui32Size;
        if (pCircBuffer->ui32WriteAddr >= pCircBuffer->ui32EndAddr)
        {
            pCircBuffer->ui32WriteAddr = (pCircBuffer->ui32WriteAddr - pCircBuffer->ui32EndAddr) + pCircBuffer->ui32BaseAddr; 
        }        
    }
}



uint32_t BRGA_TST_GetUpdatedReadPtr(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pCircBuffer, BDSP_AF_P_BufferType eBufferType, uint32_t ui32ReadPtr, uint32_t Size, BREG_Handle hReg)
{
    uint32_t    ui32BaseAddr;
    uint32_t    ui32EndAddr;
    uint32_t    ui32ReadAddr;
    uint32_t    ui32WriteAddr;
    uint32_t    ui32WrapAddr;
    
    ui32ReadAddr = 0xFFFFFFFF;
    switch (eBufferType)
    {
        case BDSP_AF_P_BufferType_eDRAM:
            /* Need to add support */
            break;
        case BDSP_AF_P_BufferType_eRAVE:
            ui32BaseAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32BaseAddr);
            ui32EndAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32EndAddr);
            ui32ReadAddr = ui32ReadPtr;
            ui32WriteAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32WriteAddr);
            ui32WrapAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32WrapAddr);
            
            ui32ReadAddr = (ui32ReadPtr + Size);
            if ((ui32ReadPtr + Size) >= ui32WrapAddr)
            {
                ui32ReadAddr = ui32BaseAddr + ((ui32ReadPtr + Size) - ui32WrapAddr);
            }
            break;
        case BDSP_AF_P_BufferType_eFMM:
            ui32BaseAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32BaseAddr);
            ui32EndAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32EndAddr);
            ui32ReadAddr = ui32ReadPtr;
            ui32WriteAddr = BREG_Read32(hReg, 0x0FFFFFFF & pCircBuffer->ui32WriteAddr);
            
            ui32ReadAddr = (ui32ReadPtr + Size);
            if ((ui32ReadPtr + Size) > ui32EndAddr)
            {
                ui32ReadAddr = ui32BaseAddr + ((ui32ReadPtr + Size) - ui32EndAddr) - 1;
            }
            break;
        case BDSP_AF_P_BufferType_eRDB:
            /* Need to add support */
            break;
        case BDSP_AF_P_BufferType_eDRAM_IS:
        default:
            /* Capture unsupported */
            break;
    }
    
    return ui32ReadAddr;
}

void BRGA_TST_GetBufferStatus(BDSP_AF_P_sIO_BUFFER *pBuffDesc, bool *wrapBit, uint32_t *readPointers, uint32_t *pDataSize, BREG_Handle hReg)
{
    uint32_t    ui32BaseAddr;
    uint32_t    ui32EndAddr;
    uint32_t    ui32ReadAddr;
    uint32_t    ui32WriteAddr;
    uint32_t    ui32WrapAddr;
    uint32_t    ui32Size;
    
    uint32_t i;
    
    for (i = 0; i < BDSP_AF_P_MAX_CHANNELS; i++)
    {
        *(pDataSize+i) = 0;
        wrapBit[i] = 0;
    }
    
    switch (pBuffDesc->eBufferType)
    {
        case BDSP_AF_P_BufferType_eDRAM:
            /* Need to add support */
            break;
        case BDSP_AF_P_BufferType_eRAVE:
            for (i = 0; i < pBuffDesc->ui32NumBuffers; i++)
            {
                ui32BaseAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32BaseAddr);
                ui32EndAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32EndAddr);
                ui32ReadAddr = readPointers[i];/*BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32ReadAddr);*/
                ui32WriteAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32WriteAddr);
                ui32WrapAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32WrapAddr);
                
                if (ui32WrapAddr == 0)
                    ui32WrapAddr = ui32EndAddr;
                
                
                ui32Size = ui32WriteAddr - ui32ReadAddr;
                if (ui32WriteAddr < ui32ReadAddr)
                {
                    /* Return only the contiguous chunk size */
                    ui32Size = (ui32WrapAddr - ui32ReadAddr) + 1;
                }
                
                *(pDataSize+i) = ui32Size;
            }
            break;
        case BDSP_AF_P_BufferType_eFMM:
            for (i = 0; i < pBuffDesc->ui32NumBuffers; i++)
            {
                ui32BaseAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32BaseAddr);
                ui32EndAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32EndAddr);
                ui32ReadAddr = readPointers[i];/*BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32ReadAddr);*/
                ui32WriteAddr = BREG_Read32(hReg, 0x0FFFFFFF & pBuffDesc->sCircBuffer[i].ui32WriteAddr);
                
                wrapBit[i] = ui32WriteAddr & 0x80000000;
                
                ui32ReadAddr &= 0x7FFFFFFF;
                ui32WriteAddr &= 0x7FFFFFFF;
                
                ui32Size = ui32WriteAddr - ui32ReadAddr;
                if (ui32WriteAddr < ui32ReadAddr)
                {
                    /* Return only the contiguous chunk size */
                    ui32Size = (ui32EndAddr - ui32ReadAddr) + 1;
                }
                
                *(pDataSize+i) = ui32Size;
            }
            break;
        case BDSP_AF_P_BufferType_eRDB:
            /* Need to add support */
            break;
        case BDSP_AF_P_BufferType_eDRAM_IS:
        default:
            /* Capture unsupported */
            break;
    }
    
    return;
}

void BRGA_TST_TransFmmBuffId2Addr(BDSP_CIT_P_sIO_BUFF_ID *psIoBuffId, BDSP_AF_P_sIO_BUFFER *psIoBuffer)
{
	uint32_t	ui32Buff;
	uint32_t	ui32BuffId;
	uint32_t	ui32Buff2BuffOffset;
	uint32_t	ui32Buff0AddrStart;
	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		*psCircBuffer;



#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
	ui32Buff0AddrStart  = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR;
	ui32Buff2BuffOffset = BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR - 
								BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR;
#else
    ui32Buff0AddrStart  = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
    ui32Buff2BuffOffset = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR - 
                                BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
#endif
	
	/*	First translate the I/O buffers */
	/* ui32NumBuffers population */ 
	psIoBuffer->ui32NumBuffers = psIoBuffId->ui32NumBuffers;
    psIoBuffer->eBufferType = psIoBuffId->eBufferType;

	for( ui32Buff=0; ui32Buff<psIoBuffId->ui32NumBuffers; ui32Buff++)
	{
		psCircBuffer = &psIoBuffer->sCircBuffer[ui32Buff];
		ui32BuffId = psIoBuffId->ui32BufferId[ui32Buff];
		
		psCircBuffer->ui32ReadAddr	= ui32Buff0AddrStart + ui32BuffId * ui32Buff2BuffOffset;
		psCircBuffer->ui32WriteAddr = psCircBuffer->ui32ReadAddr	+ 4;
		psCircBuffer->ui32BaseAddr	= psCircBuffer->ui32ReadAddr	+ 8;
		psCircBuffer->ui32EndAddr	= psCircBuffer->ui32ReadAddr	+ 12;

		psCircBuffer->ui32WrapAddr = psCircBuffer->ui32EndAddr;
	}

	return;
}


bool BRGA_TST_IsSpeechEncoder(BDSP_AudioEncode eAudioType)
{
    switch (eAudioType)
    {
        case BDSP_AudioEncode_eG711G726:
        case BDSP_AudioEncode_eG729:
        case BDSP_AudioEncode_eG723_1:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}

/* end of file */

