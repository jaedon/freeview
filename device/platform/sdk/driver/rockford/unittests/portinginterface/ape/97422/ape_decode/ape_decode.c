/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ape_decode.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/8/12 2:02a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/ape_decode/ape_decode.c $
 * 
 * Hydra_Software_Devel/4   10/8/12 2:02a jgarrett
 * SW7346-914: Adding enable for ancillary data
 * 
 * Hydra_Software_Devel/3   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-914/3   10/7/12 6:28p jgarrett
 * SW7346-914: Working ancillary data capture
 * 
 * Hydra_Software_Devel/SW7346-914/2   10/4/12 2:55a jgarrett
 * SW7346-914: Adding ancillary data fifo size
 * 
 * Hydra_Software_Devel/SW7346-914/1   9/28/12 10:08a jgarrett
 * SW7346-914: Adding ancillary data parsing
 * 
 * Hydra_Software_Devel/2   8/30/12 4:47p jgarrett
 * SW7425-3629: Fixes for cached access
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 *
 ***************************************************************************/
 
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

#include "apetest_common.h"


/* xpt */
#include "bstd.h"
#include "bxpt.h"

/* APE includes */
#include "bape.h"

/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"

#include "btst_kni.h"
#include "btst.h"
#include "bkni_multi.h"
#include "bkni.h"

BDBG_MODULE(ape_decode);


#define USE_DECODE  (1) /* may support both pcm playback and decode in this app, so don't toss the pcm code yet */


/***************************************************************************
 * main
 ***************************************************************************/
typedef struct APETEST_CmdLineOpts {
    APETEST_CommonOpts common;
    const char  * filename;
    const char  * indexname;
    const char  * capname;
    const char  * ancname;
    bool        showCallbacks;
} APETEST_CmdLineOpts;



typedef struct AppData
{
	APETEST_CmdLineOpts         * pOpts;
    APETEST_BoardInfo           * pBoardInfo;
	BXPT_Rave_Handle            rave;
	BXPT_RaveCx_Handle          ctx;
	BXPT_Handle                 xpt;
    BAPE_Handle                 ape;
    BDSP_Handle                 dsp;
    BAPE_DecoderHandle          decoder;
    BAPE_DacHandle              dac;
    BAPE_MixerHandle            dacMixer;
    BAPE_SpdifOutputHandle      spdif;
    BAPE_MixerHandle            spdifMixer;
    BAPE_MaiOutputHandle        mai;
    BAPE_MixerHandle            maiMixer;
	APETEST_PbLib_Handle        pbLib;
    bool                        decoding;
    bool                        exit;
    APETEST_Capture_Handle      capture;
    BTST_TaskHandle             ancTask;						
    BKNI_EventHandle            ancEvent;	
    FILE                       *pAncFile;
} AppData;



typedef enum MENU_MAIN {
    MENU_MAIN_HELP,
    MENU_MAIN_QUIT,
    MENU_MAIN_EXAMPLE, /* remove this once a real multi param is in use */
    MENU_MAIN_STOP_START
} MENU_MAIN;

static MENU_ENTRY menu_cmd_table_main[] = {
    { "?", MENU_MAIN_HELP, 0,
      "",
      "Help - Print this help information" },
    { "q", MENU_MAIN_QUIT, 0,
      "",
      "Quit" },
    { "example",  MENU_MAIN_EXAMPLE, 2,
      "[param0] [param1]",
      "example - with params" },
    { "s", MENU_MAIN_STOP_START, 0,
      "",
      "stop/start play" },

    { NULL, MENU_INVALID, 0,
      NULL,
      NULL }
};

