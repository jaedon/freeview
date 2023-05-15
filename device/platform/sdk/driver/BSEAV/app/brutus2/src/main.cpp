/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: main.cpp $
* $brcm_Revision: 36 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/main.cpp $
* 
* 36   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 35   11/1/12 6:50p tokushig
* SW7231-749: unlink all windows directly decendended from the bwin
* framebuffer.  for some reason these windows are not unlinked
* automatically and will result in an assert when the bwin framebuffer
* is closed.
* 
* 34   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 33   10/11/12 4:06p tokushig
* SW7231-749: enable record notifications in main screen
* 
* 32   10/10/12 8:38p tokushig
* SW7231-749: remove dead code
* 
* 31   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 30   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 29   10/7/12 3:24p tokushig
* SW7231-749: add support for auto format
* 
* 28   9/26/12 4:49p tokushig
* SW7231-749: fix hdmi output bug where output was never added to simple
* audio decoder
* 
* 27   9/21/12 8:35p tokushig
* SW7231-749: add channelmgr to model and update decode close calls
* 
* 26   9/15/12 1:13a tokushig
* SW7231-749: checkout/open graphics resource
* 
* 25   9/10/12 6:09p tokushig
* SW7231-749: update initialize method to setup brutus2 using simple
* audio/video decoders
* 
* 24   8/27/12 10:15a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 23   7/24/12 3:19p tokushig
* SW7231-749: fix ofdm tuner discovery and notifications setup
* 
* 22   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 21   5/25/12 9:28a jrubio
* SW7346-644: add HDMI audio
* 
* 20   5/23/12 5:24p jrubio
* SW7346-644: add hdmi support
* 
* 19   5/10/12 3:40p tokushig
* SW7231-749: add support for playback notifications.
* 
* 18   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 17   4/18/12 5:16p jrubio
* SW7346-644: add refresh playback list from disk
* 
* 16   4/15/12 9:48p tokushig
* SW7231-749: add start/stop decode notification
* 
* 15   4/13/12 11:42a jrubio
* SW7346-644: remove source.h
* 
* 14   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 13   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 12   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 11   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 10   3/19/12 5:27p tokushig
* SW7405-5581: add config entries to enable channel scan to save
* encrypted channels, audio only channels, and duplicate channels
* 
* 9   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 8   2/28/12 2:52p tokushig
* SW7405-5581: remove debug comments
* 
* 7   2/23/12 5:54p tokushig
* SW7405-5581: rename/move platform specific file to
* platform/(PLATFORM)/brutus_platform.h
* 
* 6   2/22/12 2:48p tokushig
* SW7405-5581: replace bwin io calls with bwidget io calls.
* 
* 6   2/22/12 2:46p tokushig
* SW7405-5581: replace bwin io calls with bwidget io calls.
* 
* 6   2/22/12 2:38p tokushig
* SW7405-5581: replace bwin io calls with bwidget io calls.
* 
* 5   2/20/12 4:48p jrubio
* SW7346-644: read file from config
* 
* 4   2/14/12 3:49p tokushig
* SW7405-5581: disable some debugging printouts
* 
* 3   2/12/12 5:20p tokushig
* SW7405-5581: add first tune support in idle loop
* 
* 2   1/20/12 5:56p jrubio
* SW7346-644: modify for 7346
* 
* 1   1/12/12 7:20p tokushig
* SW7405-5581: Brutus2 initial check in
* 
* 1   1/10/12 8:14p tokushig
* Initial check-in:
*
***************************************************************************/
#include "bstd.h"

#ifndef _WIN32_WCE
    #include <stdio.h>
    #include <signal.h>
#endif

#ifdef STACK_TRACE
#include <sys/resource.h>
#include "brcm_sig_hdl.h"
#endif

/* brutus2 includes */
#include "bwidgets.h"
#include "brutus.h"
#include "brutus_os.h"
#include "brutus_cfg.h"
#include "notification.h"
#include "brutus_platform.h"
#include "model.h"
#include "view.h"
#include "brutus_board.h"
#include "pidmgr.h"
#include "resource.h"
#include "config.h"
#include "channelmgr.h"
#include "control.h"
#include "brutus_lua.h"

BDBG_MODULE(brutus_main);

B_ThreadHandle       gSchedulerThread;
B_MutexHandle        gLock;
B_SchedulerHandle    gScheduler;

