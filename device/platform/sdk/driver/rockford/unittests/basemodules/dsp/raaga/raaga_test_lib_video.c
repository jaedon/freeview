/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: raaga_test_lib_video.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/14/12 3:03p $
 *
 ****************************************************************************/ 

#include "raaga_test_video.h"

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
	if(hRaagaTest->bXptPbEn == true)
	{
		BDBG_ERR(("uiNumStreams:=%d", hRaagaTest->uiNumStreams));
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
	unsigned int				regVal;

#if RAAGA_FWAUTHENTICATION_ENABLE
    BDSP_Raaga_DownloadStatus pStatus;    
#endif
    BDSP_Raaga_GetDefaultSettings(&raagaSettings);

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
    BDBG_ERR(("\nFirmware Info: address = 0x%x|0x%x, size=0x%x\n", pStatus.pBaseAddress, pStatus.physicalAddress, pStatus.length));


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

	/*
	Bcm97231CB.Bcm7231.SUN_TOP_CTRL.TEST_PORT_CTRL.ENCODED_TP_ENABLE=&h10&
	Bcm97231CB.Bcm7231.SUN_TOP_CTRL.TEST_PORT_CTRL.TP_SELECT=&h0&

'UART1
Bcm97231CB.Bcm7231.SUN_TOP_CTRL.UART_ROUTER_SEL.port_1_cpu_sel=&h1&
'The uart_rxd signal for this interface is mapped to alt_tp_in[0] on gpio[95].
'The uart_txd signal for this interface is mapped to alt_tp_out[0] on gpio[94]
Bcm97231CB.Bcm7231.SUN_TOP_CTRL.PIN_MUX_CTRL_11.gpio_94=&h4&
Bcm97231CB.Bcm7231.SUN_TOP_CTRL.PIN_MUX_CTRL_11.gpio_95=&h5&
*/
	regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
	regVal = (regVal & ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel)))
					| BCHP_FIELD_DATA(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL_port_1_cpu_sel_AUDIO_FP);
	BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL, regVal);

	regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
	regVal = (regVal & ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_94)))
					| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_94, 4);
	BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, regVal);

	regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
	regVal = (regVal & ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_95)))
					| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_95, 5);
	BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, regVal);
	/*
	BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, );
	*/
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
		case(BRAAGA_TEST_CxtType_eVideoEncode):
        {
			for(i=0; i<RAAGA_MAX_CONTEXT; ++i)
			{
				if(hRaagaTest->hRaagaTestCh[i]->hDspcxt == NULL)
				{
					hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
					hRaagaTestCh->sSettings.eChTaskType = BRAAGA_TEST_CxtType_eVideoEncode;
					/* Open video context */
					BDSP_Context_GetDefaultCreateSettings(hRaagaTest->hDsp,BDSP_ContextType_eVideoEncode,&settings);
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
	uint32_t					ui32PhyAddr=0,ui32RegVal=0, ui32RegOffset = 0, ui32TempRdbAddr = 0;
    BXDM_Picture *hXDMPicture = NULL;
	BRAAGA_TEST_eTaskType		eTaskType;
	uint32_t					*pPPB;
	void 						*tempAddr = NULL;

	for(i=0; i < RAAGA_MAX_CONTEXT; i++)
	{
		if((hRaagaTest->hRaagaTestCh[i] != NULL) &&
		   (hRaagaTest->hRaagaTestCh[i]->sSettings.eChTaskType == eChTaskType))
		{
			BDBG_MSG(("Found %dth testCh Valid and context type for it is %d", i, eChTaskType));
			hRaagaTestCh = hRaagaTest->hRaagaTestCh[i];
			eTaskType	= hRaagaTestCh->sSettings.eTaskType;
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

			if((eDstType == BRAAGA_TEST_TaskDstType_eRaveBuf) && (eTaskType != BRAAGA_TEST_eTaskType_eEncode))
			{
				hRaagaTaskHandle->eDstType = eDstType;
				hRaagaTaskHandle->uiStreamId = uiStreamId;
#if (RAAGA_TEST_ENABLE_TRANSPORT==1)
				/* Open Rave Context */
				err = BRAAGA_TEST_OpenRaveContext(hRaagaTest, hRaagaTaskHandle);
				BDBG_ASSERT( !err );
#endif
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
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[0] = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eStereo_PCM;
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 2;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eFMM;
				/* Allocating buffer*/
				buff1 = (void *)BMEM_AllocAligned(hRaagaTest->hMem, (1024*1024),8,0);
                if(!buff1)
                {
                    err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
                }
				err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)buff1, &ui32PhyAddr);
			    BREG_Write32(hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR, 
					        ui32PhyAddr);				

				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
							ui32PhyAddr);

				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR,
							ui32PhyAddr);

				/* program the end address */
				ui32RegVal = (BCHP_FIELD_DATA (
									AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR, 
									RINGBUF_ENDADDR, 
									(ui32PhyAddr + (1024*1024) - 1))); 
				
				
				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR , 
							ui32RegVal);

				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT, 
							ui32RegVal-256);

				buff2 = (void *)BMEM_AllocAligned(hRaagaTest->hMem, (1024*1024),8,0);
				if(!buff2)
				{
					err = BERR_TRACE (BERR_OUT_OF_DEVICE_MEMORY);
				}

				err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)buff2, &ui32PhyAddr);
			    BREG_Write32(hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_BASEADDR, 
					        ui32PhyAddr);				

				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR, 
							ui32PhyAddr);

				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_WRADDR,
							ui32PhyAddr);

				/* program the end address */
				ui32RegVal = (BCHP_FIELD_DATA (
									AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR, 
									RINGBUF_ENDADDR, 
									(ui32PhyAddr + (1024*1024) - 1))); 
				
				
				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_ENDADDR , 
							ui32RegVal);
				
				BREG_Write32 (hRaagaTest->hReg,
							BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_START_WRPOINT, 
							ui32RegVal-256);				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[0] = 0;
				BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: eDecAudioAlgo=%d",taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo));
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
			else if(eChTaskType == BRAAGA_TEST_CxtType_eVideo)
            {
				BDSP_Task_GetDefaultCreateSettings(hRaagaTestCh->hDspcxt,&taskSettings);
				if(BRAAGA_TEST_eTaskType_eEncode == eTaskType)
				{
					taskSettings.realtimeMode = BDSP_TaskRealtimeMode_eNonRealTime;
				}
				taskSettings.dspIndex =0 ;
				taskSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
				taskSettings.numBranches =1;
				
				taskSettings.pBranchInfo[0] =(BDSP_CIT_P_FwBranchInfo *)BKNI_Malloc(
												 sizeof(BDSP_CIT_P_FwBranchInfo));
				BKNI_Memset(taskSettings.pBranchInfo[0], 0 , sizeof(BDSP_CIT_P_FwBranchInfo));

				taskSettings.psVDecodeBuffCfg =(BDSP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(
												 sizeof(BDSP_VF_P_sVDecodeBuffCfg));
				BKNI_Memset(taskSettings.psVDecodeBuffCfg, 0 , sizeof(BDSP_VF_P_sVDecodeBuffCfg));
                
				taskSettings.psVEncoderConfig =(BDSP_VF_P_sVEncoderConfig *)BKNI_Malloc(
												 sizeof(BDSP_VF_P_sVEncoderConfig));
				BKNI_Memset(taskSettings.psVEncoderConfig, 0 , sizeof(BDSP_VF_P_sVEncoderConfig));

				taskSettings.pBranchInfo[0]->ui32NumStages = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32BranchId = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32StageId = 0;  

					taskSettings.pBranchInfo[0]->sFwStgInfo[0].eStageType= BDSP_CIT_P_StageType_eVideoDecode;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo= BDSP_VideoType_eVP6;		  
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode = BDSP_DecodeMode_eDecode;		  
				
				/* Get the input buffers here */
				if(BRAAGA_TEST_eTaskType_eDecode == eTaskType)
				{
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

                    /* Reset the entire structure */
                    BKNI_Memset(hXDMPicture, 0, sizeof(BXDM_Picture));

                    hXDMPicture->stBufferInfo.hHeap = hRaagaTest->hMem;
                    hXDMPicture->stBufferInfo.pLumaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
                        = (uint32_t)(ui32BaseAddr + ui32UsedSize);
                    taskSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes
                        = BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;
                    ui32UsedSize += BDSP_REALVIDEO_DECODE_LUMAFRAME_BUF_SIZE;            

                    hXDMPicture->stBufferInfo.pChromaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
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

				}

				BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: eVideoAlgo=%d",taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo));
				err = BDSP_Task_Create(hRaagaTestCh->hDspcxt,&taskSettings,&task);
				BDBG_ERR(("Task created"));
				if(err != BERR_SUCCESS)
				{
					BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: BDSP_Task_Create failed. Error is %d", err)); 
					BDBG_ASSERT( !err );
				}
				hRaagaTaskHandle->task = task;
				hRaagaTaskHandle->bused = true;
				break;
			}            
			else if(BRAAGA_TEST_CxtType_eVideoEncode == eChTaskType )
			{			
				BDSP_Task_GetDefaultCreateSettings(hRaagaTestCh->hDspcxt,&taskSettings);				
				taskSettings.realtimeMode = BDSP_TaskRealtimeMode_eNonRealTime;
				taskSettings.dspIndex =0 ;
				taskSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
				taskSettings.numBranches =1;
				
				taskSettings.pBranchInfo[0] =(BDSP_CIT_P_FwBranchInfo *)BKNI_Malloc(
												 sizeof(BDSP_CIT_P_FwBranchInfo));
				BKNI_Memset(taskSettings.pBranchInfo[0], 0 , sizeof(BDSP_CIT_P_FwBranchInfo));
				
				taskSettings.psVDecodeBuffCfg =(BDSP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(
												 sizeof(BDSP_VF_P_sVDecodeBuffCfg));
				BKNI_Memset(taskSettings.psVDecodeBuffCfg, 0 , sizeof(BDSP_VF_P_sVDecodeBuffCfg));
				
				taskSettings.psVEncoderConfig =(BDSP_VF_P_sVEncoderConfig *)BKNI_Malloc(
												 sizeof(BDSP_VF_P_sVEncoderConfig));
				BKNI_Memset(taskSettings.psVEncoderConfig, 0 , sizeof(BDSP_VF_P_sVEncoderConfig));
				
				taskSettings.pBranchInfo[0]->ui32NumStages = 1;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32BranchId = 0;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].ui32StageId = 0;  
				
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].eStageType= BDSP_CIT_P_StageType_eVideoEncode;
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoEncAlgo= hRaagaTest->sRaagaStrParams[0].eVAlgoType; /* Shijith BDSP_VideoEncode_eH264 */
				taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eEncAudioMode = BDSP_EncodeMode_eNonRealtime;
			
				/* Default configuration for video encoder */
				taskSettings.psVEncoderConfig->ui32MaxFrameHeight = 576;
				taskSettings.psVEncoderConfig->ui32MaxFrameWidth = 720;
				taskSettings.psVEncoderConfig->ui32StripeWidth = 128;
				taskSettings.psVEncoderConfig->sEncoderParams.eEncodeFrameRate = (BDSP_VF_P_eEncodeFrameRate)hRaagaTest->sVidEncodeParams.ui32FrameRate; 
				taskSettings.psVEncoderConfig->sEncoderParams.ui32Frames2Accum = 1;
				
				/*TotalHorzPadd = 96, TotalVertPadd_Luma = 96, and TotalVertPadd_Chroma = 48*/
				/*(CEILING(((MaxVertSize/2) + TotalVertPadd_Chroma)/16) x 16)*/
				taskSettings.psVEncoderConfig->sReferenceBuffParams.ui32ChromaStripeHeight = 336;
				
				/*(CEILING((MaxVertSize + TotalVertPadd_Luma)/16) x 16)*/
				taskSettings.psVEncoderConfig->sReferenceBuffParams.ui32LumaStripeHeight = 672;
				
				/*==========================================================
				Encoder settings follow now
				============================================================*/
				if(BRAAGA_TEST_eTaskType_eEncode == eTaskType)
				{
					/* Video Encoder Memory allocation*/
					/* Video Encoder buffer settings */
					hRaagaTaskHandle->psVidEncBuffer = BMEM_AllocAligned(hRaagaTest->hMem,sizeof(BRAAGA_TEST_VidEncodeBuffer), 2, 0);

					/* reference buffer size */
					uiTempSize = (BDSP_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE+BDSP_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE)
									*BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;

					/* Video FW requires the buffers to be 256 byte alligned for DMA */
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem,uiTempSize, 8, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

					ui32UsedSize = 0;
					/* TODO: Encoder settings here */
					taskSettings.psVEncoderConfig->sReferenceBuffParams.ui32NumBuffAvl = BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;
					for(i=0; i<BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL ; i++)
					{
						taskSettings.psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
							= (uint32_t)(ui32BaseAddr + ui32UsedSize);
						taskSettings.psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes
							= BDSP_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE;                           
						ui32UsedSize += BDSP_H264_ENCODE_REF_LUMAFRAME_BUF_SIZE;

						taskSettings.psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress
							= (uint32_t)(ui32BaseAddr + ui32UsedSize);
						taskSettings.psVEncoderConfig->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes
							= BDSP_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE;                           
						ui32UsedSize += BDSP_H264_ENCODE_REF_CHROMAFRAME_BUF_SIZE;
					}

					/* Need to allocate RAW picture buffers */
					uiTempSize = (BDSP_H264_ENCODE_RAW_PICTURE_BUF_SIZE) * BDSP_FWMAX_VIDEO_BUFF_AVAIL;

					/* Video FW requires the buffers to be 256 byte alligned for DMA */
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem,uiTempSize, 8, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

					hRaagaTaskHandle->psVidEncBuffer->ui32NumBuffers	= BDSP_FWMAX_VIDEO_BUFF_AVAIL;
					ui32UsedSize = 0;
					for(i=0; i<BDSP_FWMAX_VIDEO_BUFF_AVAIL ; i++)
					{
						hRaagaTaskHandle->psVidEncBuffer->ui32RawPicturePtr[i]
									= (uint32_t)(ui32BaseAddr + ui32UsedSize);
						BDBG_MSG(("Raw Picture Address 0x%x", hRaagaTaskHandle->psVidEncBuffer->ui32RawPicturePtr[i]));
						ui32UsedSize += BDSP_H264_ENCODE_RAW_PICTURE_BUF_SIZE;
					}

					/* Need to allocate Picture Parameter buffers */
					uiTempSize = (sizeof(BVENC_VF_sPicParamBuff)) * BDSP_FWMAX_VIDEO_BUFF_AVAIL;

					/* Video FW requires the buffers to be 256 byte alligned for DMA */
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem,uiTempSize, 8, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

					ui32UsedSize = 0;
					for(i=0; i<BDSP_FWMAX_VIDEO_BUFF_AVAIL ; i++)
					{
						hRaagaTaskHandle->psVidEncBuffer->psPPB[i]
									= (BVENC_VF_sPicParamBuff *)(ui32BaseAddr + ui32UsedSize);
						/* convert to offset and pass which will be passed to DSP */
						err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)hRaagaTaskHandle->psVidEncBuffer->psPPB[i], &ui32PhyAddr);
						taskSettings.psVEncoderConfig->sPPBs[i].ui32DramBufferAddress = ui32PhyAddr;
						taskSettings.psVEncoderConfig->sPPBs[i].ui32BufferSizeInBytes = sizeof(BVENC_VF_sPicParamBuff);

						/* Attach the RAW picture buffer with each PPB */
						err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)hRaagaTaskHandle->psVidEncBuffer->ui32RawPicturePtr[i], &ui32PhyAddr);