static void AncDataTask( BTST_TaskHandle Task, void *parm )
{
    BERR_Code errCode;
	AppData *pAppData = parm;
	BSTD_UNUSED( Task );

    while ( !pAppData->exit )
    {
        void *pBuffer;
        size_t bufferSize;
        BAPE_AncillaryDataHeader *pHeader;

        errCode = BKNI_WaitForEvent(pAppData->ancEvent, 100);
        if ( pAppData->exit )
        {
            return;
        }
        do
        {
            errCode = BAPE_Decoder_GetAncillaryDataBuffer(pAppData->decoder, &pBuffer, &bufferSize);
            if ( BERR_SUCCESS == errCode && bufferSize > 0 )
            {
                pHeader = pBuffer;
                if ( pHeader->payloadSize > 0 )
                {
                    fwrite(pHeader+1, pHeader->payloadSize, 1, pAppData->pAncFile);
                    fflush(pAppData->pAncFile);
                }
                (void)BAPE_Decoder_ConsumeAncillaryData(pAppData->decoder, pHeader->blockSize);
            }
        } while ( bufferSize > 0 );
    }
}

static void cmdLinePrintUsage(const char *app)
{
    printf("%s usage:\n", app);
    APETEST_PrintUsageCommon();
    printf(
        "  -playfile      - file to be used as source \n"
        "  -indexfile     - file to be used as ITB index (optional) \n"
        "  -showCB        - print audio callback info \n"
        "  -capFile       - file to capture audio to \n"
        /*        "  -capSrc        - file to capture audio to \n" */
        );
    return;
}

static int cmdLineParse(int argc, const char *argv[], APETEST_CmdLineOpts *pOpts)
{
    int i;
    int rc=-1;
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    memset(pOpts,0,sizeof(*pOpts));

    for (i=0;i<argc;i++)
    {
        rc = APETEST_ParseCommon(i, argc, argv, &pOpts->common, cmdLinePrintUsage);
        if(rc==0) break;
        if(rc<0) return rc;

        /* unknown option */
        i = rc;
        if (!strcmp(argv[i], "-playfile")) {
            pOpts->filename = argv[++i];
        }
        else if (!strcmp(argv[i], "-indexfile")) {
            pOpts->indexname = argv[++i];
        }
        else if (!strcmp(argv[i], "-showCB")) {
            pOpts->showCallbacks = true;
        }
        else if (!strcmp(argv[i], "-capFile")) {
            pOpts->capname = argv[++i];
        }
        else if (!strcmp(argv[i], "-ancFile")) {
            pOpts->ancname = argv[++i];
        }
        /* if no other params assume filename... */
        else if (!pOpts->filename) {
            pOpts->filename = argv[i];
        }
        else {
            printf("unknown parameter: '%s'\n", argv[i]);
            return -1;
        }
    }
    if(rc!=0) {
        APETEST_ParseCommon(i, argc, argv, &pOpts->common, cmdLinePrintUsage); /* allow common parser to execute finishing touches */
    }
            
    /* default -probe for playback if no -video or -audio option */
    if (pOpts->filename && (!pOpts->common.audioPid 
                            || (pOpts->common.audioCodec == APETEST_INVALID) 
                            || (pOpts->common.streamType == APETEST_INVALID))) 
    {
        pOpts->common.probe = true;
    }
    return 0;
}




/* is this a TS stream? */
bool isTs(APETEST_CmdLineOpts *pOpts)
{
    return (pOpts->common.streamType == BAVC_StreamType_eTsMpeg 
        || pOpts->common.streamType == BAVC_StreamType_eTsBDRay);
}



/* audio decode callbacks... */

static void common_cb (const char * fn, void *pParam1, int param2)
{
    AppData * pApp = (AppData *) pParam1;
    if (pParam1 && pApp->pOpts->showCallbacks)
    {   
        printf("\n %s pParam1=%p param2=0x%x \n", fn, pParam1, param2);
    }
}

static void common_cb_tsm (const char * fn, void *pParam1, int param2, const BAPE_DecoderTsmStatus *pTsmStatus)
{
    AppData * pApp = (AppData *) pParam1;
    if (pParam1 && pApp->pOpts->showCallbacks)
    {   
        printf("\n %s pParam1=%p param2=0x%x pTsmStatus=%p\n\n", fn, pParam1, param2, (void *)pTsmStatus);
    }
}