/**
main.cpp must be recompiled every time in order to make sure this
function returns the last build date. Other files can use this
and so don't have to be recompiled every time.
**/
const char *buildDate() {
    return __DATE__ " " __TIME__;
}


/* globals */
CModelConfig g_modelConfig;


#define STACKGUARD_FIX() \
    uint8_t stackReserve[8192]; \
    BKNI_Memset(stackReserve, 0, sizeof(stackReserve)); 

eRet initializeNexus()
{
    eRet                     ret                    = eRet_Ok;
    NEXUS_Error              nError                 = NEXUS_SUCCESS;
    const char             * envMaxHdDisplayFormat  = NEXUS_GetEnv("max_hd_display_format");
    NEXUS_PlatformSettings   platformSettings;

    STACKGUARD_FIX() /* DTT do we really need this? */

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    /* DTT make modification to platformSettings based on config */

    /* initializing the frontend is slow so we'll do this after we have everything else in
       the app up and running */
    /*platformSettings.openFrontend = false;*//* dtt todo - change to false and then init in idle loop */

    /* DTT todo add dac bandgap support */

    if (envMaxHdDisplayFormat) 
    {
        if (!strcasecmp(envMaxHdDisplayFormat, "1080p")) 
        {
            if(platformSettings.displayModuleSettings.fullHdBuffers.count)
            { 
               platformSettings.displayModuleSettings.fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
            }
            else
            {  
               unsigned primaryDisplayHeapIndex = platformSettings.displayModuleSettings.primaryDisplayHeapIndex;
               platformSettings.displayModuleSettings.displayHeapSettings[primaryDisplayHeapIndex].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
            }
        }
    }

    nError = NEXUS_Platform_Init(&platformSettings);
    CHECK_NEXUS_ERROR_ASSERT("error calling NEXUS_PLatformInit()", nError);

    return ret;
}

void uninitializeNexus()
{
    NEXUS_Platform_Uninit();
}

static void runScheduler(void *pParam)
{
    B_SchedulerHandle scheduler = (B_SchedulerHandle)pParam;
    B_Scheduler_Run(scheduler);
}

eRet initializeOsLib()
{
    eRet             ret      = eRet_Ok;
    NEXUS_Error      nError   = NEXUS_SUCCESS;

    nError = B_Os_Init();
    CHECK_NEXUS_ERROR_GOTO("Failed to initialize os library", ret, nError, err_os);

    gLock = B_Mutex_Create(NULL);
    CHECK_PTR_GOTO("Failed to create mutex", gLock, ret, eRet_ExternalError, err_mutex);

    gScheduler = B_Scheduler_Create(NULL);
    CHECK_PTR_GOTO("failed to create scheduler", gScheduler, ret, eRet_ExternalError, err_scheduler);

    /* run scheduler with thread */
    {
        B_ThreadSettings settings;

        B_Thread_GetDefaultSettings(&settings);
        settings.terminate = false;

        gSchedulerThread = B_Thread_Create("b_event", runScheduler, gScheduler, &settings);
        CHECK_PTR_GOTO("failed to create scheduler thread", gSchedulerThread, ret, eRet_ExternalError, err_thread);

        /* yield so sheduler can get up and running */
        B_Thread_Sleep(1);
    }

    goto done;

err_thread:
    B_Scheduler_Stop(gScheduler);
    B_Scheduler_Destroy(gScheduler);
err_scheduler:
    B_Mutex_Destroy(gLock);
err_mutex:
    B_Os_Uninit();
err_os:
done:
    return ret;
}

void uninitializeOsLib()
{
    if (gScheduler)
    {
        B_Scheduler_Stop(gScheduler);
        B_Scheduler_Destroy(gScheduler);
    }

    if (gSchedulerThread)
    {
        B_Thread_Destroy(gSchedulerThread);
    }

    if (gLock)
    {
        B_Mutex_Destroy(gLock);
    }

    B_Os_Uninit();
}


eRet initializeBrutus2()
{
    eRet ret = eRet_Ok;
    
    BDBG_WRN(("*** initialize nexus"));
    ret = initializeNexus();
    CHECK_ERROR_GOTO("Nexus failed to initialize", ret, err_initNexus);

    BDBG_WRN(("*** initialize os lib"));
    ret = initializeOsLib();
    CHECK_ERROR_GOTO("OS lib failed to initialize", ret, err_initOsLib);
    
    goto done;

err_initConfig:
    uninitializeOsLib();
err_initOsLib:
    uninitializeNexus();
err_initNexus:
done:
    return ret;
}