#ifdef GO_BIT_ENABLED						
						hRaagaTaskHandle->psVidEncBuffer->psPPB[i]->ui32BuffAddrY = ui32PhyAddr;
#else
						hRaagaTaskHandle->psVidEncBuffer->psPPB[i]->ui32BuffAddrY = ui32PhyAddr;
#endif

						ui32UsedSize += sizeof(BVENC_VF_sPicParamBuff);

						BDBG_MSG((" PPB to dsp is 0x%x, Raw picture offset physical address ) is 0x%x", taskSettings.psVEncoderConfig->sPPBs[i].ui32DramBufferAddress, ui32PhyAddr));
					}

					#if 0
					/* memory for PDQ */
					/* Need to PDQ buffer */
					uiTempSize = RAAGA_TEST_VIDEO_MAX_MSGS_PER_QUEUE*4;
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem, uiTempSize, 2, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}
					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
						(ui32RegOffset * RAAGA_TEST_DSP_P_FIFO_PDQ);

					hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32BaseAddr = ui32RegOffset + RAAGA_TEST_FIFO_BASE_OFFSET ;
					hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32EndAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32WriteAddr    = ui32RegOffset + RAAGA_TEST_FIFO_WRITE_OFFSET;
					hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32ReadAddr = ui32RegOffset + RAAGA_TEST_FIFO_READ_OFFSET;
					/* This is not used by F/W */
					hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32WrapAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;

					err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)ui32BaseAddr, &ui32PhyAddr);

					BDBG_ERR(("PDQ: MemHandle = 0x%x, Address = 0x%x Offset = 0x%x", hRaagaTest->hMem, ui32BaseAddr, ui32PhyAddr));
					BREG_Write32(hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32BaseAddr, 
								ui32PhyAddr);				

					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32ReadAddr,
								ui32PhyAddr);

					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32WriteAddr,
								ui32PhyAddr);
					
					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32EndAddr, 
								ui32PhyAddr + (uiTempSize - 1) );
					
					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPDQ.ui32WrapAddr, 
								ui32PhyAddr + (uiTempSize - 1) );	
					
					/* memory for PRQ */
					/* Need to PRQ buffer */
					uiTempSize = RAAGA_TEST_VIDEO_MAX_MSGS_PER_QUEUE*4;
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem, uiTempSize, 2, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}
						
					BKNI_Memset((void *)ui32BaseAddr, 0xAA, uiTempSize);

					/* Init the pPPBs in the queue */
					pPPB	=	(uint32_t *)ui32BaseAddr;
					for(i=0; i<BDSP_FWMAX_VIDEO_BUFF_AVAIL ; i++)
					{
						/* have to pass physical address (offset ) as MIPS address is not understood by Raaga */
						err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)hRaagaTaskHandle->psVidEncBuffer->psPPB[i], &ui32PhyAddr);
						BKNI_Memcpy( (void *)pPPB, &ui32PhyAddr, 4);
						pPPB++;
						/*
						BDBG_ERR(("pPPB Queue Address = 0x%x, written = 0x%x, pPPB address = 0x%x\n",pPPB, *(pPPB - 1),ui32BaseAddr));
						*/
					}

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
						(ui32RegOffset * RAAGA_TEST_DSP_P_FIFO_PRQ);

					hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32BaseAddr = ui32RegOffset + RAAGA_TEST_FIFO_BASE_OFFSET ;
					hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32EndAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32WriteAddr    = ui32RegOffset + RAAGA_TEST_FIFO_WRITE_OFFSET;
					hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32ReadAddr = ui32RegOffset + RAAGA_TEST_FIFO_READ_OFFSET;
					/* This is not used by F/W */
					hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32WrapAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;

					err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)ui32BaseAddr, &ui32PhyAddr);
					if(BERR_SUCCESS != err)
					{
						BDBG_ERR(("Address conversion failed with error %d", err));
					}
					BDBG_ERR(("PRQ: MemHandle = 0x%x, Address = 0x%x Offset = 0x%x", hRaagaTest->hMem, ui32BaseAddr, ui32PhyAddr));					
										
					BREG_Write32(hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32BaseAddr, 
								ui32PhyAddr);				

					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32ReadAddr,
								ui32PhyAddr);

					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32WriteAddr,
								ui32PhyAddr);
					
					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32EndAddr, 
								ui32PhyAddr + (uiTempSize - 1) );
					
					BREG_Write32 (hRaagaTest->hReg,
								hRaagaTaskHandle->psVidEncBuffer->sPRQ.ui32WrapAddr, 
								ui32PhyAddr + (uiTempSize - 1) );	
					err = BMEM_ConvertOffsetToAddress(hRaagaTest->hMem, ui32PhyAddr, &tempAddr);
					if(BERR_SUCCESS != err)
					{
						BDBG_ERR(("Address conversion failed with error %d", err));
					}
					BDBG_ERR(("PRQ: MemHandle = 0x%x, Address = 0x%x Offset = 0x%x", hRaagaTest->hMem, tempAddr, ui32PhyAddr));
					#endif
					/* Output Cdb Buffer Allocation */
					uiTempSize = H264_ENCODE_OUTPUT_CDB_SIZE;
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem, uiTempSize, 2, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
						(ui32RegOffset * H264_ENCODE_OUTPUT_CDB_FIFO_NUM);

					err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)ui32BaseAddr, &ui32PhyAddr);
					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_BASE_OFFSET ;
					BREG_Write32(hRaagaTest->hReg,
								ui32TempRdbAddr, 
								ui32PhyAddr);				

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_READ_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr);

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_WRITE_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr);

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr, 
								ui32PhyAddr + (H264_ENCODE_OUTPUT_CDB_SIZE - 1) );
					
					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr + (H264_ENCODE_OUTPUT_CDB_SIZE - 1) );

					/* Output Itb Buffer Allocation */
					uiTempSize = H264_ENCODE_OUTPUT_ITB_SIZE;
					ui32BaseAddr = (uint32_t) BMEM_AllocAligned(hRaagaTest->hMem, uiTempSize, 2, 0);
					if((void *) 0xFFFFFFFF == (void *)ui32BaseAddr)
					{
						BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory for Video encoder reference buffer!"));
						return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
					}

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
						BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

					ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
						(ui32RegOffset * H264_ENCODE_OUTPUT_ITB_FIFO_NUM);

					err = BMEM_ConvertAddressToOffset(hRaagaTest->hMem, (void *)ui32BaseAddr, &ui32PhyAddr);
					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_BASE_OFFSET ;
					BREG_Write32(hRaagaTest->hReg,
								ui32TempRdbAddr, 
								ui32PhyAddr);				

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_READ_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr);

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_WRITE_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr);

					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr, 
								ui32PhyAddr + (H264_ENCODE_OUTPUT_ITB_SIZE - 1) );
					
					ui32TempRdbAddr = ui32RegOffset + RAAGA_TEST_FIFO_END_OFFSET;
					BREG_Write32 (hRaagaTest->hReg,
								ui32TempRdbAddr,
								ui32PhyAddr + (H264_ENCODE_OUTPUT_ITB_SIZE - 1) );

					/* Do CIT input settings */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 	
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRDB;
					
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 2;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRDB;
					/* RPDQ */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = RAAGA_TEST_DSP_P_FIFO_PDQ;
					/* RPRQ */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[1] = RAAGA_TEST_DSP_P_FIFO_PRQ;

					/* Make adaptive rate invalid */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[0] = -1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[1] = -1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[2] = -1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[3] = -1;
					
					/* Configuring outputs */					
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDistinctOp = 2;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[0] = 1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.ui32NumDst[1] = 1;

					/* Set output type to be eCdb and eItb */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[0] = BDSP_AF_P_DistinctOpType_eCdb;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.eDistinctOpType[1] = BDSP_AF_P_DistinctOpType_eItb;

					/* Configure Cdb Op type */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRDB;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRDB;
					/* CDB output */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[0][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = H264_ENCODE_OUTPUT_CDB_FIFO_NUM;

					/* Configure Itb Op type */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].eType = BDSP_CIT_P_FwStgSrcDstType_eRDB;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers = 1;
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType = BDSP_AF_P_BufferType_eRDB;
					/* ITB output */
					taskSettings.pBranchInfo[0]->sFwStgInfo[0].sStgConnectivity.sDstDetails[1][0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[0] = H264_ENCODE_OUTPUT_ITB_FIFO_NUM;
				}

				BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: eVideoEncodeAlgo=%d",taskSettings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo));
				err = BDSP_Task_Create(hRaagaTestCh->hDspcxt,&taskSettings,&task);
				BDBG_ERR(("Task created"));
				if(err != BERR_SUCCESS)
				{
					BDBG_ERR(("BRAAGA_TEST_OpenRaagaTask: BDSP_Task_Create failed. Error is %d", err)); 
					BDBG_ASSERT( !err );
				}
				hRaagaTaskHandle->task = task;
				hRaagaTaskHandle->bused = true;
				break;
			}            
			else
			{
				BDBG_ERR(("This type of context is not supported by raaga_test"));
				BDBG_ASSERT((0));
			}
		}
	}
	BDBG_ERR(("hTaskHandle=%x",hTaskHandle));
		
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
#if RAAGA_TEST_ENABLE_TRANSPORT		
			if(hTaskHandle->hCtx)
			{
				err = BRAAGA_TEST_CloseRaveContext(hTaskHandle,hRaagaTest->hPcr);
			}