static void cb_firstPts      (void *pP1, int p2, const BAPE_DecoderTsmStatus *pTsm) { common_cb_tsm(__FUNCTION__, pP1, p2, pTsm); }
static void cb_tsmFail       (void *pP1, int p2, const BAPE_DecoderTsmStatus *pTsm) { common_cb_tsm(__FUNCTION__, pP1, p2, pTsm); }
static void cb_tsmPass       (void *pP1, int p2, const BAPE_DecoderTsmStatus *pTsm) { common_cb_tsm(__FUNCTION__, pP1, p2, pTsm); }
static void cb_lock          (void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }
static void cb_statusReady   (void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }
static void cb_modeChange    (void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }
static void cb_bitrateChange (void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }
static void cb_cdbItbOverflow(void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }
static void cb_inputHalted   (void *pP1, int p2) { common_cb(__FUNCTION__, pP1, p2); }

static void cb_sampleRateChange (void *pParam1, int param2, unsigned sampleRate) 
{ 
    if (pParam1 && ((AppData *)pParam1)->pOpts->showCallbacks)
    {
        printf("\n%s pParam1=%p param2=0x%x sampleRate=%d\n", __FUNCTION__, pParam1, param2, sampleRate);
    }
}

static void cb_unlock(void *pParam1, int param2)
{   
    common_cb(__FUNCTION__, pParam1, param2);
    if (pParam1)
    {
        ((AppData *)pParam1)->exit = true;
    }
}

static void cb_ancillaryData (void *pP1, int p2) 
{ 
    common_cb(__FUNCTION__, pP1, p2); 
    BKNI_SetEvent(((AppData *)pP1)->ancEvent); 
}

BERR_Code SetDecodeCallbacks( AppData * pApp )
{
    BAPE_DecoderInterruptHandlers interrupts;

    BAPE_Decoder_GetInterruptHandlers(pApp->decoder, &interrupts);

    interrupts.unlock.pCallback_isr             = cb_unlock;
    interrupts.unlock.pParam1                   = pApp;

    interrupts.firstPts.pCallback_isr           = cb_firstPts;
    interrupts.firstPts.pParam1                 = pApp;
    interrupts.tsmFail.pCallback_isr            = cb_tsmFail;
    interrupts.tsmFail.pParam1                  = pApp;
    interrupts.tsmPass.pCallback_isr            = cb_tsmPass;
    interrupts.tsmPass.pParam1                  = pApp;
    interrupts.sampleRateChange.pCallback_isr   = cb_sampleRateChange;
    interrupts.sampleRateChange.pParam1         = pApp;
    interrupts.lock.pCallback_isr               = cb_lock;
    interrupts.lock.pParam1                     = pApp;
    interrupts.statusReady.pCallback_isr        = cb_statusReady;
    interrupts.statusReady.pParam1              = pApp;
    interrupts.modeChange.pCallback_isr         = cb_modeChange;
    interrupts.modeChange.pParam1               = pApp;
    interrupts.bitrateChange.pCallback_isr      = cb_bitrateChange;
    interrupts.bitrateChange.pParam1            = pApp;
    interrupts.cdbItbOverflow.pCallback_isr     = cb_cdbItbOverflow;
    interrupts.cdbItbOverflow.pParam1           = pApp;
    interrupts.inputHalted.pCallback_isr        = cb_inputHalted;
    interrupts.inputHalted.pParam1              = pApp;
    interrupts.ancillaryData.pCallback_isr      = cb_ancillaryData;
    interrupts.ancillaryData.pParam1            = pApp;

    RET_ERR( BAPE_Decoder_SetInterruptHandlers(pApp->decoder, &interrupts) );
    
    return BERR_SUCCESS;
}