void uninitializeBrutus2()
{
    uninitializeOsLib();
    uninitializeNexus();
}


typedef enum
{
    eBrutusMode_SingleDisplay,
    eBrutusMode_HdSdSingleDisplay,
    eBrutusMode_DualDisplay,
    eBrutusMode_TripleDisplay
} eBrutusMode;


#include "screen_main.h"

class CBrutus
{
public:
    CBrutus(uint16_t                number,
            eBrutusMode             mode,
            CModelChannelMgr      * pModelChannelMgr,
            CLua                  * pLua = NULL);
    ~CBrutus();

    eRet initialize(CModelConfig * pModelConfig);
    void uninitialize(void);
    bwin_engine_t getWinEngine(void);
    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };
    eRet run();

    CControl * getControl(void) { return &_control; };

protected:
    eBrutusMode             _mode;
    CModelConfig          * _pModelConfig;
    CModelChannelMgr      * _pModelChannelMgr;
    CConfiguration        * _pCfg;
    CBoardResources       * _pBoardResources;
    uint16_t                _number;

    bwidget_engine_t        _widgetEngine;
    CLua                  * _pLua;

    CControl                _control;
    CView                   _view;
    CModel                  _model;

    CScreen               * _pBackgroundScreen;
    CScreenMain           * _pMainScreen;
};


CBrutus::CBrutus(uint16_t                number,
                 eBrutusMode             mode,
                 CModelChannelMgr      * pModelChannelMgr,
                 CLua                  * pLua) :
    _pModelConfig(NULL),
    _pCfg(NULL),
    _pBoardResources(NULL),
    _widgetEngine(NULL),
    _number(number),
    _mode(mode),
    _pModelChannelMgr(pModelChannelMgr),
    _pLua(pLua),
    _pBackgroundScreen(NULL),
    _pMainScreen(NULL)
{
    BDBG_ASSERT(NULL != _pModelChannelMgr);
    _model.setId(this);
}

CBrutus::~CBrutus()
{
    uninitialize();
}

void idle_callback(bwidget_engine_t engine, void * context, int param)
{
    CBrutus        * pBrutus = (CBrutus *)context;
    BDBG_ASSERT(NULL != pBrutus);
        CControl      * pControl = pBrutus->getControl();

    BSTD_UNUSED(param);
    BSTD_UNUSED(engine);

    /* controller will handle idle loop processing */
    pControl->onIdle();
}