#endif            
			BDSP_Task_Destroy(hTaskHandle->task);
			BKNI_Free(hTaskHandle);
			hRaagaTestCh->hTaskInfo[i] = NULL;
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
#if RAAGA_TEST_ENABLE_TRANSPORT   
	if(hTaskHandle->hCtx)
	{
		if(hRaagaTest->bXptPbEn)
		{
			err = BRAAGA_TEST_PbLib_StopPlay(hRaagaTest->hPbLib);
		}
		err = BRAAGA_TEST_StopRaveContext(hRaagaTest,hTaskHandle);
	}
#endif    
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
#if RAAGA_TEST_ENABLE_TRANSPORT                    
					err = BRAAGA_TEST_StartRaveContext(hRaagaTest, hRaagaTestCh, hTaskHandle, hRaagaTest->hPcr);
					BDBG_ASSERT( !err );
#endif                    
					err = BDSP_Task_Start(hTaskHandle->task);
					if(err != BERR_SUCCESS)
					{
						BDBG_ERR(("BRAAGA_TEST_StartRaagaTask: BDSP_Task_Start failed. Error %d", err)); 
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
		if(bXptPbEn == true)
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
#if 1
	if(BRAAGA_TEST_eTaskType_eEncode == hRaagaTest->hRaagaTestCh[0]->sSettings.eTaskType )
	{
		BRAAGA_TEST_FeederStart( 
	                        hRaagaTest->hFeeder, 
	                        hRaagaTest->sRaagaStrParams[uiStreamId].aStreamName
							);
	}
#endif 
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
#endif
	BDBG_ASSERT(err == BERR_SUCCESS);    	
    BDBG_ERR (("Watchdog callback installed"));

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
#if RAAGA_TEST_ENABLE_TRANSPORT    
    unsigned int        num =0, i=0, j=0;
#endif 
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
    uint32_t    *pItbMemPtr = NULL;

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


    uiCdbSize = file_end - file_start; 

    pItbFile = fopen(hRaagaTest->sRaagaStrParams[eChTaskType].aItbFileName, "rb");
    BDBG_ASSERT(pItbFile);

    file_start = ftell(pItbFile);
    fseek(pItbFile,0,SEEK_END);
    file_end   = ftell(pItbFile);
    fseek(pItbFile,0,SEEK_SET);

    uiItbSize = file_end - file_start; 

    BDBG_ERR(("CDB Size = %d, ITB size = %d", 
               uiCdbSize, uiItbSize));

    /* Allocate CDB memory */
    pCdbMemPtr = BMEM_AllocAligned(hMemory, uiCdbSize,
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;

    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pCdbMemPtr, &cdb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);
    printf("\nLoadStaticCdbItb: cdb_offset = 0x%x", cdb_offset);

    /* Allocate DRAM for ITB */
    pItbMemPtr = BMEM_AllocAligned(hMemory, uiItbSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;
    
    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pItbMemPtr, &itb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);
    BDBG_MSG(("LoadStaticCdbItb: itb_offset = 0x%x\n", itb_offset));

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

	for(ui32ByteCount = uiFrameOffset; ui32ByteCount < 20+uiFrameOffset; ui32ByteCount = ui32ByteCount+4)
	{

		data = BRAAGA_P_DRAMREAD((uint32_t)pCdbMemPtr+ui32ByteCount);

	    BDBG_MSG(("CDB VALUE READ at address %x is %x", (uint32_t)pCdbMemPtr+ui32ByteCount, data));
	}

	BDBG_MSG(("Checking Loaded ITB Values Till 20"));

	for(ui32ByteCount = 0; ui32ByteCount < 20; ui32ByteCount = ui32ByteCount+4)
	{
        data = BRAAGA_P_DRAMREAD((uint32_t)pItbMemPtr+ui32ByteCount);
		BDBG_MSG(("ITB VALUE READ at address %x is %x", (uint32_t)pItbMemPtr+ui32ByteCount, data));
	}

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

    pCdbMemPtr = BMEM_AllocAligned(hMemory, uiCdbSize, 
                			8, /* uiAlignBits, */
                			0 /* uiBoundary */
                			) ;

    err = BMEM_ConvertAddressToOffset(hMemory, (void *)pCdbMemPtr, &cdb_offset);
    BDBG_ASSERT(err==BERR_SUCCESS);

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
    BDBG_MSG(("LoadStaticCdbItb: itb_offset = 0x%x\n", itb_offset));

    /* Write ITB */
    for(ui32ByteCount = 0; ui32ByteCount < uiItbSize; ui32ByteCount = ui32ByteCount+4)
    {
        data = itb_array[ui32ByteCount/4];
        BRAAGA_P_DRAMWRITE((uint32_t)pItbMemPtr + ui32ByteCount, itb_array[ui32ByteCount/4]); 
    }

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
#if RAAGA_TEST_ENABLE_TRANSPORT
								if(hRaagaTest->bXptPbEn)
								{
									err = BRAAGA_TEST_PbLib_StopPlay(hRaagaTest->hPbLib);
								}
								err = BRAAGA_TEST_StopRaveContext(hRaagaTest, hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]);
								BDBG_ASSERT(err == BERR_SUCCESS);
								BDBG_ERR(("BRAAGA_TEST_Uninit: BRAAGA_TEST_StopRaveContext() %d Stopped",
											hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->uiPidChnNo));