#define PID_CHANNEL		( 0 )
#define RAVE_CHNL_NUM	( 0 )
/* Playback an encoded audio stream */
BERR_Code StartDecode( AppData * pApp )
{
	BXPT_Rave_ChannelSettings RaveDefaults;
	BXPT_Rave_AvSettings AvCtxCfg;
    BAVC_XptContextMap ctxMap;
    BAPE_DecoderStartSettings startSettings;
	BAVC_CdbItbConfig CdbItb;
	BXPT_Playback_PacketizeConfig PacketCfg;

    if (pApp->decoding)
    {
        printf("already decoding \n");
        return BERR_SUCCESS;
    }

    /* PbLib will read the file and push the data to the XPT, init it */
	pApp->pbLib = APETEST_PbLib_Init( pApp->xpt, 0 );
    RET_NULL( pApp->pbLib );

    if (pApp->capture)
    {
        RET_ERR( APETEST_Capture_Start( pApp->capture, pApp->pOpts->capname) );
    }


    /* get the cdbitb parms from decoder */
    BAPE_Decoder_GetDefaultCdbItbConfig(pApp->decoder, &CdbItb);
    
    /* open/alloc xpt, get ctx for decoder*/
	RET_ERR( BXPT_Rave_GetChannelDefaultSettings( pApp->xpt, RAVE_CHNL_NUM, &RaveDefaults ) );
	RET_ERR( BXPT_Rave_OpenChannel( pApp->xpt, &pApp->rave, RAVE_CHNL_NUM, &RaveDefaults ) );
	RET_ERR( BXPT_Rave_AllocContext( pApp->rave, BXPT_RaveCx_eAv, &CdbItb, &pApp->ctx ) );
	RET_ERR( BXPT_Rave_AddPidChannel( pApp->ctx, PID_CHANNEL, false ) );
    RET_ERR( BXPT_Rave_GetContextRegisters( pApp->ctx, &ctxMap) );

    /* convert opts to AvCtx params */
    RET_ERR( APETEST_OptsToRaveSettings( pApp->ctx, &pApp->pOpts->common, &AvCtxCfg); );
	RET_ERR( BXPT_Rave_SetAvConfig( pApp->ctx, &AvCtxCfg ) ); 
	RET_ERR( BXPT_Rave_EnableContext( pApp->ctx ) );

    /* configure/enable pid decoder */
	RET_ERR( BXPT_ConfigurePidChannel( pApp->xpt, 0, pApp->pOpts->common.audioPid, BXPT_PB_PARSER( 0 ) ) ); 
	RET_ERR( BXPT_EnablePidChannel( pApp->xpt, 0 ) );

    /* configure playback packetizer for ES/TS */
    BXPT_Playback_GetPacketizerDefaults(&PacketCfg);
    PacketCfg.Pid = pApp->pOpts->common.audioPid;
    PacketCfg.ChannelNum = PID_CHANNEL;
    PacketCfg.PacketizerMode = BXPT_Playback_PacketizerMode_Es;
    RET_ERR( BXPT_Playback_PacketizeStream( APETEST_PbLib_GetPbHandle( pApp->pbLib ), 0, &PacketCfg, !isTs(pApp->pOpts) ) );

    /* config/start audio decoder */
    BAPE_Decoder_GetDefaultStartSettings( &startSettings);
    startSettings.codec =  pApp->pOpts->common.audioCodec;
    startSettings.streamType =  pApp->pOpts->common.streamType;
    startSettings.pContextMap = &ctxMap;
    RET_ERR( BAPE_Decoder_Start(  pApp->decoder, &startSettings) );

    RET_ERR( SetDecodeCallbacks( pApp ) );

    /* choose lib playback based on ES/TS, playback lib sets PacketLength */
    if (isTs(pApp->pOpts))
    {
	    APETEST_PbLib_DoPlayNonBlocking( pApp->pbLib, (char *) pApp->pOpts->filename, BXPT_PB_SYNC_MPEG_BLIND );
    }
    else
    {
	    APETEST_PbLib_DoEsPlayNonBlocking( pApp->pbLib, (char *) pApp->pOpts->filename );
    }

    pApp->decoding = true;

	printf( "StartDecode()\n" );
    return BERR_SUCCESS;
}