eRet CBrutus::initialize(CModelConfig * pModelConfig)
{
    eRet                    ret                    = eRet_Ok;
    CDisplay              * pDisplay               = NULL;
    CGraphics             * pGraphics              = NULL;
    CResource             * pResource              = NULL;
    CSimpleVideoDecode    * pVideoDecode           = NULL;
    CSimpleAudioDecode    * pAudioDecode           = NULL;
    COutputHdmi           * pOutputHdmi            = NULL;
    CStc                  * pStc                   = NULL;
    CVideoWindow          * pVideoWindow           = NULL;
    CMixer                * pMixer                 = NULL;
    COutputAudioDac       * pAudioDacOutput        = NULL;
    CIrRemote             * pIrRemote              = NULL;
    CPlaybackList         * pPlaybackList          = NULL;

    _pModelConfig    = pModelConfig;
    _pCfg            = pModelConfig->getCfg();
    _pBoardResources = pModelConfig->getBoardResources();


    /* initialize MVC relationships */
    {
        /* register control with view */
        _view.registerObserver(&_control);
        if (NULL != _pLua)
        {
            _control.addView(_pLua, "lua");

            /* register control with lua scriptengine if given */
            _pLua->registerObserver(&_control);
        }

#if NEXUS_HAS_FRONTEND
        /* register control with tuners so control can monitor when scan completes */
        _pBoardResources->registerObserver(this, eBoardResource_frontendQam, &_control);
        _pBoardResources->registerObserver(this, eBoardResource_frontendVsb, &_control);
        _pBoardResources->registerObserver(this, eBoardResource_frontendSds, &_control);
        _pBoardResources->registerObserver(this, eBoardResource_frontendOfdm, &_control);
#endif
        _pBoardResources->registerObserver(this, eBoardResource_decodeVideo, &_control);

        /* register view with models */
        _model.registerObserver(&_view);
        _model.registerObserver(_pLua);
        _pModelChannelMgr->registerObserver(&_view);
        _pModelChannelMgr->registerObserver(_pLua);
#if NEXUS_HAS_FRONTEND
        _pBoardResources->registerObserver(this, eBoardResource_frontendQam, _pLua);
        _pBoardResources->registerObserver(this, eBoardResource_frontendVsb, _pLua);
        _pBoardResources->registerObserver(this, eBoardResource_frontendSds, _pLua);
        _pBoardResources->registerObserver(this, eBoardResource_frontendOfdm, _pLua);
#endif

        _control.addView(&_view, "testview");


        _control.setModel(&_model);
        _control.setModelChannelMgr(_pModelChannelMgr);
        _model.setModelChannelMgr(_pModelChannelMgr);
        _view.setModel(&_model);
    }

    /* open display resource */
    pDisplay = (CDisplay *)_pBoardResources->checkoutResource(this, eBoardResource_display);
    CHECK_PTR_GOTO("unable to checkout display", pDisplay, ret, eRet_NotAvailable, error);
    ret = pDisplay->open();
    CHECK_ERROR_GOTO("display failed to open", ret, error);
    
    /* add display to model */
    _model.addDisplay(pDisplay);

    /* open graphics resource */
    {
        MRect rectGraphics;

        pGraphics = (CGraphics *)_pBoardResources->checkoutResource(this, eBoardResource_graphics);
        CHECK_PTR_GOTO("unable to checkout graphics", pGraphics, ret, eRet_NotAvailable, error);
        ret = pGraphics->open();
        CHECK_ERROR_GOTO("graphics failed to open", ret, error);
        pGraphics->setDisplays(pDisplay, NULL);
        rectGraphics = pDisplay->getGraphicsGeometry();
        ret = pGraphics->initGraphics(GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH),
                                      GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT),
                                      rectGraphics.width(),
                                      rectGraphics.height());
    }
    
    /* add graphics to model */
    _model.addGraphics(pGraphics);


    /* open bwidgets */
    {
        bwidget_engine_settings settings;
        settings.idle    = idle_callback; 
        settings.context = this;
        settings.param   = 0;
        _widgetEngine = bwidget_engine_open(pGraphics->getWinEngine(), &settings);
        CHECK_PTR_GOTO("Unable to open widget engine", _widgetEngine, ret, eRet_ExternalError, error);
    }

    _control.initialize(_pModelConfig, _pModelChannelMgr, _widgetEngine);

    /* open ir remote if reserved for us */
    pIrRemote = (CIrRemote *)_pBoardResources->checkoutResource(this, eBoardResource_irRemote);
    if (NULL != pIrRemote)
    {
        ret = pIrRemote->open(_widgetEngine);
        CHECK_ERROR_GOTO("ir remote failed to open", ret, error);

        /* notifications from remote */
        pIrRemote->registerObserver(&_control);

        /* add ir remote to model */
        _model.addIrRemote(pIrRemote);
    }

    /* open STC channel for a/v sync */
    pStc = (CStc *)_pBoardResources->checkoutResource(this, eBoardResource_stcChannel);
    if (NULL != pStc)
    {
        ret = pStc->open();
        CHECK_ERROR_GOTO("stc failed to open", ret, error);

        _model.addStc(pStc);
    }

    {
        COutputComponent * pOutputComponent = NULL;
        COutputComposite * pOutputComposite = NULL;

        /* add/setup HDMI output if in resource list */
        pOutputHdmi = (COutputHdmi *)_pBoardResources->checkoutResource(this, eBoardResource_outputHdmi);
        if (NULL != pOutputHdmi)
        {
            NEXUS_Error                   nError    = NEXUS_SUCCESS;
            NEXUS_HdmiOutputSettings settings;

            /* connect output to display */
            ret = pDisplay->addOutput(pOutputHdmi);
            CHECK_ERROR_GOTO("Error adding HDMI output", ret, error);

            NEXUS_HdmiOutput_GetSettings(pOutputHdmi->getOutput(), &settings);
            settings.autoColorSpace = false;
            settings.colorSpace = NEXUS_ColorSpace_eRgb; /* or NEXUS_ColorSpace_eYCbCr444 */
            nError = NEXUS_HdmiOutput_SetSettings(pOutputHdmi->getOutput(), &settings);
            CHECK_NEXUS_ERROR_GOTO("error setting component output settings", ret, nError, error);
        } 

        /* add/setup COMPONENT output if in resource list */
        pOutputComponent = (COutputComponent *)_pBoardResources->checkoutResource(this, eBoardResource_outputComponent);
        if (NULL != pOutputComponent)
        {
            NEXUS_Error                   nError    = NEXUS_SUCCESS;
            NEXUS_ComponentOutputSettings settings;

            /* connect output to display */
            ret = pDisplay->addOutput(pOutputComponent);
            CHECK_ERROR_GOTO("Error adding component output", ret, error);

            NEXUS_ComponentOutput_GetSettings(pOutputComponent->getOutput(), &settings);
            settings.mpaaDecimationEnabled = GET_BOOL(_pCfg, MPAA_ENABLED);
            settings.type                  = pOutputComponent->getDesiredType();
            nError = NEXUS_ComponentOutput_SetSettings(pOutputComponent->getOutput(), &settings);
            CHECK_NEXUS_ERROR_GOTO("error setting component output settings", ret, nError, error);
        }

        /* dtt todo: need to open sd outputs on SD only pip display */
#if 0
        /* add/setup COMPOSITE output if in resource list */
        pOutputComposite = (COutputComposite *)_pBoardResources->checkoutResource(this, eBoardResource_outputComposite);
        if (NULL != pOutputComposite)
        {
            NEXUS_Error                   nError    = NEXUS_SUCCESS;
            NEXUS_CompositeOutputSettings settings;

            /* connect output to display */
            ret = pDisplay->addOutput(pOutputComposite);
            CHECK_ERROR_GOTO("Error adding composite output", ret, error);

            NEXUS_CompositeOutput_GetSettings(pOutputComposite->getOutput(), &settings);
            settings.dac = NEXUS_VideoDac_e0; /* dtt - do we need to set this? */
            nError = NEXUS_CompositeOutput_SetSettings(pOutputComposite->getOutput(), &settings);
            CHECK_NEXUS_ERROR_GOTO("error setting composite output settings", ret, nError, error);
        }
#endif
    }

    /* add/setup video decode if available or reserved for us */
    pVideoDecode = (CSimpleVideoDecode *)_pBoardResources->checkoutResource(this, eBoardResource_simpleDecodeVideo);
    if (NULL != pVideoDecode)
    {
        /* simple decoder will connect display, video window and video decoder */
        pVideoDecode->setDisplay(pDisplay);
        pVideoDecode->setResources(this, _pBoardResources);

        /* notifications from video decoder */
        pVideoDecode->registerObserver(&_control);

        ret = pVideoDecode->open(_widgetEngine, pStc);
        CHECK_ERROR_GOTO("video decode failed to open", ret, error);

        /* add video decode to model */
        _model.addSimpleVideoDecode(pVideoDecode);
    }