#endif
							}
							if(hRaagaTest->hRaagaTestCh[uiCurChId]->sSettings.eTaskType == BRAAGA_TEST_eTaskType_eEncode)
							{
								BDBG_ERR(("Shutting down feeder"));
								BRAAGA_TEST_Feeder_Shutdown(hRaagaTest->hFeeder);
							}
							BDBG_ERR(("Stopping task %d in context number %d", i, uiCurChId));
							err = BDSP_Task_Stop(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task);
							BDBG_ASSERT(err == BERR_SUCCESS);
							hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->bused = false;

							if(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->eDstType == 
											BRAAGA_TEST_TaskDstType_eRaveBuf)
							{
#if RAAGA_TEST_ENABLE_TRANSPORT
				                /* Close RAVE Context */
				    			err = BRAAGA_TEST_CloseRaveContext(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i],
				    			                                  hRaagaTest->hPcr);
				    			BDBG_ASSERT(err == BERR_SUCCESS);
				    			BDBG_ERR(("\nXPT Rave Context %d Closed", uiCurChId));
#endif
							}
						}
						BDBG_ERR(("Destroying task %d in context number %d", i, uiCurChId));
						BDSP_Task_Destroy(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task);
						hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]->task = NULL;
						BKNI_Free(hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i]);
						hRaagaTest->hRaagaTestCh[uiCurChId]->hTaskInfo[i] = NULL;
					}
				}
				hRaagaTest->hRaagaTestCh[uiCurChId]->bRunning = false;
			}
			BDBG_ERR(("Destroying context number %d", uiCurChId));
			BDSP_Context_Destroy(hRaagaTest->hRaagaTestCh[uiCurChId]->hDspcxt);
			BKNI_Free(hRaagaTest->hRaagaTestCh[uiCurChId]);
			hRaagaTest->hRaagaTestCh[uiCurChId] = NULL;
		}
	}
			
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


/* end of file */