BERR_Code StopDecode( AppData * pApp )
{
    if (!pApp->decoding)
    {
        printf("already stopped \n");
        return BERR_SUCCESS;
    }

    pApp->decoding = false;

    APETEST_PbLib_StopPlay( pApp->pbLib );
    if (pApp->capture)
    {
        APETEST_Capture_Stop( pApp->capture );
    }

    BAPE_Decoder_Stop(  pApp->decoder );

    APETEST_PbLib_Shutdown(pApp->pbLib);
	BXPT_Rave_RemovePidChannel( pApp->ctx, 0 );
  	BXPT_Rave_DisableContext( pApp->ctx );
  	BXPT_Rave_FreeContext( pApp->ctx );
	BXPT_Rave_CloseChannel( pApp->rave );

	printf( "StopDecode()\n" );
    return BERR_SUCCESS;
}


BERR_Code OpenResources( AppData * pApp )
{
    APETEST_HDMITX_Settings     hdmitxSettings;
    BAPE_Settings               apeSettings;
    BDSP_RaagaSettings          raagaSettings;
    BXPT_DefaultSettings        xptSettings;
    BAPE_DecoderOpenSettings    decoderOpenSettings;
    BAPE_OutputPort             outputPort;
    BAPE_Connector              connector;
    unsigned                    index = 0; /* current test is to open the 0th decoder */
    BFramework_Info         *   pFrameInfo  = &pApp->pBoardInfo->frmInfo;
    APETEST_CmdLineOpts     *   pOpts       = pApp->pOpts;




    BDSP_Raaga_GetDefaultSettings(&raagaSettings);
    RET_ERR( BDSP_Raaga_Open(&pApp->dsp, pFrameInfo->hChp, pFrameInfo->hReg, pFrameInfo->hMem, 
                                    pFrameInfo->hInt, pFrameInfo->hTmr, &raagaSettings) );
    
    if (!BDSP_Raaga_IsAudioTypeSupported(BavcAudio2BdspAudio(pOpts->common.audioCodec)))
    {
        printf("pOpts->common.audioCodec=%s not supported\n",  GetStr_BAVC_AudioCompressionStd(pOpts->common.audioCodec));
        return BERR_INVALID_PARAMETER;
    }

    /* Open APE PI */
    BAPE_GetDefaultSettings(&apeSettings);
    RET_ERR( BAPE_Open(&pApp->ape, pFrameInfo->hChp, pFrameInfo->hReg, pFrameInfo->hMem, 
                            pFrameInfo->hInt, pFrameInfo->hTmr, pApp->dsp, &apeSettings) );

    /* Open XPT */
	BXPT_GetDefaultSettings( &xptSettings, pFrameInfo->hChp );
	RET_ERR( BXPT_Open( &pApp->xpt, pFrameInfo->hChp, pFrameInfo->hReg, pFrameInfo->hMem, pFrameInfo->hInt, &xptSettings ) );

    /* Open APE decoder */
    BAPE_Decoder_GetDefaultOpenSettings(&decoderOpenSettings);
    if ( pOpts->ancname != NULL )
    {
        decoderOpenSettings.ancillaryDataFifoSize = 32768;
    }
    RET_ERR( BAPE_Decoder_Open(pApp->ape, index, &decoderOpenSettings, &pApp->decoder) );
    BAPE_Decoder_GetConnector(pApp->decoder, BAPE_ConnectorFormat_eStereo, &connector);
    if ( pOpts->ancname != NULL )
    {
        BAPE_DecoderSettings decoderSettings;
        BAPE_Decoder_GetSettings(pApp->decoder, &decoderSettings);
        decoderSettings.ancillaryDataEnabled = true;
        RET_ERR( BAPE_Decoder_SetSettings(pApp->decoder, &decoderSettings) );
    }

    /* Open DAC and mixer */
    RET_ERR( BAPE_Dac_Open(pApp->ape, 0, NULL, &pApp->dac) );
    RET_ERR( BAPE_Mixer_Create(pApp->ape, NULL, &pApp->dacMixer) ); 

    /* Open SPDIF and mixer */
    RET_ERR( BAPE_SpdifOutput_Open(pApp->ape, 0, NULL, &pApp->spdif) );
    RET_ERR( BAPE_Mixer_Create(pApp->ape, NULL, &pApp->spdifMixer) ); 

    /* Open MAI and mixer */
    RET_ERR( BAPE_MaiOutput_Open(pApp->ape, 0, NULL, &pApp->mai) );
    RET_ERR( BAPE_Mixer_Create(pApp->ape, NULL, &pApp->maiMixer)); 

    /* Init HDMITX */
    APETEST_HDMITX_GetDefaultSettings(&hdmitxSettings);
    RET_ERR( APETEST_HDMITX_Init(pFrameInfo, &hdmitxSettings, pApp->mai) );

    /* Add playback/decoder as input to each mixer */
    RET_ERR( BAPE_Mixer_AddInput(pApp->dacMixer, connector, false) );
    RET_ERR( BAPE_Mixer_AddInput(pApp->spdifMixer, connector, false) );
    RET_ERR( BAPE_Mixer_AddInput(pApp->maiMixer, connector, false) );

    /* Link DAC to DAC mixer */
    BAPE_Dac_GetOutputPort(pApp->dac, &outputPort);
    RET_ERR( BAPE_Mixer_AddOutput(pApp->dacMixer, outputPort) );

    /* Link SPDIF to SPDIF mixer */
    BAPE_SpdifOutput_GetOutputPort(pApp->spdif, &outputPort);
    RET_ERR( BAPE_Mixer_AddOutput(pApp->spdifMixer, outputPort) );

    /* Link MAI to MAI mixer */
    BAPE_MaiOutput_GetOutputPort(pApp->mai, &outputPort);
    RET_ERR( BAPE_Mixer_AddOutput(pApp->maiMixer, outputPort) );

    /* capture file */
    if (pOpts->capname != NULL)
    {
        RET_ERR( APETEST_Capture_Open( pFrameInfo->hMem, pApp->ape, &pApp->capture, pApp->maiMixer ));
    }
    if ( pOpts->ancname != NULL )
    {
        pApp->pAncFile = fopen(pOpts->ancname, "wb+");
        if ( NULL == pApp->pAncFile )
        {
            return BERR_TRACE(BERR_OS_ERROR);
        }
        RET_ERR( BKNI_CreateEvent(&pApp->ancEvent) );
        RET_ERR( BTST_CreateTask(&pApp->ancTask, AncDataTask, pApp) );
    }

    return BERR_SUCCESS;
}

