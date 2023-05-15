/**************************** Include Files**********************************/ 
#include "framework.h"

/* Linux stuff */
#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>


/* APE includes */
#include "bxpt.h"
#include "bxpt_rave.h"
#include "bxpt_pcr_offset.h"
#include "bxpt_pcr.h"
#include "bdsp.h"
#include "bdsp_raaga.h"
#include "bape.h"

BDBG_MODULE(ape_test);

#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552)
#define BAPE_TEST_XPT_INPUTBAND 2
#elif (BCHP_CHIP == 35233)
#define BAPE_TEST_XPT_INPUTBAND 3
#elif (BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7231)
#define BAPE_TEST_XPT_INPUTBAND 5
#elif (BCHP_CHIP == 7344)
#define BAPE_TEST_XPT_INPUTBAND 0
#else
#warning Input band for streamer not configured.  Defaulting to 0.
#define BAPE_TEST_XPT_INPUTBAND 0
#endif

/* 
   This app performs a simple live decode of AC3 PID 0x14 with TSM disabled from the streamer.
   It will switch between PCM and compressed on SPDIF each time enter is pressed.
*/

/*******************************************************************/
/*******************************************************************/
int app_main(int argc, char **argv)
{
    int             iErr;

	BSystem_Info    sysInfo;
    BFramework_Info frmInfo;
    BERR_Code       errCode, ret;
    BAPE_Settings               apeSettings;
    BAPE_Handle                 ape;
    BAPE_DecoderHandle          decoder;
    BAPE_SpdifOutputHandle      spdif;
    BAPE_MixerHandle            spdifMixer;
    BAPE_Connector              connector;
    BAPE_OutputPort             outputPort;
    BXPT_Handle                 hXpt;  
    BXPT_Rave_Handle            hRave;  
    BXPT_PCR_Handle             hPcr; 
    BXPT_InputBandConfig        IbConfig;
    BXPT_DefaultSettings        xptSettings;
    BXPT_Rave_ChannelSettings   raveChannelSettings;
    BDSP_Handle                 hDsp;
    BXPT_Rave_AvSettings        AvCtxCfg;
    BXPT_PCR_DefSettings        pcrSettings;
    BXPT_RaveCx_Handle          hRaveCtx;
    BAVC_CdbItbConfig           raveConfig;
    BAVC_XptContextMap          CtxMap;
    BDSP_RaagaSettings          raagaSettings;
    BAPE_DecoderStartSettings   decStartSettings;

    iErr = BSystem_Init( argc, argv, &sysInfo );
    if ( iErr )
    {
        printf( "System init FAILED!\n" );
        return iErr;
    }

    iErr = BFramework_Init( &sysInfo, &frmInfo );
    if ( iErr )
    {
        printf( "Framework init FAILED!\n" );
        return iErr;
    }

    /* Open XPT */
    BXPT_GetDefaultSettings(&xptSettings, frmInfo.hChp);
    errCode = BXPT_Open(&hXpt, 
                        frmInfo.hChp, 
                        frmInfo.hReg, 
                        frmInfo.hMem, 
                        frmInfo.hInt, 
                        &xptSettings);
    BDBG_ASSERT(!errCode);

    BXPT_GetInputBandConfig(hXpt, BAPE_TEST_XPT_INPUTBAND, &IbConfig);
    IbConfig.ParallelInputSel = false;
    IbConfig.ForceValid= true;        
    IbConfig.SyncDetectEn = true;
    BXPT_SetInputBandConfig(hXpt, BAPE_TEST_XPT_INPUTBAND, &IbConfig );

    /* Enable Input band 0 and Parser band 0 */
    ret = BXPT_SetParserDataSource( hXpt, 0, BXPT_DataSource_eInputBand, BAPE_TEST_XPT_INPUTBAND);
    BDBG_ASSERT( !ret );

    ret = BXPT_SetParserEnable( hXpt, 0, true );
    BDBG_ASSERT( !ret );

    /* Open RAVE */
    ret = BXPT_Rave_GetChannelDefaultSettings(hXpt, 0, &raveChannelSettings );
    BDBG_ASSERT( !ret );
    ret = BXPT_Rave_OpenChannel(hXpt, &hRave, 0, &raveChannelSettings );
    BDBG_ASSERT( !ret );

    BXPT_PCR_GetChannelDefSettings(hXpt, 0, &pcrSettings);   
    ret = BXPT_PCR_Open(hXpt, 0, &pcrSettings, &hPcr);
    BDBG_ASSERT( !ret );

    /* Open DSP */
    BDSP_Raaga_GetDefaultSettings(&raagaSettings);
    errCode = BDSP_Raaga_Open(&hDsp, 
                        frmInfo.hChp, 
                        frmInfo.hReg, 
                        frmInfo.hMem, 
                        frmInfo.hInt, 
                        frmInfo.hTmr, 
                        &raagaSettings);
    BDBG_ASSERT( !errCode );

    /* Open APE PI */
    BAPE_GetDefaultSettings(&apeSettings);
    errCode = BAPE_Open(&ape, 
                        frmInfo.hChp, 
                        frmInfo.hReg, 
                        frmInfo.hMem, 
                        frmInfo.hInt, 
                        frmInfo.hTmr, 
                        hDsp,
                        &apeSettings);

    /* Open Decoder */
    errCode = BAPE_Decoder_Open(ape, 0, NULL, &decoder);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    {
        /* Disable TSM */
        BAPE_DecoderTsmSettings tsmSettings;
        BAPE_Decoder_GetTsmSettings(decoder, &tsmSettings);
        tsmSettings.tsmEnabled = false;
        BAPE_Decoder_SetTsmSettings(decoder, &tsmSettings);
    }
    
    /* Open RAVE Context */
    BAPE_Decoder_GetDefaultCdbItbConfig(decoder, &raveConfig);
    ret = BXPT_Rave_AllocContext(hRave, BXPT_RaveCx_eAv, &raveConfig, &hRaveCtx);
    BDBG_ASSERT( !ret );
    BXPT_Rave_GetAvConfig(hRaveCtx, &AvCtxCfg);
    AvCtxCfg.InputFormat = BAVC_StreamType_eTsMpeg;
    AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;            /* CDB Format is ES */
    BKNI_Memset(AvCtxCfg.EsRanges, 0, sizeof(AvCtxCfg.EsRanges)); /* all disabled */    
    AvCtxCfg.ItbFormat = BAVC_ItbEsType_eAc3gAudio;  
    AvCtxCfg.StreamIdHi = 0xEF;         /* Stream ID hi = 0xEF */
    AvCtxCfg.StreamIdLo = 0xBD;         /* Stream ID lo = 0xBD */
    AvCtxCfg.BandHoldEn = false;
    ret = BXPT_Rave_SetAvConfig(hRaveCtx, &AvCtxCfg ); 
    BDBG_ASSERT( !ret );
    ret = BXPT_Rave_GetContextRegisters(hRaveCtx, &CtxMap );
    BDBG_ASSERT( !ret );

    /* Open Audio PID (0x14) */
    BXPT_ConfigurePidChannel(hXpt, 0, 0x14, 0);
    BXPT_EnablePidChannel(hXpt, 0);
    BXPT_Rave_AddPidChannel(hRaveCtx, 0, false);

    /* Open SPDIF and mixer */
    errCode = BAPE_SpdifOutput_Open(ape, 0, NULL, &spdif);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_Mixer_Create(ape, NULL, &spdifMixer); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Link SPDIF to mixer */
    BAPE_SpdifOutput_GetOutputPort(spdif, &outputPort);
    errCode = BAPE_Mixer_AddOutput(spdifMixer, outputPort);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_Decoder_GetDefaultStartSettings(&decStartSettings);
    decStartSettings.codec = BAVC_AudioCompressionStd_eAc3;
    decStartSettings.pContextMap = &CtxMap;
    for ( ;; )
    {
        #if 1
        fprintf(stderr, "Configuring for PCM on SPDIF\n");
        /* Send PCM data to SPDIF */
        BAPE_Decoder_GetConnector(decoder, BAPE_ConnectorFormat_eStereo, &connector);
        BAPE_Mixer_RemoveAllInputs(spdifMixer);
        errCode = BAPE_Mixer_AddInput(spdifMixer, connector, false);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        fprintf(stderr, "Starting decoder\n");
        ret = BAPE_Decoder_Start(decoder, &decStartSettings);
        BDBG_ASSERT( !ret );
        ret = BXPT_Rave_EnableContext(hRaveCtx); 
        BDBG_ASSERT( !ret );

        fprintf(stderr, "Press Enter to Switch to Compressed\n");
        getchar();
        BXPT_Rave_DisableContext(hRaveCtx);
        BAPE_Decoder_Stop(decoder);
#endif
        fprintf(stderr, "Configuring for AC3 on SPDIF\n");
        /* Send AC3 data to SPDIF */
        BAPE_Decoder_GetConnector(decoder, BAPE_ConnectorFormat_eCompressed, &connector);
        BAPE_Mixer_RemoveAllInputs(spdifMixer);
        errCode = BAPE_Mixer_AddInput(spdifMixer, connector, false);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        fprintf(stderr, "Starting decoder\n");
        ret = BAPE_Decoder_Start(decoder, &decStartSettings);
        BDBG_ASSERT( !ret );
        ret = BXPT_Rave_EnableContext(hRaveCtx); 
        BDBG_ASSERT( !ret );

        fprintf(stderr, "Press Enter to Switch to PCM\n");
        getchar();
        BXPT_Rave_DisableContext(hRaveCtx);
        BAPE_Decoder_Stop(decoder);
    }

    BFramework_Uninit(&frmInfo);
    BSystem_Uninit(&sysInfo);

    printf("Done\n");
    return 0;
}