#if 0
    /* open mixer */
    pMixer = (CMixer *)_pBoardResources->checkoutResource(this, eBoardResource_mixer);
    CHECK_PTR_GOTO("unable to checkout mixer", pMixer, ret, eRet_NotAvailable, error);
    ret = pMixer->open();
    CHECK_ERROR_GOTO("mixer failed to open", ret, error);
#endif

    pAudioDecode = (CSimpleAudioDecode *)_pBoardResources->checkoutResource(this, eBoardResource_simpleDecodeAudio);
    if (NULL != pAudioDecode)
    {
        /* simple decoder will connect decoders and outputs */
        pAudioDecode->setResources(this, _pBoardResources);
        /* reuse hdmi output for audio */
        pAudioDecode->setOutputHdmi(pOutputHdmi);

        ret = pAudioDecode->open(_widgetEngine, pStc);
        CHECK_ERROR_GOTO("audio decode failed to open", ret, error);

        /* add audio decode to model */
        _model.addSimpleAudioDecode(pAudioDecode);
    }

    {
        COutputAudioDac * pAudioDacOutput = NULL;

        /* add/setup audio dac */
        pAudioDacOutput = (COutputAudioDac *)_pBoardResources->checkoutResource(this, eBoardResource_outputAudioDac);
        if (NULL != pAudioDacOutput)
        {
            ret = pAudioDacOutput->connect(pAudioDecode->getConnector(0, NEXUS_AudioConnectorType_eStereo));
            CHECK_ERROR_GOTO("unable to connect dac output", ret, error);
        }
    }

    pPlaybackList = new CPlaybackList;
    CHECK_PTR_GOTO("unable to allocate playback list", pPlaybackList, ret, eRet_OutOfMemory, error);
    _model.setPlaybackList(pPlaybackList);

    /* initialize screens */
    {
        /* create full sized transparent background screen */
        _pBackgroundScreen = new CScreen(_widgetEngine, pModelConfig, NULL, pGraphics->getWinFramebuffer());
        CHECK_PTR_GOTO("background screen creation failed", _pBackgroundScreen, ret, eRet_OutOfMemory, error);
        ret = _pBackgroundScreen->initialize(&_model);
        CHECK_ERROR_GOTO("background screen failed to initialize", ret, error);

        /* create main window */
        _pMainScreen = new CScreenMain(_widgetEngine, pModelConfig, _pBackgroundScreen);
        CHECK_PTR_GOTO("menu screen creation failed", _pMainScreen, ret, eRet_OutOfMemory, error);
        ret = _pMainScreen->initialize(&_model);
        CHECK_ERROR_GOTO("menu screen failed to initialize", ret, error);

        _control.addView(_pMainScreen, "mainScreen");
        _pMainScreen->registerObserver(&_control);

        _model.registerObserver(_pMainScreen);
        _pModelChannelMgr->registerObserver(_pMainScreen);
        pDisplay->registerObserver(_pMainScreen);
        pVideoDecode->registerObserver(_pMainScreen);
        pAudioDecode->registerObserver(_pMainScreen);
        pPlaybackList->registerObserver(_pMainScreen);

#if NEXUS_HAS_FRONTEND
        _pBoardResources->registerObserver(this, eBoardResource_frontendQam,  _pMainScreen);
        _pBoardResources->registerObserver(this, eBoardResource_frontendVsb,  _pMainScreen);
        _pBoardResources->registerObserver(this, eBoardResource_frontendSds,  _pMainScreen);
        _pBoardResources->registerObserver(this, eBoardResource_frontendOfdm, _pMainScreen);
#endif
        _pBoardResources->registerObserver(this, eBoardResource_playback,     _pMainScreen);
        _pBoardResources->registerObserver(this, eBoardResource_record,       _pMainScreen);

        pDisplay->registerObserver(_pLua);
        pVideoDecode->registerObserver(_pLua);
        pAudioDecode->registerObserver(_pLua);

        _control.updatePlaybackList();
        _pMainScreen->show(true);
    }

    return ret;