void CloseResources(AppData * pApp)
{
    if (pApp == NULL)
    {
        printf("pApp == NULL\n");
        return;
    }

    if ( pApp->ancTask )
    {
        pApp->exit = true;
        BKNI_SetEvent(pApp->ancEvent);
        BKNI_Sleep(10);
        BTST_DestroyTask(pApp->ancTask);
        BKNI_DestroyEvent(pApp->ancEvent);
        fclose(pApp->pAncFile);
    }

    APETEST_HDMITX_Uninit();

    /* Teardown Connections */
    /* Close Resource Handles */
    if (pApp->capture)
    {
        APETEST_Capture_Close( pApp->capture );
    }
    if (pApp->dacMixer)
    {
        BAPE_Mixer_RemoveAllInputs(pApp->dacMixer);
        BAPE_Mixer_RemoveAllOutputs(pApp->dacMixer);
        BAPE_Mixer_Destroy(pApp->dacMixer);
    }
    if (pApp->spdifMixer)
    {
        BAPE_Mixer_RemoveAllInputs(pApp->spdifMixer);
        BAPE_Mixer_RemoveAllOutputs(pApp->spdifMixer);
        BAPE_Mixer_Destroy(pApp->spdifMixer);
    }
    if (pApp->maiMixer)
    {
        BAPE_Mixer_RemoveAllInputs(pApp->maiMixer);
        BAPE_Mixer_RemoveAllOutputs(pApp->maiMixer);
        BAPE_Mixer_Destroy(pApp->maiMixer);
    }
    if (pApp->dac)
    {
        BAPE_Dac_Close(pApp->dac);
    }
    if (pApp->spdif)
    {
        BAPE_SpdifOutput_Close(pApp->spdif);
    }
    if (pApp->mai)
    {
        BAPE_MaiOutput_Close(pApp->mai);
    }
    if (pApp->decoder)
    {
        BAPE_Decoder_Close(pApp->decoder);
    }
    if (pApp->xpt)
    {
  	    BXPT_Close(pApp->xpt);
    }
    if (pApp->ape)
    {
        /* Shutdown system */
        BAPE_Close(pApp->ape);
    }
    if (pApp->dsp)
    {
        BDSP_Close(pApp->dsp);
    }

    return;
}

void DoMainMenu(AppData * pApp)
{
    MENU_MAIN   menuCmd;
    MENU_ARGS   cmdArgs;

    while (!pApp->exit)
    {
        if (APETEST_MenuGetCmd( &pApp->exit, menu_cmd_table_main, &menuCmd, &cmdArgs))
        {
            switch (menuCmd)
            {
                case MENU_MAIN_HELP:
                    APETEST_MenuPrintHelp(menu_cmd_table_main);
                    break;

                case MENU_MAIN_QUIT:
                    pApp->exit = true;
                    break;
                case MENU_MAIN_STOP_START:
                    if (pApp->decoding)
                    {
                        StopDecode(pApp);
                    }
                    else
                    {
                        StartDecode(pApp);
                    }
                    break;
                case MENU_MAIN_EXAMPLE:
                    {
                        int i;
                        printf("Test argc=%d", cmdArgs.argc);
                        for (i=0; i< cmdArgs.argc; i++)
                        {
                            printf("  argv[i]=%s", cmdArgs.argv[i]);
                        }
                        printf("\n");
                    }
                    break;
                default:
                    printf("shouldn't be here menuCmd=%d\n", menuCmd);
                    break;
            }
        }
    }
}


int app_main(int argc, char **argv) {
    APETEST_BoardInfo       boardInfo;
	APETEST_CmdLineOpts     opts;
    AppData                 appData;
    
    memset(&boardInfo,0,sizeof(boardInfo));
    memset(&opts,0,sizeof(opts));
    memset(&appData,0,sizeof(appData));
    

    appData.pOpts = &opts;
    appData.pBoardInfo = &boardInfo;

    /* init */
    RET_ERR( APETEST_Board_Init(argc, argv, &boardInfo) );

    if (cmdLineParse(argc,(const char **)argv, &opts)) {
        return 0;
    }
    if (APETEST_Probe(&opts.common, opts.filename, &opts.indexname)) {
        return 1;
    }


    /* check audio PID for the decode */
    if (!opts.common.audioPid) {
        BDBG_ERR(("audio pid specification; See usage."));
        cmdLinePrintUsage(argv[0]);
        return 1;
    }

    /* print the current test params */
    printf( "\n\tfile = %s\n\tstreamType = %s\n\taudioCodec = %s\n\tpid = 0x%x\n\n", 
        opts.filename, GetStr_BAVC_StreamType(opts.common.streamType), GetStr_BAVC_AudioCompressionStd(opts.common.audioCodec), opts.common.audioPid );


    RET_ERR( OpenResources(&appData) );
    RET_ERR( StartDecode(&appData) );

    DoMainMenu( &appData );


    RET_ERR( StopDecode(&appData) );
    CloseResources(&appData);

	/* terminate */
    APETEST_Board_UnInit(&boardInfo);

    return 0;

}