error:
    BDBG_ASSERT(false);
}

void CBrutus::uninitialize()
{
    CSimpleAudioDecode * pSimpleAudioDecode = _model.getSimpleAudioDecode();
    CSimpleVideoDecode * pSimpleVideoDecode = _model.getSimpleVideoDecode();
    CStc               * pStc               = _model.getStc();
    CMixer             * pMixer             = _model.getMixer();
    CDisplay           * pDisplay           = _model.getDisplay();
    CGraphics          * pGraphics          = _model.getGraphics();
    CIrRemote          * pIrRemote          = _model.getIrRemote();
    COutputAudioDac    * pAudioDacOutput    = NULL;
    COutputComponent   * pOutputComponent   = NULL;
    COutputHdmi        * pOutputHdmi        = NULL;
    COutputComposite   * pOutputComposite   = NULL;
    CPlaybackList      * pPlaybackList      = _model.getPlaybackList();

    if (NULL != pPlaybackList)
    {
        _model.setPlaybackList(NULL);
        delete pPlaybackList;
    }

    /* destroy screens */
    DEL(_pMainScreen);
    /* reparent pBackgroundScreen before destroying because all windows directly
       descended from the framebuffer win don't seem to unlink automatically.
       changing the parent win to NULL will force the unlink to occur before
       destruction. */
    {
        _pBackgroundScreen->setParentWin(NULL);
        DEL(_pBackgroundScreen);
    }

    /* remove audio dac */
    pAudioDacOutput = (COutputAudioDac *)_pBoardResources->findCheckedoutResource(this, eBoardResource_outputAudioDac);
    if (NULL != pAudioDacOutput)
    {
        pAudioDacOutput->disconnect();
    }

    if ((NULL != pSimpleAudioDecode) && (NULL != pStc))
    {
        _model.removeAudioDecode(pSimpleAudioDecode);
        pSimpleAudioDecode->close();
        _pBoardResources->checkinResource(pSimpleAudioDecode);
    }

    if ((NULL != pSimpleVideoDecode) && (NULL != pDisplay) && (NULL != pStc))
    {
        _model.removeSimpleVideoDecode(pSimpleVideoDecode);
        pSimpleVideoDecode->close();
        _pBoardResources->checkinResource(pSimpleVideoDecode);
    }

    if (NULL != pMixer)
    {
        _model.removeMixer(pMixer);
        pMixer->close();
        _pBoardResources->checkinResource(pMixer);
    }

    /* remove COMPONENT output from display */
    pOutputComponent = (COutputComponent *)_pBoardResources->findCheckedoutResource(this, eBoardResource_outputComponent);
    if (NULL != pOutputComponent)
    {
        pDisplay->removeOutput(pOutputComponent);
    }

    /* remove HDMI output from display */
    pOutputHdmi = (COutputHdmi *)_pBoardResources->findCheckedoutResource(this, eBoardResource_outputHdmi);
    if (NULL != pOutputHdmi)
    {
        pDisplay->removeOutput(pOutputHdmi);
    }

    /* remove COMPOSITE output from display */
    pOutputComposite = (COutputComposite *)_pBoardResources->findCheckedoutResource(this, eBoardResource_outputComposite);
    if (NULL != pOutputComposite)
    {
        pDisplay->removeOutput(pOutputComposite);
    }

    if (NULL != pStc)
    {
        _model.removeStc(pStc);
        pStc->close();
        _pBoardResources->checkinResource(pStc);
    }

    if (NULL != pIrRemote)
    {
        _model.removeIrRemote(pIrRemote);
        pIrRemote->close();
        _pBoardResources->checkinResource(pIrRemote);
    }

    _control.uninitialize();

    if (NULL != _widgetEngine)
    {
        bwidget_engine_close(_widgetEngine);
    }

    if (NULL != pGraphics)
    {
        _model.removeGraphics(pGraphics);
        pGraphics->close();
        _pBoardResources->checkinResource(pGraphics);
    }

    if (NULL != pDisplay)
    {
        _model.removeDisplay(pDisplay);
        pDisplay->close();
        _pBoardResources->checkinResource(pDisplay);
    }

    /* uninitialize MVC relationships */

    /* unregister control with view */
    _view.unregisterObserver(&_control);
    if (NULL != _pLua)
    {
        /* register control with lua scriptengine if given */
        _pLua->unregisterObserver(&_control);
    }

    /* unregister view with models */
    _model.unregisterObserver(&_view);
    _model.unregisterObserver(_pLua);
    _pModelChannelMgr->unregisterObserver(&_view);
    _pModelChannelMgr->unregisterObserver(_pLua);
#if NEXUS_HAS_FRONTEND
    _pBoardResources->unregisterObserver(this, eBoardResource_frontendQam, _pLua);
    _pBoardResources->unregisterObserver(this, eBoardResource_frontendVsb, _pLua);
    _pBoardResources->unregisterObserver(this, eBoardResource_frontendSds, _pLua);
    _pBoardResources->unregisterObserver(this, eBoardResource_frontendOfdm, _pLua);
#endif
}

bwin_engine_t CBrutus::getWinEngine()
{
    bwin_engine_t   winEngine = NULL;
    CGraphics     * pGraphics = NULL;
    eRet            ret       = eRet_Ok;

    pGraphics = (CGraphics *)_pBoardResources->findCheckedoutResource(this, eBoardResource_graphics);
    CHECK_PTR_GOTO("Cannot get bwin engine without a graphics.", pGraphics, ret, eRet_InvalidState, error);

    winEngine = pGraphics->getWinEngine();

error:
    return winEngine;
}

eRet CBrutus::run()
{
    eRet                    ret          = eRet_Ok;
    bwidget_engine_t widgetEngine = getWidgetEngine();

    BDBG_ASSERT(NULL != widgetEngine);

    /* start lua script engine if available */
    if (NULL != _pLua)
    {
        BDBG_WRN(("starting lua script engine for brutus number: %d", _number));
        _pLua->run(widgetEngine);
    }

    bwidget_run(widgetEngine);

    _pLua->stop();

    signal(SIGINT, SIG_DFL);

error:
    return ret;
}


/**
This is the universal entry point for Brutus and
is entry point for Linux. Other operating systems (like VxWorks)
have other entry points which then call main().
**/

#include <signal.h>

static void cleanExit(int sig)
{
    CConfiguration * pCfg = g_modelConfig.getCfg();
    BDBG_ASSERT(NULL != pCfg);

    SET(pCfg, EXIT_APPLICATION, "true");
}

static void readCmdLineParams(int argc, char ** argv, CConfiguration * pCfg)
{
    for (int i = 1; i < argc; i++)
    {
        if (!strcasecmp(argv[i], "-script") && i<argc-1) 
        {
            SET(pCfg, LUA_SCRIPT, argv[++i]);
            BDBG_WRN(("-script specified:%s", GET_STR(pCfg, LUA_SCRIPT)));

            /* since we are running a lua script, don't bother with initial tune */
            SET(pCfg, FIRST_TUNE, false);
        }
    }
}


int main(int argc, char **argv)
{
    const char *cfgfile;
    char *ipstr = NULL;
    bool scriptmode = false;
    bool interactivemode = false;
    bool skin_specified = false;
    char *skin_name = NULL;
    int wakeupFirstChannel = 0;

    eRet ret = eRet_Ok;

    signal(SIGINT, cleanExit);

    fprintf(stderr,
        "brutus2 (C)2001-2011, Broadcom Corporation\n"
    "  Built on %s, BCHP_CHIP %d\n", buildDate(), BCHP_CHIP);

    BDBG_WRN(("*** initialize brutus2"));
    ret = initializeBrutus2();
    CHECK_ERROR_GOTO("brutus2 initialization failed", ret, error);

    BDBG_WRN(("*** start brutus2"));
    {
        eRet               ret              = eRet_Ok;
        CModelChannelMgr   modelChannelMgr;
        CConfiguration   * pCfg             = NULL;
        char             * pListName        = NULL;

        /* read chips info, board features, and board resources */
        ret = g_modelConfig.initialize();
        CHECK_ERROR_GOTO("failed to initialize model config", ret, error);
        /*g_modelConfig.dump();*/ /* debug */

        pCfg = g_modelConfig.getCfg();
        CHECK_PTR_GOTO("configuration failure", pCfg, ret, eRet_InvalidParameter, error);
        modelChannelMgr.setCfg(pCfg);

        readCmdLineParams(argc, argv, pCfg);

        BDBG_WRN(("*** load channel list"));
        
        pListName = (char *)GET_STR(pCfg, CHANNELS_LIST);
        if ((NULL != pListName) && (0 < strlen(pListName)))
        {
            modelChannelMgr.loadChannelList(pListName);
            /*modelChannelMgr.dumpChannelList();*/ /* debug */
        } 

        {
            CResource         * pResource       = NULL;
            CBrutus           * pBrutus0        = NULL;
            uint16_t            number          = 0;
            CBoardResources   * pBoardResources = NULL;
            CLua                lua;

            ret = lua.initialize(&g_modelConfig);
            CHECK_ERROR_GOTO("lua failed to initialize", ret, error);

            pBrutus0 = new CBrutus(number, eBrutusMode_SingleDisplay, &modelChannelMgr, &lua);
            CHECK_PTR_GOTO("unable to instantiate CBrutus", pBrutus0, ret, eRet_OutOfMemory, error);

            number++;

            /* 
            some resources are pre-reserved for each instance of brutus.  other resources will be 
            dynamically checked out by each brutus instance based on given eBrutusMode. 
            */ 
            pBoardResources = g_modelConfig.getBoardResources();

            pResource = pBoardResources->reserveResource(pBrutus0, eBoardResource_outputComponent);
            CHECK_PTR_GOTO("requested resource unavailable", pResource, ret, eRet_NotSupported, error);
            pResource = pBoardResources->reserveResource(pBrutus0, eBoardResource_outputComposite);
            CHECK_PTR_GOTO("requested resource unavailable", pResource, ret, eRet_NotSupported, error);
            pResource = pBoardResources->reserveResource(pBrutus0, eBoardResource_outputHdmi);
            CHECK_PTR_GOTO("requested resource unavailable", pResource, ret, eRet_NotSupported, error);

            pBrutus0->initialize(&g_modelConfig);
            pBrutus0->run();

            delete pBrutus0;
        }
    }

    /*uninitializeBrutus2();*/

    return 0;
error:
    return -1;
}

